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

HANDLE CUtil::RunProc(LPCTSTR strcmd, LPCTSTR strparam, LPCTSTR strPath)
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strcmd;            
	ShExecInfo.lpParameters = strparam;    
	ShExecInfo.lpDirectory = strPath;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;      
	ShellExecuteEx(&ShExecInfo);
	//WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	return ShExecInfo.hProcess;
}

HANDLE CUtil::FindProcessByName(LPCTSTR szFileName, BOOL bKill, INT exit_code)
{
    /************************************************************************/
    // 采用直接枚举进程的方法查找指定名称进程
    /* 名    称: FindProcessByName( LPCTSTR szFileName )
    /* 功    能: 查询指定名称的进程是否有实例正在运行
    /* 参　　数: LPCTSTR szFileName, 可执行程序名称, 例如"c:\\notepad.exe"
    /* 返 回 值: BOOL.TRUE, 有同名实例正在运行; FALSE, 没有同名实例正在运行.
    /* 编 写 者: Xianren Li
    /* 创建日期: 2007-08-17
    /* 附加说明: 请注意进程的查找只是依据名称, 无法确保找到的进程调用的是预料
    /*             中的可执行文件.
    /* 修改记录:
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


void UC2ToUtf8(CString& strGBK) 
{
    INT len = WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)strGBK, -1, NULL, 0, NULL, NULL); 
    char *szUtf8=new char[len + 1];
    memset(szUtf8, 0, len + 1);
    WideCharToMultiByte (CP_UTF8, 0, (LPCTSTR)strGBK, -1, szUtf8, len, NULL,NULL);
	strGBK = szUtf8;
    delete[] szUtf8;
}

void Utf8ToUC2(CString& strUtf8) {
    int len=MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(LPCTSTR)strUtf8, -1, NULL,0);
    WCHAR * wszGBK = new WCHAR[len+1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(LPCTSTR)strUtf8, -1, wszGBK, len);
    strUtf8 = wszGBK;
    delete[] wszGBK;
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

CStringA CUtil::AscFile(LPCTSTR sfilename)
{
	CStringA strAsc;
	CFile of;
	if (of.Open(sfilename, CFile::modeRead))
	{
		int len = of.GetLength();
		CHAR * pbuf = strAsc.GetBuffer(len + 1);
		of.Read(pbuf, len);
		of.Close();
		pbuf[len] = 0;
		strAsc.ReleaseBuffer();
		return strAsc;
	}
}


void W2U(const char  * bstr, WCHAR  * wstr,  int lens,int lenw)
{
    ::MultiByteToWideChar(CP_UTF8,0,bstr,lens, wstr, lenw);
}


void U2W(LPCTSTR   wstr, char  * bstr,  int lenw,int lens)
{
    //USES_CONVERSION;
    int ret = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1,bstr, lens,0,0);
}


void QW2U(LPCSTR  astr, CString &wstr)
{
    int slen = lstrlenA(astr)+1;
    WCHAR * pwbuf = wstr.GetBuffer(slen);
    ::MultiByteToWideChar(CP_UTF8,0,astr,slen, pwbuf, slen);
    wstr.ReleaseBuffer();
}

void QU2W(LPCTSTR wstr, AString &astr )
{
    int slen = lstrlenW(wstr)+1;
    char * psbuf = astr.GetBuffer(slen*4);
    int ret = ::WideCharToMultiByte(CP_UTF8, 0, wstr, slen,psbuf, slen*4,0,0);
    astr.ReleaseBuffer();
}

WCHAR* QA2W(LPCSTR astr)
{
	int slen = lstrlenA(astr)+1;
	WCHAR * pwbuf = new WCHAR[slen];
	::MultiByteToWideChar(CP_ACP,0,astr,slen, pwbuf, slen);
	return pwbuf;
}

void QA2W(LPCSTR  astr, CString &wstr)
{
    int slen = lstrlenA(astr)+1;
    WCHAR * pwbuf = wstr.GetBuffer(slen);
    ::MultiByteToWideChar(CP_ACP,0,astr,slen, pwbuf, slen);
    wstr.ReleaseBuffer();
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
	  // 分析标准外壳命令、DDE、打开文件操作的命令行 
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

    //创建互斥对象
    TCHAR strAppName[] = TEXT("eExpressMutex");
    HANDLE hMutex = NULL;   
	hMutex = CreateMutex(NULL, FALSE, strAppName);
	if (hMutex != NULL)
    {
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			//显示原窗口,关闭互斥对象，退出程序
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