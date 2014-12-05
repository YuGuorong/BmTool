// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirPackge.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg dialog

IMPLEMENT_DYNAMIC(CSettingDlg, CExDialog)

static INT proxy_ctrls_id[] = { IDC_EDIT_PROXY_IP, IDC_EDIT_PROXY_PORT, IDC_EDIT_PROXY_USER, IDC_EDIT_PROXY_PWD,
IDC_ST_PROXY_IP, IDC_ST_PROXY_PORT, IDC_ST_PROXY_USER, IDC_ST_PROXY_PWD };

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CSettingDlg::IDD, pParent)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROXY_PORT, m_nProxyPort);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CExDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &CSettingDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_CHK_PROXY, &CSettingDlg::OnBnClickedChkProxy)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_CANCLE, &CSettingDlg::OnBnClickedBtnOk2)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSettingDlg message handlers
static const INT btnids[] = { IDC_BTN_OK, IDC_BTN_CANCLE };


BOOL CSettingDlg::OnInitDialog()
{
	m_bTransprent = TRUE;
	CExDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_SERVER_IP)->SetWindowText(g_pSet->m_strServerIP);

	int txtids[] = { IDC_ST_PROXY_IP, IDC_ST_PROXY_PORT, IDC_ST_PROXY_USER, IDC_ST_PROXY_PWD };
	SubTextItems(txtids, sizeof(txtids) / sizeof(int), NULL, NULL);
	
	m_ProxyBox.SubclassDlgItem(IDC_CHK_PROXY, this);
	m_ProxyBox.SetCheck(g_pSet->m_bEnProxy);

	GetDlgItem(IDC_EDIT_PROXY_IP)->SetWindowText(g_pSet->m_strProxyIP);
	GetDlgItem(IDC_EDIT_PROXY_USER)->SetWindowText(g_pSet->m_strProxyUser);
	GetDlgItem(IDC_EDIT_PROXY_PWD)->SetWindowText(g_pSet->m_strProxyPwd);
	m_nPort = g_pSet->m_nPort;
	m_nProxyPort = g_pSet->m_nProxyPort;
	this->UpdateData(false);

	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		m_pbtns[i] = new CSkinBtn();
		m_pbtns[i]->SubclassDlgItem(btnids[i], this);
		m_pbtns[i]->SetImage(IDB_BITMAP_SLIVE_BTN, 102, 26);
	}

	

	for (int i = 0; i < sizeof(proxy_ctrls_id) / sizeof(INT); i++)
	{
		CWnd * pwnd = GetDlgItem(proxy_ctrls_id[i]);
		pwnd->ShowWindow(g_pSet->m_bEnProxy ? SW_SHOW : SW_HIDE);
		//pwnd->EnableWindow(benProxy);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		delete m_pbtns[i];
	}
}


void SwitchBackDlg(CWnd * pwndHide);
void CSettingDlg::OnOK()
{
	SwitchBackDlg(this);
}

void CSettingDlg::OnBnClickedBtnOk()
{
	// TODO:  在此添加控件通知处理程序代码 http://fixopen.xicp.net:8091
	CString str;
	UpdateData();
	GetDlgItem(IDC_EDIT_SERVER_IP)->GetWindowText(g_pSet->m_strServerIP);
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(str);
	g_pSet->m_nPort = _ttoi(str);
	GetDlgItem(IDC_EDIT_PROXY_IP)->GetWindowText(g_pSet->m_strProxyIP);
	GetDlgItem(IDC_EDIT_PROXY_USER)->GetWindowText(g_pSet->m_strProxyUser);
	GetDlgItem(IDC_EDIT_PROXY_PWD)->GetWindowText(g_pSet->m_strProxyPwd);
	GetDlgItem(IDC_EDIT_PROXY_PORT)->GetWindowText(str);
	g_pSet->m_nProxyPort = _ttoi(str);
	g_pSet->m_bEnProxy = m_ProxyBox.GetCheck();

	g_pSet->Save();

	this->OnOK();

}
void InvalidateMainRect();
void CSettingDlg::EnableProxy(BOOL benProxy)
{
	
	for (int i = 0; i < sizeof(proxy_ctrls_id) / sizeof(INT); i++ )
	{
		CWnd * pwnd = GetDlgItem(proxy_ctrls_id[i]);
		pwnd->ShowWindow(benProxy? SW_SHOW: SW_HIDE);
		//pwnd->EnableWindow(benProxy);
	}
	InvalidateMainRect();

}


void CSettingDlg::OnBnClickedChkProxy()
{
	UpdateData();
	CButton * pbox = (CButton *)GetDlgItem(IDC_CHK_PROXY);
	EnableProxy(pbox->GetCheck());
	//this->Invalidate(1);
	//this->UpdateWindow();

}


void CSettingDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	CExDialog::OnPaint();
}


void CSettingDlg::OnBnClickedBtnOk2()
{
	this->OnOK();
}


void CSettingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CExDialog::OnShowWindow(bShow, nStatus);

	// TODO:  在此处添加消息处理程序代码
}


