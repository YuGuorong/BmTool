#pragma once
#include "ExDialog.h"

// CLoginStateDlg �Ի���

class CLoginStateDlg : public CExDialog
{
	DECLARE_DYNAMIC(CLoginStateDlg)

public:
	CLoginStateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginStateDlg();
	void SetLoginStatuText(LPCTSTR szStatus);
	CLink * m_txtStatus;

// �Ի�������
	enum { IDD = IDD_LOGIN_STATUS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CString m_strStatus;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
};
