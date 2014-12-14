#pragma once

#include "ExDialog.h"
#include "MetaDlg.h"
#include "ReaderView.h"
#include "PackerProj.h"
//#include "ListCtrlCl.h"
#include "afxwin.h"
#include "afxcmn.h"
// CBMDlg dialog
#define MAX_COLOR_BTNS  6

class CBMDlg : public CExDialog
{
	DECLARE_DYNAMIC(CBMDlg)

public:
	CBMDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBMDlg();

	view_type ChangeView(view_type vtype); //view_type
	view_type m_cur_view;
	CPackerProj  * m_proj;
	CReaderView * GetProjView();
	void ResOnPageChange();
	
// Dialog Data
	enum { IDD = IDD_BMDLG };
protected:
	CReaderView * m_pViews[VIEW_MAX];
	CGdipButton	*m_cBtns[MAX_COLOR_BTNS];
	int   m_nResIdCol;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void MoveCtrlRect(int id, int right, int top, CRect &r);
	void InsertRes(CResMan* pRes);
	void UpdateInfoText();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_FrameMeta;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTreeCtrl m_trMeta;
	CTreeCtrl m_trDir;
	CListCtrl m_listRes;
	CString   m_strInfo;
	
	afx_msg void OnDestroy();
	afx_msg LRESULT OnViewProjMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPdfPageEvent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnAddDir();
	afx_msg void OnBnClickedBtnRemoveDir();
	afx_msg void OnNMClickTreeMeta(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAddRes();
	afx_msg void OnBnClickedBtnRemoveRes();
	afx_msg void OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	afx_msg void OnTvnEndlabeleditTreeDir(NMHDR *pNMHDR, LRESULT *pResult);
};
