#include "StdAfx.h"
#include <locale.h>
#include "resource.h"
#include "Util.h"
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

HGLOBAL CUtil::m_hG;
LPCTSTR CUtil::m_szMimeType; //contend type

CPMutex::CPMutex(const TCHAR* name)
{
	memset(m_cMutexName, 0, sizeof(m_cMutexName));
	int min = _tcslen(name) > (sizeof(m_cMutexName) - 1) ? (sizeof(m_cMutexName) - 1) : _tcslen(name);
	_tcsncpy_s(m_cMutexName, name, min);
	m_pMutex = CreateMutex(NULL, false, m_cMutexName);
}

CPMutex::~CPMutex()
{
	CloseHandle(m_pMutex);
}

BOOL CPMutex::Lock()
{
	//?￥3a??′′?¨ê§°ü
	if (NULL == m_pMutex)
	{
		return FALSE;
	}

	DWORD nRet = WaitForSingleObject(m_pMutex, INFINITE);
	if (nRet != WAIT_OBJECT_0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CPMutex::UnLock()
{
	return ReleaseMutex(m_pMutex);
}
CUtil::CUtil(void)
{
	HRSRC hr = ::FindResource(NULL, MAKEINTRESOURCE(IDR_MIME_TYPE), _T("MIME"));
	if (hr)
	{
		ULONG nResSize = ::SizeofResource(NULL, hr);  // Data size/length  
		m_hG = ::LoadResource(NULL, hr);
		m_szMimeType = (LPCTSTR)LockResource(m_hG);    // Data Ptr 
	}
}

CUtil::~CUtil(void)
{
	BaseAppExit();
	::UnlockResource(m_hG);
}

void CUtil::GetCurPath(CString &strPath)
{
    TCHAR buff[MAX_PATH*2];
    ::GetModuleFileName(NULL,buff,MAX_PATH*2);
    strPath = buff;
    int pos = strPath.ReverseFind('\\');
    strPath = strPath.Left(pos+1);
}

CString CUtil::GetUserFolder()
{
	CString strUserFolder;
	PIDLIST_ABSOLUTE  pidl;
	SHGetSpecialFolderLocation(::AfxGetMainWnd()->GetSafeHwnd(), CSIDL_MYDOCUMENTS, &pidl);
	SHGetPathFromIDList(pidl, strUserFolder.GetBuffer(MAX_PATH));
	strUserFolder.ReleaseBuffer();
	return strUserFolder;
}

CString CUtil::GetFilePath(CString &strFile)
{
	int pos = strFile.ReverseFind('\\');
	if (pos == -1) return strFile;
	return strFile.Left(pos);
}

CString CUtil::GetFileName(CString &strFilePath)
{
	int pos = strFilePath.ReverseFind('\\');
	if (pos == -1) return strFilePath;
	CString str = strFilePath;
	return strFilePath.Right(strFilePath.GetLength() - pos - 1);
}

BOOL CUtil::GetFileMemiType(LPCTSTR szExt, CString &stype)
{
	if (szExt && *szExt != 0)
	{
		CString strext = _T("\n");
		strext += szExt;
		strext += _T(":");
		LPCTSTR ptr = _tcsstr(m_szMimeType, strext);
		if (ptr)
		{
			LPCTSTR pstart = ptr + strext.GetLength();
			LPCTSTR pend = _tcschr(pstart, _T('\r'));
			if (pend)
			{
				int cplen = pend - pstart;
				TCHAR * pdst = stype.GetBuffer(cplen + 1);
				memcpy(pdst, pstart, cplen*sizeof(TCHAR));
				pdst[cplen] = 0;
				stype.ReleaseBuffer();
				return TRUE;
			}
		}
	}
	stype = _T("application/octet-stream");
	return FALSE;

}

int CUtil::GetFileExt(CString &Sfile, CString &sext)
{
	int ps = Sfile.ReverseFind('\\');
	int pt = Sfile.ReverseFind('.');
	if (pt > 0 && (ps < pt))
	{
		sext = Sfile.Right(Sfile.GetLength() - pt -1); 
		return 1;
	}
	return 0;
}


DWORD CUtil::GetFileSize(LPCTSTR sfile)
{
	CFileStatus status;
	if (CFile::GetStatus(sfile, status))
	{
		return status.m_size;
	}
	return 0;
}

CString CUtil::GetFileType(LPCTSTR szFile)
{
	CString strFile = szFile;
	int ps = strFile.ReverseFind('\\');
	int pt = strFile.ReverseFind('.');
	if (pt > 0 && (ps < pt) )
	{
		CString strExt = strFile.Right(strFile.GetLength()-pt);
		CString strtype;
		GetFileMemiType(strExt, strtype);
		return strtype;
	}
	else
		return CString(TEXT("application/octet-stream"));
}

HANDLE CUtil::RunProc(LPCTSTR strcmd, LPCTSTR strparam, LPCTSTR strPath, BOOL bsync ,BOOL bfhide )
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strcmd;            
	ShExecInfo.lpParameters = strparam;    
	ShExecInfo.lpDirectory = strPath;
	ShExecInfo.nShow = bsync ? SW_SHOW : SW_SHOW; //SW_HIDE
	if (bfhide)
		ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;      
	ShellExecuteEx(&ShExecInfo);
	if ( bsync )
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	return ShExecInfo.hProcess;
}



int RunCmd(LPCTSTR scmd, CString &sresult, LPCTSTR sdir, BOOL bshow)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return FALSE;
	}
	TCHAR command[1024];    //长达1K的命令行，够用了吧  
	_tcscpy_s(command, 1024, _T("Cmd.exe /C "));
	_tcscat_s(command, 1024, scmd);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入  
	si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入  
	si.wShowWindow = bshow? SW_SHOW : SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//关键步骤，CreateProcess函数参数意义请查阅MSDN  
	if (!CreateProcess(NULL, command, NULL, NULL, TRUE, NULL, NULL, sdir, &si, &pi))
	{
		CloseHandle(hWrite);
		CloseHandle(hRead);
		return FALSE;
	}
	CloseHandle(hWrite);
	char buffer[4096] = { 0 };          //用4K的空间来存储输出的内容，只要不是显示文件内容，一般情况下是够用了。  
	DWORD bytesRead;
	//if (WaitForInputIdle(pi.hProcess, INFINITE) == 0)
	{
		while (true)
		{
			BOOL bret = ReadFile(hRead, buffer, 4095, &bytesRead, NULL);
			buffer[bytesRead] = 0;
			if (bret == false)
				break;
			CString str;
			QUtf2Unc(buffer, str);
			sresult += str;
		}
		//buffer中就是执行的结果，可以保存到文本，也可以直接输出  		
	}
	CloseHandle(hRead);
	return TRUE;
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

