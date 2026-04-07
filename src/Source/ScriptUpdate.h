#pragma once
#include "stdafx.h"
#include "resource.h"
#include "AboutBox.h"   // CTransparentBitmap, ANIM_SIZE, IDB_UPD_*
#include <string>

#define WM_SCRIPTUPDATE_LOG   (WM_USER + 110)
#define WM_SCRIPTUPDATE_DONE  (WM_USER + 111)

class CScriptUpdateDlg : public CDialogImpl<CScriptUpdateDlg>
{
public:
    enum { IDD = IDD_SCRIPTUPDATE };

    BEGIN_MSG_MAP(CScriptUpdateDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_CLOSE,              OnWindowClose)
        MESSAGE_HANDLER(WM_TIMER, OnAnimTimer)
        MESSAGE_HANDLER(WM_SCRIPTUPDATE_LOG,   OnLogMsg)
        MESSAGE_HANDLER(WM_SCRIPTUPDATE_DONE,  OnDoneMsg)
        COMMAND_ID_HANDLER(IDOK,     OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancelCmd)
    END_MSG_MAP()

    volatile bool m_bCancel;
    bool          m_bCloseRequested;

private:
    HANDLE m_hThread;
    CTransparentBitmap m_Pict;
    CEdit m_result;
    CBitmap m_AnimBitmaps[ANIM_SIZE];
    CBitmap m_StatusBitmaps[3];
    int m_AnimIdx;

    struct ThreadParam {
        HWND           hwnd;
        volatile bool* pCancel;
    };

    static DWORD WINAPI ThreadProc(LPVOID pParam);

    LRESULT OnInitDialog  (UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnWindowClose (UINT, WPARAM, LPARAM, BOOL& bHandled);
    LRESULT OnAnimTimer   (UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnLogMsg      (UINT, WPARAM, LPARAM lParam, BOOL&);
    LRESULT OnDoneMsg     (UINT, WPARAM wParam, LPARAM lParam, BOOL&);
    LRESULT OnCloseCmd    (WORD, WORD, HWND, BOOL&);
    LRESULT OnCancelCmd   (WORD, WORD, HWND, BOOL&);
};
