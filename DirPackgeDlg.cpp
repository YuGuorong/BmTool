
// DirPackgeDlg.cpp : implementation file
// Visual Studio Professional 2013  KEY（密钥）XDM3T-W3T3V-MGJWK-8BFVD-GVPKY

#include "stdafx.h"
#include "DirPackge.h"
#include "DirPackgeDlg.h"
#include "afxdialogex.h"
#include "LoginStateDlg.h"
#include "AsyncHttp.h"
#include "json/json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void NewProcessWnd(int max, int min);
void FreeProcessWnd();


class CAboutDlg : public CExDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CExDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog()
{
	BOOL ret = CExDialog::OnInitDialog();
	CString str; str.LoadString(IDS_VERNDOR_NAME);
	SetWindowText(str);
	for (int i = 0; i < m_atxts.GetCount(); i++)
	{
		m_atxts[i]->GetWindowText(str);
		if (str.Compare(_T("about")) == 0)
		{
			str.LoadString(IDS_VERNDOR_NAME);
			CString sinfo;
			sinfo.Format(_T("%s 版本：%s"), str, STR_VERSION);
			m_atxts[i]->SetWindowText(sinfo);
		}
	}
	return ret;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CExDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void ShowAboutDlg()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}
CPackerProj * GetPackProj()
{
	CDirPackgeDlg * pdlg = (CDirPackgeDlg *)::AfxGetMainWnd();
	return pdlg->m_Proj;
}
// CDirPackgeDlg dialog

CDirPackgeDlg::CDirPackgeDlg(CWnd* pParent /*=NULL*/)
	: CeExDialog(CDirPackgeDlg::IDD, pParent,EX_STRETCH_BK|EX_FILL_BK)
{
	MyTracex("CDirPackgeDlg!\n");

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCurDlg = NULL;
	m_Proj = NULL;
	m_LogDlgIdx = 0;
	for (int i = 0; i < MAX_TAB_ITEM; i++)
	{
		m_pSubDlgs[i] = NULL;
		m_cTabBtns[i] = NULL;
	}
	m_ohttp.SetSize(1);
}

void CDirPackgeDlg::DoDataExchange(CDataExchange* pDX)
{
	CeExDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_FRAME, m_frame);
}

BEGIN_MESSAGE_MAP(CDirPackgeDlg, CeExDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_VIEW_PROJ, &CDirPackgeDlg::OnProjStateChange)		//自定义事件
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDirPackgeDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CDirPackgeDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_NEWPRJ, &CDirPackgeDlg::OnBnClickedBtnNewprj)
	ON_BN_CLICKED(IDC_BTN_SETTING, &CDirPackgeDlg::OnBnClickedBtnSetting)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDirPackgeDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_EXPLORE, &CDirPackgeDlg::OnBnClickedBtnExplore)
	ON_BN_CLICKED(IDC_BTN_EXPORT, &CDirPackgeDlg::OnBnClickedBtnExport)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CDirPackgeDlg::OnBnClickedBtnOpen)
END_MESSAGE_MAP()


// CDirPackgeDlg message handlers