CStringA CUtil::File2Asc(LPCTSTR sfilename)
{
	CStringA strAsc;
	CFile of;
	if (of.Open(sfilename, CFile::modeRead|CFile::shareDenyNone))
	{
		UINT len = (UINT)of.GetLength();
		CHAR * pbuf = strAsc.GetBuffer(len + 1);
		of.Read(pbuf, len);
		of.Close();
		pbuf[len] = 0;
		strAsc.ReleaseBuffer();
	}
	return strAsc;
}

CString CUtil::File2Unc(LPCTSTR sfilename)
{
	CString strUnc;
	CFile of;
	if (of.Open(sfilename, CFile::modeRead | CFile::shareDenyNone))
	{
		int len = (INT)of.GetLength();
		TCHAR * pbuf = strUnc.GetBuffer(len /2 + 1);
		of.Read(pbuf, len);
		of.Close();
		pbuf[len/2] = 0;
		strUnc.ReleaseBuffer();
	}
	return strUnc;
}

int CUtil::Asc2File(LPCTSTR sfilename,  CStringA &sa)
{
	CFile of;
	if (of.Open(sfilename, CFile::modeCreate | CFile::modeWrite))
	{
		of.Write(sa, sa.GetLength());
		of.Close();
		return 1;
	}
	return 0;
}


HICON CUtil::GetFileIcon(LPCTSTR sfilename)
{
	HICON hIcon = ExtractIcon(AfxGetInstanceHandle(), sfilename, 0);
	if (hIcon == NULL)
	{
		CString sf = sfilename;
		int n_ext = sf.ReverseFind(_T('.'));
		CString sext = _T(".*");
		if (n_ext >= 0 )
		{
			sext = sf.Right(sf.GetLength() - n_ext);
		}
		SHFILEINFO   sfi;
		SHGetFileInfo(sext, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
		hIcon = sfi.hIcon;
	}
	return hIcon;
}

HICON CUtil::GetFileIcon(CFileDialog &fdlg)
{
	HICON hIcon;
	hIcon = ExtractIcon(AfxGetInstanceHandle(), fdlg.GetPathName(), 0);
	if (hIcon == NULL)
	{
		CString str = fdlg.GetFileExt();
		str = _T(".") + str;
		SHFILEINFO   sfi;
		SHGetFileInfo(str, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
		hIcon = sfi.hIcon;
	}
	return hIcon;
}

CString CUtil::GenGuidString()
{
	CString sguid;
	GUID guid;
	HRESULT hResult = CoCreateGuid(&guid);
	if (S_OK == hResult)
	{
		CComBSTR bstrGuid(guid);
		sguid = bstrGuid;
	}
	sguid.Remove(_T('-'));
	sguid.Remove(_T('{'));
	sguid.Remove(_T('}'));
	return sguid;
}

BOOL CUtil::GetFileSize(LPCTSTR  sfile, DWORD &flen)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	DWORD fileSize;
	hFind = FindFirstFile(sfile, &fileInfo);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		fileSize = fileInfo.nFileSizeLow;
		FindClose(hFind);
		flen = fileSize;
		return TRUE;
	}
	return FALSE;
}

