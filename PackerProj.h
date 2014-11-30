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
	NONE_PROJ  = 0x0001,
	LOGIN_PROJ = 0x0002,
	NEW_PROJ   = 0x0004,
	OPEN_PROJ  = 0x0008,
	SAVE_PROJ  = 0x0010,
	CLOSE_PROJ = 0x0020,
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
typedef struct tagResId
{
	time_t curtime;
	DWORD  tick;
}RES_ID;

class CResMan
{
public:
	CResMan(CPackerProj * proj);
	~CResMan();
	BOOL NewRes(RES_RELATION rel);
	BOOL SaveRes(CString &strXml);
public:
	int    m_icon_id;
	RES_ID m_id;
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
	CString m_szProj;
	CString m_szProjPath;
	CString m_szTarget;
	CString m_szTargetFileName;
	CString m_szType;


	CString m_szPathMeta;
	CString m_szPathRes;
	CString m_strTmCreateProj;
	CString m_strTmModifyProj;
	CString m_strTmCreateSrc;
	CString m_strTmModifySrc;

	UINT32  m_state;
	view_type m_type;

	CImageList m_imglist;
	CResMan * m_pRes;
	CWnd * m_MainWnd;
	CWnd * m_pMetaWnd;
	CString m_strSession;
	CString m_strLoginUser;
	CTime   m_tmLogin;
	CString m_strId;//uuid
	int     m_logState;
	int     m_ProjState;
	int     m_nCurPage;
	int     m_nBookPageCount;

public:
	CPackerProj(CWnd * pParent);
	~CPackerProj();
	INT CreateProj(LPCTSTR szTarget);
	int Save();
	int Open(LPCTSTR szProj);
	BOOL ZipRes(LPCTSTR szZipFile);
	void SetProjStatus(int proj_st);
};


CPackerProj * GetPackProj();