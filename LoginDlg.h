#pragma once
#include "ExDialog.h"

// CLoginDlg dialog
#define WM_LOGIN_STATE    (WM_USER + 312)

#define LOGIN_ST_SUCESS   1
#define LOGIN_ST_FAIL     2
#define LOGIN_ST_TIMEOUT  3
#define LOGIN_ST_SUCESS   1
#define LOGIN_ST_SUCESS   1


class CLoginDlg : public CExDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginDlg();

// Dialog Data
	enum { IDD = IDD_LOGINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
