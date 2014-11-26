#pragma once
#include <map>
#include "ExDialog.h"
#include "ReaderView.h"
#include "CGdiPlusBitmap.h"
#include "PackerProj.h"

// CMetaDlg dialog
#define ITEM_CAPTION_SIZE  120
#define ITEM_HIGHT         28
#define ID_META_CTRL_START  0x3e00
#define ID_META_CTRL_END    0x5e00

typedef enum 
{
	META_NOT_SHOW   = 0x0001, 
	META_READONLY   = 0x0002,
	META_READWRITE  = 0x0004,
	META_SUB_ITEM   = 0x0008,
	META_PASSWORD   = 0x0010, 
	META_PWDCONFIRM = 0x0020,
    META_SECUREITY1 = 0x0040,
    META_SECUREITY2 = 0x0080,
    META_COMBOBOX   = 0x0100,
	META_PICTURE    = 0x0200,
	META_DATETIME   = 0x0400,
	META_MULTLINE   = 0x0800,
}META_STYLE;

typedef CWnd * PCWnd;

class CMetaExtend
{
public:
	CString strKey;
	TCHAR * pszMetaDetail;
	
public:
	CMetaExtend(){ pszMetaDetail= NULL;};
	INT NewExtendItem(CString strFile, CWnd * pParent);
	~CMetaExtend(){ if(pszMetaDetail) FreePtr(pszMetaDetail);};
};

class CMetaDataItem
{
public:
	CString  strKey;
	CString	 strValue;
	CString  strDefVal;
	DWORD    style;   /*在设置窗口显示 META_STYLE*/
	int		 nCtrlID;
	PCWnd    pWnd[3];
	CBitmap *pimg;
	CMetaDataItem * pNext;
	CMetaExtend   * pExt;
	INT      nSubIdx;
public:
	CMetaDataItem()
	{ 
		nSubIdx =-1;
		nCtrlID = ID_META_CTRL_START;
		for(int i=0;i<3;i++) pWnd[i] = NULL;  
		pimg = NULL; pExt = NULL; pNext=NULL;
	};
	~CMetaDataItem()
	{
		for(int i=0; i< 3; i++) { if( pWnd[i] ) { delete pWnd[i]; pWnd[i] = NULL; }} 
		if(pimg) { pimg->DeleteObject();delete pimg; pimg = NULL;}
	}
};


class CMetaDlg : public CReaderView
{
	DECLARE_DYNAMIC(CMetaDlg)

public:
	CMetaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMetaDlg();
public:
	INT GetItemValue(LPCTSTR ItemCaption, CString &strValue);
	INT SaveMetaData(CPackerProj  * proj);
	CMetaDataItem * NewMetaItem(int style, LPCTSTR szKey, LPCTSTR strDefV);
	void CreateTitle(PCWnd * pWnd , LPCTSTR szKey, CRect &r);
	void CreateItem( CMetaDataItem * pItem, LPCTSTR strV, CRect &r);
	void LoadMetaImage( CMetaDataItem * pItem, LPCTSTR strV, CRect r);
	INT  LoadMetaData(LPCTSTR szMetaFile);
	INT  ParseMetaItem(CString &strRawMeta);
	CMetaExtend *  ParseExtMeta(CString strExt , int &nSubIndex);
	INT  LoadExtMetaValue(CMetaDataItem * pit, CString &strKey );
// Dialog Data
	enum { IDD = IDD_META_DLG };
	CFont m_ftCaption;
	CFont m_ftEdit;
	int   m_nMaxCapLen;
protected:
	std::map<CString, CMetaExtend *> m_MetaExtMap; 

	CMetaDataItem * m_pItem;
	CMetaDataItem * GetCtrlItem(int nid);
	CMetaDataItem * FindSubItem(CMetaDataItem * pit);
	CMetaDataItem * ChangeSubComboBox(CMetaDataItem * pit);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void CMetaDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeCombboxs(UINT id);
	afx_msg void OnPictureClick(UINT id);
};
