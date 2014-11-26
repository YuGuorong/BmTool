#pragma once

#include "ExDialog.h"
// CReaderView �Ի���
typedef enum
{
	VIEW_UNKNOWN  = -1,
	VIEW_META_DATA = 0,
	VIEW_EPUB,
	VIEW_PDF,
	VIEW_MAX
}view_type;

class CReaderView : public CExDialog
{
	DECLARE_DYNAMIC(CReaderView)

public:
	CReaderView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CReaderView();
	virtual void ViewFile(LPCTSTR szpdf) ;
	view_type  m_vType;
// �Ի�������
	enum { IDD = IDD_READER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};



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
