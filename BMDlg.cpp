// BMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iMobleAgent.h"
#include "BMDlg.h"
#include "afxdialogex.h"


// CBMDlg dialog

IMPLEMENT_DYNAMIC(CBMDlg, CExDialog)

CBMDlg::CBMDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CBMDlg::IDD, pParent)
{

}

CBMDlg::~CBMDlg()
{
}

void CBMDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAME_META_DETAIL, m_FrameMeta);
}


BEGIN_MESSAGE_MAP(CBMDlg, CExDialog)
END_MESSAGE_MAP()


// CBMDlg message handlers


BOOL CBMDlg::OnInitDialog()
{
	CExDialog::OnInitDialog();

	CREATE_SUB_WND(m_pMetaDlg,CMetaDlg, &m_FrameMeta);
	CRect r;
	m_FrameMeta.GetWindowRect(r);
	m_FrameMeta.ScreenToClient(r);
	m_pMetaDlg->MoveWindow(r, FALSE);
	m_pMetaDlg->ShowWindow(SW_SHOW);

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
