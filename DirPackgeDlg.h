
// DirPackgeDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "ExDialog.h"
#include "LoginDlg.h"
#include "BMDlg.h"
#include "afxwin.h"
#include "SettingDlg.h"
#include "PackerProj.h"
#include "ResManDlg.h"

#define MAX_TAB_ITEM   8

#define LOGIN_TAB   0
#define BM_TAB      1
#define IMPORT_TAB  2
#define EXPORT_TAB  3 
#define RES_EXP_TAB 4
#define SETTING_TAB 5

typedef enum 
{
	BMST_LOGIN     = 0x00001,
	BMST_ONLINE    = 0x00002, 
	BMST_OFFLINE   = 0x00004,
	BMST_OPENNING  = 0x00008,
	BMST_NEW_PROJ  = 0x00010,
	BMST_SETTING   = 0x00020,
	BMST_EDIT_PROJ = 0x00040,
	BMST_UPLOADING = 0x00080,
	BMST_SAVING    = 0x00100,
	BMST_UNKOWN    = 0x80000
}BM_STATE;

// CDirPackgeDlg dialog
class CDirPackgeDlg : public CeExDialog
{
// Construction
public:
	CDirPackgeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DIRPACKGE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	CExDialog * m_pSubDlgs[MAX_TAB_ITEM];
	CExDialog * m_pCurDlg;
	void SwitchDlg(int id);
	CPackerProj * m_Proj;

// Implementation
protected:
	HICON m_hIcon;
	UINT m_BmState;
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
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnExplore();
	afx_msg void OnBnClickedBtnExport();
};
