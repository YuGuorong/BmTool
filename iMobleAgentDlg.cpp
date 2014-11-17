
// iMobleAgentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iMobleAgent.h"
#include "iMobleAgentDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CeExDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CeExDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CeExDialog::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CeExDialog)
END_MESSAGE_MAP()


// CiMobleAgentDlg dialog

CiMobleAgentDlg::CiMobleAgentDlg(CWnd* pParent /*=NULL*/)
	: CeExDialog(CiMobleAgentDlg::IDD, pParent,EX_STRETCH_BK|EX_FILL_BK)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCurDlg = NULL;
}

void CiMobleAgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CeExDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAME, m_frame);
}

BEGIN_MESSAGE_MAP(CiMobleAgentDlg, CeExDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_LOGIN_STATE,&CiMobleAgentDlg::OnLoginState)		//自定义托盘事件
	ON_BN_CLICKED(IDC_BTN_EXIT, &CiMobleAgentDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CiMobleAgentDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_NEWPRJ, &CiMobleAgentDlg::OnBnClickedBtnNewprj)
	ON_BN_CLICKED(IDC_BTN_SETTING, &CiMobleAgentDlg::OnBnClickedBtnSetting)
END_MESSAGE_MAP()


// CiMobleAgentDlg message handlers

BOOL CiMobleAgentDlg::OnInitDialog()
{
	SetWndStyle(0,RGB(45,100,217), RGB(18,76,199));
	CeExDialog::OnInitDialog();

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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

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
	}
	
	CREATE_SUB_WND(m_pSubDlgs[LOGIN_TAB],   CLoginDlg, &m_frame);
	CREATE_SUB_WND(m_pSubDlgs[BM_TAB],      CBMDlg,    &m_frame);
	CREATE_SUB_WND(m_pSubDlgs[IMPORT_TAB],  CLoginDlg, &m_frame);
	CREATE_SUB_WND(m_pSubDlgs[EXPORT_TAB],  CLoginDlg, &m_frame);
	CREATE_SUB_WND(m_pSubDlgs[RES_EXP_TAB], CLoginDlg, &m_frame);
	CREATE_SUB_WND(m_pSubDlgs[SETTING_TAB], CSettingDlg, &m_frame);

	this->SetFullScreen();

	SwitchDlg(LOGIN_TAB);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CiMobleAgentDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CeExDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CiMobleAgentDlg::OnPaint()
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
HCURSOR CiMobleAgentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CiMobleAgentDlg::OnBnClickedBtnExit()
{
	this->EndDialog(IDOK);
}


void CiMobleAgentDlg::SwitchDlg(int id)
{
	if( m_pCurDlg )
	{
		m_pCurDlg->ShowWindow(SW_HIDE);
	}
	m_pCurDlg = m_pSubDlgs[id];
	m_pCurDlg->ShowWindow(SW_SHOW);
}

void CiMobleAgentDlg::OnBnClickedBtnLogin()
{
	SwitchDlg(LOGIN_TAB);
}


void CiMobleAgentDlg::OnBnClickedBtnNewprj()
{
	SwitchDlg(BM_TAB);
}


void CiMobleAgentDlg::OnBnClickedBtnSetting()
{
	SwitchDlg(SETTING_TAB);
}

LRESULT CiMobleAgentDlg::OnLoginState(WPARAM wParam, LPARAM lParam)
{
	if( wParam == LOGIN_ST_SUCESS )
	{
		SwitchDlg(BM_TAB);
	}
	return TRUE;
}