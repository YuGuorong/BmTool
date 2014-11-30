#pragma once

#include "ExDialog.h"
// CSettingDlg dialog

class CSettingDlg : public CExDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDlg();
	virtual void OnOK();

// Dialog Data
	enum { IDD = IDD_SETTING_DLG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void EnableProxy(BOOL benProxy);
	CLCheckBox m_ProxyBox;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedChkProxy();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnOk2();
};
