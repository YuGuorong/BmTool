#pragma once

#include "ExDialog.h"
// CCovtMainDlg �Ի���

class CCovtMainDlg : public CExDialog
{
	DECLARE_DYNAMIC(CCovtMainDlg)

public:
	CCovtMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCovtMainDlg();

// �Ի�������
	enum { IDD = IDD_CONVT_MAIN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
