// ReaderView.cpp : 实现文件
//

#include "stdafx.h"
#include "ReaderView.h"
#include "afxdialogex.h"
#include "EpubReaderDlg.h"


#define WM_EPUB_INFO (WM_APP+529)
#define PM_EPUB_SUB_WINDOW  0
#define PM_EPUB_CUR_PAGE    1
#define PM_EPUB_TOTAL_PAGE  2
#define PM_EPUB_GOTO_PAGE   3

#define TMID_PING_EPUB_PROC  1103

IMPLEMENT_DYNAMIC(CEpubReaderDlg ,CReaderView)

CEpubReaderDlg::CEpubReaderDlg(CWnd* pParent /*=NULL*/)
: CReaderView( pParent)
{
	m_hReadProc = INVALID_HANDLE_VALUE;
	m_hwdReader = NULL;
	m_nCurPage = -1;
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
	ON_MESSAGE(WM_EPUB_INFO, &CEpubReaderDlg::OnProjStateChange)		//自定义托盘事件
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
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
		m_hReadProc = CUtil::RunProc(strExe, strParm, strExePath, FALSE);
	}
}

LRESULT CEpubReaderDlg::OnProjStateChange(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case PM_EPUB_CUR_PAGE:
		if (m_nCurPage != lParam)
		{
			m_nCurPage = lParam;			
			::PostMessage(GetParent()->GetParent()->GetSafeHwnd(), WM_PDF_PAGE, m_nCurPage, m_nCurPage);
		}
		m_nCurPage = lParam;
		break;
	case PM_EPUB_SUB_WINDOW:
		if (lParam && ::IsWindow((HWND)lParam))
		{
			m_hwdReader = (HWND)lParam;
			::PostMessage(m_hwdReader, WM_EPUB_INFO, PM_EPUB_CUR_PAGE, (LPARAM)GetSafeHwnd());
			SetTimer(TMID_PING_EPUB_PROC, 300, NULL);
		}
		break;
	}
	return TRUE;
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

void CEpubReaderDlg::GotoPage(int pgnum)
{
	if (m_hwdReader && ::IsWindow(m_hwdReader))
	{
		::SendMessage(m_hwdReader, WM_EPUB_INFO, PM_EPUB_GOTO_PAGE, pgnum);
		::SendMessage(m_hwdReader, WM_PAINT, 0, 0);
		this->UpdateWindow();
	}
}

void CEpubReaderDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	
	switch (nIDEvent)
	{
	case TMID_PING_EPUB_PROC:
		if (m_hwdReader && ::IsWindow(m_hwdReader))
			::PostMessage(m_hwdReader, WM_EPUB_INFO, PM_EPUB_CUR_PAGE, (LPARAM)GetSafeHwnd());
		else
		{
			KillTimer(nIDEvent);
			m_hwdReader = NULL;
		}
		break;
	}

	CReaderView::OnTimer(nIDEvent);
}


void CEpubReaderDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CReaderView::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		if (m_hwdReader && ::IsWindow(m_hwdReader))
			::PostMessage(m_hwdReader, WM_PAINT, 0, 0);
	}
	// TODO:  在此处添加消息处理程序代码
}
