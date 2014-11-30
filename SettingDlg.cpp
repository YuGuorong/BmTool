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
}


BEGIN_MESSAGE_MAP(CSettingDlg, CExDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &CSettingDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_CHK_PROXY, &CSettingDlg::OnBnClickedChkProxy)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_OK2, &CSettingDlg::OnBnClickedBtnOk2)
END_MESSAGE_MAP()


// CSettingDlg message handlers


BOOL CSettingDlg::OnInitDialog()
{
	m_bTransprent = TRUE;
	CExDialog::OnInitDialog();
//	::AfxGetApp()->SetRegistryKey( _T("AppSettings"));
	


	GetDlgItem(IDC_EDIT_SERVER_IP)->SetWindowText(g_pSet->m_strServerIP);
	CString str; str.Format(_T("%d"), g_pSet->m_nPort);
	GetDlgItem(IDC_EDIT_PORT)->SetWindowText(str);

	int txtids[] = { IDC_ST_PROXY_IP, IDC_ST_PROXY_PORT, IDC_ST_PROXY_USER, IDC_ST_PROXY_PWD };
	SubTextItems(txtids, sizeof(txtids) / sizeof(int), NULL, NULL);
	
	m_ProxyBox.SubclassDlgItem(IDC_CHK_PROXY, this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
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
	
	GetDlgItem(IDC_EDIT_SERVER_IP)->GetWindowText(g_pSet->m_strServerIP);
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(str);
	g_pSet->m_nPort = _ttoi(str);
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
	this->Invalidate(1);
	this->UpdateWindow();

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
