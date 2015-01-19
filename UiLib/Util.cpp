#include "StdAfx.h"
#include <locale.h>
#include "resource.h"
#include "Util.h"
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

HGLOBAL CUtil::m_hG;
LPCTSTR CUtil::m_szMimeType; //contend type

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
	TCHAR szPath[MAX_PATH];
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

CString CUtil::GetFileType(LPCTSTR szFile)
{
	CString strFile = szFile;
	int ps = strFile.ReverseFind('\\');
	int pt = strFile.ReverseFind('.');
	if (pt > 0 && (ps < pt) )
	{
		CString strExt = strFile.Right(strFile.GetAllocLength()-pt);
		CString strtype;
		GetFileMemiType(strExt, strtype);
		return strtype;
	}
	else
		return CString(TEXT("application/octet-stream"));
}

HANDLE CUtil::RunProc(LPCTSTR strcmd, LPCTSTR strparam, LPCTSTR strPath, BOOL bsync )
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = strcmd;            
	ShExecInfo.lpParameters = strparam;    
	ShExecInfo.lpDirectory = strPath;
	ShExecInfo.nShow = bsync ? SW_HIDE : SW_SHOW;
	ShExecInfo.hInstApp = NULL;      
	ShellExecuteEx(&ShExecInfo);
	if ( bsync )
		WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	return ShExecInfo.hProcess;
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
    char * psbuf = astr.GetBuffer(slen*2);
    int ret = ::WideCharToMultiByte(CP_UTF8, 0, wstr, slen,psbuf, slen*2,0,0);
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

CString ConvertXmlString(CString &str)
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

#include<stdlib.h> ����
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
}

CSetting * g_pSet = NULL;

#include "libs.h"
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
	}
}


