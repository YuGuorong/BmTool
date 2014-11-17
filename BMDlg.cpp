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
	DDX_Control(pDX, IDC_TREE_META, m_trMeta);
	DDX_Control(pDX, IDC_TREE_DIR, m_trDir);
	DDX_Control(pDX, IDC_LIST_RES, m_listRes);
}


BEGIN_MESSAGE_MAP(CBMDlg, CExDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
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


void CBMDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);
	if( ::IsWindow(m_trDir.GetSafeHwnd() ) )
	{
		CRect r;
		m_trDir.GetWindowRect(r);
		this->ScreenToClient(r);
		r.bottom = cy -1;
		m_trDir.MoveWindow(r);
		m_listRes.GetWindowRect(r);
		this->ScreenToClient(r);
		r.right = cx -1;
		r.MoveToY(cy - r.Height() -1);
		m_listRes.MoveWindow(r);
		r.bottom = r.top -1;
		r.top = 8;
		r.right = cx - 1;
		m_FrameMeta.MoveWindow(r);
		r.OffsetRect(-r.left, -r.top );
		m_pMetaDlg->MoveWindow(r);
	}
}


void CBMDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	
	if( m_pMetaDlg && ::IsWindow(m_pMetaDlg->GetSafeHwnd()))
	{
		m_pMetaDlg->DestroyWindow();
		delete m_pMetaDlg;
		m_pMetaDlg = NULL;
	}

	// TODO: Add your message handler code here
}
