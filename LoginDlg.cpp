// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iMobleAgent.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// CLoginDlg dialog

IMPLEMENT_DYNAMIC(CLoginDlg, CExDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CLoginDlg::IDD, pParent)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CExDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CLoginDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CLoginDlg message handlers
void CLoginDlg::OnBnClickedButton1()
{
	::AfxGetMainWnd()->PostMessage(WM_LOGIN_STATE, LOGIN_ST_SUCESS, 0);	
}
