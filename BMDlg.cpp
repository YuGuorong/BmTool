// BMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirPackge.h"
#include "BMDlg.h"
#include "afxdialogex.h"
#include "PdfReaderDlg.h"


#define GetMetaWnd() ((CMetaDlg*)m_pViews[VIEW_META_DATA])
// CBMDlg dialog

IMPLEMENT_DYNAMIC(CBMDlg, CExDialog)
CReaderView * CBMDlg::GetCurView()
{
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		if (m_pViews[i]->m_vType == m_proj->m_state )
			return m_pViews[i];

	}	
	return NULL; //m_pViews[0]
}
CBMDlg::CBMDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CBMDlg::IDD, pParent)
{
	memset(m_pViews, 0, sizeof(m_pViews));
}

CBMDlg::~CBMDlg()
{
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		if (m_pViews[i]) FreePtr(m_pViews[i]);		
	} 
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
	ON_MESSAGE(WM_VIEW_PROJ, &CBMDlg::OnViewProjMsg)
END_MESSAGE_MAP()


// CBMDlg message handlers
view_type CBMDlg::ChangeView(view_type vtype)  //view_type
{
	view_type oldview = m_cur_view;

	m_pViews[vtype]->ShowWindow(SW_SHOW);
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		if (i != vtype)
			m_pViews[i]->ShowWindow(SW_HIDE);

	}
	m_cur_view = vtype;
	return oldview;
}

BOOL CBMDlg::OnInitDialog()
{
	CExDialog::OnInitDialog();

	CREATE_SUB_WND(m_pViews[VIEW_META_DATA], CMetaDlg, &m_FrameMeta);
	CREATE_SUB_WND(m_pViews[VIEW_EPUB], CReaderView, &m_FrameMeta);
	CREATE_SUB_WND(m_pViews[VIEW_PDF], CPdfReaderDlg, &m_FrameMeta);
	::SetForegroundWindow(m_pViews[VIEW_EPUB]->GetSafeHwnd());
	CRect r;
	m_FrameMeta.GetWindowRect(r);
	m_FrameMeta.ScreenToClient(r);
	r.bottom += 100;
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		m_pViews[i]->MoveWindow(r, FALSE);
	}
	
	ChangeView(VIEW_PDF);
	

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
		for (int i = 0; i < (int)VIEW_MAX; i++)
		{
			m_pViews[i]->MoveWindow(r, TRUE);
		}
	}
}


void CBMDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		if (m_pViews[i] && ::IsWindow(m_pViews[i]->GetSafeHwnd()))
		{
			m_pViews[i]->DestroyWindow();
			FreePtr(m_pViews[i]);
		}
	}
	// TODO: Add your message handler code here
}

LRESULT CBMDlg::OnViewProjMsg(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case NEW_PROJ:
		GetCurView()->ViewFile(m_proj->m_szTarget);
		break;
	case SAVE_PROJ:
		GetMetaWnd()->SaveMetaData(m_proj);
	default:
		break;
	}
	return 0;
}