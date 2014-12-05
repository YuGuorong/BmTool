#pragma once
#include "ExDialog.h"

// CLoginStateDlg 对话框

class CLoginStateDlg : public CExDialog
{
	DECLARE_DYNAMIC(CLoginStateDlg)

public:
	CLoginStateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginStateDlg();
	void SetLoginStatuText(LPCTSTR szStatus);
	CLink * m_txtStatus;
	CSkinBtn * m_pbtns[2];
// 对话框数据
	enum { IDD = IDD_LOGIN_STATUS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CString m_strStatus;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	afx_msg void OnBnClickedBtnLogout();
	afx_msg void OnBnClickedBtnOk();
};