BOOL CDirPackgeDlg::OnInitDialog()
{
	MyTracex("OnInitDialog!\n");
	SetWndStyle(0,RGB(202,200,237), RGB(149,176,199));
	m_Proj = new CPackerProj(this);
	MyTracex("Proj created!\n");
	CeExDialog::OnInitDialog();
	CString str; str.LoadString(IDS_VERNDOR_NAME);
	SetWindowText(str);
	m_BmState = BMST_LOGIN | BMST_OFFLINE;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	m_frame.SubclassDlgItem(IDC_FRAME, this);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	NewProcessWnd(100, 0);

	UINT btnids[][2] = { 
		{IDC_BTN_LOGIN,  IDB_PNG_LOGIN  },
		{IDC_BTN_NEWPRJ, IDB_PNG_NEWPRJ },
		{IDC_BTN_OPEN,   IDB_PNG_OPENPRJ},
		{IDC_BTN_EXPORT, IDB_PNG_EXPORT },
		{IDC_BTN_EXPLORE,IDB_PNG_EXPLORE},
		{IDC_BTN_SETTING,IDB_PNG_SETTING},
		{IDC_BTN_SAVE,   IDB_PNG_SAVEPRJ},
		{IDC_BTN_EXIT,   IDB_PNG_EXIT  } };
	for(int i=0; i<MAX_TAB_ITEM; i++)
	{
		m_cTabBtns[i] = new CGdipButton	;
		m_cTabBtns[i]->SubclassDlgItem(btnids[i][0], this);
		m_cTabBtns[i]->LoadStdImage(btnids[i][1], _T("PNG"));		
		//m_cTabBtns[i]->EnableButton(FALSE);
	}
	
	CRect rbt;
	m_cTabBtns[0]->GetWindowRect(rbt);
	rbt.MoveToXY(0,2);
	for(int i=0; i<MAX_TAB_ITEM; i++)
	{
		m_cTabBtns[i]->MoveWindow(rbt);
		rbt.OffsetRect(rbt.Width()+1, 0);
	}
	MyTracex("Buttons beauty!\n");
	
	//CREATE_SUB_WND(m_pSubDlgs[LOGIN_TAB],   CLoginDlg, &m_frame);
	CREATE_SUB_WND(m_pSubDlgs[BM_TAB],      CBMDlg,    &m_frame);
	MyTracex("BM_TAB created!\n");
	CREATE_SUB_WND(m_pSubDlgs[IMPORT_TAB],  CLoginDlg, &m_frame);
	MyTracex("IMPORT_TAB created!\n");
	CREATE_SUB_WND(m_pSubDlgs[EXPORT_TAB],  CLoginDlg, &m_frame);
	MyTracex("EXPORT_TAB created!\n");
	CREATE_SUB_WND(m_pSubDlgs[RES_EXP_TAB], CResManDlg, &m_frame);
	MyTracex("RES_EXP_TAB created!\n");
	CREATE_SUB_WND(m_pSubDlgs[SETTING_TAB], CSettingDlg, &m_frame);
	MyTracex("SETTING_TAB created!\n");

	CREATE_SUB_WND(m_plogDlgs[0], CLoginDlg, &m_frame);
	CREATE_SUB_WND(m_plogDlgs[1], CLoginStateDlg, &m_frame);
	MyTracex("LOGIN_TAB created!\n");
	m_pSubDlgs[LOGIN_TAB] = m_plogDlgs[m_LogDlgIdx];

	ShowWindow(SW_SHOWMAXIMIZED); 
	SwitchDlg(LOGIN_TAB);

	m_Proj->m_pMetaWnd = m_pSubDlgs[BM_TAB];
	((CBMDlg*)m_pSubDlgs[BM_TAB])->m_proj = (m_Proj);
	
	m_Proj->SetProjStatus(NONE_PROJ);
	MyTracex("Init done!\n");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDirPackgeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	CeExDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDirPackgeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CeExDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDirPackgeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDirPackgeDlg::OnBnClickedBtnExit()
{
	this->EndDialog(IDOK);
}



void CDirPackgeDlg::QueryClassType()
{
	if (m_ohttp.GetCount() <= 0 || m_ohttp[0]) return;
	CPackerProj * proj = ::GetPackProj();
	//::CreateDirectory(str, NULL);

	proj->m_strSession.TrimRight();
	TCHAR hKeepAlive[] = _T("Connection: keep-alive\r\n");
	TCHAR headerLanguage[] = _T("Accept-Language: zh-CN,zh;q=0.8\r\n");
	TCHAR headerEncoding[] = _T("Accept-Encoding: gzip, deflate, sdch\r\n");
	TCHAR headerContentType[] = _T("Referer: http://115.182.41.175/backstage/professional/professionalManagement.html\r\n"); //text/xml
	
	CString strSession;
	strSession.Format(_T("Cookie: sessionId=%s; uid=%s; token=11111\r\n"), proj->m_strSession, proj->m_strLogId);
	LPCTSTR   cookies[5];
	int hdrs = 0;
	cookies[hdrs++] = hKeepAlive;
	cookies[hdrs++] = headerLanguage;
	cookies[hdrs++] = headerEncoding;
	cookies[hdrs++] = headerContentType;
	cookies[hdrs++] = strSession;

	CGetHttp * pTask = new CGetHttp(g_pSet->m_strServerIP, _T("/api/subjects"), this, g_pSet->m_nPort, cookies, hdrs);

	pTask->GetFile();
	m_ohttp[0] = (pTask);
}

void CDirPackgeDlg::OnHttpObjProc(int idHttpObj, int stat)
{
	if (stat >= 0 && m_ohttp.GetCount() > idHttpObj && m_ohttp[idHttpObj])
	{
		CHttpPost * pHttp = (CHttpPost *)m_ohttp[idHttpObj];
		if (pHttp->m_pBody)
		{
			CString serr;
			CString stxt ;
			try{
				Json::Reader reader;
				Json::Value root;
				const char * pjs = (const char *)pHttp->m_pBody;
				if (reader.parse(pjs, root))
				{
					Json::Value sa_1 = root["data"];
					for (int i = 0; i < sa_1.size(); i++)
					{
						Json::Value scap = sa_1[i];
						CString str = qUtf2Unc(scap["caption"].asCString());
						stxt += str  ;

						Json::Value sa_2 = scap["childs"];
						for (int j = 0; j < sa_2.size(); j++)
						{
							CString str = qUtf2Unc(sa_2[j]["caption"].asCString());
							stxt += _T("\t") + str ;

							Json::Value sa_3 = sa_2[j]["childs"];
							for (int k = 0; k < sa_3.size(); k++)
							{
								CString str3 = qUtf2Unc(sa_3[k]["caption"].asCString());
								if (k)
									stxt += _T('，');
								else
									stxt += _T('\t');
								stxt += str3;
							}
							stxt += _T("\r\n");
						}
					}
				}
				CStringA s;
				QW2A(stxt, s);
				CDigest din;
				din.CalDigest((LPCSTR)s, s.GetLength());

				CPackerProj * proj = ::GetPackProj();
				if (proj->m_MetaExtMap.size() <= 0) return;
				if (proj->m_MetaExtMap.begin()->second->md5_MetaDetail.m_sDigest.Compare(din.m_sDigest) != 0)
				{
					CString spath = g_pSet->strCurPath + proj->m_MetaExtMap.begin()->first;
					CFile of;
					if (of.Open(spath, CFile::modeCreate | CFile::modeWrite))
					{
						of.Write(s, s.GetLength());
						of.Close();
					}
					if (MessageBox(_T("系统数据库有更新， 请重新登录"), _T("系统更新"), MB_OK) == IDOK)
					{
						ReStart();
						ExitProcess(-1);
					}					
				}
			}
			catch (...)
			{
				serr = _T("服务器返回内容不能解析");
				stat = -1;
			}
		}
	}
}

void CDirPackgeDlg::SwitchDlg(CExDialog * pnew)
{
	if (m_pCurDlg == pnew) return;
	if (m_pCurDlg)
	{
		m_pCurDlg->ShowWindow(SW_HIDE);
		CRect r;
		m_frame.GetWindowRect(r);
		this->ScreenToClient(r);
		this->InvalidateRect(r);
	}
	m_pCurDlg = pnew;
	m_pCurDlg->ShowWindow(SW_SHOW);
}


void CDirPackgeDlg::SwitchDlg(int id)
{
	SwitchDlg(m_pSubDlgs[id]);	
}

void CDirPackgeDlg::PopLastWnd(CExDialog * pHideWnd)
{
	if (m_wndStack.GetCount())
	{
		int last = m_wndStack.GetCount() - 1;
		CExDialog * pnew = m_wndStack.GetAt(last);
		SwitchDlg(pnew);
		m_wndStack.RemoveAt(last);
	}
}

void CDirPackgeDlg::PushCurWnd()
{
	bool bPushed = FALSE;
	for (int i = 0; i < m_wndStack.GetCount(); i++)
	{
		if (m_pCurDlg == m_wndStack[i])
		{
			m_wndStack.RemoveAt(i);
			break;
		}
	}
	m_wndStack.Add(m_pCurDlg);
}

void CDirPackgeDlg::LockMainWnd(BOOL block)
{
	int tab;
	int state = m_Proj->m_ProjState;
	if (block)
	{
		m_LogDlgIdx = 0;
		tab = LOGIN_TAB;
		state |= LOCKED;
		m_plogDlgs[1]->GetDlgItem(ID_BTN_OK)->EnableWindow(FALSE);
	}
	else
	{
		m_LogDlgIdx = 1;
		CString strinfo;
		CString strLogTm = m_Proj->m_tmLogin.Format(TIME_FMT);
		QueryClassType();
		strinfo.Format(_T(" %s 登录成功！（登录时间：%s)"), \
			m_Proj->m_strLoginUser, strLogTm);
		((CLoginStateDlg*)m_plogDlgs[1])->SetLoginStatuText(strinfo);
		m_plogDlgs[1]->GetDlgItem(ID_BTN_OK)->EnableWindow(TRUE);
		tab = BM_TAB;
	}
	m_pSubDlgs[LOGIN_TAB] = m_plogDlgs[m_LogDlgIdx];
	SwitchDlg(tab);
	SetWindowStatus(state);
}

void CDirPackgeDlg::SetWindowStatus(int proj_state)
{
	BOOL ben[MAX_TAB_ITEM];
	memset(ben, 0, sizeof(ben));
	ben[SETTING_TAB] = TRUE;
	ben[BTN_EXIT] = TRUE;
	m_Proj->m_bProjModified = FALSE; 
	if (proj_state & LOCKED)
	{
		m_LogDlgIdx = 0;
		m_pSubDlgs[LOGIN_TAB] = m_plogDlgs[m_LogDlgIdx];
		for (int i = 1; i < MAX_TAB_ITEM; i++)
		{
			m_cTabBtns[i]->EnableButton(ben[i]);
		}
		SwitchDlg(LOGIN_TAB);
		return;
	}
	
	if (proj_state & NONE_PROJ)
	{
		ben[LOGIN_TAB] = TRUE;
	}
	if (proj_state & (LOGIN_PROJ|CLOSE_PROJ))
	{
		ben[BM_TAB] = TRUE;
		ben[IMPORT_TAB] = TRUE;
		ben[RES_EXP_TAB] = TRUE;
	}
	if (proj_state & (NEW_PROJ|OPEN_PROJ))
	{
		ben[BM_TAB] = TRUE;
		ben[IMPORT_TAB] = TRUE;
		ben[RES_EXP_TAB] = TRUE;
		ben[BTN_SAVE] = TRUE;
		
	}

	for (int i = 1; i < MAX_TAB_ITEM; i++)
	{
		m_cTabBtns[i]->EnableButton(ben[i]);
	}
}

LRESULT CDirPackgeDlg::OnProjStateChange(WPARAM wParam, LPARAM lParam)
{
	SetWindowStatus(wParam);
	
	if (wParam & LOGIN_PROJ)
	{
	
	}
	return TRUE;
}

void CDirPackgeDlg::OnSize(UINT nType, int cx, int cy)
{
	CeExDialog::OnSize(nType, cx, cy);

	if (m_cTabBtns[0] == NULL) return;

	// TODO: Add your message handler code here
	CRect rbt, rc;
	this->GetWindowRect(rc);
	this->ScreenToClient(rc);
	//
	
	m_cTabBtns[0]->GetWindowRect(rbt);
	rbt.MoveToXY(0,2);
/*	reverse direction
	rbt.OffsetRect(rc.Width()  - rbt.Width()-2, 0);
	for(int i=MAX_TAB_ITEM-1; i>=0; i--)
	{
		m_cTabBtns[i] ->MoveWindow(rbt);
		rbt.OffsetRect(0-rbt.Width()-2, 0);
		//m_cTabBtns[i]->EnableButton(FALSE);
	}*/
	rc.top = rbt.bottom+2;
	rc.InflateRect(-1,-1);
	m_frame.MoveWindow(rc);
	rc.OffsetRect(-rc.left,-rc.top);
	m_pSubDlgs[BM_TAB]->MoveWindow(rc);
	m_pSubDlgs[RES_EXP_TAB]->MoveWindow(rc);

	CRect r;
	int tabid[] = {LOGIN_TAB,IMPORT_TAB, EXPORT_TAB, SETTING_TAB};
	for(int i=0; i<sizeof(tabid)/sizeof(int); i++)
	{
		m_pSubDlgs[tabid[i]]->GetWindowRect(r);
		r.MoveToXY(rc.Width()/2-r.Width()/2, rc.Height()/2 - r.Height()/2 - rbt.Height()/2);
		m_pSubDlgs[tabid[i]]->MoveWindow(r);
	}
	CExDialog * pwnd = m_plogDlgs[1 - m_LogDlgIdx];
	pwnd->GetWindowRect(r);
	r.MoveToXY(rc.Width() / 2 - r.Width() / 2, rc.Height() / 2 - r.Height() / 2 - rbt.Height() / 2);
	pwnd->MoveWindow(r);
	m_frame.GetWindowRect(r);
	this->ScreenToClient(r);
	this->InvalidateRect(r);
}


void CDirPackgeDlg::OnDestroy()
{
	for(int i=0 ;i<MAX_TAB_ITEM; i++)
	{
		if( m_pSubDlgs[i] && ::IsWindow(m_pSubDlgs[i]->GetSafeHwnd() ) )
		{
			m_pSubDlgs[i]->DestroyWindow();
			delete m_pSubDlgs[i];
			m_pSubDlgs[i] = NULL;
		}
		delete m_cTabBtns[i];
	}

	m_plogDlgs[1 - m_LogDlgIdx]->DestroyWindow();
	delete m_plogDlgs[1 - m_LogDlgIdx];

	if (m_Proj) FreePtr(m_Proj);
	CeExDialog::OnDestroy();

	if (m_ohttp[0]) FreePtr(m_ohttp[0]);
	m_ohttp.RemoveAll();
}

void InvalidateMainRect()
{
	CDirPackgeDlg * pwnd = (CDirPackgeDlg*)::AfxGetMainWnd();
	CRect r;
	pwnd->m_pCurDlg->GetWindowRect(r);
	pwnd->ScreenToClient(r);
	pwnd->m_pCurDlg->ShowWindow(SW_HIDE);
	pwnd->Invalidate();
	pwnd->UpdateWindow();
	pwnd->m_pCurDlg->ShowWindow(SW_SHOW);

}

void SwitchBackDlg(CWnd * pwndHide)
{
	CDirPackgeDlg * pwnd = (CDirPackgeDlg*)::AfxGetMainWnd();
	pwnd->PopLastWnd((CExDialog*)pwndHide);
}


void SwitchBackMainDlg()
{
	CDirPackgeDlg * pwnd = (CDirPackgeDlg*)::AfxGetMainWnd();
	pwnd->m_wndStack.RemoveAll();
	pwnd->SwitchDlg(BM_TAB);
}

static BOOL g_bSysLocked = FALSE;

BOOL GetLockState()
{
	return g_bSysLocked;
}

void LockSystem(BOOL block)
{
	g_bSysLocked = block;
	CDirPackgeDlg * pwnd = (CDirPackgeDlg*)::AfxGetMainWnd();
	pwnd->LockMainWnd(block);
}


void CDirPackgeDlg::OnBnClickedBtnLogin()
{
	PushCurWnd();
	SwitchDlg(LOGIN_TAB);
}


void CDirPackgeDlg::OnBnClickedBtnNewprj()
{
	if (m_Proj)
	{
		if (m_Proj->m_bProjModified)
		{
			if (MessageBox(_T("工程已被修改，新建会清除现有数据！确认要新建吗？"), _T("新建工程"), MB_YESNO) == IDNO)
				return;
		}
		if (!m_Proj->m_szTargetPath.IsEmpty())
		{
			m_Proj->DestoryProj();
		}
	}
}


void CDirPackgeDlg::OnBnClickedBtnSetting()
{
	PushCurWnd();
	SwitchDlg(SETTING_TAB);
}


void CDirPackgeDlg::OnBnClickedBtnOpen()
{
	// TODO:  在此添加控件通知处理程序代码

	if (!m_Proj->Open(NULL))
	{
	}
}


void CDirPackgeDlg::OnBnClickedBtnSave()
{
	// TODO: Add your control notification handler code here
	if (m_Proj)
	{
		int ret = m_Proj->Save();
		switch (ret)
		{
		case PR_ERR_CONTENT:
			SwitchDlg(BM_TAB);
			((CBMDlg*)m_pSubDlgs[BM_TAB])->SetFocus();
			((CBMDlg*)m_pSubDlgs[BM_TAB])->ChangeView(VIEW_META_DATA);
			break;
		}
	}
}


void CDirPackgeDlg::OnBnClickedBtnExplore()
{
	SwitchDlg(RES_EXP_TAB);
}


void CDirPackgeDlg::OnBnClickedBtnExport()
{
	// TODO:  在此添加控件通知处理程序代码

}




// CProgressWnd 对话框

class CProgressWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressWnd)

