
// iMobleAgentDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "ExDialog.h"
#include "LoginDlg.h"
#include "BMDlg.h"
#include "afxwin.h"
#include "SettingDlg.h"

#define MAX_TAB_ITEM   8

#define LOGIN_TAB   0
#define BM_TAB      1
#define IMPORT_TAB  2
#define EXPORT_TAB  3 
#define RES_EXP_TAB 4
#define SETTING_TAB 5

// CiMobleAgentDlg dialog
class CiMobleAgentDlg : public CeExDialog
{
// Construction
public:
	CiMobleAgentDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IMOBLEAGENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	CExDialog * m_pSubDlgs[MAX_TAB_ITEM];
	CExDialog * m_pCurDlg;
	void SwitchDlg(int id);

// Implementation
protected:
	HICON m_hIcon;
	CGdipButton	*m_cTabBtns[MAX_TAB_ITEM];
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnLoginState(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnLogin();
	CStatic m_frame;
	afx_msg void OnBnClickedBtnNewprj();
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
