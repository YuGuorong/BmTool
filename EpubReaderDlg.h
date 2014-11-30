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
	void ViewFile(LPCTSTR szpdf);

	HANDLE m_hReadProc;
	HWND m_hwdReader;

	// 对话框数据
	enum { IDD = IDD_READER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
