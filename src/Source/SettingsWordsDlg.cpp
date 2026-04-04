// SettingsWordsDlg.cpp : Implementation of CSettingsWordsDlg

#include "stdafx.h"
#include "SettingsWordsDlg.h"
#include "Settings.h"

extern CSettings _Settings;

static int compare_counted_asc(const void* v1, const void* v2)
{
	const WordsItem* w1 = (const WordsItem*)v1;
	const WordsItem* w2 = (const WordsItem*)v2;

	int cmp = w1->m_count - w2->m_count;
	return cmp ? cmp : w1->m_word.CompareNoCase(w2->m_word);
}

static int compare_counted_desc(const void* v1, const void* v2)
{
	const WordsItem* w1 = (const WordsItem*)v1;
	const WordsItem* w2 = (const WordsItem*)v2;

	int cmp = w2->m_count - w1->m_count;
	return cmp ? cmp : w2->m_word.CompareNoCase(w1->m_word);
}

static int compare_word_asc(const void* v1, const void* v2)
{
	const WordsItem* w1 = (const WordsItem*)v1;
	const WordsItem* w2 = (const WordsItem*)v2;
	return w1->m_word.CompareNoCase(w2->m_word);
}

static int compare_word_desc(const void* v1, const void* v2)
{
	const WordsItem* w1 = (const WordsItem*)v1;
	const WordsItem* w2 = (const WordsItem*)v2;
	return w2->m_word.CompareNoCase(w1->m_word);
}

static int (*g_compare_funcs[])(const void*, const void*) =
{
	compare_counted_asc,
	compare_counted_desc,
	compare_word_asc,
	compare_word_desc
};

// CSettingsWordsDlg
CSettingsWordsDlg::CSettingsWordsDlg() : m_sort(0), m_sel_all(false), m_ct(0)
{
	unsigned int size = _Settings.m_words.size();
	for(unsigned int i = 0; i < size; ++i)
	{
		m_words.Add(_Settings.m_words[i]);
	}
}

