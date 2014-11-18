#include "StdAfx.h"
#include "resource.h"
#include "Util.h"
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

CUtil::CUtil(void)
{
}


CUtil::~CUtil(void)
{
}

void CUtil::GetCurPath(CString &strPath)
{
    TCHAR buff[MAX_PATH*2];
    ::GetModuleFileName(NULL,buff,MAX_PATH*2);
    strPath = buff;
    int pos = strPath.ReverseFind('\\');
    strPath = strPath.Left(pos+1);
}

void CUtil::RunProc(LPCTSTR strcmd, LPCTSTR strparam, LPCTSTR strPath)
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strcmd;            
	ShExecInfo.lpParameters = strparam;    
	ShExecInfo.lpDirectory = strPath;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;      
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

HANDLE CUtil::FindProcessByName(LPCTSTR szFileName, BOOL bKill, INT exit_code)
{
    /************************************************************************/
    // ����ֱ��ö�ٽ��̵ķ�������ָ�����ƽ���
    /* ��    ��: FindProcessByName( LPCTSTR szFileName )
    /* ��    ��: ��ѯָ�����ƵĽ����Ƿ���ʵ����������
    /* �Ρ�����: LPCTSTR szFileName, ��ִ�г�������, ����"c:\\notepad.exe"
    /* �� �� ֵ: BOOL.TRUE, ��ͬ��ʵ����������; FALSE, û��ͬ��ʵ����������.
    /* �� д ��: Xianren Li
    /* ��������: 2007-08-17
    /* ����˵��: ��ע����̵Ĳ���ֻ����������, �޷�ȷ���ҵ��Ľ��̵��õ���Ԥ��
    /*             �еĿ�ִ���ļ�.
    /* �޸ļ�¼:
    /************************************************************************/

    CString strFileName(szFileName);
    strFileName.MakeLower();

    DWORD aProcesses[1024], cbNeeded, cProcesses;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return NULL;

    cProcesses = cbNeeded / sizeof(DWORD);

	CString strCurPath;
	CUtil::GetCurPath(strCurPath);

    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcess = NULL;
    CString strName;
    for (unsigned int i = 0; i < cProcesses; i++)
    {
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);

        if (NULL != hProcess)
        {
            HMODULE hMod;
            DWORD cbNeeded;

            if (::EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
            {
                ::GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
                strName = szProcessName;
                strName.MakeLower();
                if (-1 != strFileName.Find(strName))
                {
					TRACE(_T("Find App process and Kill=%d\r\n"), bKill);
                    if( bKill )
					{
						TerminateProcess( hProcess,exit_code) ;
						CloseHandle(hProcess);
						Sleep(100);
						continue;
					}                    
					else 
						return hProcess;
                }
            }
            CloseHandle(hProcess);
        }
    }
	TRACE(_T("Didn't find app process \r\n"));
    return NULL;
}



void CUtil::DrawGradientFill(CDC * pdc, CRect &r, COLORREF clt, COLORREF cbt,int fmode)
{
	if( pdc )  
	{  
		TRIVERTEX        vert[2] ;  
		GRADIENT_RECT    gRect;  
		vert [0] .x      = r.left;  
		vert [0] .y      = r.top;  
		vert [0] .Red    = GetRValue(clt)<<8;  
		vert [0] .Green  = GetGValue(clt)<<8;  
		vert [0] .Blue   = GetBValue(clt)<<8;  
		vert [0] .Alpha  = 0x0000;  

		vert [1] .x      = r.right;  
		vert [1] .y      = r.bottom;   
		vert [1] .Red    = GetRValue(cbt)<<8;
		vert [1] .Green  = GetGValue(cbt)<<8;
		vert [1] .Blue   = GetBValue(cbt)<<8;
		vert [1] .Alpha  = 0x0000;  

		gRect.UpperLeft  = 1;  
		gRect.LowerRight = 0;  
		GradientFill(pdc->GetSafeHdc(),vert,2,&gRect,1,fmode);  
	}  
}

CSize CUtil::GetBitmapSize(CBitmap &bmp)
{
	BITMAP sbtmp;
	bmp.GetBitmap(&sbtmp);
	CSize sz(sbtmp.bmWidth, sbtmp.bmHeight);
	return sz;
}



void ParseCommandLine(CCommandLineInfo& rCmdInfo) 
{ 
    for (int i = 1; i < __argc; i++) 
    { 
        LPCTSTR pszParam = __targv[i]; 
        BOOL bFlag = FALSE; 
        BOOL bLast = ((i + 1) == __argc); 
        if (pszParam[0] == '-' || pszParam[0] == '/') 
        { 
            // remove flag specifier 
            bFlag = TRUE; 
            ++pszParam; 
        } 
        rCmdInfo.ParseParam(pszParam, bFlag, bLast); 
    } 
}


BOOL ShowTips()
{
    return TRUE;
}


CSetting::CSetting()
{
	CUtil::GetCurPath(strCurPath);
}

CSetting::~CSetting()
{
	
}

CSetting * g_pSet = NULL;
int BaseAppInit()
{
	  // ������׼������DDE�����ļ������������� 
    CCommandLineInfo cmdInfo; 
    ParseCommandLine(cmdInfo); 

    switch(cmdInfo.m_nShellCommand)
    {
    case RUN_SHOW_TIP:
        if( ShowTips() )
            return FALSE;
        break;
    case RUN_EXIT_PROCESS:
        return FALSE;
    default:
        break;
    }

    //�����������
    TCHAR strAppName[] = TEXT("eExpressMutex");
    HANDLE hMutex = NULL;   
	hMutex = CreateMutex(NULL, FALSE, strAppName);
	if (hMutex != NULL)
    {
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			//��ʾԭ����,�رջ�������˳�����
            CString strTitle;
            strTitle.LoadString(IDSTR_VENDOR_APPNAME);
            HWND hwnd = ::FindWindow(NULL,strTitle);
			if( hwnd )
			{
				::ShowWindow(hwnd,SW_SHOW);
				::SetForegroundWindow(hwnd);
			}
			CloseHandle(hMutex);
			return (-1);
        }
    }

	g_pSet = new CSetting;
	return 0;
}


void BaseAppExit()
{
	if( g_pSet )
	{
		delete g_pSet;
		g_pSet = NULL;
	}
}


class CSys
{
public: 
	CSys(){};
	~CSys(){BaseAppExit();};
};

CSys g_sys;