public:
	CProgressWnd(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressWnd();
	CProgressCtrl m_prog;
	BOOL m_bCancel;

	// 对话框数据
	enum { IDD = IDD_PROGRESS };
	void ProcMsg();
	void SetInfo(LPCTSTR szInfo);
	CLink m_sinfo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void PreInitDialog();
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnCancel();
	virtual BOOL OnInitDialog();
};


// CProgressWnd 对话框

IMPLEMENT_DYNAMIC(CProgressWnd, CDialogEx)

CProgressWnd::CProgressWnd(CWnd* pParent /*=NULL*/)
: CDialogEx(CProgressWnd::IDD, pParent)
{
	m_bCancel = FALSE;
}

CProgressWnd::~CProgressWnd()
{
}

void CProgressWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_prog);
}

BEGIN_MESSAGE_MAP(CProgressWnd, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CProgressWnd::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CProgressWnd 消息处理程序


BOOL CProgressWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	///m_sinfo.SubclassDlgItem(IDC_ST_INFO, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CProgressWnd::OnBnClickedBtnCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	this->ShowWindow(SW_HIDE);
	this->CenterWindow(::AfxGetMainWnd());
	m_bCancel = TRUE;
}

void CProgressWnd::SetInfo(LPCTSTR szInfo)
{
	if (szInfo)
	{
		CWnd * pwnd = GetDlgItem(IDC_ST_INFO);
		pwnd->ShowWindow(SW_HIDE);
		pwnd->SetWindowText(szInfo);
		pwnd->ShowWindow(SW_SHOW);
	}
}

