#pragma once

#include "ExDialog.h"
#include "CGdiPlusBitmap.h"
// CMetaDlg dialog
typedef enum 
{
	META_NOT_SHOW,
	META_READONLY,
	META_READWRITE,
	META_SYSPWDCHK,
	META_PASSWORD   = 0x010000, 
	META_PWDCONFIRM = 0x020000,
    META_SECUREITY1 = 0x040000,
    META_SECUREITY2 = 0x080000,
    META_COMBOBOX   = 0x100000,
	META_PICTURE    = 0x200000  
}META_STYLE;

typedef CWnd * PCWnd;
class CMetaDataItem
{
public:
	LPCTSTR  strKey;
	LPCTSTR  strDefVal;
	DWORD    style;   /*在设置窗口显示 META_STYLE*/
	int		 nCtrlID;
	PCWnd    pWnd[3];
	CRect    rPos;
	CImage * pimg;
	CMetaDataItem * pNext;
public:
	CMetaDataItem()
	{ 
		for(int i=0;i<3;i++) pWnd[i] = NULL;  
		pimg = NULL; pNext=NULL;
	};
	~CMetaDataItem()
	{
		for(int i=0; i< 3; i++) { if( pWnd[i] ) { delete pWnd[i]; pWnd[i] = NULL; }} 
		if(pimg) { pimg->Destroy();delete pimg; pimg = NULL;}		
	}
};

#define ITEM_CAPTION_SIZE  120
#define ITEM_HIGHT         28

class CMetaDlg : public CExDialog
{
	DECLARE_DYNAMIC(CMetaDlg)

public:
	CMetaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMetaDlg();
public:
	void CreateTitle(PCWnd * pWnd , LPCTSTR szKey, CRect &r);
	void CreateItem( CMetaDataItem * pItem, CString &strV, CRect &r);
// Dialog Data
	enum { IDD = IDD_META_DLG };

protected:
	CMetaDataItem * m_pItem;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void CMetaDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
