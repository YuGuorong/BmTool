#pragma once

#define WM_VIEW_PROJ    (WM_USER + 313)
typedef enum{
	NONE_PROJ,
	NEW_PROJ,
	SAVE_PROJ,
	CLOSE_PROJ,
};
typedef enum
{
	TYPE_META = 0x1,
	TYPE_EPUB = 0x2,
	TYPE_PDF  = 0x4,
	TYPE_MASK = (TYPE_META|TYPE_EPUB|TYPE_PDF)
};

class CPackerProj
{
public:
	CString m_szProj;
	CString m_szProjPath;
	CString m_szTarget;
	CString m_szType;


	CString m_szPathMeta;
	CString m_szPathRes;
	UINT32  m_state;
protected:
	CWnd * m_Parent;
public:
	CPackerProj(CWnd * pParent);
	~CPackerProj();
	INT CreateProj(LPCTSTR szTarget);
	int Save();
	int Open(LPCTSTR szProj);
};

