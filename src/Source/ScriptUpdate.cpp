#include "stdafx.h"
#include "ScriptUpdate.h"
#include "../../lib/miniz/miniz.h"
#include <string>
#include <vector>
#include <algorithm>
#include <winhttp.h>
#include <Shlobj.h>
#pragma comment(lib, "winhttp.lib")

using namespace std;

// Prefix of entries inside the ZIP we care about.
// GitHub names the root folder as "{repo}-{branch}/"
static const char ZIP_PREFIX[] = "fictionbookeditor-vs2026/files/";
static const size_t ZIP_PREFIX_LEN = sizeof(ZIP_PREFIX) - 1;

// ---------------------------------------------------------------------------
// Download entire ZIP into memory from codeload.github.com (no redirect)
// ---------------------------------------------------------------------------
static vector<char> DownloadZip(HWND hwLog, volatile bool* pCancel)
{
    auto PostLog = [&](const wchar_t* s) {
        ::PostMessage(hwLog, WM_SCRIPTUPDATE_LOG, 0, (LPARAM) new wstring(s));
    };

    HINTERNET hSession = WinHttpOpen(L"FBE/1.0",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return {};

    HINTERNET hConn = WinHttpConnect(hSession,
        L"codeload.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConn) { WinHttpCloseHandle(hSession); return {}; }

    HINTERNET hReq = WinHttpOpenRequest(hConn, L"GET",
        L"/cobra838/fictionbookeditor/zip/refs/heads/vs2026",
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    vector<char> data;

    if (hReq)
    {
        DWORD policy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
        WinHttpSetOption(hReq, WINHTTP_OPTION_REDIRECT_POLICY, &policy, sizeof(policy));

        if (WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
            WinHttpReceiveResponse(hReq, NULL))
        {
            // Pre-allocate from Content-Length if available
            wchar_t clBuf[32] = {};
            DWORD clBufSz = sizeof(clBuf);
            if (WinHttpQueryHeaders(hReq,
                    WINHTTP_QUERY_CONTENT_LENGTH,
                    WINHTTP_HEADER_NAME_BY_INDEX,
                    clBuf, &clBufSz, WINHTTP_NO_HEADER_INDEX))
            {
                DWORD64 cl = _wcstoui64(clBuf, NULL, 10);
                if (cl > 0 && cl < 500ULL * 1024 * 1024)
                    data.reserve((size_t)cl);
            }

            char buf[65536]; DWORD nRead;
            size_t lastReport = 0;
            while (!*pCancel &&
                   WinHttpReadData(hReq, buf, sizeof(buf), &nRead) && nRead > 0)
            {
                data.insert(data.end(), buf, buf + nRead);
                if (data.size() - lastReport >= 512 * 1024)
                {
                    lastReport = data.size();
                    wstring msg = wstring(L"Downloading... ") +
                                  to_wstring(data.size() / 1024) + L" KB";
                    PostLog(msg.c_str());
                }
            }
        }
        WinHttpCloseHandle(hReq);
    }

    WinHttpCloseHandle(hConn);
    WinHttpCloseHandle(hSession);
    return data;
}

// ---------------------------------------------------------------------------
// UTF-8 path → wstring
// ---------------------------------------------------------------------------
static wstring Utf8ToWide(const char* s)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (len <= 1) return {};
    wstring ws(len - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, s, -1, &ws[0], len);
    return ws;
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------
DWORD WINAPI CScriptUpdateDlg::ThreadProc(LPVOID pParam)
{
    ThreadParam* p = (ThreadParam*)pParam;
    HWND hw = p->hwnd;
    volatile bool* pCancel = p->pCancel;
    delete p;

    auto PostLog = [&](const wchar_t* s) {
        ::PostMessage(hw, WM_SCRIPTUPDATE_LOG, 0, (LPARAM) new wstring(s));
    };

    PostLog(L"Downloading archive from GitHub...");

    vector<char> zipData = DownloadZip(hw, pCancel);

    if (*pCancel || zipData.empty())
    {
        ::PostMessage(hw, WM_SCRIPTUPDATE_DONE, *pCancel ? 2 : 0, 0);
        return 0;
    }

    {
        wstring msg = wstring(L"Downloaded ") +
                      to_wstring(zipData.size() / 1024) + L" KB. Extracting...";
        PostLog(msg.c_str());
    }

    // Get exe directory
    wchar_t exePath[MAX_PATH];
    ::GetModuleFileName(NULL, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wstring exeDir = exePath;

    // Open ZIP from memory
    mz_zip_archive zip;
    mz_zip_zero_struct(&zip);
    if (!mz_zip_reader_init_mem(&zip, zipData.data(), zipData.size(), 0))
    {
        PostLog(L"Error: could not open ZIP archive.");
        ::PostMessage(hw, WM_SCRIPTUPDATE_DONE, 0, 0);
        return 0;
    }

    int zipTotal  = (int)mz_zip_reader_get_num_files(&zip);
    int matched   = 0;
    int done      = 0;
    int failed    = 0;

    for (int i = 0; i < zipTotal && !*pCancel; ++i)
    {
        if (mz_zip_reader_is_file_a_directory(&zip, i))
            continue;

        mz_zip_archive_file_stat stat;
        if (!mz_zip_reader_file_stat(&zip, i, &stat))
            continue;

        if (strncmp(stat.m_filename, ZIP_PREFIX, ZIP_PREFIX_LEN) != 0)
            continue;

        const char* relPathA = stat.m_filename + ZIP_PREFIX_LEN;
        if (*relPathA == '\0') continue;

        ++matched;

        wstring wRel = Utf8ToWide(relPathA);
        for (auto& c : wRel) if (c == L'/') c = L'\\';
        wstring localPath = exeDir + L"\\" + wRel;

        // Create parent directory
        wstring parentDir = localPath.substr(0, localPath.rfind(L'\\'));
        SHCreateDirectoryExW(NULL, parentDir.c_str(), NULL);

        // Extract to heap buffer
        size_t fileSize = 0;
        void* pData = mz_zip_reader_extract_to_heap(&zip, i, &fileSize, 0);
        if (pData)
        {
            HANDLE hf = CreateFileW(localPath.c_str(), GENERIC_WRITE, 0,
                NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hf != INVALID_HANDLE_VALUE)
            {
                DWORD written;
                WriteFile(hf, pData, (DWORD)fileSize, &written, NULL);
                CloseHandle(hf);
                ++done;
            }
            else
            {
                ++failed;
                PostLog((wstring(L"[ERR write] ") + wRel).c_str());
            }
            mz_free(pData);
        }
        else
        {
            ++failed;
            PostLog((wstring(L"[ERR unzip] ") + wRel).c_str());
        }

        // Log every 10 files to keep UI responsive
        if (done % 10 == 0 || failed > 0)
        {
            wstring msg = wstring(L"[") + to_wstring(done) + L"/" + to_wstring(matched) + L"]";
            PostLog(msg.c_str());
        }
    }

    mz_zip_reader_end(&zip);

    wstring* pResult = new wstring(
        wstring(L"Matched: ") + to_wstring(matched) +
        L"  Done: " + to_wstring(done) +
        L"  Failed: " + to_wstring(failed) +
        L"\r\nDone! " + to_wstring(done) + L" files updated.");

    ::PostMessage(hw, WM_SCRIPTUPDATE_DONE, *pCancel ? 2 : 1, (LPARAM)pResult);
    return 0;
}

// ---------------------------------------------------------------------------
// Dialog handlers
// ---------------------------------------------------------------------------

LRESULT CScriptUpdateDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
    m_bCancel         = false;
    m_bCloseRequested = false;
    m_hThread         = NULL;
    m_AnimIdx         = 0;

    m_result.Attach(GetDlgItem(IDC_SCRIPTUPDATE_LOG));
    m_Pict.SubclassWindow(GetDlgItem(IDC_PIC_UPDATE));
    m_Pict.m_transparentColor = RGB(0, 0, 0);

    for (int i = 0; i < ANIM_SIZE; i++)
        m_AnimBitmaps[i].LoadBitmap(IDB_UPD_CHECK1 + i);
    m_StatusBitmaps[0].LoadBitmap(IDB_UPD_OK);
    m_StatusBitmaps[1].LoadBitmap(IDB_UPD_UPDATE);
    m_StatusBitmaps[2].LoadBitmap(IDB_UPD_ERR);

    m_Pict.SetBitmap(m_AnimBitmaps[0]);
    SetDlgItemText(IDC_TEXT_STATUS, L"Downloading archive from GitHub...");
    GetDlgItem(IDOK).EnableWindow(FALSE);

    SetTimer(1, 100, NULL);

    ThreadParam* p = new ThreadParam{ m_hWnd, &m_bCancel };
    m_hThread = CreateThread(NULL, 0, ThreadProc, p, 0, NULL);
    return TRUE;
}

LRESULT CScriptUpdateDlg::OnAnimTimer(UINT, WPARAM, LPARAM, BOOL&)
{
    if (m_AnimIdx >= ANIM_SIZE) m_AnimIdx = 0;
    m_Pict.SetBitmap(m_AnimBitmaps[m_AnimIdx++]);
    return 0;
}

LRESULT CScriptUpdateDlg::OnWindowClose(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
    bHandled = TRUE;
    if (GetDlgItem(IDOK).IsWindowEnabled())
        EndDialog(IDOK);
    else {
        m_bCancel         = true;
        m_bCloseRequested = true;
        GetDlgItem(IDCANCEL).EnableWindow(FALSE);
        SetDlgItemText(IDC_TEXT_STATUS, L"Cancelling...");
    }
    return 0;
}

LRESULT CScriptUpdateDlg::OnLogMsg(UINT, WPARAM, LPARAM lParam, BOOL&)
{
    wstring* s = (wstring*)lParam;
    SetDlgItemText(IDC_TEXT_STATUS, s->c_str());
    delete s;
    return 0;
}

LRESULT CScriptUpdateDlg::OnDoneMsg(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
{
    KillTimer(1);
    if (m_hThread) { CloseHandle(m_hThread); m_hThread = NULL; }

    wstring* pResult = (wstring*)lParam;

    if (m_bCloseRequested) { delete pResult; EndDialog(IDOK); return 0; }

    if (wParam == 1 && pResult)
    {
        m_Pict.SetBitmap(m_StatusBitmaps[0]); // OK
        SetDlgItemText(IDC_TEXT_STATUS, L"Update complete.");
        m_result.SetWindowText(pResult->c_str());
    }
    else if (wParam == 2)
    {
        m_Pict.SetBitmap(m_StatusBitmaps[2]);
        SetDlgItemText(IDC_TEXT_STATUS, L"Cancelled.");
    }
    else
    {
        m_Pict.SetBitmap(m_StatusBitmaps[2]);
        SetDlgItemText(IDC_TEXT_STATUS, L"Error: download failed.");
    }

    delete pResult;
    GetDlgItem(IDOK).EnableWindow(TRUE);
    GetDlgItem(IDCANCEL).EnableWindow(FALSE);
    return 0;
}

LRESULT CScriptUpdateDlg::OnCloseCmd(WORD, WORD, HWND, BOOL&)
{
    EndDialog(IDOK);
    return 0;
}

LRESULT CScriptUpdateDlg::OnCancelCmd(WORD, WORD, HWND, BOOL&)
{
    m_bCancel         = true;
    m_bCloseRequested = true;
    GetDlgItem(IDCANCEL).EnableWindow(FALSE);
    SetDlgItemText(IDC_TEXT_STATUS, L"Cancelling...");
    return 0;
}
