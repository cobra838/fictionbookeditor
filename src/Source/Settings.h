#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include "resource.h"
#include "res1.h"
#include "utils.h"
#include "XMLSerializer\Serializable.h"

/*const int ILANG_ENGLISH = 0;
const int ILANG_RUSSIAN = 1;*/

class WordsItem : public ISerializable, public IObjectFactory
{
public:
	CString	m_word;

	int		m_count;

	WordsItem()
		: m_word()
		, m_count(0)
	{
	}

	WordsItem(const CString& word, int count)
		: m_word(word)
		, m_count(count)
	{
	}

	int GetProperties(std::vector<CString>& properties)
	{
		properties.push_back(L"Value");
		properties.push_back(L"Counted");
		return properties.size();
	}

	bool GetPropertyValue(const CString& sProperty, CProperty& property)
	{
		if(sProperty == L"Value")
		{
			property = m_word;
			return true;
		}
		else if(sProperty == L"Counted")
		{
			CString counted;
			counted.Format(L"%d", m_count);
			property = counted;
			return true;
		}
		return false;
	}

	bool SetPropertyValue(const CString& sProperty, CProperty& property)
	{
		if(sProperty == L"Value")
		{
			m_word = property;
			return true;
		}
		else if(sProperty == L"Counted")
		{
			m_count = StrToInt(property.GetStringValue());
			return true;
		}
		return false;
	}

	bool HasMultipleInstances()
	{
		return true;
	}

	CString GetClassName()
	{
		return L"Word";
	}


	CString GetID()
	{
		return L"";
	}

	ISerializable* Create()
	{
		return new WordsItem;
	}

	void Destroy(ISerializable* obj)
	{
		delete obj;
	}

	bool operator==(const WordsItem& wi)
	{
		return m_word.CompareNoCase(wi.m_word) == 0;
	}
};

class CHotkey : public ISerializable, public IObjectFactory
{
public:
	CString m_name;
	CString m_reg_name;
	ACCEL m_accel;
	ACCEL m_def_accel;
	CString m_desc;
	wchar_t m_char_val; // value for symbol hotkey

	CHotkey() {}

	CHotkey(CString reg_name, int IDS_CMD_NAME, WORD fVirt, WORD cmd, WORD key, CString descr = L"")
	{
		m_reg_name = reg_name;
		m_name.LoadString(IDS_CMD_NAME);

		m_def_accel.fVirt = FVIRTKEY | fVirt;
		m_def_accel.cmd = cmd;
		m_def_accel.key = key;

		m_accel = m_def_accel;

		m_desc = descr;
	}

	CHotkey(CString reg_name, int IDS_CMD_NAME, CString uchar,  WORD fVirt, WORD cmd, WORD key, CString descr = L"")
	{
		m_reg_name = reg_name;
		m_name.LoadString(IDS_CMD_NAME);
		m_name += uchar;

		m_def_accel.fVirt = FVIRTKEY | fVirt;
		m_def_accel.cmd = cmd;
		m_def_accel.key = key;

		m_accel = m_def_accel;

		m_desc = descr;
	}

	CHotkey(CString reg_name, CString name, wchar_t symbol,  WORD fVirt, WORD cmd, WORD key, CString descr = L"")
	{
		m_reg_name = reg_name;
		m_name = name;

		m_char_val = symbol;

		m_def_accel.fVirt = FVIRTKEY | fVirt;
		m_def_accel.cmd = cmd;
		m_def_accel.key = key;

		m_accel = m_def_accel;

		m_desc = descr;
	}

	CHotkey(CString reg_name, CString cmd_name, WORD fVirt, WORD cmd, WORD key, CString descr = L"")
	{
		m_reg_name = reg_name;
		m_name = cmd_name;

		m_def_accel.fVirt = FVIRTKEY | fVirt;
		m_def_accel.cmd = cmd;
		m_def_accel.key = key;

		m_accel = m_def_accel;

		m_desc = descr;
	}

	bool operator < (const CHotkey& other) const
	{
		return (m_name.CompareNoCase(other.m_name) < 0);
	}

	// ISerializable interface
	int GetProperties(std::vector<CString>& properties)
	{
		properties.push_back(L"Name");
		properties.push_back(L"Accel");

		return properties.size();
	}