void CSettingsWordsDlg::RefreshWordsList()
{
	if (m_list_words.m_hWnd == NULL)
		return;

	ListView_SetItemCountEx(m_list_words.m_hWnd, m_words.GetSize(), LVSICF_NOINVALIDATEALL);
	m_list_words.RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void CSettingsWordsDlg::ResortWordsList()
{
	if (m_words.GetSize() <= 1)
		return;

	if (m_sort != 0)
	{
		qsort(m_words.GetData(), m_words.GetSize(), sizeof(WordsItem),
		g_compare_funcs[abs(m_sort) * 2 - (m_sort < 0 ? 1 : 2)]);
	}
	else
	{
		// Сохраняем текущее поведение окна:
		// при начальной загрузке список сортируется по counted desc.
		qsort(m_words.GetData(), m_words.GetSize(), sizeof(WordsItem), g_compare_funcs[1]);
	}
}

int CSettingsWordsDlg::FindWordIndexNoCase(const CString& word) const
{
	for (int i = 0; i < m_words.GetSize(); ++i)
	{
		if (m_words[i].m_word.CompareNoCase(word) == 0)
			return i;
	}

	return -1;
}

bool CSettingsWordsDlg::AreAllCountsZero() const
{
	for (int i = 0; i < m_words.GetSize(); ++i)
	{
		if (m_words[i].m_count != 0)
			return false;
	}

	return true;
}

void CSettingsWordsDlg::ApplyPreferredSort()
{
	if (m_chk_disable_words_stats.GetCheck() != 0)
	{
		m_sort = 2; // вторая колонка, прямой порядок
		qsort(m_words.GetData(), m_words.GetSize(), sizeof(WordsItem), g_compare_funcs[2]);
	}
	else if (AreAllCountsZero())
	{
		m_sort = 1; // первая колонка, прямой порядок
		qsort(m_words.GetData(), m_words.GetSize(), sizeof(WordsItem), g_compare_funcs[0]);
	}
	else
	{
		m_sort = -1; // первая колонка, обратный порядок
		qsort(m_words.GetData(), m_words.GetSize(), sizeof(WordsItem), g_compare_funcs[1]);
	}
}

void CSettingsWordsDlg::SelectWord(int index)
{
	if (m_list_words.m_hWnd == NULL)
		return;

	if (index < 0 || index >= m_words.GetSize())
		return;

	ListView_SetItemState(m_list_words.m_hWnd, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	ListView_SetItemState(m_list_words.m_hWnd, index,
		LVIS_SELECTED | LVIS_FOCUSED,
		LVIS_SELECTED | LVIS_FOCUSED);

	m_list_words.EnsureVisible(index, FALSE);
}

LRESULT CSettingsWordsDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_list_words.SubclassWindow(GetDlgItem(IDC_LIST_WORDS));
	m_list_words.ModifyStyle(LVS_SORTASCENDING | LVS_SORTDESCENDING, 0);
	m_list_words.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	RECT rc;
	m_list_words.GetClientRect(&rc);

	const int countedWidth = 100;
	int wordWidth = (rc.right - rc.left) - countedWidth - ::GetSystemMetrics(SM_CXVSCROLL);
	if (wordWidth < 80)
		wordWidth = 80;

	CString header;

	header.LoadString(IDS_SETTINGS_WLIST_COUNTED);
	m_list_words.InsertColumn(0, header, LVCFMT_LEFT, countedWidth);

	header.LoadString(IDS_SETTINGS_WLIST_WORD);
	m_list_words.InsertColumn(1, header, LVCFMT_LEFT, wordWidth);
	
	ListView_SetItemCountEx(m_list_words.m_hWnd, m_words.GetSize(), LVSICF_NOINVALIDATEALL);

	m_edt_new = GetDlgItem(IDC_EDIT_NEW);
	m_btn_add = GetDlgItem(IDC_BUTTON_ADD);
	m_chk_all = GetDlgItem(IDC_CHECK_SELALL);
	m_chk_disable_words_stats = GetDlgItem(IDC_CHECK_DISABLE_WORDS_STATS);
	m_btn_reset_words_stats = GetDlgItem(IDC_BUTTON_RESET_WORDS_STATS);

	m_edit = GetDlgItem(IDC_EDIT_LV);
	m_show_words_excls = GetDlgItem(IDC_CHECK_SHOW_EXCLUSIONS);
	m_show_words_excls.SetCheck(_Settings.GetShowWordsExcls());
	m_chk_disable_words_stats.SetCheck(_Settings.GetDisableWordsStats());

	ApplyPreferredSort();

	return 0;
}

LRESULT CSettingsWordsDlg::OnListDispInfo(int id, NMHDR *hdr, BOOL&)
{
	NMLVDISPINFO *ni = (NMLVDISPINFO*)hdr;

	if (ni->item.iItem < 0 || ni->item.iItem >= m_words.GetSize())
		return 0;

	WordsItem *w = &m_words[ni->item.iItem];

	if ((ni->item.mask & LVIF_TEXT) && ni->item.pszText != NULL && ni->item.cchTextMax > 0)
	{
		switch (ni->item.iSubItem)
		{
		case 0:
		{
			CString sCount;
			sCount.Format(L"%i", w->m_count);
			_tcsncpy(ni->item.pszText, sCount, ni->item.cchTextMax - 1);
			ni->item.pszText[ni->item.cchTextMax - 1] = 0;
			break;
		}

		case 1:
			_tcsncpy(ni->item.pszText, w->m_word, ni->item.cchTextMax - 1);
			ni->item.pszText[ni->item.cchTextMax - 1] = 0;
			break;

		default:
			ni->item.pszText[0] = 0;
			break;
		}
	}

	return 0;
}

LRESULT CSettingsWordsDlg::OnListSort(int id, NMHDR *hdr, BOOL&)
{
	NMLISTVIEW* lv = (NMLISTVIEW*)hdr;

	if (lv->iSubItem + 1 == abs(m_sort))
		m_sort = -m_sort;
	else
		m_sort = lv->iSubItem + 1;

	int funcIndex = abs(m_sort) * 2 - (m_sort < 0 ? 1 : 2);

	qsort(m_words.GetData(), m_words.GetSize(), sizeof(WordsItem), g_compare_funcs[funcIndex]);

	m_list_words.InvalidateRect(NULL);

	return 0;
}

LRESULT CSettingsWordsDlg::OnListClick(int id, NMHDR *hdr, BOOL&)
{
	NMITEMACTIVATE *ai = (NMITEMACTIVATE*) hdr;

	if ((::GetTickCount() - m_ct) < 500 || ai->iItem < 0 || ai->iSubItem != 1)
	{
		m_edit.ShowWindow(SW_HIDE);
		return 0;
	}

	m_editidx = ai->iItem;

	WordsItem* w = &m_words[m_editidx];

	m_edit.SetWindowText(w->m_word);

	RECT rci;
	m_list_words.GetSubItemRect(m_editidx, 1, LVIR_BOUNDS, &rci);
	m_list_words.ClientToScreen(&rci);
	ScreenToClient(&rci);
	m_edit.SetWindowPos(NULL, rci.left, rci.top, rci.right - rci.left, rci.bottom - rci.top + 5, SWP_SHOWWINDOW | SWP_NOACTIVATE);
	m_edit.SetSel(w->m_word.GetLength(), w->m_word.GetLength());
	m_edit.SetFocus();

	return 0;
}

LRESULT CSettingsWordsDlg::OnEditLVDefocused(int id, NMHDR *hdr, BOOL&)
{	
	m_edit.ShowWindow(SW_HIDE);

	return 0;
}

LRESULT CSettingsWordsDlg::OnListChanged(int id, NMHDR *hdr, BOOL&)
{
	m_ct = ::GetTickCount();

	return 0;
}

LRESULT CSettingsWordsDlg::OnBnClickedButtonAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString newWord;
	m_edt_new.GetWindowText(newWord);

	if(AddNewWord(newWord))
		m_edt_new.SetWindowText(L"");
		
	return 0;
}