void CProgressWnd::ProcMsg()
{
	MSG msg;
	while (m_bCancel == FALSE && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		if (msg.message == WM_QUIT)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CProgressWnd::PreInitDialog()
{
	// TODO:  在此添加专用代码和/或调用基类

	CDialogEx::PreInitDialog();
}


void CProgressWnd::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
	FreeProcessWnd();
}


CProgressWnd * g_progWnd = NULL;
void FreeProcessWnd(void)
{
	if (g_progWnd)
	{
		FreePtr(g_progWnd);		
	}

}

void NewProcessWnd(int max, int min)
{
	if (g_progWnd == NULL)
	{
		g_progWnd = new CProgressWnd(::AfxGetMainWnd());
		g_progWnd->Create(CProgressWnd::IDD, ::AfxGetMainWnd());
		CRect r;
		::AfxGetMainWnd()->GetWindowRect(r);
		CRect rs;
		g_progWnd->GetWindowRect(rs);
	}
	
	
	g_progWnd->m_prog.SetRange32(min, max);
	g_progWnd->m_prog.SetPos(min);
	g_progWnd->ShowWindow(SW_HIDE);
}

void SetProgWndLimit(int max, int min)
{
	if (g_progWnd)
	{
		g_progWnd->m_prog.SetRange32(min, max);
		if (min != -1 )
			g_progWnd->m_prog.SetPos(min);
		g_progWnd->ShowWindow(SW_SHOW);
		g_progWnd->CenterWindow(::AfxGetMainWnd());
		g_progWnd->ProcMsg();
	}
}

BOOL OffsetPorgPos(int offset, LPCTSTR szinfo)
{
	if (g_progWnd)
	{
		g_progWnd->SetInfo(szinfo);
		int cur = g_progWnd->m_prog.GetPos();
		g_progWnd->m_prog.SetPos(cur+offset);
		g_progWnd->ProcMsg();
		return !g_progWnd->m_bCancel;
	}
	return FALSE;
}

BOOL SetCurProgPos(int cur, LPCTSTR szinfo)
{
	if (g_progWnd)
	{
		g_progWnd->SetInfo(szinfo);
		g_progWnd->m_prog.SetPos(cur);
		g_progWnd->ProcMsg();
		return !g_progWnd->m_bCancel;
	}
	return FALSE;
}

void EndProgWnd()
{
	if (g_progWnd)
	{
		g_progWnd->ShowWindow(SW_HIDE);
		g_progWnd->ProcMsg();
		::AfxGetMainWnd()->UpdateWindow();
	}
}

