#pragma once

#include "ExDialog.h"
#include "PackerProj.h"
// CReaderView �Ի���


typedef struct tagViewInfo
{
	CString strAuthor;
	UINT  pagecount;
	UINT  wordcount;
	UINT  fsize;
	CString strCreateTime;
	CString strModifyTime;
}VIEW_INFO;

class CReaderView : public CExDialog
{
	DECLARE_DYNAMIC(CReaderView)

public:
	CReaderView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CReaderView();
	virtual void ViewFile(LPCTSTR szpdf){};
	virtual void GetInfo(VIEW_INFO * info){};
	virtual int  GetPageInfo(INT *cur, INT *count){ return 0; };
	virtual void GotoPage(int pgnum){};
	virtual void AddResource(LPCTSTR szResFile){};
	virtual void Save(){};
	virtual void GetBookInfo(LPCTSTR skey, CString &str){};
	view_type  m_vType;
	
// �Ի�������
	enum { IDD = IDD_READER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnStnClickedStSelectBook();
};


