// CovtMainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CovtMainDlg.h"
#include "afxdialogex.h"


// CCovtMainDlg �Ի���

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


// CCovtMainDlg ��Ϣ�������


BOOL CCovtMainDlg::OnInitDialog()
{
	CExDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CCovtMainDlg::OnOK()
{
	// TODO:  �ڴ����ר�ô����/����û���

	CExDialog::OnOK();
}


BOOL CCovtMainDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���

	return CExDialog::PreTranslateMessage(pMsg);
}


void CCovtMainDlg::OnDestroy()
{
	CExDialog::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
}


void CCovtMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
}


void CCovtMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CExDialog::OnTimer(nIDEvent);
}