	bool GetPropertyValue(const CString& sProperty, CProperty& property)
	{
		if(sProperty == L"Name")
		{
			property = m_reg_name;
			return true;
		}
		if(sProperty == L"Accel")
		{
			CString temp;
			temp.Format(L"%u;%u",
				m_accel.fVirt, m_accel.key);
			property = temp;
			return true;
		}

		return false;
	}

	bool SetPropertyValue(const CString& sProperty, CProperty& sValue)
	{
		if(sProperty == L"Name")
		{
			m_reg_name = sValue.GetStringValue();
			return true;
		}
		else if(sProperty == L"Accel")
		{
			CString str = sValue.GetStringValue();
			int n = 0, curPos = 0;

			while(str.Tokenize(L";", curPos) != L"")
				n++;

			CString* tokens = new CString[n];
			curPos = n =0;

			CString temp;
			while((temp = str.Tokenize(L";", curPos)) != L"")
			{
				tokens[n] = temp;
				n++;
			}

			if(n == 2)
			{
				m_accel.fVirt = StrToInt(tokens[0]);
				m_accel.key = StrToInt(tokens[1]);
			}

			delete[] tokens;

			return true;
		}

		return false;
	}

	bool HasMultipleInstances()
	{
		return false;
	}

	CString GetClassName()
	{
		return L"Hotkey";
	}

	CString GetID()
	{
		return L"";
	}

	ISerializable* Create()
	{
		return new CHotkey;
	}

	void Destroy(ISerializable* obj)
	{
		delete obj;
	}
};

class CHotkeysGroup : public ISerializable, public IObjectFactory
{
public:
	CString m_name;
	CString m_reg_name;
	std::vector<CHotkey> m_hotkeys;

	CHotkey m_hotkey_factory;
	std::vector<void*> m_ptr_hotkeys;

	CHotkeysGroup()
	{
	}

	CHotkeysGroup(CString reg_name, int IDS_GROUP_NAME)
	{
		m_reg_name = reg_name;
		::LoadString(_Module.GetResourceInstance(), IDS_GROUP_NAME, m_name.GetBufferSetLength(MAX_LOAD_STRING + 1), 
			MAX_LOAD_STRING + 1);
	}

	bool operator < (const CHotkeysGroup& other) const
	{
		return (m_name.CompareNoCase(other.m_name) < 0);
	}

	int GetProperties(std::vector<CString>& properties)
	{
		properties.push_back(L"GroupName");
		properties.push_back(L"Hotkey");

		return properties.size();
	}

	bool GetPropertyValue(const CString& sProperty, CProperty& property)
	{
		if(sProperty == L"GroupName")
		{
			property = m_reg_name;
			return true;
		}
		if(sProperty == L"Hotkey")
		{
			m_ptr_hotkeys.clear(); // Очищаем кэш перед каждым сохранением, чтобы не плодить дубликаты

			for(unsigned long i = 0 ; i < m_hotkeys.size(); ++i)
				m_ptr_hotkeys.push_back(&m_hotkeys[i]);
			
			property = m_ptr_hotkeys;
			property.SetFactory(&m_hotkey_factory);

			return true;
		}

		return false;
	}

	bool SetPropertyValue(const CString& sProperty, CProperty& sValue)
	{
		if(sProperty == L"GroupName")
		{
			m_reg_name = sValue.GetStringValue();
			return true;
		}
		if(sProperty == L"Hotkey")
		{
			std::vector<void*>::iterator iter = m_ptr_hotkeys.begin();

			while(iter != m_ptr_hotkeys.end())
			{
				CHotkey* pHotkey = (CHotkey*)&iter;
				delete pHotkey;
				iter++;
			}

			CProperty::CopyPtrList(m_ptr_hotkeys, sValue.GetObjectList());

			m_hotkeys.clear();
			for(unsigned long i = 0 ; i < m_ptr_hotkeys.size(); ++i)
			{
				CHotkey* temp = (CHotkey*)m_ptr_hotkeys[i];
				m_hotkeys.push_back(*temp);
			}

			return true;
		}

		return false;
	}

	bool HasMultipleInstances()
	{
		return true;
	}

	CString GetClassName()
	{
		return L"HkGroup";
	}

	CString GetID()
	{
		return L"";
	}

