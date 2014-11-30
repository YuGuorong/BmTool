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
	m_vType = VIEW_EMPTY;
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
	ON_STN_CLICKED(IDC_ST_SELECT_BOOK, &CReaderView::OnStnClickedStSelectBook)
END_MESSAGE_MAP()



BOOL CReaderView::OnInitDialog()
{
	CExDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	INT ids[] = { IDC_ST_SELECT_BOOK };
	COLORREF clrTxt[] = {RGB(0,46,200)};
	if (SubTextItems(ids, 1, NULL, clrTxt))
	{
		CLink * plk = m_atxts.GetAt(m_atxts.GetCount() - 1);
		HCURSOR hCurHand = LoadCursor(NULL, IDC_HAND);
		plk->SetLinkCursor(hCurHand);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CReaderView::OnDestroy()
{
	CExDialog::OnDestroy();

}


void CReaderView::OnStnClickedStSelectBook()
{
	// TODO:  在此添加控件通知处理程序代码
	CPackerProj * pProj = ::GetPackProj();
	if (pProj)
	{
		pProj->CreateProj(NULL);
	}
	
}
