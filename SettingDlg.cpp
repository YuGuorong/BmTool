// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iMobleAgent.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg dialog

IMPLEMENT_DYNAMIC(CSettingDlg, CExDialog)

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
END_MESSAGE_MAP()


// CSettingDlg message handlers