LRESULT CSettingsWordsDlg::OnBnClickedButtonResetWordsStats(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	for (int i = 0; i < m_words.GetSize(); ++i)
	{
		m_words[i].m_count = 0;
	}

	ApplyPreferredSort();
	RefreshWordsList();

	return 0;
}

LRESULT CSettingsWordsDlg::OnBnClickedCheckDisableWordsStats(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ApplyPreferredSort();
	RefreshWordsList();

	return 0;
}

bool CSettingsWordsDlg::AddNewWord(CString& word, bool test)
{
	word.Trim();
	bool ambigous = word.IsEmpty();
	int symbol = 0;

	int hyphens = 0;

	while (symbol != word.GetLength())
	{
		if (!iswalpha(word[symbol]) && word[symbol] != L'-')
		{
			ambigous = true;
			break;
		}

		if (word[symbol] == L'-')
		{
			if ((symbol == 0 && word.GetLength() == 1) || symbol == word.GetLength() - 1 || hyphens > 1)
			{
				ambigous = true;
				break;
			}

			hyphens++;
		}

		symbol++;
	}

	if (hyphens != 1)
		ambigous = true;

	if (!ambigous)
	{
		unsigned int size = m_words.GetSize();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (word.CompareNoCase(m_words[i].m_word) == 0)
			{
				CString errMsg[2];
				errMsg[0].LoadString(IDS_SETTINGS_WORDS_ADD_ERR_TEXT);
				errMsg[1].LoadString(IDS_SETTINGS_WORDS_ADD_ERR_CAP);
				MessageBox(errMsg[0], errMsg[1], MB_OK | MB_ICONERROR);

				return false;
			}
		}

		if (!test)
		{
			word.MakeLower();

			WordsItem wi(word, 0);

			m_words.Add(wi);
			ResortWordsList();
			RefreshWordsList();

			int newIndex = FindWordIndexNoCase(word);
			if (newIndex != -1)
				SelectWord(newIndex);
		}

		return true;
	}
	else
	{
		CString errMsg[2];
		errMsg[0].LoadString(IDS_SETTINGS_WORDS_ADD_ERR_SYM);
		errMsg[1].LoadString(IDS_SETTINGS_WORDS_ADD_ERR_CAP);
		MessageBox(errMsg[0], errMsg[1], MB_OK | MB_ICONERROR);

		return false;
	}
}

