#include "StdAfx.h"
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
