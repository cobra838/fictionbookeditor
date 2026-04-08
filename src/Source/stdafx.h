// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__149F645E_518F_4EA9_B603_63D633FFB194__INCLUDED_)
#define AFX_STDAFX_H__149F645E_518F_4EA9_B603_63D633FFB194__INCLUDED_

// check for unicode
#ifndef UNICODE
#error This program requires unicode support to run
#endif

// Change these values to use different versions
#define WINVER			0x0600
#define _WIN32_WINNT	0x0600
#define _WIN32_IE		0x0600

// Insert your headers here
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _ATL_FREE_THREADED

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS
#pragma warning(disable : 4996)

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>
#include <atlstr.h>
#include <atlpath.h>
#include <atlcoll.h>
#include <atlfile.h>
#include <atlcomtime.h>
#include "atlimage.h"

#include <shellapi.h>

// #pragma warning(push)
// #pragma warning(disable: 4838 4302)
#include <atlapp.h>

extern CAppModule _Module;

#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
#include <wtl/atlmisc.h>
#include <wtl/atluser.h>

#include <wtl/atlframe.h>
#include <wtl/atlctrls.h>
#include <wtl/atldlgs.h>
#include <wtl/atlctrlw.h>
#include <wtl/atlctrlx.h>
#include <wtl/atlsplit.h>
#include <wtl/atlddx.h>

#include <wtl/atltheme.h>
// #pragma warning(pop)

// C library
#include <ctype.h>
#include <time.h>

// MSXML
// #pragma warning(push)
// #pragma warning(disable: 4192)
#import <msxml6.dll>
// #pragma warning(pop)

// vb regexps
#import "vbscript3.tlb"

// mshtml additional includes
#include <exdispid.h>
#include <mshtmdid.h>
#include <mshtmcid.h>
#import <shdocvw.dll> no_auto_exclude rename_namespace("SHD") rename("FindText","FindTextX")
#import <mshtml.tlb> no_auto_exclude rename("TranslateAccelerator","TranslateAcceleratorX") rename("max","mshtml_max") rename("min","mshtml_min")

// use com utils
using namespace _com_util;

// extra defines
#ifndef I_IMAGENONE
#define	I_IMAGENONE -1
#endif
#ifndef BTNS_BUTTON
#define	BTNS_BUTTON TBSTYLE_BUTTON
#endif
#ifndef BTNS_AUTOSIZE
#define BTNS_AUTOSIZE TBSTYLE_AUTOSIZE
#endif
#ifndef ODS_HOTLIGHT
#define ODS_HOTLIGHT 0x0040
#endif
#ifndef SPI_GETDROPSHADOW
#define	SPI_GETDROPSHADOW 0x1024
#endif

#define  UIS_WM_UPDATE_PROGRESS_UI	(WM_APP + 0x100)
#define  WM_RESIZE_OPENGL_WINDOW	(WM_APP + 0x101)
#define  WM_CHECK_UPDATE_RESULT		(WM_APP + 0x102)
#define  WM_UPDATE_CHECK_DONE		(WM_APP + 0x103)

#define  FBE_GITHUB_ACCEPT   L"Accept: application/vnd.github.v3+json\r\n"

// scripting support
#include <activscp.h>

// XML serialization
#include "XMLSerializer/XMLSerializer.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__149F645E_518F_4EA9_B603_63D633FFB194__INCLUDED_)