	ISerializable* Create()
	{
		return new CHotkeysGroup;
	}

	void Destroy(ISerializable* obj)
	{
		delete obj;
	}
};

class DESCSHOWINFO : public ISerializable, public IObjectFactory
{
public:
	std::map<CString, bool> elements;

	DESCSHOWINFO()
	{
		SetDefaults();
	}

	// Default fields showing in description
	void SetDefaults()
	{
		elements[L"ci_all"] = true;
		elements[L"sti_all"] = false;
		elements[L"di_id"] = true;
		elements[L"id"] = true;
		elements[L"ti_kw"] = true;
		elements[L"ti_nic_mail_web"] = true;
		elements[L"ti_genre_match"] = true;
	}

	// ISerializable interface
	int GetProperties(std::vector<CString>& properties);
	bool GetPropertyValue(const CString& sProperty, CProperty& sValue);
	bool SetPropertyValue(const CString& sProperty, CProperty& sValue);
	bool HasMultipleInstances();
	CString GetClassName();
	CString GetID();

	// IObjectFactory
	ISerializable* Create();
	void Destroy(ISerializable*);
};

class TREEITEMSHOWINFO : public ISerializable, public IObjectFactory
{
public:
	std::map<CString, bool> items;

	TREEITEMSHOWINFO();
	void SetDefaults();

	// ISerializable interface
	int GetProperties(std::vector<CString>& properties);
	bool GetPropertyValue(const CString& sProperty, CProperty& sValue);
	bool SetPropertyValue(const CString& sProperty, CProperty& sValue);
	bool HasMultipleInstances();
	CString GetClassName();
	CString GetID();

	// IObjectFactory
	ISerializable* Create();
	void Destroy(ISerializable*);
};

class CSettings : public ISerializable, public IObjectFactory
{
	CRegKey		m_key;
	CString		m_key_path;

	bool		m_keep_encoding; // save with opened encoding
	CString		m_default_encoding;

	DWORD		m_search_options;

	DWORD		m_collorBG;
	DWORD		m_collorFG;
	DWORD		m_font_size;
	CString		m_font;
	CString		m_srcfont;

	bool		m_xml_src_wrap;
	bool		m_xml_src_syntaxHL;
	bool		m_xml_src_tagHL;
	bool		m_xml_src_showEOL;
	bool		m_xml_src_showSpace;

	bool		m_fast_mode;
	bool		m_view_status_bar;
	bool		m_view_doc_tree;

	// added by SeNS
	bool		m_usespell_check;
	bool		m_highlght_check;
	CString		m_custom_dict;
	DWORD		m_custom_dict_codepage;
	CString		m_nbsp_char;
	CString		m_old_nbsp;
	bool		m_change_kbd_layout_check;
	DWORD		m_keyb_layout;
	bool		m_show_line_numbers;
	DWORD		m_image_type;
	DWORD		m_jpeg_quality;
	///

	DWORD		m_splitter_pos;
	CString		m_toolbars_settings;

	bool		m_restore_file_position;

	DWORD		m_interface_lang_id;

	bool		m_need_restart;

	CString		m_scripts_folder;

	bool		m_insimage_ask;
	bool		m_ins_clear_image;

	bool		m_show_words_excls;
	bool		m_disable_words_stats;

	WINDOWPLACEMENT m_words_dlg_placement;
	WINDOWPLACEMENT m_wnd_placement;

	DESCSHOWINFO m_desc;
	TREEITEMSHOWINFO m_tree_items;

	// added by SeNS: view dimensions for external helper
	int m_viewWidth, m_viewHeight;
	HWND m_hMainWindow;

public:
	std::vector<CHotkeysGroup> m_hotkey_groups;
	int keycodes; // total number of accelerators
	std::vector<WordsItem> m_words;

public:
	CSettings();
	~CSettings();

	void InitHotkeyGroups();
	void Close();

	// ISerializable interface
	int GetProperties(std::vector<CString>& properties);
	bool GetPropertyValue(const CString& sProperty, CProperty& sValue);
	bool SetPropertyValue(const CString& sProperty, CProperty& sValue);
	bool HasMultipleInstances();
	CString GetClassName();
	CString GetID();

	// IObjectFactory
	ISerializable* Create();
	void Destroy(ISerializable*);
	void SetDefaults();

