// PdfReaderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PdfReaderDlg.h"
#include "afxdialogex.h"


extern "C" {
	HANDLE MfcPdf(HWND hparent, const wchar_t * wfilename);
	void FreePdf(HANDLE ctx);
	void OnPdfFrameSizing(HWND hwnd);
	void OnPdfFrameSize(HWND hwnd, int wParam);
	int OnPdfFrameClose(HWND hwnd);
	HWND GetViewHand();
}


// CPdfReaderDlg 对话框

IMPLEMENT_DYNAMIC(CPdfReaderDlg, CReaderView)

CPdfReaderDlg::CPdfReaderDlg(CWnd* pParent /*=NULL*/)
: CReaderView( pParent)
{
	m_vType = VIEW_PDF;
}

CPdfReaderDlg::~CPdfReaderDlg()
{
}

void CPdfReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CReaderView::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPdfReaderDlg, CReaderView)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_APP + 5, &CPdfReaderDlg::OnViewFocus)
END_MESSAGE_MAP()


// CPdfReaderDlg 消息处理程序


BOOL CPdfReaderDlg::OnInitDialog()
{
	CReaderView::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_hpdf = NULL;
	CFileDialog fdlg(TRUE, 0, _T("*.pdf"), OFN_HIDEREADONLY,
		_T("PFD文件|*.pdf|所有文件|*.*||"), this);
	if (fdlg.DoModal() == IDOK)
	{
		m_hpdf = MfcPdf(this->GetSafeHwnd(), fdlg.GetPathName());
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CPdfReaderDlg::OnSize(UINT nType, int cx, int cy)
{
	CReaderView::OnSize(nType, cx, cy);

	OnPdfFrameSize(this->GetSafeHwnd(), nType);
}


void CPdfReaderDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CReaderView::OnSizing(fwSide, pRect);

	OnPdfFrameSizing(this->GetSafeHwnd());
}


BOOL CPdfReaderDlg::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_NOTIFY:
	case WM_COMMAND:
		return ::SendMessage(GetViewHand(), pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_CLOSE:
		OnPdfFrameClose(GetSafeHwnd());
		break;
	case WM_KEYDOWN:
	case WM_CHAR:
		::SendMessage(GetViewHand(), pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	}
	return CReaderView::PreTranslateMessage(pMsg);
}


void CPdfReaderDlg::OnDestroy()
{
	CReaderView::OnDestroy();
	if (m_hpdf)
		FreePdf(m_hpdf);
}



LRESULT CPdfReaderDlg::OnViewFocus(WPARAM wParam, LPARAM lParam)
{
	::SetFocus(GetViewHand());
	return FALSE;
}

void CPdfReaderDlg::OnSetFocus(CWnd* pOldWnd)
{
	CReaderView::OnSetFocus(pOldWnd);

	// TODO:  在此处添加消息处理程序代码
}
