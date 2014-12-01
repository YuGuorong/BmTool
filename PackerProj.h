#pragma once
#include <time.h>
#define WM_PDF_PAGE     (WM_APP  + 179)
#define WM_VIEW_PROJ    (WM_USER + 313)
class CPackerProj;
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
	BOOL NewRes(RES_RELATION rel);
	BOOL SaveRes(CString &strXml);
public:
	int    m_icon_id;
	CString  m_strResId;
	int    m_type;
	int    m_subtype;
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
	CString m_strUuid; //Project uuid
	CString m_szProj;
	CString m_szProjPath;
	CString m_szTarget;
	CString m_szTargetFileName;
	CString m_szCoverPath;
	CString m_szType;


	CString m_szPathMeta;
	CString m_szPathRes;
	CString m_strTmCreateProj;
	CString m_strTmModifyProj;
	CString m_strTmCreateSrc;
	CString m_strTmModifySrc;

	UINT32  m_ProjState; //project working state, empty ? new ? open? saving?
	view_type m_type;//project focus view type, epub?pdf?meta?empty_dialog

	CImageList m_imglist;//image list of resource icon
	CResMan * m_pRes;  //Resources
	CWnd * m_pMetaWnd; //Meta input window

	CString m_strSession; //Login session
	CString m_strLoginUser;//Login user name
	CTime   m_tmLogin;  //Login time
	CString m_strLogId;//Login uuid
	int     m_logState;

	int     m_nCurPage;  //reader cur page, for resource reference relations
	int     m_nBookPageCount;

protected:
	LPCTSTR m_szMimeType; //contend type
	HGLOBAL m_hG;

public:
	CPackerProj(CWnd * pParent);
	~CPackerProj();
	INT CreateProj(LPCTSTR szTarget);
	INT DestoryProj();
	int Save();
	int Open(LPCTSTR szProj);
	BOOL ZipRes(LPCTSTR szZipFile);
	BOOL UnzipProj();
	BOOL Res2Xml(CString &strResXml);
	void SetProjStatus(int proj_st);
	BOOL GetFileType(LPCTSTR szExt, CString &type);
	BOOL UpLoadProj();	
};


CPackerProj * GetPackProj();