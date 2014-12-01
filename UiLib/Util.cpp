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
	if (of.Open(sfilename, CFile::modeRead))
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
	if (of.Open(sfilename, CFile::modeRead))
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
	return sguid;
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

void CSetting::Load()
{
	CWinApp* pApp = AfxGetApp();
	m_strUserName = pApp->GetProfileString(_T("AppSettings"), _T("UserName"), _T("admin"));
	m_strPassword = pApp->GetProfileString(_T("AppSettings"), _T("Password"), _T(""));
	m_strServerIP = pApp->GetProfileString(_T("AppSettings"), _T("ServerIp"), _T("fixopen.xicp.net"));
	m_nPort = pApp->GetProfileInt(_T("AppSettings"), _T("Port"), 8080);
}

void CSetting::Save()
{
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("AppSettings"), _T("ServerIp"), m_strServerIP);
	pApp->WriteProfileInt(_T("AppSettings"), _T("Port"), m_nPort);
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
	HINTERNET hRequest = HttpOpenRequest(hSession, _T("POST"), url, NULL, NULL, szAccept, INTERNET_FLAG_NO_CACHE_WRITE, 0);

	// �ⷢ��header
	TCHAR headerLanguage[] = _T("Accept-Language: zh-cn\r\n");
	TCHAR headerEncoding[] = _T("Accept-Encoding: gzip, deflate\r\n");
	TCHAR headerContentType[] = _T("Content-Type: application/json\r\n\r\n");//text/xml
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

void HttpPostEx(LPCTSTR szServer, int nPort, LPCTSTR url, LPCTSTR szCookies[], int ncookies,\
			void * postdata, int dlen, CString &strResponse)
{
	BOOL bResult = FALSE;

	// ��ʼ��WinInet ����
	HINTERNET hInternet = InternetOpen(_T("CEHTTP"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	// ��http session
	HINTERNET hSession = InternetConnect(hInternet, szServer, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	// ��http post ����ľ��
	LPCTSTR szAccept[] = { _T("*/*"), NULL };
	HINTERNET hRequest = HttpOpenRequest(hSession, _T("POST"), url, NULL, NULL, szAccept, INTERNET_FLAG_NO_CACHE_WRITE, 0);


	CString headerHost, headerContentLength;
	headerHost.Format(_T("Host: %s:%d\r\n"), szServer, nPort);
	headerContentLength.Format(_T("Content-Length: %d\r\n\r\n"), dlen);// strlen(post_data));

	// ���header ��Ϣ
	for (int i = 0; i < ncookies; i++)
	{
		bResult = HttpAddRequestHeaders(hRequest, szCookies[i], -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
	}
	bResult = HttpAddRequestHeaders(hRequest, headerHost, -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);
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
	// �رվ��
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hSession);
	InternetCloseHandle(hInternet);
}