CString CUtil::FormatMessageFor(HRESULT hr)
{
	CString strMsg;
	LPVOID pvMsgBuf = NULL;
	LPCTSTR pszMsg = NULL;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, (DWORD)hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pvMsgBuf, 0, NULL))
	{
		pszMsg = (LPTSTR)pvMsgBuf;
		// Do something with the error message.
		//printf( "Windows error: 0X%.8X (%s)\n", a_hResult, a_pszMsg );
		strMsg.Format(_T("0X%.8X (%s)"), hr, pszMsg);
		LocalFree(pvMsgBuf);
	}
	return strMsg;
}

void Utf2Unc(const char  * bstr, WCHAR  * wstr, int lens, int lenw)
{
    ::MultiByteToWideChar(CP_UTF8,0,bstr,lens, wstr, lenw);
}


void Unc2Utf(LPCTSTR   wstr, char  * bstr, int lenw, int lens)
{
    int ret = ::WideCharToMultiByte(CP_UTF8, 0, wstr, -1,bstr, lens,0,0);
}

CString qUtf2Unc(LPCSTR astr)
{
	CString wstr;
    int slen = lstrlenA(astr)+1;
    WCHAR * pwbuf = wstr.GetBuffer(slen);
    ::MultiByteToWideChar(CP_UTF8,0,astr,slen, pwbuf, slen);
    wstr.ReleaseBuffer();
	return wstr;
}

CStringA qUnc2Utf(LPCWSTR wstr)
{
	AString astr;
	int slen = lstrlenW(wstr)+1;
    char * psbuf = astr.GetBuffer(slen*4);
    int ret = ::WideCharToMultiByte(CP_UTF8, 0, wstr, slen,psbuf, slen*4,0,0);
    astr.ReleaseBuffer();
	return astr;
}

void QUtf2Unc(LPCSTR  astr, CString &wstr)
{
    int slen = lstrlenA(astr)+1;
    WCHAR * pwbuf = wstr.GetBuffer(slen);
    ::MultiByteToWideChar(CP_UTF8,0,astr,slen, pwbuf, slen);
    wstr.ReleaseBuffer();
}

void QUnc2Utf(LPCWSTR wstr, AString &astr)
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

void QW2A(LPCTSTR wstr, CStringA &astr)
{
    int slen = lstrlenW(wstr)+1;
    char * psbuf = astr.GetBuffer(slen*2);
	int ret = ::WideCharToMultiByte(CP_ACP, 0, wstr, slen, psbuf, slen * 2, 0, 0);
    astr.ReleaseBuffer();
}

CStringA ConvtXmlChars(LPCSTR str)
{
	CStringA strval = str;
	strval.Replace(("&"), ("&amp;"));
	strval.Replace(("<"), ("&lt;"));
	strval.Replace((">"), ("&gt;"));
	strval.Replace(("\'"), ("&apos;"));
	strval.Replace(("\""), ("&quot;"));
	return strval;
}

CStringA ConvtXmlChars(CStringA &str)
{
	CStringA strval = str;
	strval.Replace(("&"), ("&amp;"));
	strval.Replace(("<"), ("&lt;"));
	strval.Replace((">"), ("&gt;"));
	strval.Replace(("\'"),("&apos;"));
	strval.Replace(("\""),("&quot;"));
	return strval;
}

void ReplaceXmlItem(LPCSTR szKey, CStringA &strval, CStringA &szXml)
{
	strval.Replace(("&"),  ("&amp;"));
	strval.Replace(("<"),  ("&lt;"));
	strval.Replace((">"),  ("&gt;"));
	strval.Replace(("\'"), ("&apos;"));
	strval.Replace(("\""), ("&quot;"));
	szXml.Replace(szKey, (LPCSTR)strval);
}

CString ConvtXmlChars(CString &str)
{
	CString strval = str;
	strval.Replace(_T("&"), _T("&amp;"));
	strval.Replace(_T("<"), _T("&lt;"));
	strval.Replace(_T(">"), _T("&gt;"));
	strval.Replace(_T("\'"), _T("&apos;"));
	strval.Replace(_T("\""), _T("&quot;"));
	return strval;
}

void ReplaceXmlItem(LPCTSTR szKey, CString &strval, CString &szXml)
{
	strval.Replace(_T("&"), _T("&amp;"));
	strval.Replace(_T("<"), _T("&lt;"));
	strval.Replace(_T(">"), _T("&gt;"));
	strval.Replace(_T("\'"), _T("&apos;"));
	strval.Replace(_T("\""), _T("&quot;"));
	szXml.Replace(szKey, strval);
}

int atox(const char *str)
{
	DWORD	sum;
	int		i = 0;
	char	ch;
	if (str == NULL) return 0;
	str += strspn(str, " \t0xX");
	sum = 0;
	while (*str && i<8)
	{
		ch = *(str++);
		if ((ch >= '0') && (ch <= '9'))
		{
			sum = sum * 0x10 + ch - '0';
			i++;
		}
		else
		{
			ch |= 0x20;
			if ((ch >= 'a') && (ch <= 'f'))
			{
				sum = sum * 0x10 + ch - 'a' + 0xA;
				i++;
			}
			else
			{
				break;
			}
		}
	}
	return (int)sum;
}


