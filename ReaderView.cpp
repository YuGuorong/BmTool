// ReaderView.cpp : 实现文件
//

#include "stdafx.h"
#include "ReaderView.h"
#include "afxdialogex.h"


// CReaderView 对话框

IMPLEMENT_DYNAMIC(CReaderView, CExDialog)

CReaderView::CReaderView(CWnd* pParent /*=NULL*/)
	: CExDialog(CReaderView::IDD, pParent)
{
	m_vType = VIEW_UNKNOWN;
}

CReaderView::~CReaderView()
{

}

void CReaderView::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CReaderView, CExDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


//// CReaderView 消息处理程序
void CReaderView::ViewFile(LPCTSTR szpdf)
{

}

BOOL CReaderView::OnInitDialog()
{
	CExDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CReaderView::OnDestroy()
{
	CExDialog::OnDestroy();

}
///////////////===================================================================
//
// CReaderView 对话框

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


//// CReaderView 消息处理程序
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

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CEpubReaderDlg::OnSize(UINT nType, int cx, int cy)
{
	CReaderView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
}


void CEpubReaderDlg::OnDestroy()
{
	CReaderView::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
	if (m_hReadProc != INVALID_HANDLE_VALUE)
	{
		TerminateProcess(m_hReadProc, 0);
		CloseHandle(m_hReadProc);
		m_hReadProc = INVALID_HANDLE_VALUE;
	}

}
