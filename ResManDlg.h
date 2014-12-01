#pragma once
#include "ExDialog.h"
#include "ReaderView.h"
#include "PackerProj.h"
// CResManDlg 对话框
class CResManDlg : public CExDialog
{
	DECLARE_DYNAMIC(CResManDlg)

public:
	CResManDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CResManDlg();

// 对话框数据
	enum { IDD = IDD_RESMAN_DLG };

protected:
	CSkinBtn * m_pbtns[8];
	CListCtrl  m_listRes;
	CPackerProj  * m_proj;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnLocal();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnUpload();
	afx_msg void OnBnClickedBtnResupload();
	afx_msg void OnBnClickedBtnSelect();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnRemove();
	afx_msg void OnBnClickedBtnReturn();
};
