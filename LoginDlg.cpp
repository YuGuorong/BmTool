// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirPackge.h"
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
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CLoginDlg::OnBnClickedBtnLogin)
END_MESSAGE_MAP()


// CLoginDlg message handlers
void CLoginDlg::OnBnClickedButton1()
{
	//::AfxGetMainWnd()->PostMessage(WM_LOGIN_STATE, LOGIN_ST_SUCESS, 0);	
}


void CLoginDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


BOOL CLoginDlg::OnInitDialog()
{
	m_bTransprent = TRUE;
	CExDialog::OnInitDialog();
	GetDlgItem(IDC_EDIT_USER)->SetWindowText(g_pSet->m_strUserName);
	

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE   _T("192.168.1.61")
}

//
INT GetJsonInt(CString &sjson, LPCTSTR skey, INT &val)
{
	CString strkey = skey;
	int npf = sjson.Find(skey);
	if (npf < 0) return -1;
	CString s = sjson.Right(sjson.GetLength() - npf - _tcslen(skey));
	s.TrimLeft(_T(" :"));
	_stscanf_s(s, _T("%d"), &val);	
	return 0;
}

INT GetJsonString(CString &sjson, LPCTSTR skey, CString &sval)
{
	CString strkey = skey;
	int npf = sjson.Find(skey);
	if (npf < 0) return -1;
	CString s = sjson.Right(sjson.GetLength() - npf - _tcslen(skey));
	s.TrimLeft(_T(" :\""));
	int nl = s.Find(_T(','));
	if (nl >= 0)
		sval = s.Left(nl);
	else
		sval = s;
	sval.TrimRight(_T("\",}"));	
	return 0;
}

#include "PackerProj.h"
//#define DEBUG_LOGIN 
//192.168.1.61  fixopen.xicp.net
void CLoginDlg::OnBnClickedBtnLogin()
{//{"state": 200, "sessionId": "12ea2d36e9b638e88887b0081deabeaec11d7bad", "id" : "33542438663913472"}
	char post_data[64] = ("{\"password\":\"admin\"}");
	CString str, strUrl, strdata, strResp, strUser;
	GetDlgItem(IDC_EDIT_USER)->GetWindowText(strUser);
	strUrl.Format(_T("/api/users/%s/sessions"), strUser);
	GetDlgItem(IDC_EDIT_PWD)->GetWindowText(str);
	strdata.Format(_T("{\"password\":\"%s\"}"), str);
	USES_CONVERSION;
	CHAR * pdata = W2A(strdata);	
	int nLogState = 0;
	CTime tmlogin = CTime::GetCurrentTime();
#ifdef DEBUG_LOGIN
	nLogState = 200;
#else
	HttpPost(g_pSet->m_strServerIP, g_pSet->m_nPort, strUrl, pdata, strdata.GetLength(), strResp);
	nLogState = 0;
	GetJsonInt(strResp, _T("\"state\""), nLogState);
#endif	
	if (nLogState == 200)
	{
		CPackerProj * proj = ::GetPackProj();
		proj->m_logState = 1;
#ifdef DEBUG_LOGIN
		proj->m_strSession = _T("12ea2d36e9b638e88887b0081deabeaec11d7bad");
		proj->m_strLogId = _T("33542438663913472");
#else
		GetJsonString(strResp, _T("\"sessionId\""), proj->m_strSession);
		GetJsonString(strResp, _T("\"id\""), proj->m_strLogId);
#endif
		proj->m_strLoginUser = strUser;
		proj->m_tmLogin = tmlogin;
		proj->SetProjStatus(LOGIN_PROJ);
	}
}
