#pragma once

#include "ExDialog.h"
// CMetaDlg dialog
typedef CWnd * PCWnd;
typedef struct tag_Setting
{
	LPCTSTR  strKey;
	LPCTSTR  strDefVal;
	DWORD    style;   /*在设置窗口显示*/
	PCWnd    pWnd[4];
}META_ITEM;


class CMetaDlg : public CExDialog
{
	DECLARE_DYNAMIC(CMetaDlg)

public:
	CMetaDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMetaDlg();
public:
	void CreateTitle(PCWnd * pWnd , LPCTSTR szKey, CRect &r);
	void CreateItem(PCWnd * pWnd, META_ITEM * pItem, CString &strV, CRect &r,  int nID);
// Dialog Data
	enum { IDD = IDD_META_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void CMetaDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