CString GetReadableSize(UINT32 size)
{
	UINT32 ip, pp;
	int i = 0;
	ip = size; pp = 0;
	TCHAR salias[] = { _T(" KMG") };
	while (ip>1024)
	{
		if (i >= sizeof(salias) / sizeof(TCHAR) - 1) break;
		i++;
		pp = ip % 1024;
		ip = ip / 1024;
	}
	pp = (pp + 5) / 10;
	CString stri;
	stri.Format(_T("%d.%02d"), ip, pp);
	stri.TrimRight(_T('0'));
	stri.TrimRight(_T('.'));
	stri += salias[i];
	stri.TrimRight();
	stri += _T('B');
	return stri;
}


BOOL ShowTips()
{
    return TRUE;
}

#include<stdlib.h> 　　
CSetting::CSetting()
{
	CUtil::GetCurPath(strCurPath);
	m_strAppUserPath = CUtil::GetUserFolder();
	m_strAppUserPath += _T("\\") CFG_OPENCN_FOLDER;
	::CreateDirectory(m_strAppUserPath, NULL);
	CString strHomedrv = _wgetenv(_T("HOMEDRIVE"));
	m_strHomePath = strHomedrv + _wgetenv(_T("HOMEPATH"));
}

CSetting::~CSetting()
{
}

void CSetting::Load()
{
	CWinApp* pApp = AfxGetApp();
	m_nPort = pApp->GetProfileInt(_T("AppSettings"), _T("Port"), 8080);
	m_strPassword = pApp->GetProfileString(_T("AppSettings"), _T("Password"), _T(""));
	m_strUserName = pApp->GetProfileString(_T("AppSettings"), _T("UserName"), _T("admin"));
	m_strServerIP = pApp->GetProfileString(_T("AppSettings"), _T("ServerIp"), _T("fixopen.xicp.net"));

	m_bEnProxy = pApp->GetProfileInt(_T("AppSettings"), _T("ProxyEnable"), 0);
	m_nProxyPort = pApp->GetProfileInt(_T("AppSettings"), _T("ProxyPort"), 80);
	m_strProxyIP = pApp->GetProfileString(_T("AppSettings"), _T("ProxyIp"), _T(""));
	m_strProxyPwd = pApp->GetProfileString(_T("AppSettings"), _T("ProxyPwd"), _T(""));
	m_strProxyUser = pApp->GetProfileString(_T("AppSettings"), _T("ProxyUser"), _T(""));

	m_strSrcDir = pApp->GetProfileString(_T("AppSettings"), _T("SourceDir"), _T(""));
	m_strDstDir = pApp->GetProfileString(_T("AppSettings"), _T("DestanceDir"), _T(""));
}

void CSetting::Save()
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("AppSettings"), _T("ServerIp"), m_strServerIP);
	pApp->WriteProfileInt(_T("AppSettings"), _T("Port"), m_nPort);

	pApp->WriteProfileInt(_T("AppSettings"), _T("ProxyEnable"), m_bEnProxy);
	pApp->WriteProfileInt(_T("AppSettings"), _T("ProxyPort"), m_nProxyPort);
	pApp->WriteProfileString(_T("AppSettings"), _T("ProxyIp"), m_strProxyIP);
	pApp->WriteProfileString(_T("AppSettings"), _T("ProxyPwd"), m_strProxyPwd);
	pApp->WriteProfileString(_T("AppSettings"), _T("ProxyUser"), m_strProxyUser);

	pApp->WriteProfileString(_T("AppSettings"), _T("SourceDir"), m_strSrcDir);
	pApp->WriteProfileString(_T("AppSettings"), _T("DestanceDir"), m_strDstDir);

}

CSetting * g_pSet = NULL;

#include "libs.h"
#include "XZip\XUnzip.h"
void UnzipFile(CString &sin, CStringArray * pFiles)
{
	//TCHAR scdir[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH, scdir);

	TCHAR * sbuf = sin.GetBuffer();
	HZIP hz = OpenZip(sbuf, 0, ZIP_FILENAME);
	sin.ReleaseBuffer();
	if (hz == NULL)
	{
		return ;
	}
	ZIPENTRYW ze; GetZipItem(hz, -1, &ze);
	int numitems = ze.index;
	for (int i = 0; i < numitems; i++)
	{
		GetZipItem(hz, i, &ze);
		UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);		
		if (pFiles) pFiles->Add(ze.name);
	}
	CloseZip(hz);

	//::SetCurrentDirectory(scdir);
}

BOOL UnzipLimitFile(CString &sin, CStringArray * pFiles, int max_size, LPCTSTR sname)
{
	TCHAR * sbuf = sin.GetBuffer();
	HZIP hz = OpenZip(sbuf, 0, ZIP_FILENAME);
	sin.ReleaseBuffer();
	if (hz == NULL)
	{
		return FALSE;
	}
	ZIPENTRYW ze; GetZipItem(hz, -1, &ze);
	int numitems = ze.index;
	CString sext;
	for (int i = 0; i < numitems; i++)
	{
		GetZipItem(hz, i, &ze);
		if (ze.unc_size >= max_size)
		{
			CString s(ze.name);
			if (sname== NULL || (CUtil::GetFileExt(s, sext) && sext.CollateNoCase(sname) == 0) )
			{
				CloseZip(hz);
				return FALSE;
			}
		}
	}

	for (int i = 0; i < numitems; i++)
	{
		GetZipItem(hz, i, &ze);
		if (UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME) != ZR_OK)
		{
				CloseZip(hz);
				return FALSE;
		}
		if (pFiles) pFiles->Add(ze.name);
	}
	CloseZip(hz);
	return TRUE;
}


