#pragma once

#include <time.h>
#include "tables.h"
#include "Util.h"
#include <map>
using std::map;

#define PR_OK                    0
#define PR_ERR_CONTENT         -101
#define PR_ERR_NOT_FIND_FILE   -102
#define PR_ERR_ZIP_FAIL        -103
#define PR_ERR_UNKOWN_TYPE     -104
#define PR_ERR_OPEN_BOOK       -105


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
	int   m_len;
public:
	void CalDigest(const void * ptr, int len, DIGEST_MODE mode = MODE_MD5);
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
	static INT Remove(CResMan * pres);
	static INT Remove(LPCTSTR szResId);
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


typedef enum
{
	META_NOT_SHOW = 0x0001,
	META_READONLY = 0x0002,
	META_READWRITE = 0x0004,
	META_SUB_ITEM = 0x0008,
	META_PASSWORD = 0x0010,
	META_PWDCONFIRM = 0x0020,
	META_SECUREITY1 = 0x0040,
	META_SECUREITY2 = 0x0080,
	META_COMBOBOX = 0x0100,
	META_PICTURE = 0x0200,
	META_DATETIME = 0x0400,
	META_MULTLINE = 0x0800,
}META_STYLE;

typedef CWnd * PCWnd;

#define ID_META_CTRL_START  0x3e00
#define ID_META_CTRL_END    0x5e00
class CMetaExtend
{
public:
	CString strKey;
	TCHAR * pszMetaDetail;
	CDigest md5_MetaDetail;

public:
	CMetaExtend(){ pszMetaDetail = NULL; };
	INT NewExtendItem(CString strFile, CWnd * pParent);
	~CMetaExtend(){ if (pszMetaDetail) FreePtr(pszMetaDetail); };
};

class CMetaDataItem
{
public:
	CString  strKey;
	CString	 strValue;
	CString  strDefVal;
	DWORD    style;   /*在设置窗口显示 META_STYLE*/
	BOOL     bUniq;
	int		 nCtrlID;
	PCWnd    pWnd[3];
	CBitmap *pimg;
	CMetaDataItem * pNext;
	CMetaExtend   * pExt; //Next Group item
	INT      nSubIdx;
public:
	CMetaDataItem()
	{
		nSubIdx = -1;
		nCtrlID = ID_META_CTRL_START;
		for (int i = 0; i<3; i++) pWnd[i] = NULL;
		pimg = NULL; pExt = NULL; pNext = NULL;
	};
	~CMetaDataItem()
	{
		for (int i = 0; i< 3; i++) { if (pWnd[i]) { delete pWnd[i]; pWnd[i] = NULL; } }
		if (pimg) { pimg->DeleteObject(); delete pimg; pimg = NULL; }
	}
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

	CMetaDataItem * m_pMeta;

	CString m_strSession; //Login session
	CString m_strLoginUser;//Login user name
	CTime   m_tmLogin;  //Login time
	CString m_strLogId;//Login uuid
	int     m_logState;

	int     m_nCurPage;  //reader cur page, for resource reference relations
	int     m_nBookPageCount; //Main book page count

	//xml parser used purpose--->
	void * m_ptrUserData;  //parser
	CString m_sXmlData;  //for combine element data

	CTreeCtrl * m_pProjDir;
	HTREEITEM  m_hDirCur;  //reorgenize directory tree
	map<CStringA, CString> m_mapMetaValue;  //all xml value here
	map <CStringA, CString> m_mapKeyCaps;

	map<CString, CMetaExtend *> m_MetaExtMap;  //combobox detail files
	//<-----------------------------
	void * m_ptrDbCol[MAX_BOOK_DB_COL];
	SQL_DB_BOOK  m_db_items;
	CString strError;
	BOOL   m_bProjModified;

public:
	INT CreateProj(LPCTSTR szTarget);
	INT DestoryProj();
	int Save();
	int Open(LPCTSTR szProj);
	BOOL UpLoadProj();	
	BOOL ClearResTable();
	void SetProjStatus(int proj_st);                     //login? new proj? save proj? close proj?
	BOOL SetBookState(LPCTSTR sbookid, LPCTSTR sState);  //uploaded? uploading? local 
	void LoadMetaImage(CMetaDataItem * pItem, LPCTSTR strV, CRect r);
	void GenPreview(void * hz);
	void Pdf2Swf();
public:
	CPackerProj(CWnd * pParent);
	~CPackerProj();
protected:
	CMetaDataItem *  PrecheckContent(); //NULL is OK. other value identicate what item fail
	void SaveMeta(CString &sxml);
	void SaveDirToXml(HTREEITEM hit, CString &sxml, int sublevel);
	void SaveDirs(CString &sxml);
	BOOL Res2Xml(CString &strResXml);
	BOOL ZipRes(LPCTSTR szZipFile);
	BOOL EncZip(HANDLE hzip, LPCTSTR szIn, LPCTSTR szZipName, BOOL bEnc);
	UINT32 PreCountZipSize();

	BOOL ParseXml();
	BOOL UnzipProj();
	int UnzipEncFile(HANDLE hz, void * pze, LPCTSTR sfile, BOOL bres = FALSE);
	BOOL SaveProjToDb();
	BOOL LoadMetaCol();
};


CPackerProj * GetPackProj();