#pragma once
#include <map>
#include "ExDialog.h"
#include "ReaderView.h"
#include "CGdiPlusBitmap.h"
#include "PackerProj.h"

// CMetaDlg dialog
#define ITEM_CAPTION_SIZE  120
#define ITEM_HIGHT         28
#define ITEM_LARG_HIGH     (64)

class CMetaDlg : public CReaderView
{
	DECLARE_DYNAMIC(CMetaDlg)

public:
	CMetaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMetaDlg();
	CPackerProj * m_proj;
public:
	CMetaDataItem * FindNextMetaItem(CMetaDataItem * pit);
	INT GetItemValue(LPCTSTR ItemCaption, CString &strValue);
	INT SetItemValue(LPCTSTR ItemCaption, LPCTSTR strValue, BOOL bSetSubCombox=TRUE);
	CMetaDataItem * NewMetaItem(int style, LPCTSTR szKey, LPCTSTR strDefV);
	void CreateTitle(PCWnd * pWnd , LPCTSTR szKey, CRect &r);
	void CreateItem( CMetaDataItem * pItem, LPCTSTR strV, CRect &r);
	void LoadMetaImage( CMetaDataItem * pItem, LPCTSTR strV, CRect r);
	INT  LoadMetaData(LPCTSTR szMetaFile);
	INT  ParseMetaItem(CString &strRawMeta);
	CMetaExtend *  ParseExtMeta(CString strExt , int &nSubIndex);
	INT  LoadExtMetaValue(CMetaDataItem * pit, CString &strKey );
	INT LoadCoverImage();
	INT  SetMetaValues();
// Dialog Data
	enum { IDD = IDD_META_DLG };
	CFont m_ftCaption;
	CFont m_ftEdit;
	int   m_nMaxCapLen;
	int   m_nMaxHight;
protected:
	std::map<CString, CMetaExtend *> m_MetaExtMap; 

	
	CMetaDataItem * GetCtrlItem(int nid);
	CMetaDataItem * FindSubItem(CMetaDataItem * pit);
	CMetaDataItem * ChangeSubComboBox(CMetaDataItem * pit);
	void MoveItem(CMetaDataItem * pItem, CRect &rs);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void CMetaDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeCombboxs(UINT id);
	afx_msg void OnPictureClick(UINT id);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
