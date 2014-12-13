#pragma once

#include "ExDialog.h"
#include "ReaderView.h"
// CReaderView 对话框


class CEpubReaderDlg : public CReaderView
{
	DECLARE_DYNAMIC(CEpubReaderDlg)

public:
	CEpubReaderDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEpubReaderDlg();
	virtual void ViewFile(LPCTSTR szpdf);
	virtual int  GetPageInfo(INT *cur, INT *count){ if (cur) *cur = m_nCurPage;  return m_nCurPage; };
	virtual void GotoPage(int pgnum);

	HANDLE m_hReadProc;
	HWND m_hwdReader;

	// 对话框数据
	enum { IDD = IDD_PDF_DLG };
protected:
	INT  m_nCurPage;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnProjStateChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
