#include "stdafx.h"
#include "Utils.h"
#include "AboutBox.h"
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

LRESULT CAboutDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	m_bAllowResize = false;
	m_hCheckThread = NULL;
	memset(&m_InitialRect, 0, sizeof(m_InitialRect));
	m_initCW = m_initCH = 0;

	SetIcon(LoadIcon(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_MAINFRAME)));

	CString stamp(build_timestamp);
	::SetWindowText(GetDlgItem(IDC_BUILDSTAMP), stamp);

	CString bname(build_name);
	::SetWindowText(GetDlgItem(IDC_STATIC_AB_APPNAMEVER), bname);

	m_Contributors = GetDlgItem(IDC_CONTRIBS);
	HRSRC hres = ::FindResource(NULL, L"ABOUT_FILE", L"ABOUT_FILE");
	HGLOBAL hbytes = ::LoadResource(NULL, hres);
	CA2CT contribs((char*)::LockResource(hbytes), 65001);  // UTF-8
	CString s(contribs);
	m_Contributors.SetWindowText(s.Left(s.ReverseFind(L'\n')-1));

	// create OpenGL logo window
	m_glLogo.SubclassWindow(GetDlgItem(IDC_AB_BANNER));
	if (m_glLogo.OpenGLError())
	{
		m_glLogo.UnsubclassWindow(TRUE);
		GetDlgItem(IDC_AB_BANNER).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_AB_STATIC_BANNER).ShowWindow(SW_SHOW);
	}
	else GetDlgItem(IDC_AB_BANNER).ShowWindow(SW_SHOW);

	// setup automatic updates engine
	m_UpdateButton = GetDlgItem(IDC_UPDATE);
	m_UpdateButton.ShowWindow(SW_HIDE);

	m_AnimIdx = 0;
	m_UpdatePict.SubclassWindow(GetDlgItem(IDC_PIC_UPDATE));
	m_UpdatePict.m_transparentColor = RGB(0,0,0);
	for (int i=0; i<ANIM_SIZE; i++)
		m_AnimBitmaps[i].LoadBitmap(IDB_UPD_CHECK1+i);
	m_UpdatePict.SetBitmap(m_AnimBitmaps[0]);

	m_StatusBitmaps[0].LoadBitmap(IDB_UPD_OK);
	m_StatusBitmaps[1].LoadBitmap(IDB_UPD_UPDATE);
	m_StatusBitmaps[2].LoadBitmap(IDB_UPD_ERR);

	// load localized messages
	m_sCheckingUpdate.LoadString(IDS_UPDATE_CHECK);
	m_sConnecting.LoadString(IDS_UPDATE_CONNECTING);
	m_sCantConnect.LoadString(IDS_UPDATE_CANTCONNECT);
	m_sDownloadedFrom.LoadString(IDS_UPDATE_DOWNLOADEDFROM);
	m_sDownloaded.LoadString(IDS_UPDATE_DOWNLOADED);
	m_sDownloadCompleted.LoadString(IDS_UPDATE_DOWNLOADCOMPLETE);
	m_sDownloadReady.LoadString(IDS_UPDATE_DOWNLOADREADY);
	m_sDownloadError.LoadString(IDS_UPDATE_DOWNLOADERROR);
	m_sError404.LoadString(IDS_UPDATE_404ERROR);
	m_sError403.LoadString(IDS_UPDATE_403ERROR);
	m_sError407.LoadString(IDS_UPDATE_407ERROR);
	m_sNotSupportRange.LoadString(IDS_UPDATE_NOTSUPPORTEDRANGE);
	m_sDownloadErrorStatus.LoadString(IDS_UPDATE_DOWNLOADERRORSTATUS);
	m_sIncorrectChecksum.LoadString(IDS_UPDATE_INCORRECTMD5);
	m_sNewVersionAvailable.LoadString(IDS_UPDATE_NEWVERSIONAVAILABLE);
	m_sHaveLatestVersion.LoadString(IDS_UPDATE_HAVELATESTVERSION);
	m_sLogoCaption.LoadString(IDS_ABOUT_LOGOCAPTION);

	// check FBE update via GitHub API
	CheckUpdate();

	return 0;
}

