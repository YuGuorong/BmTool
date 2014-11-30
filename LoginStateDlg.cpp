// LoginStateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LoginStateDlg.h"
#include "afxdialogex.h"


// CLoginStateDlg �Ի���

IMPLEMENT_DYNAMIC(CLoginStateDlg, CExDialog)

CLoginStateDlg::CLoginStateDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CLoginStateDlg::IDD, pParent)
{
	m_txtStatus = NULL;
}

CLoginStateDlg::~CLoginStateDlg()
{
}

void CLoginStateDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_LOGIN_STATUS, m_strStatus);
	
}


BEGIN_MESSAGE_MAP(CLoginStateDlg, CExDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CLoginStateDlg ��Ϣ�������


BOOL CLoginStateDlg::OnInitDialog()
{
	m_bTransprent = TRUE;
	CExDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_txtStatus = new CLink();
	m_txtStatus->SubclassDlgItem(IDC_ST_LOGIN_STATUS, this);
	m_txtStatus->SetTxtColor(RGB(0, 0, 0));
	m_txtStatus->SetWindowText(_T("Login sucess!"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CLoginStateDlg::OnDestroy()
{
	CExDialog::OnDestroy();

	if (m_txtStatus)
		FreePtr(m_txtStatus);
}


void CLoginStateDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);
}


void CLoginStateDlg::OnOK()
{
	::SwitchBackDlg(this);
}


void CLoginStateDlg::SetLoginStatuText(LPCTSTR szStatus)
{
	m_txtStatus->SetWindowText(szStatus);
}