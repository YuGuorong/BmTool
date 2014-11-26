#pragma once
#include "ExDialog.h"

// CResManDlg �Ի���

class CManifestMan
{
public:
	int AddRes(CString strRes, void * prelation);
	int Save();
public:
	CManifestMan();
	~CManifestMan();
};


class CResManDlg : public CExDialog
{
	DECLARE_DYNAMIC(CResManDlg)

public:
	CResManDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CResManDlg();

// �Ի�������
	enum { IDD = IDD_RESMAN_DLG };

protected:
	CSkinBtn * m_pbtns[8];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnLocal();
};
