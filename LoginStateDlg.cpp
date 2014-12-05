// LoginStateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LoginStateDlg.h"
#include "afxdialogex.h"
#include "DirPackgeDlg.h"

// CLoginStateDlg 对话框

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
	ON_BN_CLICKED(IDC_BTN_LOGOUT, &CLoginStateDlg::OnBnClickedBtnLogout)
	ON_BN_CLICKED(ID_BTN_OK, &CLoginStateDlg::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// CLoginStateDlg 消息处理程序

static const INT btnids[] = { IDC_BTN_LOGOUT, ID_BTN_OK };

BOOL CLoginStateDlg::OnInitDialog()
{
	m_bTransprent = TRUE;
	CExDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_txtStatus = new CLink();
	m_txtStatus->SubclassDlgItem(IDC_ST_LOGIN_STATUS, this);
	m_txtStatus->SetTxtColor(RGB(0, 0, 0));
	m_txtStatus->SetWindowText(_T("Login sucess!"));

	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		m_pbtns[i] = new CSkinBtn();
		m_pbtns[i]->SubclassDlgItem(btnids[i], this);
		m_pbtns[i]->SetImage(IDB_BITMAP_SLIVE_BTN, 102, 26);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CLoginStateDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		delete m_pbtns[i] ;
	}
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

void LockSystem(BOOL block);
void CLoginStateDlg::OnBnClickedBtnLogout()
{
	::LockSystem(TRUE);
}


void CLoginStateDlg::OnBnClickedBtnOk()
{
	// TODO:  在此添加控件通知处理程序代码
	this->OnOK();
}
