// SettingsWordsDlg.h : Declaration of the CSettingsWordsDlg
#pragma once

#include "resource.h"

class WordsItem;

class CWordsVirtualListCtrl : public CWindowImpl<CWordsVirtualListCtrl, CListViewCtrl, CControlWinTraits>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CWordsVirtualListCtrl)
		MESSAGE_HANDLER(WM_VSCROLL, OnAfterScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnAfterScroll)
	END_MSG_MAP()

	LRESULT OnAfterScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lr = DefWindowProc(uMsg, wParam, lParam);

		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);

		return lr;
	}
};

// CSettingsWordsDlg
class CSettingsWordsDlg : public CDialogImpl<CSettingsWordsDlg>
{
public:
	CWordsVirtualListCtrl	m_list_words;
	CButton					m_btn_add;
	CEdit					m_edt_new;
	CEdit					m_edit;
	CButton					m_chk_all;
	CButton					m_show_words_excls;
	CButton					m_chk_disable_words_stats;
	CButton					m_btn_reset_words_stats;

	int m_sort;
	bool m_sel_all;
	DWORD m_ct;
	int m_editidx;

	CSimpleArray<WordsItem> m_words;

	CSettingsWordsDlg();

	enum { IDD = IDD_SETTINGS_WORDS };

	BEGIN_MSG_MAP(CSettingsWordsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_LIST_WORDS, LVN_GETDISPINFO, OnListDispInfo)
		NOTIFY_HANDLER(IDC_LIST_WORDS, LVN_COLUMNCLICK, OnListSort)
		COMMAND_HANDLER(IDC_BUTTON_ADD, BN_CLICKED, OnBnClickedButtonAdd)
		COMMAND_HANDLER(IDC_BUTTON_RESET_WORDS_STATS, BN_CLICKED, OnBnClickedButtonResetWordsStats)
		COMMAND_HANDLER(IDC_CHECK_DISABLE_WORDS_STATS, BN_CLICKED, OnBnClickedCheckDisableWordsStats)
		NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnCustomDraw)
		COMMAND_HANDLER(IDC_CHECK_SELALL, BN_CLICKED, OnBnClickedCheckSelall)
		COMMAND_HANDLER(IDC_BUTTON_REMOVESEL, BN_CLICKED, OnBnClickedButtonRemovesel)
		NOTIFY_HANDLER(IDC_LIST_WORDS, NM_CLICK, OnListClick)
		NOTIFY_HANDLER(IDC_LIST_WORDS, LVN_ITEMCHANGED, OnListChanged)
		NOTIFY_HANDLER(IDC_EDIT_LV, EN_KILLFOCUS, OnEditLVDefocused)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnListDispInfo(int, NMHDR*, BOOL&);
	LRESULT OnListSort(int, NMHDR*, BOOL&);
	LRESULT OnListClick(int, NMHDR*, BOOL&);
	LRESULT OnListChanged(int, NMHDR*, BOOL&);
	LRESULT OnEditLVDefocused(int, NMHDR*, BOOL&);
	LRESULT OnCustomDraw(int id,NMHDR *hdr,BOOL&);
	LRESULT OnOK(WORD, WORD wID, HWND, BOOL&);
	LRESULT OnCancel(WORD, WORD wID, HWND, BOOL&);

	bool AddNewWord(CString&, bool test = false);
	void RemoveWord(int);

	void RefreshWordsList();
	void ResortWordsList();
	int FindWordIndexNoCase(const CString&) const;
	void SelectWord(int index);
	bool AreAllCountsZero() const;
	void ApplyPreferredSort();

	LRESULT OnBnClickedButtonAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonResetWordsStats(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckDisableWordsStats(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckSelall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonRemovesel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
