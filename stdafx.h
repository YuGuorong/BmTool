
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


#include "deelx.h"
void MyTracex(const char * fmt, ...);
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//#include <vld.h> 
// VC6
#if defined(_MSC_VER) && _MSC_VER == 1200

#ifndef ULONG_PTR
#define ULONG_PTR unsigned long*
#endif

#include <Specstrings.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

// VS2005
#else 
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#endif
#define VENDOR_OPENCE   1  //奥鹏教育编目工具
#define VENDOR_TEACHING 2  //上海中小学教材编著工具
#define VENDOR_NAME VENDOR_OPENCE//VENDOR_TEACHING 

void SwitchBackDlg(CWnd * pwndHide);
void SwitchBackMainDlg();
#define TIME_FMT  _T("%Y-%m-%d %H:%M")
#define DATE_FMT  _T("yyyy-MM-dd HH:mm")
#define PROJ_EXT  _T("zip")
#define CFG_META_FILE   _T("__meta.xml")
#define CFG_COVER_FILE  _T("__cover.jpg")
#define CFG_PREVIEW_FILE     _T("__preview.swf")
#define CFG_PDF2SWF_EXE      _T("pdf2swf.exe")
#define	CFG_EPUB2PDF_TOOL_DIR _T("epub2pdfbin\\")
#define CFG_EPUB2PDF_INF_FILE _T("epub2pdf.properties")
#define CFG_EPUB2PDF_EXE     _T("epub2pdf.bat")
#define CFG_RES_FOLDER       _T("res")
#define CFG_MAX_TASK_LIMIT   1


#define OPEN_ENC
#define MEM_ENC
#define DEC_MEM


#define STR_VERSION     _T("1.0.2.1")

#if VENDOR_NAME == VENDOR_OPENCE
#define IDS_VERNDOR_NAME  IDSTR_VENDOR_OPENCN
#define CFG_OPENCN_FOLDER    _T("北京教育资源网")  // _T("奥鹏书籍")

#elif VENDOR_NAME == VENDOR_TEACHING
#define IDS_VERNDOR_NAME  IDSTR_VENDOR_TEACHING
#define CFG_OPENCN_FOLDER     _T("教材编著书籍")//_T("奥鹏书籍")


#endif 