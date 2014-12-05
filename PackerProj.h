#pragma once

#include <time.h>
#include "tables.h"
#include <map>
using std::map;

#define WM_PDF_PAGE     (WM_APP  + 179)
#define WM_VIEW_PROJ    (WM_USER + 313)
class CPackerProj;

using namespace std;

typedef enum
{
	VIEW_UNKNOWN = -1,
	VIEW_META_DATA = 0,
	VIEW_EPUB,
	VIEW_PDF,
	VIEW_EMPTY,
	VIEW_MAX
}view_type;

typedef enum{
	NONE_PROJ     =  0x0001,
	LOGIN_PROJ    =  0x0002,
	NEW_PROJ      =  0x0004,
	OPEN_PROJ     =  0x0008,
	SAVE_PROJ     =  0x0010,
	CLOSE_PROJ    =  0x0020,
	LOCKED        =  0x1000
};


class CDigest
{
public:
	typedef enum{
		MODE_MD5 =0,
		MODE_SHA1,
		MODE_SHA256
	}DIGEST_MODE;

	DIGEST_MODE m_mode;
	CString m_sDigest;
public:
	void CalDigest(LPVOID ptr, int len, DIGEST_MODE mode = MODE_MD5);
	CDigest(CString &sfile, DIGEST_MODE mode = MODE_MD5);
	LPCTSTR GetModeString();
	CDigest(){};
	~CDigest(){};

};

typedef int  RES_RELATION;

class CResMan
{
public:
	CResMan(CPackerProj * proj);
	~CResMan();
	static CResMan * NewRes(const char ** sxml);
	static CResMan * NewRes(CFileDialog &fdlg);
	BOOL SaveRes(CString &strXml);
public:
	int    m_icon_id;
	CString  m_strResId;
	RES_RELATION m_relation;
	CString m_sfileName;
	CString m_sPath;
	CTime   m_tmCreate;
	ULONG64 m_fsize;
	CString m_sformat;
	CDigest m_digest;
	CResMan * pNext;
	CPackerProj * m_proj;
};


class CPackerProj
{
public:
	BOOL    m_bLock; //Login lock state.

	CString m_strUuid; //Project uuid
	CString m_szProj;  //Project name
	CString m_szProjPath; //Prjoect path
	CString m_szZipPath;//Project Zip file path 
	CString m_szTargetPath; //Project main book path
	CString m_szTargetFileName; //Project main book name
	UINT32  m_nTargetFLen; //Project main book file length
	CString m_szCoverPath; //Project cover picture path
	CString m_szType;      //Project main book type, PDF?EPub?... 

	CString m_szPathRes;  //Prjoect res folder path
	CString m_strTmCreateProj; //string of time for project create
	CString m_strTmModifyProj; //string of time for last project modificaton 
	CString m_strTmCreateSrc;  //string of time for main book create
	CString m_strTmModifySrc; //string of time for main book create

	UINT32  m_ProjState; //project working state, empty ? new ? open? saving?
	view_type m_type;    //project focus view type, epub?pdf?meta?empty_dialog

	CImageList m_imglist;//image list of resource icon
	CResMan * m_pRes;  //Resources
	CWnd * m_pMetaWnd; //Meta input window

	CString m_strSession; //Login session
	CString m_strLoginUser;//Login user name
	CTime   m_tmLogin;  //Login time
	CString m_strLogId;//Login uuid
	int     m_logState;

	int     m_nCurPage;  //reader cur page, for resource reference relations
	int     m_nBookPageCount; //Main book page count

	//xml parser used purpose--->
	void * m_ptrUserData;  //parser
	CStringA m_sXmlData;  //for combine element data

	CTreeCtrl * m_pProjDir;
	HTREEITEM  m_hDirCur;  //reorgenize directory tree
	map<CStringA, CStringA> m_mapMetaValue;  //all xml value here
	//<-----------------------------
	void * m_ptrDbCol[MAX_BOOK_DB_COL];
	SQL_DB_BOOK  m_db_items;
	CString strError;
protected:
	LPCTSTR m_szMimeType; //contend type
	HGLOBAL m_hG;  //globale resource of MIME type map.

public:
	CPackerProj(CWnd * pParent);
	~CPackerProj();
	INT CreateProj(LPCTSTR szTarget);
	INT DestoryProj();
	int Save();
	int Open(LPCTSTR szProj);
	BOOL ZipRes(LPCTSTR szZipFile);
	BOOL UnzipProj();
	BOOL ParseXml();
	BOOL Res2Xml(CString &strResXml);
	void SetProjStatus(int proj_st);
	BOOL GetFileMemiType(LPCTSTR szExt, CString &type);
	BOOL UpLoadProj();	
	BOOL SaveProjToDb();
	BOOL SetBookState(LPCTSTR sbookid, LPCTSTR sState);
	BOOL ClearResTable();
};


CPackerProj * GetPackProj();