BOOL IsOsVistaExt()
{
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osinfo);

	DWORD dwPlatformId = osinfo.dwPlatformId;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	//Vista & Windows Server 2008         : 6.0
	//Windows 7 & Windows Server 2008 R2  : 6.1
	//Windows 8 & Windows Server 2012     : 6.2
	//Windows 8.1 & Windows Server 2012 R2: 6.3
	return ((dwPlatformId == 2) &&
		(dwMajorVersion >= 6));
}


BOOL DelTree(LPCTSTR lpszPath)
{
	CString sd = lpszPath;
	int sl = sd.GetLength();
	sd.GetBufferSetLength(sl + 8);
	sd.SetAt(sl+1, '\0');
	sd.SetAt(sl, '\0');

	SHFILEOPSTRUCT shfileop;
	shfileop.hwnd = NULL;
	shfileop.wFunc = FO_DELETE;
	shfileop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
	shfileop.pFrom = sd;
	shfileop.pTo = _T("");
	shfileop.lpszProgressTitle = _T("");
	shfileop.fAnyOperationsAborted = TRUE;
	int nOK = SHFileOperation(&shfileop);
	if (nOK)
		nOK = GetLastError();
	return nOK == 0;
}

BOOL PickupFolder(CString &strDir, CWnd * pown, LPCTSTR szTitle, LPCTSTR szDispDir)
{
	if (IsOsVistaExt())
	{
		CFolderPickerDialog fd(NULL, 0, pown, 0);
		fd.m_ofn.lpstrTitle = (szTitle == NULL) ? _T("选择文件夹") : szTitle;
		int ret = fd.DoModal();
		if (ret == IDOK)
		{
			strDir = fd.GetFolderPath();
			return TRUE;
		}
	}
	else
	{
		TCHAR   szDir[MAX_PATH];
		BROWSEINFO   bi;
		ITEMIDLIST   *pidl;
		szDir[0] = 0;
		if (szDispDir)
			_tcscpy_s(szDir, MAX_PATH, szDispDir);

		bi.hwndOwner = pown->m_hWnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szDir;
		bi.lpszTitle = (szTitle == NULL) ? _T("选择文件夹") : szTitle;
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;
		bi.iImage = 0;

		pidl = SHBrowseForFolder(&bi);
		if (pidl && SHGetPathFromIDList(pidl, szDir))
		{
			strDir = szDir;
			return TRUE;
		}
	}
	return FALSE;	
}

void ParseCommandLine(CCommandLineInfo& rCmdInfo)
{
	for (int i = 1; i < __argc; i++)
	{
		CString  pszParam = __targv[i];
		if (pszParam.CompareNoCase(_T("-dec")) == 0)
		{
			if ((i < __argc - 1) && __targv[i + 1]!= NULL )
			{
				CString strin = __targv[i + 1];
				if (!strin.IsEmpty())
				{
					CString strout = strin;
					int np = strout.ReverseFind(_T('\\'));
					strout.Insert(np + 1, _T("._dec_"));
					decode_file(strin, strout);
				}
			}
			exit(0);
		}
		if (pszParam.CompareNoCase(_T("-uzip")) == 0)
		{
			if ((i < __argc - 1) && __targv[i + 1]!= NULL )
			{
				CString strin = __targv[i + 1];
				UnzipFile(strin );
			}
            exit(0);
        }
	}
}


