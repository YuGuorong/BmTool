#pragma once

#include "ExDialog.h"
#include "MetaDlg.h"
#include "afxwin.h"
// CBMDlg dialog

class CBMDlg : public CExDialog
{
	DECLARE_DYNAMIC(CBMDlg)

public:
	CBMDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBMDlg();

// Dialog Data
	enum { IDD = IDD_BMDLG };

	CExDialog * m_pMetaDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_FrameMeta;
};
