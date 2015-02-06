// CovtMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CovtMainDlg.h"
#include "afxdialogex.h"


// CCovtMainDlg 对话框

IMPLEMENT_DYNAMIC(CCovtMainDlg, CExDialog)

CCovtMainDlg::CCovtMainDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CCovtMainDlg::IDD, pParent)
{

}

CCovtMainDlg::~CCovtMainDlg()
{
}

void CCovtMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCovtMainDlg, CExDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCovtMainDlg 消息处理程序


BOOL CCovtMainDlg::OnInitDialog()
{
	CExDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CCovtMainDlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类

	CExDialog::OnOK();
}


BOOL CCovtMainDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CExDialog::PreTranslateMessage(pMsg);
}


void CCovtMainDlg::OnDestroy()
{
	CExDialog::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


void CCovtMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
}


void CCovtMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CExDialog::OnTimer(nIDEvent);
}