	void Load();
	void Save();

	void LoadHotkeyGroups();
	void SaveHotkeyGroups();

	CHotkeysGroup* GetGroupByName(const CString& name);
	CHotkey* GetHotkeyByName(const CString& name, CHotkeysGroup& group);

	void LoadWords();
	void SaveWords();

	bool NeedRestart()const;

	bool KeepEncoding()const;
	bool XmlSrcWrap()const;
	bool XmlSrcSyntaxHL()const;
	bool XmlSrcTagHL()const;
	bool XmlSrcShowEOL()const;
	bool XmlSrcShowSpace()const;
	bool FastMode()const;
	bool ViewStatusBar()const;
	bool ViewDocumentTree()const;
	bool RestoreFilePosition()const;

	CString GetKeyPath()const;

	CString GetDefaultEncoding()const;
	DWORD	GetSearchOptions()const;
	DWORD	GetColorBG()const;
	DWORD	GetColorFG()const;
	DWORD	GetFontSize()const;
	CString	GetFont()const;
	CString	GetSrcFont()const;
	DWORD	GetSplitterPos()const;	
	CString GetToolbarsSettings()const;
	const CRegKey& GetKey()const;

	// added by SeNS
	bool    GetUseSpellChecker()const;
	bool	GetHighlightMisspells()const;
	CString GetCustomDict()const;
	DWORD	GetCustomDictCodepage()const;
	CString GetNBSPChar()const;
	CString GetOldNBSPChar()const;
	bool	GetChangeKeybLayout()const;
	DWORD	GetKeybLayout()const;
	bool	XMLSrcShowLineNumbers()const;
	DWORD	GetImageType()const;
	DWORD	GetJpegQuality()const;

	bool	GetExtElementStyle(const CString& elem)const;
	bool	GetWindowPosition(WINDOWPLACEMENT& wpl)const;
	DWORD	GetInterfaceLanguageID()const;
	CString GetInterfaceLanguageDllName()const;
	CString GetLocalizedGenresFileName()const;
	CString GetInterfaceLanguageName()const;
	CString GetScriptsFolder() const;
	CString GetDefaultScriptsFolder();
	bool	IsDefaultScriptsFolder();
	bool	GetInsImageAsking()const;
	bool	GetIsInsClearImage()const;
	bool	GetShowWordsExcls()const;
	bool	GetDisableWordsStats()const;
	bool	GetWordsDlgPosition(WINDOWPLACEMENT &wpl)const;

	bool	GetDocTreeItemState(const CString& item, bool default_state);

	void	SetKeepEncoding(bool keep, bool apply = false);
	void	SetDefaultEncoding(const CString& encoding, bool apply = false);	
	void	SetSearchOptions(DWORD opt, bool apply = false);
	void	SetColorBG(DWORD color, bool apply = false);
	void	SetColorFG(DWORD color, bool apply = false);
	void	SetFontSize(DWORD size, bool apply = false);
	void	SetXmlSrcWrap(bool wrap, bool apply = false);
	void	SetXmlSrcSyntaxHL(bool hl, bool apply = false);
	void	SetXmlSrcTagHL(bool hl, bool apply = false);
	void	SetXmlSrcShowEOL(bool eol, bool apply = false);
	void	SetXmlSrcShowSpace(bool eol, bool apply = false);
	void	SetFastMode(bool mode,  bool apply = false);
	void	SetFont(const CString& font, bool apply = false);
	void	SetSrcFont(const CString& font, bool apply = false);
	void	SetViewStatusBar(bool view,  bool apply = false);
	void	SetViewDocumentTree(bool view,  bool apply = false);
	void	SetSplitterPos(DWORD pos,  bool apply = false);
	void	SetToolbarsSettings(CString& settings,  bool apply = false);
	void	SetExtElementStyle(const CString& elem, bool ext, bool apply = false);
	void	SetWindowPosition(const WINDOWPLACEMENT& wpl,  bool apply = false);
	void	SetRestoreFilePosition(bool restore, bool apply = false);	
	void	SetInterfaceLanguage(DWORD Language, bool apply = false);
	void	SetInsImageAsking(const bool value, bool apply = false);
	void	SetIsInsClearImage(const bool value, bool apply = false);
	void	SetDocTreeItemState(const CString& item, bool state);
	void	SetShowWordsExcls(const bool value, bool apply = false);
	void	SetDisableWordsStats(const bool value, bool apply = false);
	void	SetWordsDlgPosition(const WINDOWPLACEMENT& wpl,  bool apply = false);

