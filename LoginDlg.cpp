// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirPackge.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "AsyncHttp.h"
#include "PackerProj.h"


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
	ON_WM_DESTROY()
	ON_MESSAGE(WM_HTTP_DONE, &CLoginDlg::OnHttpFinishMsg)		//�Զ����¼�
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
	
	m_pbtns = new CSkinBtn();
	m_pbtns->SubclassDlgItem(IDC_BTN_LOGIN, this);
	m_pbtns->SetImage(IDB_BITMAP_SLIVE_BTN, 102, 26);
	GetDlgItem(IDC_EDIT_PWD)->SetWindowText(_T("admin"));
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

//#ifdef DEBUG
//#define DEBUG_LOGIN 
//#endif
//192.168.1.61  fixopen.xicp.net
void TestGetSocket()
{
//	CGetHttp *ot = new CGetHttp(_T("www.baidu.com"), _T("/img/bdlogo.png"));
//	ot->GetFile(_T("C:\\temp\\bdlogo.png"));

}


LRESULT CLoginDlg::OnHttpFinishMsg(WPARAM wParam, LPARAM lParam)
{
	CAsyncHttp * pHttp = (CAsyncHttp*)wParam;
	BOOL  stat = (BOOL)lParam;
	if (stat)
	{
		int len = pHttp->GetHttpHeader(pHttp->m_szRespHeader);
		if (pHttp->m_szRespHeader.Find(" 200 OK") >= 0)
		{
			pHttp->GetBody();
			CString strResp; QUtf2Unc((LPCSTR)(pHttp->m_pBody), strResp);
			CPackerProj * proj = ::GetPackProj();
			proj->m_logState = 1;
#ifdef DEBUG_LOGIN
			proj->m_strSession = _T("12ea2d36e9b638e88887b0081deabeaec11d7bad");
			proj->m_strLogId = _T("33542438663913472");
#else
			GetJsonString(strResp, _T("\"sessionId\""), proj->m_strSession);
			GetJsonString(strResp, _T("\"id\""), proj->m_strLogId);
#endif
			BOOL GetLockState();
			if (!GetLockState())
				proj->SetProjStatus(LOGIN_PROJ);
			void LockSystem(BOOL block);
			LockSystem(FALSE);
			stat = TRUE;
		}
	}
	if (stat == FALSE)
	{
		MessageBox(_T("��¼ʧ��"), _T("��¼"));
	}
	GetDlgItem(IDC_BTN_LOGIN)->SetWindowText(TEXT("��  ¼"));
	EndWaitCursor();
	::AfxGetMainWnd()->EndWaitCursor();
 
	pHttp->stop();
	delete pHttp;
	return 0;
}

void CLoginDlg::OnBnClickedBtnLogin()
{//{"state": 200, "sessionId": "12ea2d36e9b638e88887b0081deabeaec11d7bad", "id" : "33542438663913472"}
	BeginWaitCursor();
	::AfxGetMainWnd()->BeginWaitCursor();
	CWnd * pwnd = GetDlgItem(IDC_BTN_LOGIN);
	CString strtxt;
	pwnd->GetWindowText(strtxt);
	if (strtxt.Compare(TEXT("��¼��...")) == 0)
		return;
	pwnd->SetWindowText(TEXT("��¼��..."));
	void TestSocket();
	TestGetSocket();
	//return;
//	TestSocket();
	CString str, strUrl, strdata, strResp, strUser;
	GetDlgItem(IDC_EDIT_USER)->GetWindowText(strUser);
	strUrl.Format(_T("/api/users/%s/sessions"), strUser);
	GetDlgItem(IDC_EDIT_PWD)->GetWindowText(str);
	strdata.Format(_T("{\"password\":\"%s\"}"), str);

	CHttpPost * opost = new CHttpPost(g_pSet->m_strServerIP, strUrl, this, g_pSet->m_nPort);
	QUnc2Utf(strdata, m_strHttpData);
	opost->SendFile((LPCSTR)m_strHttpData, m_strHttpData.GetLength(), TEXT("application/json"));
	CPackerProj * proj = ::GetPackProj();
	proj->m_strLoginUser = strUser;
	CTime tmlogin = CTime::GetCurrentTime();
	proj->m_tmLogin = tmlogin;
	return;

//	USES_CONVERSION;
//	CHAR * pdata = W2A(strdata);	
//	int nLogState = 0;
//	CTime tmlogin = CTime::GetCurrentTime();
//	
//#ifdef DEBUG_LOGIN
//	nLogState = 200;
//#else
//	HttpPost(g_pSet->m_strServerIP, g_pSet->m_nPort, strUrl, pdata, strdata.GetLength(), strResp);
//	nLogState = 0;
//	GetJsonInt(strResp, _T("\"state\""), nLogState);
//#endif	
//	if (nLogState == 200)
//	{
//		CPackerProj * proj = ::GetPackProj();
//		proj->m_logState = 1;
//#ifdef DEBUG_LOGIN
//		proj->m_strSession = _T("12ea2d36e9b638e88887b0081deabeaec11d7bad");
//		proj->m_strLogId = _T("33542438663913472");
//#else
//		GetJsonString(strResp, _T("\"sessionId\""), proj->m_strSession);
//		GetJsonString(strResp, _T("\"id\""), proj->m_strLogId);
//#endif
//		proj->m_strLoginUser = strUser;
//		proj->m_tmLogin = tmlogin;
//		BOOL GetLockState();
//		if (!GetLockState())
//			proj->SetProjStatus(LOGIN_PROJ);
//		void LockSystem(BOOL block);
//		LockSystem(FALSE);
//	}
//	else
//	{
//		MessageBox(_T("��¼ʧ��"), _T("��¼"));
//	}
//	EndWaitCursor();
}


void CLoginDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	delete m_pbtns;
	// TODO:  �ڴ˴�������Ϣ�����������
}