LRESULT CAboutDlg::OnCloseCmd(WORD, WORD wID, HWND, BOOL&)
{
	KillTimer(1);
	DeleteAllDownload();
	if (m_hCheckThread)
	{
		// don't wait — thread will PostMessage to dead HWND (safe, ignored)
		CloseHandle(m_hCheckThread);
		m_hCheckThread = NULL;
	}
	EndDialog(wID);
	return 0;
}

LRESULT CAboutDlg::OnCtlColor(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hwndEdit = (HWND) lParam;
	if (hwndEdit == GetDlgItem(IDC_CONTRIBS))
	{
		HDC hdc = (HDC)wParam;
		::SetBkColor(hdc, RGB(255,255,255));
		return (LRESULT) ::GetStockObject(WHITE_BRUSH);
	}
	return 0;
}

LRESULT CAboutDlg::OnNMClickSyslinkAbLinks(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL&)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	CString args;

	args.Format(L"url.dll, FileProtocolHandler %s", pNMLink->item.szUrl);
	ShellExecute(NULL, L"open", L"rundll32.exe", args, NULL, SW_SHOW);

	return 0;
}

void CAboutDlg::CheckUpdate()
{
    m_UpdateReady = false;
    m_UpdateURL = L"";
    SetDlgItemText(IDC_TEXT_STATUS, m_sCheckingUpdate);
    m_AnimIdx = 0;
    SetTimer(1, 100, NULL); // animation timer
    m_hCheckThread = CreateThread(NULL, 0, CheckUpdateProc, (LPVOID)m_hWnd, 0, NULL);
}

