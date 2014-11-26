#pragma once

#include "ExDialog.h"
#include "MetaDlg.h"
#include "ReaderView.h"
#include "PackerProj.h"
#include "afxwin.h"
#include "afxcmn.h"
// CBMDlg dialog


class CBMDlg : public CExDialog
{
	DECLARE_DYNAMIC(CBMDlg)

public:
	CBMDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBMDlg();

	view_type ChangeView(view_type vtype); //view_type
	view_type m_cur_view;
	CPackerProj  * m_proj;
	CReaderView * GetCurView();
// Dialog Data
	enum { IDD = IDD_BMDLG };
protected:
	CReaderView * m_pViews[VIEW_MAX];
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_FrameMeta;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTreeCtrl m_trMeta;
	CTreeCtrl m_trDir;
	CListCtrl m_listRes;
	afx_msg void OnDestroy();
	afx_msg LRESULT OnViewProjMsg(WPARAM wParam, LPARAM lParam);
};