int BaseAppInit()
{
	  // ������׼������DDE�����ļ������������� 
    CCommandLineInfo cmdInfo; 
    ParseCommandLine(cmdInfo); 

	_tsetlocale(LC_CTYPE, _T("chs"));
    //�����������
#if 0
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

	// ��ʼ��WinInet ����
	HINTERNET hInternet = InternetOpen(_T("CEHTTP"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	// ��http session
	HINTERNET hSession = InternetConnect(hInternet, szServer, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	// ��http post ����ľ��
	LPCTSTR szAccept[] = { _T("*/*"), NULL };
	HINTERNET hRequest = HttpOpenRequest(hSession, _T("POST"), url, NULL, NULL, szAccept, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES, 0);

	// �ⷢ��header
	TCHAR headerLanguage[] = _T("Accept-Language: zh-cn\r\n");
	TCHAR headerEncoding[] = _T("Accept-Encoding: gzip, deflate\r\n");
	TCHAR headerContentType[] = _T("Content-Type: application/json\r\n");//text/xml
	CString headerHost , headerContentLength;

	headerHost.Format(_T("Host: %s:%d\r\n"), szServer, nPort);
	headerContentLength.Format(_T("Content-Length: %d\r\n\r\n"), dlen);// strlen(post_data));

	// ���header ��Ϣ
	bResult = HttpAddRequestHeaders(hRequest, headerLanguage, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerEncoding, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerHost, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerContentType, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	bResult = HttpAddRequestHeaders(hRequest, headerContentLength, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

	/*=====================================================================
	// �򵥵ķ������ݵķ����������������������ݣ����ύGET����
	//===================================================================*/
	//bResult = HttpSendRequest(hRequest, NULL, 0, post_data, _tcsclen(post_data));

	/*=====================================================================
	// ���ʹ������ݰ��ķ���
	//===================================================================*/
	INTERNET_BUFFERS BufferIn = { 0 };

	BufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
	BufferIn.dwBufferTotal = dlen;// strlen(post_data);

	bResult = HttpSendRequestEx(hRequest, &BufferIn, NULL, 0, 0);

	DWORD written = 0;
	bResult = InternetWriteFile(hRequest, (LPVOID)(char *)postdata, dlen, &written);

	bResult = HttpEndRequest(hRequest, NULL, 0, 0);
	/*=====================================================================
	// ���ʹ������ݰ�����
	//===================================================================*/

	LPSTR     lpszData = NULL; // buffer for the data
	DWORD     dwSize = 0;           // size of the data available
	DWORD     dwDownloaded = 0; // size of the downloaded data

	// ����header �Ĵ�С��ע�������bResult ΪFALSE
	bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &dwSize, NULL);

	// Ϊ����header �����ڴ�ռ�
	CHAR* lpHeadersA = new CHAR[dwSize];

	// ����http response �е�header
	bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpHeadersA, &dwSize, NULL);

	strResponse.Empty();
	CStringA str;
	// ѭ����ȡ���� 
	do
	{
		// �����http response ���ж����ֽڿ��Զ�ȡ
		if (InternetQueryDataAvailable(hRequest, &dwSize, 0, 0))
		{
			lpszData = new char[dwSize + 1];
			// ��ȡ����
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
	// �رվ��
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
}

//// �ⷢ��header
//TCHAR headerLanguage[] = _T("Accept-Language: zh-cn\r\n");
//TCHAR headerEncoding[] = _T("Accept-Encoding: gzip, deflate\r\n");
//TCHAR headerContentType[] = _T("Content-Type: application/zip\r\n\r\n");//text/xml
//

BOOL HttpPostEx(LPCTSTR szServer, int nPort, LPCTSTR url, LPCTSTR szCookies[], int nheaders, \
	int ncookies, void * postdata, int dlen, CString &strResponse)
{
	BOOL bResult = FALSE;

	// ��ʼ��WinInet ����
	HINTERNET hInternet = InternetOpen(_T("UPHTTP"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	// ��http session
	HINTERNET hSession = InternetConnect(hInternet, szServer, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	// ��http post ����ľ��
	LPCTSTR szAccept[] = { _T("*/*"), NULL };
	HINTERNET hRequest = HttpOpenRequest(hSession, _T("POST"), url, NULL, NULL, szAccept, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES, 0);

	try{
		CString headerHost, headerContentLength;
		headerHost.Format(_T("Host: %s:%d\r\n"), szServer, nPort);
		headerContentLength.Format(_T("Content-Length: %d\r\n\r\n"), dlen);// strlen(post_data));

		// ���header ��Ϣ
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
		// �򵥵ķ������ݵķ����������������������ݣ����ύGET����
		//===================================================================*/
		//bResult = HttpSendRequest(hRequest, NULL, 0, post_data, _tcsclen(post_data));

		/*=====================================================================
		// ���ʹ������ݰ��ķ���
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
		// ���ʹ������ݰ�����
		//===================================================================*/

		LPSTR     lpszData = NULL; // buffer for the data
		DWORD     dwSize = 0;           // size of the data available
		DWORD     dwDownloaded = 0; // size of the downloaded data

		// ����header �Ĵ�С��ע�������bResult ΪFALSE
		bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, NULL, &dwSize, NULL);
		if (bResult == FALSE)	throw "HEADER error";

		// Ϊ����header �����ڴ�ռ�
		CHAR* lpHeadersA = new CHAR[dwSize];

		// ����http response �е�header
		bResult = HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpHeadersA, &dwSize, NULL);
		if (bResult == FALSE)	throw "HEADER error";

		strResponse.Empty();
		CStringA str;
		// ѭ����ȡ���� 
		do
		{
			// �����http response ���ж����ֽڿ��Զ�ȡ
			if (InternetQueryDataAvailable(hRequest, &dwSize, 0, 0))
			{
				lpszData = new char[dwSize + 1];
				// ��ȡ����
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
	// �رվ��
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
	return bResult;
}

#include "iostream" 
using namespace std;
#pragma comment(lib,"ws2_32.lib")    
long l_file_len;
//��ȡ�ļ�����    
bool file_con(char **buffer, LPCSTR file)
{
	FILE *fp = fopen(file, "rb");
	if (fp == NULL)
	{
		printf("�ļ��ϴ�ʧ�ܣ������ļ�·��.....\n");
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
	//----------------------postͷ��ʼ--------------------------------    
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
	content += "Content-Disposition: form-data; name=\"file\"; filename=\"�����ĺ�ʵ�����.rar\"\r\n";
	content += "Content-Type: aapplication/octet-stream\r\n\r\n";

	//postβʱ���    
	std::string strContent("\r\n------------------------------64b23e4066ed\r\n");
	char temp[64] = { 0 };
	//ע�������������Content-Length���������ֵ�ǣ�http����ͷ����+����β����+�ļ��ܳ���    
	sprintf(temp, "Content-Length: %d\r\n\r\n", content.length() + l_file_len + strContent.length());
	header += temp;
	std::string str_http_request;
	str_http_request.append(header).append(content);
	//----------------------postͷ����-----------------------------------    
	//����postͷ    
	send(sock, str_http_request.c_str(), str_http_request.length(), 0);

	char fBuff[4096];
	int nPacketBufferSize = 4096; // ÿ�����ݰ�����ļ���buffer��С    
	int nStart;//��¼post��ʼλ��    
	int nSize;//��¼ʣ���ļ���С    
	// �ͷֿ鴫��    
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