int BaseAppInit()
{
	  // 分析标准外壳命令、DDE、打开文件操作的命令行 
    CCommandLineInfo cmdInfo; 
    ParseCommandLine(cmdInfo); 

	_tsetlocale(LC_CTYPE, _T("chs"));
    //创建互斥对象
#if 0
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
#endif
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

CUtil g_util;

#include <Wininet.h>  

#pragma comment(lib, "Wininet.lib")  
BOOL HttpRequestPut(LPCTSTR pHomeUrl, LPCTSTR pPageUrl, LONG nPort,
	LPCTSTR pFile, CString *psRes, PBOOL pbExit)
{
	LPINTERNET_BUFFERS pBufferIn = new INTERNET_BUFFERS;
	ZeroMemory(pBufferIn, sizeof(INTERNET_BUFFERS));
	LPBYTE pBuf = new BYTE[1024];
	HINTERNET hInternet = NULL;
	HINTERNET hSession = NULL;
	HINTERNET hRequest = NULL;
	DWORD dwBytesRead;
	DWORD dwBytesWritten;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	CString sFormat(_T("Connection: Keep-Alive\r\n"));
	sFormat += _T("Content-Type: application/octet-stream\r\n");
	sFormat += _T("Content-Length: %u\r\n");
	sFormat += _T("User-Agent:Test\r\n");
	sFormat += _T("Host: %s:%u\r\n");
	sFormat += _T("Accept: *.*,*/*\r\n");
	sFormat += _T("\r\n");
	CString sHeader(_T(""));
	CString sRes(_T(""));

	do
	{
		hInternet = InternetOpen(_T("Mozilla/4.0 (compatible; Indy Library)"),
			INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (NULL == hInternet)
		{
			sRes.Format(_T("Open link error. ErrCode=[%u]"), GetLastError());
			break;
		}

		hSession = InternetConnect(hInternet, pHomeUrl, (INTERNET_PORT)nPort,
			NULL, NULL, INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);
		hRequest = HttpOpenRequest(hSession, _T("PUT"), pPageUrl,
			NULL, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (FALSE == hRequest)
		{
			sRes.Format(_T("Open request handle error. ErrCode=[%u]"),
				GetLastError());
			break;
		}

		hFile = CreateFile(pFile, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			sRes.Format(_T("Open File error. ErrCode=[%u] File=[%s]"),
				GetLastError(), pFile);
			break;
		}

		pBufferIn->dwStructSize = sizeof(INTERNET_BUFFERS);
		pBufferIn->dwBufferTotal = GetFileSize(hFile, NULL);
		sHeader.Format(sFormat, pBufferIn->dwBufferTotal, pHomeUrl, nPort);
		pBufferIn->lpcszHeader = sHeader;
		pBufferIn->dwHeadersLength = sHeader.GetLength();

		if (FALSE ==
			HttpSendRequestEx(hRequest, pBufferIn, NULL, HSR_INITIATE, 0))
		{
			sRes.Format(_T("Send request error."));
			break;
		}

		DWORD dwSendSize = 0;
		while (dwSendSize < pBufferIn->dwBufferTotal)
		{
			if ((NULL != pbExit) && (FALSE != (*pbExit)))
			{
				sRes.Format(_T("Stop upload because receive exit cmd."));
				break;
			}

			if (FALSE == ReadFile(hFile, pBuf, 1024, &dwBytesRead, NULL))
			{
				sRes.Format(_T("Read File error. ErrCode=[%u] File=[%s]"),
					GetLastError(), pFile);
				break;
			}

			if (FALSE == InternetWriteFile(hRequest, pBuf, dwBytesRead,
				&dwBytesWritten))
			{// ERROR_INTERNET_CONNECTION_ABORTED  
				sRes.Format(_T("Upload File error. ErrCode=[%u] File=[%s]"),
					GetLastError(), pFile);
				break;
			}

			dwSendSize += dwBytesWritten;
		}

		if (FALSE == HttpEndRequest(hRequest, NULL, 0, 0))
		{
			sRes.Format(_T("End request error. ErrCode=[%u] File=[%s]"),
				GetLastError(), pFile);
		}

	} while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	if (hRequest)
	{
		InternetCloseHandle(hRequest);
	}
	if (hSession)
	{
		InternetCloseHandle(hSession);
	}
	if (hInternet)
	{
		InternetCloseHandle(hInternet);
	}

	delete[]pBuf;
	delete pBufferIn;

	if (NULL != psRes)
	{
		*psRes = sRes;
	}

	return sRes.IsEmpty();
}


void HttpPost(LPCTSTR szServer, int nPort, LPCTSTR url, void * postdata, int dlen, CString &strResponse)
{
	BOOL bResult = FALSE;

	// 初始化WinInet 环境
	HINTERNET hInternet = InternetOpen(_T("CEHTTP"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	// 打开http session
	HINTERNET hSession = InternetConnect(hInternet, szServer, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	// 打开http post 请求的句柄
	LPCTSTR szAccept[] = { _T("*/*"), NULL };
	HINTERNET hRequest = HttpOpenRequest(hSession, _T("POST"), url, NULL, NULL, szAccept, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES, 0);

	// 外发的header
	TCHAR headerLanguage[] = _T("Accept-Language: zh-cn\r\n");
	TCHAR headerEncoding[] = _T("Accept-Encoding: gzip, deflate\r\n");
	TCHAR headerContentType[] = _T("Content-Type: application/json\r\n");//text/xml
	CString headerHost , headerContentLength;

	headerHost.Format(_T("Host: %s:%d\r\n"), szServer, nPort);
	headerContentLength.Format(_T("Content-Length: %d\r\n\r\n"), dlen);// strlen(post_data));

	// 添加header 信息
	bResult = HttpAddRequestHeaders(hRequest, headerLanguage, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerEncoding, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerHost, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerContentType, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerContentLength, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

	/*=====================================================================
	// 简单的发送数据的方法，可用来发送少量数据，或提交GET请求
	//===================================================================*/
	//bResult = HttpSendRequest(hRequest, NULL, 0, post_data, _tcsclen(post_data));

	/*=====================================================================
	// 发送大量数据包的方法
	//===================================================================*/
	INTERNET_BUFFERS BufferIn = { 0 };

	BufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
	BufferIn.dwBufferTotal = dlen;// strlen(post_data);

	bResult = HttpSendRequestEx(hRequest, &BufferIn, NULL, 0, 0);

	DWORD written = 0;
	bResult = InternetWriteFile(hRequest, (LPVOID)(char *)postdata, dlen, &written);

	bResult = HttpEndRequest(hRequest, NULL, 0, 0);
	/*=====================================================================
	// 发送大量数据包结束
	//===================================================================*/

	LPSTR     lpszData = NULL; // buffer for the data
	DWORD     dwSize = 0;           // size of the data available
	DWORD     dwDownloaded = 0; // size of the downloaded data

	// 请求header 的大小，注意这里的bResult 为FALSE
	bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &dwSize, NULL);

	// 为接收header 分配内存空间
	CHAR* lpHeadersA = new CHAR[dwSize];

	// 接收http response 中的header
	bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpHeadersA, &dwSize, NULL);

	strResponse.Empty();
	CStringA str;
	// 循环读取数据 
	do
	{
		// 检查在http response 还有多少字节可以读取
		if (InternetQueryDataAvailable(hRequest, &dwSize, 0, 0))
		{
			lpszData = new char[dwSize + 1];
			// 读取数据
			if (!InternetReadFile(hRequest, (LPVOID)lpszData, dwSize, &dwDownloaded))
			{
				delete[] lpszData;
				break;
			}
			else
			{
				if (dwDownloaded)
				{
					lpszData[dwDownloaded] = 0;
					str += lpszData;
					delete[] lpszData;
				}
			}
		}
		else
		{
			break;
		}
	} while (dwDownloaded != 0);

	QA2W(str, strResponse);
	delete lpHeadersA;
	// 关闭句柄
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
}

//// 外发的header
//TCHAR headerLanguage[] = _T("Accept-Language: zh-cn\r\n");
//TCHAR headerEncoding[] = _T("Accept-Encoding: gzip, deflate\r\n");
//TCHAR headerContentType[] = _T("Content-Type: application/zip\r\n\r\n");//text/xml
//

BOOL HttpPostEx(LPCTSTR szServer, int nPort, LPCTSTR url, LPCTSTR szCookies[], int nheaders, \
	int ncookies, void * postdata, int dlen, CString &strResponse)
{
	BOOL bResult = FALSE;

	// 初始化WinInet 环境
	HINTERNET hInternet = InternetOpen(_T("UPHTTP"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	// 打开http session
	HINTERNET hSession = InternetConnect(hInternet, szServer, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	// 打开http post 请求的句柄
	LPCTSTR szAccept[] = { _T("*/*"), NULL };
	HINTERNET hRequest = HttpOpenRequest(hSession, _T("POST"), url, NULL, NULL, szAccept, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES, 0);

	try{
		CString headerHost, headerContentLength;
		headerHost.Format(_T("Host: %s:%d\r\n"), szServer, nPort);
		headerContentLength.Format(_T("Content-Length: %d\r\n\r\n"), dlen);// strlen(post_data));

		// 添加header 信息
		for (int i = 0; i < nheaders; i++)
		{
			bResult = HttpAddRequestHeaders(hRequest, szCookies[i], -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
			if ( bResult == FALSE)	throw "HEADER error";
		}
		bResult = HttpAddRequestHeaders(hRequest, headerHost, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
		if (bResult == FALSE)	throw "HEADER error";
		bResult = HttpAddRequestHeaders(hRequest, headerContentLength, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
		if (bResult == FALSE)	throw "HEADER error";



		/*=====================================================================
		// 简单的发送数据的方法，可用来发送少量数据，或提交GET请求
		//===================================================================*/
		//bResult = HttpSendRequest(hRequest, NULL, 0, post_data, _tcsclen(post_data));

		/*=====================================================================
		// 发送大量数据包的方法
		//===================================================================*/
		INTERNET_BUFFERS BufferIn = { 0 };

		BufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
		BufferIn.dwBufferTotal = dlen;// strlen(post_data);

		bResult = HttpSendRequestEx(hRequest, &BufferIn, NULL, 0, 0);
		if (bResult == FALSE)	throw "HEADER error";

		DWORD written = 0;
		bResult = InternetWriteFile(hRequest, (LPVOID)(char *)postdata, dlen, &written);
		if (bResult == FALSE)	throw "HEADER error";

		bResult = HttpEndRequest(hRequest, NULL, 0, 0);
		if (bResult == FALSE)	throw "HEADER error";
		/*=====================================================================
		// 发送大量数据包结束
		//===================================================================*/

		LPSTR     lpszData = NULL; // buffer for the data
		DWORD     dwSize = 0;           // size of the data available
		DWORD     dwDownloaded = 0; // size of the downloaded data

		// 请求header 的大小，注意这里的bResult 为FALSE
		bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &dwSize, NULL);
		if (bResult == FALSE)	throw "HEADER error";

		// 为接收header 分配内存空间
		CHAR* lpHeadersA = new CHAR[dwSize];

		// 接收http response 中的header
		bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpHeadersA, &dwSize, NULL);
		if (bResult == FALSE)	throw "HEADER error";

		strResponse.Empty();
		CStringA str;
		// 循环读取数据 
		do
		{
			// 检查在http response 还有多少字节可以读取
			if (InternetQueryDataAvailable(hRequest, &dwSize, 0, 0))
			{
				lpszData = new char[dwSize + 1];
				// 读取数据
				if (!InternetReadFile(hRequest, (LPVOID)lpszData, dwSize, &dwDownloaded))
				{
					delete[] lpszData;
					break;
				}
				else
				{
					if (dwDownloaded)
					{
						lpszData[dwDownloaded] = 0;
						str += lpszData;
						delete[] lpszData;
					}
				}
			}
			else
			{
				break;
			}
		} while (dwDownloaded != 0);

		QA2W(str, strResponse);
		bResult = TRUE;
	}
	catch (...)
	{
		bResult = FALSE;
	}
	// 关闭句柄
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
	return bResult;
}

#include "iostream" 
using namespace std;
#pragma comment(lib,"ws2_32.lib")    
long l_file_len;
//获取文件内容    
bool file_con(char **buffer, LPCSTR file)
{
	FILE *fp = fopen(file, "rb");
	if (fp == NULL)
	{
		printf("文件上传失败，请检查文件路径.....\n");
		return false;
	}
	fseek(fp, 0, SEEK_END);
	l_file_len = ftell(fp);
	rewind(fp);

	*buffer = new char[l_file_len + 1];
	memset(*buffer, 0, l_file_len + 1);
	fseek(fp, 0, SEEK_SET);
	fread(*buffer, sizeof(char), l_file_len, fp);
	fclose(fp);
	return true;
}

std::string upload(LPCSTR lpszServer, LPCSTR lpszAddr, LPCSTR fileUrl)
{
	char *file = NULL;
	if (!file_con(&file, fileUrl))
	{
		return "0";
	}
	SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		return "0";
	SOCKADDR_IN server;
	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	struct hostent *host_addr = gethostbyname(lpszServer);
	if (host_addr == NULL)
		return "host_addr == NULL";
	server.sin_addr.s_addr = *((int *)*host_addr->h_addr_list);
	if (::connect(sock, (SOCKADDR *)&server, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		::closesocket(sock);
		return "0";
	}
	printf("ip address = %s, port = %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));

	std::string header("");
	std::string content("");
	//----------------------post头开始--------------------------------    
	header += "post ";
	header += lpszAddr;
	header += " HTTP/1.1\r\n";
	header += "Host: ";
	header += lpszServer;
	header += "\r\n";
	header += "User-Agent: Mozilla/4.0\r\n";
	header += "Connection: Keep-Alive\r\n";
	header += "Accept: */*\r\n";
	header += "Pragma: no-cache\r\n";
	header += "Content-Type: multipart/form-data; charset=\"gb2312\"; boundary=----------------------------64b23e4066ed\r\n";

	content += "------------------------------64b23e4066ed\r\n";
	content += "Content-Disposition: form-data; name=\"file\"; filename=\"大论文和实验材料.rar\"\r\n";
	content += "Content-Type: aapplication/octet-stream\r\n\r\n";

	//post尾时间戳    
	std::string strContent("\r\n------------------------------64b23e4066ed\r\n");
	char temp[64] = { 0 };
	//注意下面这个参数Content-Length，这个参数值是：http请求头长度+请求尾长度+文件总长度    
	sprintf(temp, "Content-Length: %d\r\n\r\n", content.length() + l_file_len + strContent.length());
	header += temp;
	std::string str_http_request;
	str_http_request.append(header).append(content);
	//----------------------post头结束-----------------------------------    
	//发送post头    
	send(sock, str_http_request.c_str(), str_http_request.length(), 0);

	char fBuff[4096];
	int nPacketBufferSize = 4096; // 每个数据包存放文件的buffer大小    
	int nStart;//记录post初始位置    
	int nSize;//记录剩余文件大小    
	// 就分块传送    
	for (int i = 0; i < l_file_len; i += nPacketBufferSize)
	{
		nStart = i;
		if (i + nPacketBufferSize + 1> l_file_len)
		{
			nSize = l_file_len - i;
		}
		else
		{
			nSize = nPacketBufferSize;
		}

		memcpy(fBuff, file + nStart, nSize);
		::send(sock, fBuff, nSize, 0);
		Sleep(0.2);
	}

	::send(sock, strContent.c_str(), strContent.length(), 0);

	char szBuffer[1024] = { 0 };
	while (true)
	{

		int nRet = ::recv(sock, szBuffer, sizeof(szBuffer), 0);
		if (nRet == 0 || nRet == WSAECONNRESET)
		{
			printf("Connection Closed.\n");
			break;
		}
		else if (nRet == SOCKET_ERROR)
		{
			printf("socket error\n");
			break;
		}
		else
		{
			printf("recv() returned %d bytes\n", nRet);
			printf("received: %s\n", szBuffer);
			break;
		}
	}
	::closesocket(sock);
	delete[] file;
	return szBuffer;
}

void TestSocket()
{
	upload("10.96.25.67", "/api/users/admin/sessions", "C:\\temp.zip");
}