	void	SetNeedRestart();

	CString	m_initial_scripts_folder;

	// added by SeNS
	void	SetUseSpellChecker(const bool value, bool apply = false);
	void	SetHighlightMisspells(const bool value, bool apply = false);
	void	SetCustomDict(const ATL::CString &value, bool apply = false);
	void	SetCustomDictCodepage(const DWORD value, bool apply = false);
	void	SetNBSPChar(const ATL::CString &value, bool apply = false);
	void	SetChangeKeybLayout(const bool value, bool apply = false);
	void	SetKeybLayout(const DWORD value, bool apply = false);
	void	SetXMLSrcShowLineNumbers(const bool value, bool apply = false);
	void	SetImageType(const DWORD value, bool apply = false);
	void	SetJpegQuality(const DWORD value, bool apply = false);

	void	SetViewWidth(int width) { m_viewWidth = width; }
	void	SetViewHeight(int height) { m_viewHeight = height; }
	int		GetViewWidth() { return m_viewWidth; }
	int		GetViewHeight() { return m_viewHeight; }
	void	SetMainWindow(HWND hwnd) { m_hMainWindow = hwnd; }
	HWND	GetMainWindow() { return m_hMainWindow; }

};

// НАЧАЛО КЛАССА ИСТОРИИ (PORTABLE)
#include <map>

class CHistory
{
public:
    CSimpleArray<CString> m_mru_list;
    CSimpleArray<CString> m_search_list;
    CSimpleArray<CString> m_replace_list;
    std::map<CString, int> m_documents;

    // Вспомогательная функция для экранирования спецсимволов в путях и словах
    CString EscapeXML(const CString& str) {
        CString res = str;
        res.Replace(L"&", L"&amp;"); 
        res.Replace(L"<", L"&lt;");
        res.Replace(L">", L"&gt;");  
        res.Replace(L"\"", L"&quot;");
        res.Replace(L"\'", L"&apos;");
        return res;
    }

    void Load() {
        m_mru_list.RemoveAll(); m_search_list.RemoveAll(); m_replace_list.RemoveAll(); m_documents.clear();
        
        CString path = U::GetSettingsDir() + L"History.xml";
        if (::GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES) return;

        MSXML2::IXMLDOMDocument2Ptr pXMLDoc;
        if (FAILED(pXMLDoc.CreateInstance(L"Msxml2.DOMDocument.6.0"))) return;
        
        pXMLDoc->async = VARIANT_FALSE; // Выключаем асинхронную загрузку
        if (pXMLDoc->load(_variant_t(path)) == VARIANT_TRUE) {
            
            // 1. Загружаем позиции курсора (Documents)
            MSXML2::IXMLDOMNodeListPtr docNodes = pXMLDoc->selectNodes(L"//FBE/History/Documents/Document");
            if (docNodes) {
                for (int i = 0; i < docNodes->length; i++) {
                    MSXML2::IXMLDOMElementPtr elem = docNodes->item[i];
                    if (elem) {
                        _variant_t vPath = elem->getAttribute(L"path");
                        _variant_t vPos = elem->getAttribute(L"pos");
                        if (vPath.vt == VT_BSTR && vPos.vt == VT_BSTR)
                            m_documents[(const wchar_t*)vPath.bstrVal] = _wtoi(vPos.bstrVal);
                    }
                }
            }
            
            // 2. Загружаем недавние файлы (Recent Document List)
            MSXML2::IXMLDOMNodeListPtr mruNodes = pXMLDoc->selectNodes(L"//FBE/History/RecentDocumentList/Document");
            if (mruNodes) {
                for (int i = 0; i < mruNodes->length; i++) {
                    MSXML2::IXMLDOMElementPtr elem = mruNodes->item[i];
                    if (elem) {
                        _variant_t vPath = elem->getAttribute(L"path");
                        if (vPath.vt == VT_BSTR) m_mru_list.Add((const wchar_t*)vPath.bstrVal);
                    }
                }
            }

            // 3. Загружаем историю замены (ReplaceHistory)
            MSXML2::IXMLDOMNodeListPtr replaceNodes = pXMLDoc->selectNodes(L"//FBE/History/ReplaceHistory/Item");
            if (replaceNodes) {
                for (int i = 0; i < replaceNodes->length; i++) {
                    MSXML2::IXMLDOMElementPtr elem = replaceNodes->item[i];
                    if (elem && elem->text.length() > 0) m_replace_list.Add((const wchar_t*)elem->text);
                }
            }

            // 4. Загружаем историю поиска (SearchHistory)
            MSXML2::IXMLDOMNodeListPtr searchNodes = pXMLDoc->selectNodes(L"//FBE/History/SearchHistory/Item");
            if (searchNodes) {
                for (int i = 0; i < searchNodes->length; i++) {
                    MSXML2::IXMLDOMElementPtr elem = searchNodes->item[i];
                    if (elem && elem->text.length() > 0) m_search_list.Add((const wchar_t*)elem->text);
                }
            }
        }
    }