// Static thread: WinHTTP GET to GitHub API, posts WM_UPDATE_CHECK_DONE
DWORD WINAPI CAboutDlg::CheckUpdateProc(LPVOID pParam)
{
    HWND hwnd = (HWND)pParam;

    HINTERNET hSession = WinHttpOpen(L"FBE/1.0",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) { ::PostMessage(hwnd, WM_UPDATE_CHECK_DONE, 0, (LPARAM)GetLastError()); return 0; }

    HINTERNET hConn = WinHttpConnect(hSession, L"api.github.com",
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConn) {
        DWORD err = GetLastError();
        WinHttpCloseHandle(hSession);
        ::PostMessage(hwnd, WM_UPDATE_CHECK_DONE, 0, (LPARAM)err);
        return 0;
    }

    HINTERNET hReq = WinHttpOpenRequest(hConn, L"GET",
        L"/repos/cobra838/fictionbookeditor/releases?per_page=1",
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    if (hReq)
    {
        if (WinHttpSendRequest(hReq, FBE_GITHUB_ACCEPT, (DWORD)-1,
                WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
            WinHttpReceiveResponse(hReq, NULL))
        {
            string* pJson = new string();
            char buf[8192]; DWORD nRead;
            while (WinHttpReadData(hReq, buf, sizeof(buf), &nRead) && nRead > 0)
                pJson->append(buf, nRead);
            ::PostMessage(hwnd, WM_UPDATE_CHECK_DONE, 1, (LPARAM)pJson);
        }
        else ::PostMessage(hwnd, WM_UPDATE_CHECK_DONE, 0, (LPARAM)GetLastError());
        WinHttpCloseHandle(hReq);
    }
    else ::PostMessage(hwnd, WM_UPDATE_CHECK_DONE, 0, (LPARAM)GetLastError());

    WinHttpCloseHandle(hConn);
    WinHttpCloseHandle(hSession);
    return 0;
}

LRESULT CAboutDlg::OnAnimTimer(UINT, WPARAM, LPARAM, BOOL&)
{
    if (m_AnimIdx >= ANIM_SIZE) m_AnimIdx = 0;
    m_UpdatePict.SetBitmap(m_AnimBitmaps[m_AnimIdx++]);
    return 0;
}

LRESULT CAboutDlg::OnUpdateCheckDone(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
{
    KillTimer(1);
    m_UpdatePict.SetBitmap(m_StatusBitmaps[2]); // default: error

    if (!wParam)
    {
        CString s;
        if (lParam)
            s.Format(L"%s (err %lu)", (LPCWSTR)m_sCantConnect, (DWORD)lParam);
        else
            s = m_sCantConnect;
        SetDlgItemText(IDC_TEXT_STATUS, s);
        return 0;
    }
    if (!lParam)
    {
        SetDlgItemText(IDC_TEXT_STATUS, m_sCantConnect);
        return 0;
    }

    string* pJson = (string*)lParam;
    const string& json = *pJson;

    // extract local version from build_name ("FictionBook Editor Release 2.8.0") -> "2.8.0"
    // same N.N.N scan used for remote tag — avoids build_version LTCG link issues
    string localVer;
    for (size_t i = 0; build_name[i]; i++)
    {
        if (!isdigit((unsigned char)build_name[i])) continue;
        int a, b, c;
        if (sscanf(build_name + i, "%d.%d.%d", &a, &b, &c) == 3)
        {
            localVer = build_name + i;
            break;
        }
    }

    // extract "tag_name": "FBE-Portable-2.8.0" -> "2.8.0"
    // handles any prefix (v, FBE-Portable-, etc.) by finding first N.N.N pattern
    string remoteVer;
    size_t pos = json.find("\"tag_name\"");
    if (pos != string::npos)
    {
        pos = json.find('"', pos + 10) + 1;
        size_t end = json.find('"', pos);
        string tag = json.substr(pos, end - pos);
        // scan tag for first position that parses as N.N.N
        for (size_t i = 0; i < tag.size(); i++)
        {
            if (!isdigit((unsigned char)tag[i])) continue;
            int a, b, c;
            if (sscanf(tag.c_str() + i, "%d.%d.%d", &a, &b, &c) == 3)
            {
                remoteVer = tag.substr(i);
                break;
            }
        }
    }

    // extract first browser_download_url (any extension)
    string dlUrl;
    size_t upos = json.find("browser_download_url");
    if (upos != string::npos)
    {
        upos = json.find('"', upos + 20) + 1; // after closing " of field name → ':'
        upos = json.find('"', upos) + 1;       // after opening " of URL value → 'h' in https
        size_t uend = json.find('"', upos);    // closing " of URL
        dlUrl = json.substr(upos, uend - upos);
    }

    delete pJson;

    if (!remoteVer.empty() && !dlUrl.empty())
    {
        int cmp = CompareVersions(remoteVer.c_str(), localVer.c_str());
        // Show remote version under Build stamp
        CString remoteVerW(remoteVer.c_str());
        SetDlgItemText(IDC_LATESTVER, remoteVerW);

        if (cmp > 0)
        {
            m_UpdateReady = true;
            m_UpdateURL = CString(dlUrl.c_str());
            SetDlgItemText(IDC_TEXT_STATUS, m_sNewVersionAvailable);
            m_UpdatePict.SetBitmap(m_StatusBitmaps[1]);
            m_UpdateButton.ShowWindow(SW_SHOW);
            // reposition immediately in case window was resized before update check finished
            BOOL bHandled = FALSE;
            OnSize(WM_SIZE, 0, 0, bHandled);
        }
        else
        {
            SetDlgItemText(IDC_TEXT_STATUS, m_sHaveLatestVersion);
            m_UpdatePict.SetBitmap(m_StatusBitmaps[0]);
        }
    }
    else
    {
        SetDlgItemText(IDC_TEXT_STATUS, m_sCantConnect);
    }
    return 0;
}

LRESULT CAboutDlg::OnUpdate(WORD, WORD wID, HWND, BOOL&)
{
	if (!m_UpdateURL.IsEmpty())
	{
		m_UpdateButton.ShowWindow(SW_HIDE);
		DeleteAllDownload();

		CString filename = GetUpdateFileName();
		if (ATLPath::FileExists(filename))
		{
			if (U::MessageBox(MB_YESNO | MB_ICONEXCLAMATION, IDR_MAINFRAME, IDS_UPDATEEXISTS, filename) == IDYES)
			{
				RunUpdate(filename);
			}
		}

		SetDlgItemText(IDC_TEXT_STATUS, m_sConnecting);
		
		HTTP_SEND_HEADER ht = PrepareHeader(m_UpdateURL);

		// clear stringstream
		m_file.str("");

		m_TotalDownloadSize = 0;
		int   nTaskID = AddDownload(ht);
		m_monitor.reset (new CDownloadMonitor(m_hWnd, nTaskID));
	}
	return 0L;
}

void CAboutDlg::OnAfterDownloadConnected (FCHttpDownload* pTask)
{
    const HTTP_RESPONSE_INFO   & resp = pTask->GetResponseInfo();

    if (resp.m_status_code == 0)
    {
        SetDlgItemText (IDC_TEXT_STATUS, m_sCantConnect);
        DeleteDownload (pTask->GetTaskID());
		m_UpdatePict.SetBitmap(m_StatusBitmaps[2]);
        return;
    }

#ifdef DOWNLOAD_STATISTIC
    // total length
    CString   totalLength(L"Unknow");
    int   nTotal = resp.m_content_length;
    if (nTotal)
    {
        totalLength.Format(L"%d Kb", (int)nTotal/1024.0);
    }
#endif
}

void CAboutDlg::AcceptReceivedData (FCHttpDownload* pTask)
{
    BYTE *p;
    int n;

    pTask->PopReceived(p, n);
    if (p)
    {
		m_file.write ((const char*) p, n);
        delete[] p;
		m_TotalDownloadSize += n;
    }
}

LRESULT CAboutDlg::OnUpdateProgressUI (UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (!m_monitor.get())
        return 0;

    FCHttpDownload *p = FindDownload((int)wParam);
    if (!p)
    {
        m_monitor.reset();
        return 0;
    }

	if (m_AnimIdx >= ANIM_SIZE) m_AnimIdx = 0;
	m_UpdatePict.SetBitmap(m_AnimBitmaps[m_AnimIdx++]);

    AcceptReceivedData(p);

	// show percent only for update download
	if (m_UpdateReady)
	{
		CString currPercent,currSpeed,avgSpeed;
		// download
		int nDownload = p->GetDownloadByte();
		int nTotal = p->GetResponseInfo().m_content_length;
		if (nTotal)
		{
			int   nPercent = (int)(100 * (INT64)nDownload / nTotal);
			currPercent.Format(m_sDownloadedFrom, (int)ceil(nDownload/1024.0), (int)ceil(nTotal/1024.0), nPercent);
		}
		else
		{
			currPercent.Format(m_sDownloaded, (int)ceil(nDownload/1024.0));
		}
		SetDlgItemText (IDC_TEXT_STATUS, currPercent);
	}
	
#ifdef DOWNLOAD_STATISTIC
    // current speed
    currSpeed.Format(L"%d Kb / S", (int)ceil(p->GetCurrentSpeed()/1024.0));
    // average speed
    avgSpeed.Format(L"%d Kb / S", (int)ceil(p->GetAverageSpeed()/1024.0));
#endif
    return 0;
}

bool CAboutDlg::FinishUpdateStatus (FCHttpDownload* pTask)
{
	bool bStatus = false;
    const HTTP_RESPONSE_INFO& resp = pTask->GetResponseInfo();
	int nDownload = m_file.tellp();

    CString s = m_sDownloadError;
    switch (resp.m_status_code)
    {
        case HTTP_STATUS_OK:
        case HTTP_STATUS_PARTIAL_CONTENT:
            if (resp.m_content_length)
                bStatus = (resp.m_content_length == nDownload);
            else
                bStatus = resp.m_final_read_result != 0;

            if (pTask->GetSendHeader().m_start && (resp.m_status_code == HTTP_STATUS_OK))
                s += m_sNotSupportRange;
            break;

        case HTTP_STATUS_NOT_FOUND:
            s = m_sError404;
            break;

        case HTTP_STATUS_FORBIDDEN:
            s = m_sError403;
            break;

        case HTTP_STATUS_PROXY_AUTH_REQ:
            s = m_sError407;
            break;

        default:
            s.Format(m_sDownloadErrorStatus, resp.m_status_code);
            break;
    }

	if (!bStatus)
	{
		m_UpdatePict.SetBitmap(m_StatusBitmaps[2]);
		SetDlgItemText(IDC_TEXT_STATUS, s);
	}
	return bStatus;
}

void CAboutDlg::OnAfterDownloadFinish (FCHttpDownload* pTask)
{
	BOOL b;
    OnUpdateProgressUI(0, (WPARAM)pTask->GetTaskID(), 0, b);

    bool bStatus = FinishUpdateStatus(pTask);

	// FCHttpDownload is only used for downloading the update .exe
	{
		if (bStatus)
		try
		{
			SetDlgItemText(IDC_TEXT_STATUS, m_sDownloadCompleted);
			m_UpdatePict.SetBitmap(m_StatusBitmaps[0]);
			DeleteAllDownload();

			CString filename = GetUpdateFileName();
			ofstream outFile(filename.GetBuffer(), ios::out | ios::binary);
			outFile << m_file.str();
			outFile.close();
			SetDlgItemText(IDC_TEXT_STATUS, m_sDownloadReady);

			RunUpdate(filename);
		}
		catch (...) {}
	}
}

HTTP_SEND_HEADER CAboutDlg::PrepareHeader(const CString url)
{
	HTTP_SEND_HEADER ht;
    ht.m_url = url;
    ht.m_user_agent = L"Mozilla/4.0";
    ht.m_start = 0;
    ht.m_header = L"";
/*  ht.m_proxy_ip = DlgSetProxy::s_task.m_proxy_ip;
    ht.m_proxy_port = DlgSetProxy::s_task.m_proxy_port;
    ht.m_proxy_username = DlgSetProxy::s_task.m_proxy_username;
    ht.m_proxy_password = DlgSetProxy::s_task.m_proxy_password; */
	return ht;
}

LRESULT CAboutDlg::OnGetMinMaxInfo(UINT, WPARAM, LPARAM lParam, BOOL&)
{
	if (!m_bAllowResize && m_InitialRect.right > 0)
	{
		LPMINMAXINFO pMMI = (LPMINMAXINFO)lParam;
		pMMI->ptMinTrackSize.x = m_InitialRect.right - m_InitialRect.left;
		pMMI->ptMinTrackSize.y = m_InitialRect.bottom - m_InitialRect.top;
	}
	return TRUE;
}

LRESULT CAboutDlg::OnSize(UINT, WPARAM, LPARAM, BOOL&)
{
	if (m_bAllowResize)
	{
		RECT rect;
		GetClientRect(&rect);
		m_glLogo.SetWindowPos(0, &rect, 0);
		return FALSE;
	}

	// capture initial layout on first WM_SIZE (window fully shown, all controls visible)
	if (m_InitialRect.right == 0)
	{
		GetWindowRect(&m_InitialRect);
		RECT initClient;
		GetClientRect(&initClient);
		m_initCW = initClient.right;
		m_initCH = initClient.bottom;

		auto getRC = [&](int id, RECT& rc) {
			::GetWindowRect(GetDlgItem(id), &rc);
			ScreenToClient(&rc);
		};
		getRC(IDOK,            m_rcOK);
		getRC(IDC_UPDATE,      m_rcUpdate);
		getRC(IDC_CONTRIBS,    m_rcContribs);
		getRC(IDC_TEXT_STATUS, m_rcStatus);
		getRC(IDC_PIC_UPDATE,  m_rcPic);
		getRC(IDC_STATIC_AB_APPNAMEVER, m_rcAppName);
		getRC(IDC_BUILDSTAMP, m_rcBuildStamp);
		getRC(IDC_LATESTVER, m_rcLatestVer);
		getRC(IDC_SYSLINK_AB_LINKS, m_rcLinks);
	}

	// normal mode: stretch contributors text, anchor buttons to bottom-right
	RECT client;
	GetClientRect(&client);
	int cw = client.right;
	int ch = client.bottom;

	// Gaps derived from initial template layout — pixel-perfect at original size
	int rightGap     = m_initCW - m_rcOK.right;              // OK right → client right
	int bottomGap    = m_initCH - m_rcOK.bottom;             // OK bottom → client bottom
	int okUpdGap     = m_rcOK.left - m_rcUpdate.right;       // Update right → OK left
	int statUpdGap   = m_rcUpdate.left - m_rcStatus.right;   // Status right → Update left
	int contribBtnGap = m_rcOK.top - m_rcContribs.bottom;   // Contributors bottom → buttons top

	int btnW  = m_rcOK.right      - m_rcOK.left;
	int btnH  = m_rcOK.bottom     - m_rcOK.top;
	int updW  = m_rcUpdate.right  - m_rcUpdate.left;
	int statH = m_rcStatus.bottom - m_rcStatus.top;

	// OK — anchored to bottom-right
	int okX = cw - rightGap - btnW;
	int okY = ch - bottomGap - btnH;
	::SetWindowPos(GetDlgItem(IDOK), NULL, okX, okY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Update — to the left of OK, always repositioned (so it doesn't ghost)
	int updX = okX - okUpdGap - updW;
	::SetWindowPos(GetDlgItem(IDC_UPDATE), NULL, updX, okY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Contributors — stretch width and height
	::SetWindowPos(GetDlgItem(IDC_CONTRIBS), NULL, 0, 0,
		cw - m_rcContribs.left - rightGap,
		okY - contribBtnGap - m_rcContribs.top,
		SWP_NOMOVE | SWP_NOZORDER);

	// Status text — right edge: just left of Update (if visible) or just left of OK (if hidden)
	// statUpdGap may be ≤0 in template (status and Update overlap; only one shown at a time)
	bool updateVisible = GetDlgItem(IDC_UPDATE).IsWindowVisible() != FALSE;
	int statusRightEdge = updateVisible ? (updX - statUpdGap) : (okX - okUpdGap);
	int statusW = statusRightEdge - m_rcStatus.left;
	int statusY = okY + (btnH - statH) / 2;
	::SetWindowPos(GetDlgItem(IDC_TEXT_STATUS), NULL,
		m_rcStatus.left, statusY, statusW, statH, SWP_NOZORDER);
	::SetWindowPos(GetDlgItem(IDC_PIC_UPDATE), NULL,
		m_rcPic.left, statusY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Top-right text controls — stretch width, keep position
	auto stretchW = [&](int id, const RECT& rc) {
		int rGap = m_initCW - rc.right;
		::SetWindowPos(GetDlgItem(id), NULL, 0, 0,
			cw - rc.left - rGap, rc.bottom - rc.top,
			SWP_NOMOVE | SWP_NOZORDER);
	};
	stretchW(IDC_STATIC_AB_APPNAMEVER, m_rcAppName);
	stretchW(IDC_BUILDSTAMP, m_rcBuildStamp);
	stretchW(IDC_LATESTVER, m_rcLatestVer);
	stretchW(IDC_SYSLINK_AB_LINKS, m_rcLinks);

	// force full repaint so hidden controls don't leave ghost artifacts
	InvalidateRect(NULL, TRUE);

	return FALSE;
}

LRESULT CAboutDlg::OnResizeOpenGLWindow(UINT, WPARAM, LPARAM, BOOL&)
{
	CButton btn = GetDlgItem(IDOK);
	// switch glLogo to full client area
	if (btn.IsWindowVisible())
	{
		// hide controls
		btn.ShowWindow(SW_HIDE);
		btn.EnableWindow(FALSE);

		m_SaveBtnState = GetDlgItem(IDC_UPDATE).IsWindowVisible();
		if (m_SaveBtnState)
		{
			GetDlgItem(IDC_UPDATE).ShowWindow(SW_HIDE);
			GetDlgItem(IDC_UPDATE).EnableWindow(FALSE);
		}

		GetDlgItem(IDC_PIC_UPDATE).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TEXT_STATUS).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CONTRIBS).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_AB_APPICON).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_AB_APPNAMEVER).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUILDSTAMP).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SYSLINK_AB_LINKS).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_BUILD).ShowWindow(SW_HIDE);

		// save dialog size & position
		GetWindowRect(&m_SaveRect);

		// save control size & position
		m_glLogo.GetWindowRect(&m_LogoRect);
		ScreenToClient(&m_LogoRect);

		RECT rect;
		GetClientRect(&rect);
		m_glLogo.SetWindowPos(0, &rect, 0);
		m_glLogo.SetFocus();

		GetWindowText (m_AboutCaption);
		SetWindowText (m_sLogoCaption);
		ModifyStyle(0, WS_MAXIMIZEBOX, SWP_FRAMECHANGED);
		UpdateWindow();

		m_bAllowResize = true;
	}
	else
	{
		// restore dialog size and position
		SetWindowPos(0, &m_SaveRect, 0);

		m_glLogo.SetWindowPos(0, &m_LogoRect, 0);

		btn.ShowWindow(SW_SHOW);
		btn.EnableWindow(TRUE);

		if (m_SaveBtnState)
		{
			GetDlgItem(IDC_UPDATE).ShowWindow(SW_SHOW);
			GetDlgItem(IDC_UPDATE).EnableWindow(TRUE);
		}

		GetDlgItem(IDC_PIC_UPDATE).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TEXT_STATUS).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CONTRIBS).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_AB_APPICON).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_AB_APPNAMEVER).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUILDSTAMP).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SYSLINK_AB_LINKS).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_BUILD).ShowWindow(SW_SHOW);

		SetWindowText (m_AboutCaption);
		ModifyStyle(WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);

		m_bAllowResize = false;
	}
	return TRUE;
}

