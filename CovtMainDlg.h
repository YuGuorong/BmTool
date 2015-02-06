#pragma once

#include "ExDialog.h"
// CCovtMainDlg 对话框

class CCovtMainDlg : public CExDialog
{
	DECLARE_DYNAMIC(CCovtMainDlg)

public:
	CCovtMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCovtMainDlg();

// 对话框数据
	enum { IDD = IDD_CONVT_MAIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
