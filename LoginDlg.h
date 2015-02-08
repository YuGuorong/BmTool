#pragma once
#include "ExDialog.h"

// CLoginDlg dialog

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
	virtual void OnHttpObjProc(int idHttpObj, int stat);
// Dialog Data
	enum { IDD = IDD_LOGINDLG };
	CSkinBtn * m_pbtns;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CStringA m_strHttpData;
	CString m_strUser;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnHttpFinishMsg(WPARAM wParam, LPARAM lParam);
};