CString CAboutDlg::GetUpdateFileName()
{
	wchar_t tempDir[1024];
	GetTempPath(sizeof(tempDir)/sizeof(TCHAR), &tempDir[0]);
	CString filename (ATLPath::FindFileName(m_UpdateURL));
	return tempDir+U::URLDecode(filename);
}

void CAboutDlg::RunUpdate(CString filename)
{
	if (U::MessageBox(MB_YESNO | MB_ICONEXCLAMATION, IDR_MAINFRAME, IDS_UPDATE_CLOSE, filename) == IDYES)
	{
		wchar_t appPath[MAX_PATH];
		GetModuleFileName(NULL, appPath, MAX_PATH);
		CString appExe(appPath);
		CString appDir(appPath);
		int slash = appDir.ReverseFind(L'\\');
		if (slash >= 0) appDir = appDir.Left(slash);

		// Detect format by extension
		int dot = filename.ReverseFind(L'.');
		CString ext = (dot >= 0) ? filename.Mid(dot).MakeLower() : L"";

		if (ext == L".zip")
		{
			// Extract via PowerShell: wait for app to exit, expand, relaunch
			wchar_t tempDir[MAX_PATH];
			GetTempPath(MAX_PATH, tempDir);
			CString ps1(tempDir);
			ps1 += L"fbe_update.ps1";

			CString script;
			script.Format(
				// L"Start-Sleep -Milliseconds 500\r\n"
				L"Write-Host 'Extracting update...'\r\n"
				L"Add-Type -AssemblyName System.IO.Compression.FileSystem\r\n"
				L"$tmp = Join-Path $env:TEMP 'fbe_update_extract'\r\n"
				L"Remove-Item -Path $tmp -Recurse -Force -ErrorAction SilentlyContinue\r\n"
				L"[System.IO.Compression.ZipFile]::ExtractToDirectory('%s', $tmp)\r\n"
				L"$items = Get-ChildItem $tmp\r\n"
				L"if ($items.Count -eq 1 -and $items[0].PSIsContainer) { $src = $items[0].FullName } else { $src = $tmp }\r\n"
				L"Write-Host 'Installing...'\r\n"
				L"Copy-Item -Path \"$src\\*\" -Destination '%s' -Recurse -Force\r\n"
				L"Remove-Item -Path $tmp -Recurse -Force -ErrorAction SilentlyContinue\r\n"
				L"Remove-Item -LiteralPath '%s' -Force -ErrorAction SilentlyContinue\r\n"
				L"Write-Host 'Done. Starting FBE...'\r\n"
				// L"Start-Sleep -Milliseconds 500\r\n"
				L"Start-Process '%s'\r\n",
				(LPCWSTR)filename,
				(LPCWSTR)appDir,
				(LPCWSTR)filename,
				(LPCWSTR)appExe);

			HANDLE hf = CreateFile(ps1, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hf != INVALID_HANDLE_VALUE)
			{
				BYTE bom[] = { 0xFF, 0xFE };
				DWORD dw;
				WriteFile(hf, bom, 2, &dw, NULL);
				WriteFile(hf, (LPCWSTR)script, script.GetLength() * sizeof(wchar_t), &dw, NULL);
				CloseHandle(hf);
			}

			CString args;
			args.Format(L"-ExecutionPolicy Bypass -File \"%s\"", (LPCWSTR)ps1);
			ShellExecute(0, L"open", L"powershell.exe", args, NULL, SW_SHOWDEFAULT);
		}
		else
		{
			// .exe or other: run directly (self-extracting or installer)
			ShellExecute(0, L"open", filename, NULL, NULL, SW_SHOW);
		}

		::PostMessage(GetParent().m_hWnd, WM_CLOSE, 0, 0);
	}
}
