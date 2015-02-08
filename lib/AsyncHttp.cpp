// AsyncHttp.cpp : 实现文件
//

#include "stdafx.h"
#include "AsyncHttp.h"
#include "Util.h"
#include <process.h>
CHttpResMan * g_pHttpResMan = NULL;
//>>>>>>>>====================>thread class============================>
unsigned int __stdcall threadFunction(void * object)
{
	Thread * thread = (Thread *)object;
	return (unsigned int)thread->run(thread->param);
}

Thread::Thread()
{
	started = false;
	detached = false;
}

Thread::~Thread()
{
	stop();
}

int Thread::start(void* pra)
{
	if (!started)
	{
		param = pra;
		if (threadHandle = (HANDLE)_beginthreadex(NULL, 0, threadFunction, this, 0, &threadID))
		{
			if (detached)
			{
				CloseHandle(threadHandle);
			}
			started = true;
		}
	}
	return started;
}

//wait for current thread to end.
void * Thread::join()
{
	DWORD status = (DWORD)NULL;
	if (started && !detached)
	{
		WaitForSingleObject(threadHandle, INFINITE);
		GetExitCodeThread(threadHandle, &status);
		CloseHandle(threadHandle);
		detached = true;
	}
	return (void *)status;
}

void Thread::detach()
{
	if (started && !detached)
	{
		CloseHandle(threadHandle);
	}
	detached = true;
}

void Thread::stop()
{
	DWORD status = (DWORD)NULL;
	if (started && !detached)
	{
		if (!GetExitCodeThread(threadHandle, &status) || status == STILL_ACTIVE)
		{
			WaitForSingleObject(threadHandle, 1000);
			TerminateThread(threadHandle, 0);
		}
		CloseHandle(threadHandle);
		detached = true;
	}
}

void Thread::Reset()
{
	started = false;
	detached = false;
}
//<====================thread class<============================<<<<<<<<<


//---------------------------------------------------------------------------
CDealSocket::CDealSocket()
{
	// 套接字初始化.
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	m_curTxBytes = 0;
	m_CurRecvBytes = 0;

	// 初始化WinSock.
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		//TRACE("WSAStartup\n");
		return;
	}

	// 检查 WinSock 版本.
	if (wsaData.wVersion != wVersionRequested)
	{
		//TRACE("WinSock version not supported\n");
		WSACleanup();
		return;
	}
}

//------------
CDealSocket::~CDealSocket()
{
	// 释放WinSock.
	WSACleanup();
}

//------------
CStringA CDealSocket::GetResponse()
{
	char szBufferA[MAX_RECV_LEN];  	// ASCII字符串. 
	int	iReturn;					// recv函数返回的值.

	CString szError;
	CStringA strPlus;
	strPlus.Empty();

	while (1)
	{
		// 从套接字接收资料.
		iReturn = recv(m_hSocket, szBufferA, MAX_RECV_LEN, 0);
		szBufferA[iReturn] = 0;
		strPlus += szBufferA;

		//TRACE(szBufferA);

		if (iReturn == SOCKET_ERROR)
		{
			szError.Format(TEXT("No data is received, recv failed. Error: %d"),
				WSAGetLastError());
			MessageBox(NULL, szError, TEXT("Client"), MB_OK);
			break;
		}
		else if (iReturn<MAX_RECV_LEN){
			//TRACE("Finished receiving data\n");
			break;
		}
	}
	return strPlus;
}

BOOL CDealSocket::Send(CStringA sdata)
{
	int iLen = sdata.GetLength();
	if (send(m_hSocket, sdata, iLen, 0) == SOCKET_ERROR)
	{
		closesocket(m_hSocket);
		TRACE("发送请求失败!\n");
		return FALSE;
	}
	return TRUE;
}

INT CDealSocket::Send(void * buf, int len)
{
	m_SendTotLen = len;
	m_curTxBytes = 0;
	while (m_curTxBytes < m_SendTotLen)
	{
		int leftlen = m_SendTotLen - m_curTxBytes;
		int slen = leftlen > 64 * 1024 ? 64 * 1024 : leftlen;
		slen = send(m_hSocket, ((char*)buf) + m_curTxBytes, slen, 0);
		if (slen <= 0)// SOCKET_ERROR)
		{
			return 0;
		}
		m_curTxBytes += slen;
	}
	return m_curTxBytes;
}

