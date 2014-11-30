// ReaderView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ReaderView.h"
#include "afxdialogex.h"


// CReaderView �Ի���

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	INT ids[] = { IDC_ST_SELECT_BOOK };
	COLORREF clrTxt[] = {RGB(0,46,200)};
	if (SubTextItems(ids, 1, NULL, clrTxt))
	{
		CLink * plk = m_atxts.GetAt(m_atxts.GetCount() - 1);
		HCURSOR hCurHand = LoadCursor(NULL, IDC_HAND);
		plk->SetLinkCursor(hCurHand);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

void CReaderView::OnDestroy()
{
	CExDialog::OnDestroy();

}


void CReaderView::OnStnClickedStSelectBook()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CPackerProj * pProj = ::GetPackProj();
	if (pProj)
	{
		pProj->CreateProj(NULL);
	}
	
}
