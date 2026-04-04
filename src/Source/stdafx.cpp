// stdafx.cpp : source file that includes just the standard includes
//	FBE.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

// Фикс для современных версий Visual Studio
ATL::CImage::CDCCache ATL::CImage::s_cache;
ATL::CImage::CInitGDIPlus ATL::CImage::s_initGDIPlus;