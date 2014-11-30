#pragma once

#include "ExDialog.h"
#include "ReaderView.h"
// CReaderView �Ի���


class CEpubReaderDlg : public CReaderView
{
	DECLARE_DYNAMIC(CEpubReaderDlg)

public:
	CEpubReaderDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEpubReaderDlg();
	void ViewFile(LPCTSTR szpdf);

	HANDLE m_hReadProc;
	HWND m_hwdReader;

	// �Ի�������
	enum { IDD = IDD_READER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
