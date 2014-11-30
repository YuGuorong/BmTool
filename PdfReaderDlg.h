#pragma once
#include "resource.h"
#include "ReaderView.h"

// CPdfReaderDlg �Ի���

class CPdfReaderDlg : public CReaderView
{
	DECLARE_DYNAMIC(CPdfReaderDlg)

	HANDLE m_hpdf;
public:
	CPdfReaderDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPdfReaderDlg();
	virtual void ViewFile(LPCTSTR szpdf);
	virtual int  GetPageInfo(INT *cur, INT *count);
	virtual void GotoPage(int pgnum);
// �Ի�������
	enum { IDD = IDD_PDF_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPdfPageEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnViewFocus(WPARAM wParam, LPARAM lParam);
};