LRESULT CSettingsWordsDlg::OnCustomDraw(int id, NMHDR *hdr, BOOL&)
{
	if(hdr->hwndFrom == m_list_words.GetHeader())
	{
		NMCUSTOMDRAW *cd =(NMCUSTOMDRAW*)hdr;

		switch(cd->dwDrawStage)
		{
		  case CDDS_PREPAINT:
			  return CDRF_NOTIFYITEMDRAW;
		  case CDDS_ITEMPREPAINT:
			  return CDRF_NOTIFYPOSTPAINT;
		  case CDDS_ITEMPOSTPAINT:
			  // paint sort indicator on top of the item
			  if (cd->dwItemSpec + 1 == (unsigned)abs(m_sort))
			  {
				  HGDIOBJ old=::SelectObject(cd->hdc,::GetSysColorBrush(COLOR_BTNTEXT));
				  int	h=cd->rc.bottom-cd->rc.top;
				  int	ah=h/4;
				  if (ah<5)
					  ah=5;
				  if (ah>20)
					  ah=20;
				  enum { off=5 };
				  POINT	  pt[3];
				  pt[0].x=cd->rc.right-off-ah;
				  pt[0].y=cd->rc.top+(h-ah)/2;
				  pt[1].x=cd->rc.right-off;
				  pt[1].y=pt[0].y+ah;
				  pt[2].x=cd->rc.right-off-ah*2;
				  pt[2].y=pt[1].y;
				  if (m_sort<0) {
					  pt[0].y=cd->rc.bottom+cd->rc.top-pt[0].y;
					  pt[1].y=cd->rc.bottom+cd->rc.top-pt[1].y;
					  pt[2].y=cd->rc.bottom+cd->rc.top-pt[2].y;
				  }
				  ::Polygon(cd->hdc,pt, 3);
				  ::SelectObject(cd->hdc, old);
			  }
			  return 0;
		}
	}
	return 0;
}

LRESULT CSettingsWordsDlg::OnBnClickedCheckSelall(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!m_list_words.GetItemCount())
	{
		m_chk_all.SetCheck(m_sel_all = false);
		return 0;
	}

	if (m_sel_all)
	{
		ListView_SetItemState(m_list_words.m_hWnd, -1, 0, LVIS_SELECTED);
		m_sel_all = false;
		m_chk_all.SetCheck(BST_UNCHECKED);
	}
	else
	{
		ListView_SetItemState(m_list_words.m_hWnd, -1, LVIS_SELECTED, LVIS_SELECTED);
		m_sel_all = true;
		m_chk_all.SetCheck(BST_CHECKED);
		::SetFocus(m_list_words);
	}

	m_list_words.RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);

	return 0;
}

LRESULT CSettingsWordsDlg::OnBnClickedButtonRemovesel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSimpleArray<int> selected;

	for (int i = 0; i < m_list_words.GetItemCount(); ++i)
	{
		if (m_list_words.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			selected.Add(i);
	}

	if (!selected.GetSize())
		return 0;

	// Удаляем снизу вверх, чтобы индексы не съезжали
	for (int i = selected.GetSize() - 1; i >= 0; --i)
	{
		RemoveWord(selected[i]);
	}

	m_sel_all = false;
	m_chk_all.SetCheck(BST_UNCHECKED);

	RefreshWordsList();

	if (m_words.GetSize() > 0)
	{
		int newSel = selected[0];
		if (newSel >= m_words.GetSize())
			newSel = m_words.GetSize() - 1;

		SelectWord(newSel);
	}

	return 0;
}

void CSettingsWordsDlg::RemoveWord(int index)
{
	if (index < 0 || index >= m_words.GetSize())
		return;

	m_words.RemoveAt(index);
}

LRESULT CSettingsWordsDlg::OnOK(WORD, WORD wID, HWND, BOOL&)
{
	if(GetFocus() == m_edit)
	{
		CString edWord = U::GetWindowText(m_edit);

		if(m_words[m_editidx].m_word != edWord.Trim())
		{
			if(AddNewWord(edWord, true))
			{
				RemoveWord(m_editidx);
				AddNewWord(edWord);
			}
		}

		m_edit.ShowWindow(SW_HIDE);
	}
	else if(GetFocus() == m_edt_new)
	{
		SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_ADD, BN_CLICKED), (LPARAM)m_btn_add.m_hWnd);
	}
	else
	{
		_Settings.SetShowWordsExcls(m_show_words_excls.GetCheck() != 0);
		_Settings.SetDisableWordsStats(m_chk_disable_words_stats.GetCheck() != 0);

		_Settings.m_words.clear();
		int n = m_words.GetSize();
		for(int i = 0; i < n; ++i)
			_Settings.m_words.push_back(m_words[i]);

		_Settings.SaveWords();
	}

	return 0;
}

LRESULT CSettingsWordsDlg::OnCancel(WORD, WORD wID, HWND, BOOL&)
{
	if(GetFocus() == m_edit)
	{
		m_edit.ShowWindow(SW_HIDE);
		return 0;
	}

	return 1;
}