    void Save() {
        CString path = U::GetSettingsDir() + L"History.xml";
        CString xml = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
        xml += L"<FBE>\r\n\t<History>\r\n";
        
        // 1. Ветка Documents
        xml += L"\t\t<Documents>\r\n";
        std::map<CString, int>::iterator it;
        for (it = m_documents.begin(); it != m_documents.end(); ++it) {
            CString entry;
            entry.Format(L"\t\t\t<Document path=\"%s\" pos=\"%d\"/>\r\n", EscapeXML(it->first), it->second);
            xml += entry;
        }
        xml += L"\t\t</Documents>\r\n";

        // 2. Ветка Recent Document List (Нумерация начинается с 1)
        CString mruCount;
        mruCount.Format(L"\t\t<RecentDocumentList DocumentCount=\"%d\">\r\n", m_mru_list.GetSize());
        xml += mruCount;
        for (int i = 0; i < m_mru_list.GetSize(); i++) {
            CString entry;
            entry.Format(L"\t\t\t<Document id=\"%d\" path=\"%s\"/>\r\n", i + 1, EscapeXML(m_mru_list[i]));
            xml += entry;
        }
        xml += L"\t\t</RecentDocumentList>\r\n";

        // 3. Ветка ReplaceHistory (Нумерация начинается с 0, идет перед поиском)
        CString repCount;
        repCount.Format(L"\t\t<ReplaceHistory Count=\"%d\">\r\n", m_replace_list.GetSize());
        xml += repCount;
        for (int i = 0; i < m_replace_list.GetSize(); i++) {
            CString entry;
            entry.Format(L"\t\t\t<Item id=\"%d\">%s</Item>\r\n", i, EscapeXML(m_replace_list[i]));
            xml += entry;
        }
        xml += L"\t\t</ReplaceHistory>\r\n";

        // 4. Ветка SearchHistory (Нумерация начинается с 0)
        CString searchCount;
        searchCount.Format(L"\t\t<SearchHistory Count=\"%d\">\r\n", m_search_list.GetSize());
        xml += searchCount;
        for (int i = 0; i < m_search_list.GetSize(); i++) {
            CString entry;
            entry.Format(L"\t\t\t<Item id=\"%d\">%s</Item>\r\n", i, EscapeXML(m_search_list[i]));
            xml += entry;
        }
        xml += L"\t\t</SearchHistory>\r\n";

        xml += L"\t</History>\r\n</FBE>";

        // Сохраняем в UTF-8
        HANDLE hFile = ::CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            // Узнаем размер буфера для конвертации
            int utf8Len = ::WideCharToMultiByte(CP_UTF8, 0, xml, -1, NULL, 0, NULL, NULL);
            if (utf8Len > 0) {
                char* utf8Buf = new char[utf8Len];
                ::WideCharToMultiByte(CP_UTF8, 0, xml, -1, utf8Buf, utf8Len, NULL, NULL);
                DWORD written = 0;
                // utf8Len - 1 чтобы не писать завершающий нулевой байт в текстовый файл
                ::WriteFile(hFile, utf8Buf, utf8Len - 1, &written, NULL);
                delete[] utf8Buf;
            }
            ::CloseHandle(hFile);
        }
    }
};

extern CHistory _History;