INT CDealSocket::Recv(void * buf, int len)
{
	char * ptr = (char *)buf;
	m_CurRecvBytes = 0;
	int timeout = 4;
	m_RecvTotLen = len;
	while (m_CurRecvBytes < len)
	{
		int plen = (len - m_CurRecvBytes) > 4096 ? 4096 : (len - m_CurRecvBytes);
		int rxb = 0;
		rxb = recv(m_hSocket, ptr, plen, 0);
		if (rxb <= 0)
		{
			if (--timeout == 0) return m_CurRecvBytes;
		}
		else
		{
			timeout = 4; //TRACE("socked read timeout %d\r\n", m_CurRecvBytes);
		}
		m_CurRecvBytes += rxb;
		ptr += rxb;
	}	
	return m_CurRecvBytes;
}

void CDealSocket::GetRecvStatus(INT * p_ncur, INT * p_nTot)
{
	if (p_ncur) *p_ncur = m_CurRecvBytes;
	if (p_nTot) *p_nTot = m_RecvTotLen;
}

//--------------- 
SOCKET CDealSocket::GetConnect(LPCSTR host, int port)
{
	SOCKET hSocket;
	SOCKADDR_IN saServer;          // 服务器套接字地址.
	PHOSTENT phostent = NULL;	   // 指向HOSTENT结构指针.

	// 创建一个绑定到服务器的TCP/IP套接字.
	if ((hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		TRACE("Allocating socket failed. Error: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	unsigned long ulSockeMode = 0; // 0 block mode, 1, unblock mode
	if (ioctlsocket(hSocket, FIONBIO, &ulSockeMode) == SOCKET_ERROR)
	{
		TRACE("ioctlsocket errno:\t%d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	// 使用TCP/IP协议.
	saServer.sin_family = AF_INET;

	// 获取与主机相关的信息.
	if ((phostent = gethostbyname(host)) == NULL)
	{
		TRACE("Unable to get the host name. Error: %d\n", WSAGetLastError());
		closesocket(hSocket);
		return INVALID_SOCKET;
	}

	// 给套接字IP地址赋值.
	memcpy((char *)&(saServer.sin_addr),
		phostent->h_addr,
		phostent->h_length);

	// 设定套接字端口号.
	saServer.sin_port = htons(port);

	// 建立到服务器的套接字连接.
	if (connect(hSocket, (PSOCKADDR)&saServer,
		sizeof(saServer)) == SOCKET_ERROR)
	{
		TRACE("Connecting to the server failed. Error: %d\n", WSAGetLastError());
		closesocket(hSocket);
		return INVALID_SOCKET;
	}
	m_hSocket = hSocket;
	return hSocket;
}

//------------------ 
SOCKET CDealSocket::Listening(int port)
{
	SOCKET ListenSocket = INVALID_SOCKET;	// 监听套接字.
	SOCKADDR_IN local_sin;				    // 本地套接字地址.

	// 创建TCP/IP套接字.
	if ((ListenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		TRACE("Allocating socket failed. Error: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	// 给套接字信息结构赋值.
	local_sin.sin_family = AF_INET;
	local_sin.sin_port = htons(port);
	local_sin.sin_addr.s_addr = htonl(INADDR_ANY);

	// 进行本机地址与监听套接字绑定.
	if (bind(ListenSocket,
		(struct sockaddr *) &local_sin,
		sizeof(local_sin)) == SOCKET_ERROR)
	{
		TRACE("Binding socket failed. Error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		return INVALID_SOCKET;
	}

	// 建立套接字对外部连接的监听.
	if (listen(ListenSocket, MAX_PENDING_CONNECTS) == SOCKET_ERROR)
	{
		TRACE("Listening to the client failed. Error: %d\n",
			WSAGetLastError());
		closesocket(ListenSocket);
		return INVALID_SOCKET;
	}

	return ListenSocket;
}

void CDealSocket::Close()
{
	closesocket(m_hSocket);
}

//>>>>>>>>====================>CAsyncHttp class============================>
CAsyncHttp::CAsyncHttp() :Thread()
{
	m_nport = 80;
	m_bProxyMode = FALSE;
	m_szHttpType = "GET ";
	param = this;
	m_pSocket = NULL;
	m_pBody = NULL;
	m_nBodyLen = 0;
	onFinish = NULL;
	m_pMsgWnd = g_pHttpResMan;
	m_pBuff = NULL;
}

CAsyncHttp::CAsyncHttp(LPCTSTR szIP, LPCTSTR szUrl, CWnd * pmsgWnd, int port )
	:Thread()
{
	QUnc2Utf(szIP, m_hostIP);
	QUnc2Utf(szUrl, m_url);
	m_nport = port;
	param = this;
	onFinish = NULL;
	m_pSocket = NULL;
	m_pBody = NULL;
	m_nBodyLen = 0;
	m_nTotolLen = -1;
	m_bProxyMode = FALSE;
	m_pBuff = NULL;
	m_hFile = NULL;
	m_hFileMap = NULL;
	m_pMsgWnd = pmsgWnd == NULL ?  g_pHttpResMan : pmsgWnd ;
}

CAsyncHttp::~CAsyncHttp()
{
	if (m_pSocket)
		delete m_pSocket;
	if (m_pBuff != NULL)
	{
		free(m_pBuff); //malloc different to new
		m_pBuff = NULL;;
	}
	m_vstrHeaders.clear();
}

void CAsyncHttp::SetProxy(LPCTSTR szProxyIp, int nProxyPort, LPCTSTR szUser, LPCTSTR szpwd)
{
	m_bProxyMode = TRUE;
	QUnc2Utf(szProxyIp, m_szProxyIp);
	m_nProxyPort = nProxyPort;
	if (szUser)
	{
		QUnc2Utf(szUser, m_szProxyUser);
		if (szpwd)
			QUnc2Utf(szpwd, m_szProxyPwd);
		else
			m_szProxyPwd.Empty();
	}
	else
	{
		m_szProxyUser.Empty();
	}
}

void * CAsyncHttp::run(void * param)
{
	if (m_pSocket == NULL)
		m_pSocket = new CDealSocket();
	int bret = 0;
	if (m_bProxyMode)
	{
		bret = ConnectProxyHttp();
	}
	else
	{
		bret = ConnectHttp();
	}
	if (bret > 0)
	{
		if ((bret = SendHttpHeader()) == TRUE)
		{
			if (( bret = OnHttpHeaderSend()) == TRUE)
			{
				bret = SendData();		
				if (bret > 0)
				{
					bret = OnHttpSend();
				}
			}
		}

		if (onFinish  && onFinish(param, bret) < 0 )
			return NULL;	
	}

	if (m_pMsgWnd && m_pMsgWnd->GetSafeHwnd() &&
		PostMessage(m_pMsgWnd->GetSafeHwnd(), WM_HTTP_DONE, (WPARAM)this, bret) )
	{
		return NULL;
	}
	if (m_pSocket) m_pSocket->Close();	
	return NULL;
}

INT CAsyncHttp::Connect()
{
	start(this->param);
	return 1;
}

INT CAsyncHttp::Disconnect()
{
	
	stop();//thread	
	if (m_pSocket) m_pSocket->Close();
	return 1;
}

INT CAsyncHttp::ConnectHttp()
{
	//TRACE("正在建立连接\n");
	SOCKET hSocket = m_pSocket->GetConnect(m_hostIP, m_nport);
	if (hSocket == INVALID_SOCKET)
		return -1;
	return 1;
}

INT CAsyncHttp::ConnectProxyHttp()
{
	AString sTemp;
	SOCKET hSocket = m_pSocket->GetConnect(m_szProxyIp, m_nProxyPort);
	if (hSocket == INVALID_SOCKET)
	{
		TRACE("连接http服务器失败！\n");
		return -1;
	}
	sTemp.Format("CONNECT %s:%d HTTP/1.1\r\nUser-Agent:"\
		"HttpApp/0.1\r\n\r\n", m_szProxyIp, m_nProxyPort);

	if (!m_pSocket->Send(sTemp))
	{
		m_pSocket->Close();
		TRACE("连接代理失败\n");
		return -2;
	}

	int nLen = GetHttpHeader(m_szRespHeader);
	if (m_szRespHeader.Find("200 Connection established") < 0)
	{
		m_pSocket->Close();
		return -3;
	}
	TRACE("代理连接完成\n");
	return 1;
}


DWORD CAsyncHttp::GetHttpHeader(CStringA &strResp)
{
	char sbuf[2] = {0,0};
	int iread = 0;
	while (1)
	{
		int ret = recv(m_pSocket->m_hSocket, sbuf, 1, 0);
		if (ret > 0)
		{
			iread++;
			strResp += sbuf;
			if (iread > 4)
			{
				LPCSTR ptr = strResp;
				if (memcmp(ptr + iread - 4, "\r\n\r\n", 4) == 0)
					return iread;
			}
		}
		else if (ret == 0)
			return 0; //socket closed
		else 
		{	// if (!(ret == EINTR || ret == EWOULDBLOCK || ret == EAGAIN)
			return -1;
		}
	}
}

void CAsyncHttp::AppendHeader(LPCWSTR szheader)
{
	CStringA sheadr;
	QUnc2Utf(szheader, sheadr);
	m_vstrHeaders.insert(m_vstrHeaders.end(), sheadr);
}

void  CAsyncHttp::AppendHeader(LPCSTR szheader)
{
	m_vstrHeaders.insert(m_vstrHeaders.end(), szheader);
}

BOOL CAsyncHttp::SendHttpHeader()
{
	CStringA sheadr;
	sheadr.Format("%s %s HTTP/1.1\r\n", m_szHttpType, m_url);
	CStringA stemp;
	stemp.Format("Host: %s:%d\r\n", m_hostIP, m_nport);
	sheadr += stemp;
	sheadr += "Accept: */*\r\n"
		"User-Agent: Mozilla/4.0\r\n";
	for (vector<CStringA>::iterator it = m_vstrHeaders.begin(); it != m_vstrHeaders.end(); it++)
	{
		sheadr += *it;
		sheadr.TrimRight();
		LPCSTR ptr = (LPCSTR)sheadr + sheadr.GetLength() -2;
		if (ptr[0] != '\r' && ptr[1] != '\n')
			sheadr += "\r\n";
	}
	sheadr += "\r\n";
	return m_pSocket->Send(sheadr);
}

INT CAsyncHttp::GetContentLen(CStringA &sheadr)
{
	const char szContLen[] = { "Content-Length:" };
	int ps = sheadr.Find(szContLen);
	int len = -1;
	if (ps > 0)
	{
		sscanf_s((LPCSTR)sheadr + ps + sizeof(szContLen), "%d", &len);
	}
	return len;
}

INT CAsyncHttp::SendData()
{
	if (m_pSocket)
	{
		if (m_pBody == NULL || m_nBodyLen <= 0) return TRUE;
		return m_pSocket->Send(m_pBody, m_nBodyLen);
	}
	return FALSE;
}


INT CAsyncHttp::GetChunckSize()
{
	int iread = 0;
	char sbuf[64];
	while (1)
	{
		int ret = recv(m_pSocket->m_hSocket, sbuf + iread, 1, 0);
		if (ret > 0)
		{
			iread++;
			if (sbuf[iread - 1] == '\n' && iread > 2)
			{
				if ( sbuf[iread - 2] == '\r')
				{
					ret = atox(sbuf);
					return ret;
				}
			}
		}
		else if (ret == 0)
		{
			return 0; //socket closed
		}
		else
		{	// if (!(ret == EINTR || ret == EWOULDBLOCK || ret == EAGAIN)
			return -1;
		}
	}
}

void CAsyncHttp::CancelHttp()
{
	Disconnect();
	
	if (m_hFileMap)
	{
		UnmapViewOfFile(m_pBody);
		CloseHandle(m_hFileMap);
	}
	if (m_hFile)
	{
		::CancelIo(m_hFile);
		CloseHandle(m_hFile);
	}
	m_hFile = NULL;
	m_hFileMap = NULL;

}

INT CAsyncHttp::GetBody(LPCTSTR szLocalFile)
{
	if (szLocalFile  == NULL )return GetBody();

	if (m_pBuff != NULL) { free(m_pBuff); m_pBuff = NULL; }

	if (m_szRespHeader.IsEmpty())
		GetHttpHeader(m_szRespHeader);
	m_hFile = CreateFile(szLocalFile, GENERIC_READ | GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)		return -1;

	DWORD wb = 0;
	INT len = GetContentLen(m_szRespHeader);
	m_nTotolLen = len;
	m_nBodyLen = 0;
	if (len <= 0)
	{
		if (m_szRespHeader.Find("Transfer-Encoding: chunked") >= 0)
		{
			int nbuf_len = 16 * 1024;
			BYTE* ptr = (BYTE*)malloc(nbuf_len);
			while (1)
			{
				len = GetChunckSize();
				if (len == 0) break;	
				if (len > nbuf_len)
				{
					free(ptr);
					ptr = (BYTE *)malloc(len + 16);
					nbuf_len = len + 16;
				}
				if (ptr)
				{
					len = m_pSocket->Recv(ptr, len);
					if (len == 0) break;
					WriteFile(m_hFile, ptr, len, &wb, NULL);
					m_nBodyLen += wb;
				}
				else
					break;
			}
		}
		else 
			m_nBodyLen = -1;
	}
	else
	{
		m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, len, NULL);
		if (m_hFileMap)
		{
			m_pBody = (BYTE *)MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, len);// (BYTE*)malloc(len + 1);
			len = m_pSocket->Recv(m_pBody, len);
			m_nBodyLen = len;
			UnmapViewOfFile(m_pBody);
			CloseHandle(m_hFileMap);
			m_hFileMap = NULL;
		}
	}

	CloseHandle(m_hFile);
	m_hFile = NULL;
	
	return m_nBodyLen;
}

INT CAsyncHttp::GetChunk(int len)
{
	return 0;
}

INT CAsyncHttp::GetBody( )
{
	if (m_szRespHeader.IsEmpty())
		GetHttpHeader(m_szRespHeader);
	INT len = GetContentLen(m_szRespHeader);
	
	m_nTotolLen = len;
	m_nBodyLen = 0;
	if (len <= 0 )
	{
		if (m_pBuff != NULL) { free(m_pBuff); m_pBuff = NULL; }
		if (m_szRespHeader.Find("Transfer-Encoding: chunked") >= 0)
		{
			while (1)
			{
				len = GetChunckSize();
				if (len == 0) break;
				BYTE * ptr = (BYTE *)realloc(m_pBuff, m_nBodyLen + len + 1);
				if (ptr)
				{
					m_pBuff = ptr;
					len = m_pSocket->Recv(m_pBuff + m_nBodyLen, len);
					if (len == 0) break;
					m_nBodyLen += len;
				}
				else
					break;
			}
		}
	}
	else
	{
		if (m_pBuff == NULL|| m_nBodyLen < len + 1)
		{
			if (m_pBuff != NULL) { free(m_pBuff); m_pBuff = NULL; }
			m_pBuff = (BYTE*)malloc(len + 1);
		}
		m_pSocket->Recv(m_pBody, len);
		m_nBodyLen = len;
	}
	m_pBuff[m_nBodyLen] = 0;
	m_pBody = m_pBuff;
	return m_nBodyLen;
}


INT CAsyncHttp::ParseUrl(CString &url, CString &sIp, int &port)
{
	int p = url.Find(_T("://"), 0);
	if (p < 0) p = 0;
	int p2 = url.Find(_T('/'), p+3);
	if (p2 < 3) return -1;
	sIp = url.Mid(p+3, p2 - p-3);
	url.Delete(0, p2);
	p = sIp.Find(_T(':'));
	if (p < 0)
	{
		port = 80;
	}
	else
	{
		port = _ttoi((LPCTSTR)sIp + p + 1);
		sIp.Delete(p, sIp.GetLength() - p);
	}
	return p;
}

INT CAsyncHttp::ParseUrlFile(CString url, CString &sfile)
{
	sfile = url;
	int p = url.ReverseFind(_T('//'));
	if (p >= 0)
		sfile.Delete(0, p + 1);
	return 0;
}

//<====================CAsyncHttp class<============================<<<<<<<<<
CHttpPost::~CHttpPost()
{
	
}

CHttpPost::CHttpPost(LPCTSTR szIP, LPCTSTR szUrl, CWnd * pmsgWnd, int port, LPCTSTR shdr[], int headerCount)
	:CAsyncHttp(szIP, szUrl, pmsgWnd, port)
{	
	m_szHttpType = "POST";
	for (int i = 0; i < headerCount; i++)
		AppendHeader(shdr[i]);
}


INT CHttpPost::SendFile(LPCTSTR slclfname, void * param)
{
	m_strLocalFile = slclfname;
	CFile of;
	if (of.Open(slclfname, CFile::modeRead | CFile::shareDenyNone))
	{
		m_nBodyLen = of.GetLength();
		m_pBuff = (BYTE *)malloc(m_nBodyLen);
		m_pBody = m_pBody;
		of.Read(m_pBody, m_nBodyLen);
		of.Close();
		CString strtype = CUtil::GetFileType(slclfname);
		CStringA stype; QUnc2Utf(strtype, stype);
		CStringA sheadr;
		sheadr.Format("Content-Type: %s\r\n", stype);
		AppendHeader(sheadr);
		sheadr.Format("Content-Length: %d\r\n\r\n", m_nBodyLen);
		AppendHeader(sheadr);
		if (param != NULL)
			this->param = param;
		Connect();
	}
	return 0;
}

INT CHttpPost::SendFile(const void * ptr, int len, LPCTSTR sztype)
{
	CStringA stype; 
	if (sztype == NULL) 
		stype = "application/octet-stream";
	else
		QUnc2Utf(sztype, stype);
	CStringA sheadr;
	sheadr.Format("Content-Type: %s\r\n", stype);
	AppendHeader(sheadr);
	sheadr.Format("Content-Length: %d\r\n\r\n", len);
	AppendHeader(sheadr);
	m_pBody = (BYTE*)ptr;
	m_nBodyLen = len;
	Connect();
	return 0;
}


INT CHttpPost::OnHttpHeaderSend()
{
	return 1;
}

INT CHttpPost::OnHttpSend()
{
	int len = GetHttpHeader(m_szRespHeader);
	if (len > 0)
	{
		if (m_szRespHeader.Find(" 200 OK") >= 0)
		{
			len = GetBody();
		}
		else
			return -5;
	}
	return len>0;
}


//<====================CHttpPost class<============================<<<<<<<<<
INT OnHttpGetDone(void * param, int http_status)
{
	CGetHttp * phttp = (CGetHttp*)param;
	return 0;
}


CGetHttp::~CGetHttp()
{

}

CGetHttp::CGetHttp(LPCTSTR szIP, LPCTSTR szUrl, CWnd * pmsgWnd, int port, LPCTSTR shdr[], int headerCount)
	:CAsyncHttp(szIP, szUrl, pmsgWnd, port)
{
	m_szHttpType = "GET";
	this->onFinish = OnHttpGetDone;
	for (int i = 0; i < headerCount; i++)
		AppendHeader(shdr[i]);
}

void CGetHttp::GetFile(LPCTSTR slclfname)
{
	m_strLocalFile = slclfname;
	Connect();
}

void CGetHttp::GetFile()
{
	Connect();
}

void CGetHttp::GetFile(const void * ptr, int len, LPCTSTR sztype )
{
	CStringA stype;
	if (sztype == NULL)
		stype = "application/octet-stream";
	else
		QUnc2Utf(sztype, stype);
	CStringA sheadr;
	sheadr.Format("Content-Type: %s\r\n", stype);
	AppendHeader(sheadr);
	sheadr.Format("Content-Length: %d\r\n\r\n", len);
	AppendHeader(sheadr);
	m_pBuff = (BYTE *)malloc(len);
	memcpy(m_pBuff, ptr, len);
	m_pBody = (BYTE*)m_pBuff;
	m_nBodyLen = len;
	Connect();
}


INT CGetHttp::OnHttpSend()
{
	int len = GetHttpHeader(m_szRespHeader);
	if (len > 0)
	{
		if (m_szRespHeader.Find(" 200 OK") >= 0)
		{
			LPCTSTR szLclFile = NULL;
			if(!m_strLocalFile.IsEmpty() && m_strLocalFile.GetLength() > 1 )
				szLclFile = m_strLocalFile;
			len = GetBody(szLclFile);
		}
		else
			return -5;
	}
	return len>0;
}


//<====================CHttpPost class<============================<<<<<<<<<


// CHttpResMan

IMPLEMENT_DYNAMIC(CHttpResMan, CWnd)

CHttpResMan::CHttpResMan()
{
}

CHttpResMan::~CHttpResMan()
{
}


BEGIN_MESSAGE_MAP(CHttpResMan, CWnd)
	ON_MESSAGE(WM_HTTP_DONE, &CHttpResMan::OnHttpFinishMsg)		//自定义事件
END_MESSAGE_MAP()


LRESULT CHttpResMan::OnHttpFinishMsg(WPARAM wParam, LPARAM lParam)
{
	CAsyncHttp * pHttp = (CAsyncHttp*)wParam;
	delete pHttp;
	return 0;
}

CWnd * CreateHttpManageWnd(CWnd * pParent)
{
	CHttpResMan * phttpMan = new CHttpResMan();
	phttpMan->Create(NULL, _T(""), WS_CHILD, CRect(0, 0, 0, 0), pParent, IDC_STATIC, NULL);
	g_pHttpResMan = phttpMan;
	return phttpMan;
}

// CHttpResMan 消息处理程序