
// DirPackge.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DirPackge.h"
#include "DirPackgeDlg.h"

#include <Dbghelp.h>
using namespace std;

#pragma auto_inline (off)
#pragma comment( lib, "DbgHelp" )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDirPackgeDlgApp

BEGIN_MESSAGE_MAP(CDirPackgeDlgApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDirPackgeDlgApp construction

CDirPackgeDlgApp::CDirPackgeDlgApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

LONG WINAPI MyUnhandledExceptionFilter( struct _EXCEPTION_POINTERS* ExceptionInfo 	)
{
	HANDLE lhDumpFile = CreateFile(_T("CrashDump.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);
	CloseHandle(lhDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}

// The one and only CDirPackgeDlgApp object

CDirPackgeDlgApp theApp;
int ConnectDb(LPCTSTR db_name);
int DisconnetDB(void);
void ShowAboutDlg();
// CDirPackgeDlgApp initialization

BOOL CDirPackgeDlgApp::InitInstance()
{
	SetRegistryKey(_T("Local AppWizard-Generated Applications\\DirPackger"));
	if( BaseAppInit() < 0 ) return FALSE;
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	CString str = g_pSet->strCurPath;
	str += _T("ebook.db");

	ConnectDb(str);

	g_pSet->Load();
	 
	CDirPackgeDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	DisconnetDB();
	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


