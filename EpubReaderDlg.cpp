// ReaderView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ReaderView.h"
#include "afxdialogex.h"
#include "EpubReaderDlg.h"

IMPLEMENT_DYNAMIC(CEpubReaderDlg ,CReaderView)

CEpubReaderDlg::CEpubReaderDlg(CWnd* pParent /*=NULL*/)
: CReaderView( pParent)
{
	m_hReadProc = INVALID_HANDLE_VALUE;
	m_hwdReader = NULL;
	m_vType = VIEW_EPUB;
}

CEpubReaderDlg::~CEpubReaderDlg()
{
	if (m_hReadProc != INVALID_HANDLE_VALUE)
	{
		TerminateProcess(m_hReadProc, 0);
		CloseHandle(m_hReadProc);
		m_hReadProc = INVALID_HANDLE_VALUE;
	}
}

void CEpubReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CReaderView::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEpubReaderDlg,CReaderView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


//// CReaderView ��Ϣ�������
void CEpubReaderDlg::ViewFile(LPCTSTR szpdf)
{
	if (g_pSet)
	{
		CString strExePath = g_pSet->strCurPath;
		strExePath += _T("MyReader");
		CString strExe = strExePath;
		strExe += _T("\\MyReader.exe");
		CString strParm;
		strParm.Format(_T("\"%s\" --parent=%d"), szpdf, (int)(this->GetSafeHwnd()));
		if (m_hReadProc != INVALID_HANDLE_VALUE)
		{
			TerminateProcess(m_hReadProc, 0);
			CloseHandle(m_hReadProc);
			m_hReadProc = INVALID_HANDLE_VALUE;
		}
		m_hReadProc = CUtil::RunProc(strExe, strParm, strExePath);
	}
}

BOOL CEpubReaderDlg::OnInitDialog()
{
	CReaderView::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CEpubReaderDlg::OnSize(UINT nType, int cx, int cy)
{
	CReaderView::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
}


void CEpubReaderDlg::OnDestroy()
{
	CReaderView::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
	if (m_hReadProc != INVALID_HANDLE_VALUE)
	{
		TerminateProcess(m_hReadProc, 0);
		CloseHandle(m_hReadProc);
		m_hReadProc = INVALID_HANDLE_VALUE;
	}

}
