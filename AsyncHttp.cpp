// AsyncHttp.cpp : 实现文件
//

#include "stdafx.h"
#include "AsyncHttp.h"
#include "Util.h"
#include <process.h>

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
	if (started && !detached)
	{
		TerminateThread(threadHandle, 0);
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

	// 初始化WinSock.
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		TRACE("WSAStartup\n");
		return;
	}

	// 检查 WinSock 版本.
	if (wsaData.wVersion != wVersionRequested)
	{
		TRACE("WinSock version not supported\n");
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

		TRACE(szBufferA);

		if (iReturn == SOCKET_ERROR)
		{
			szError.Format(TEXT("No data is received, recv failed. Error: %d"),
				WSAGetLastError());
			MessageBox(NULL, szError, TEXT("Client"), MB_OK);
			break;
		}
		else if (iReturn<MAX_RECV_LEN){
			TRACE("Finished receiving data\n");
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
//--------------- 
SOCKET CDealSocket::GetConnect(LPCSTR host, int port)
{
	SOCKET hSocket;
	SOCKADDR_IN saServer;          // 服务器套接字地址.
	PHOSTENT phostent = NULL;	   // 指向HOSTENT结构指针.

	// 创建一个绑定到服务器的TCP/IP套接字.
	if ((hSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		TRACE("Allocating socket failed. Error: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}
	m_hSocket = hSocket;

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
}

CAsyncHttp::CAsyncHttp(LPCTSTR szIP, LPCTSTR szUrl, int port )
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
	m_bProxyMode = FALSE;
}

CAsyncHttp::~CAsyncHttp()
{
	if (m_pSocket)
		delete m_pSocket;
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

void CAsyncHttp::HttpProcess()
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
		if (SendHttpHeader())
		{
			if (SendData() && onFinish)
			{
				if (onFinish(param) > 0)
					return ;
			}
		}
		if (m_pSocket) m_pSocket->Close();
	}
}
void * CAsyncHttp::run(void * param)
{
	CAsyncHttp * ptr = (CAsyncHttp*)param;
	ptr->HttpProcess();
	return NULL;
}

INT CAsyncHttp::Connect()
{
	start(this->param);
	return 1;
}

INT CAsyncHttp::Disconnect()
{
	if (m_pSocket) m_pSocket->Close();
	stop();//thread	
	return 1;
}

INT CAsyncHttp::ConnectHttp()
{
	TRACE("正在建立连接\n");
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

void CAsyncHttp::AppendHeader(LPCTSTR szheader)
{
	CStringA sheadr;
	QUnc2Utf(szheader, sheadr);
	m_vstrHeaders.insert(m_vstrHeaders.end(), sheadr);
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

INT CAsyncHttp::SendData()
{
	if (m_pSocket)
	{
		if (m_pBody == NULL) return TRUE;
		if (send(m_pSocket->m_hSocket, (char*)m_pBody, m_nBodyLen, 0) != SOCKET_ERROR)
		{
			return TRUE;
		}
	}
	closesocket(m_pSocket->m_hSocket);
	TRACE("发送请求失败!\n");
	return FALSE;
}

//<====================CAsyncHttp class<============================<<<<<<<<<

CHttpPost::~CHttpPost()
{
	
}
CHttpPost::CHttpPost(LPCTSTR szIP, LPCTSTR szUrl, int port, LPCTSTR shdr[], int headerCount)
	:CAsyncHttp( szIP,  szUrl,  port )
{
	m_szHttpType = "POST";
	/*	sheadr += "Content-Type: ";
	sheadr += (ftype == NULL) ? "application/octet-stream" : ftype;
	sheadr += "\r\n";
	stemp.Format("Content-Length: %d\r\n\r\n", len);
	sheadr += stemp;*/
}

//<====================CHttpPost class<============================<<<<<<<<<
INT OnHttpHeaderSend(void * param)
{
	CGetHttp * phttp = (CGetHttp*)param;
	phttp->RecvData();

	
	return 0;
}


CGetHttp::~CGetHttp()
{

}

CGetHttp::CGetHttp(LPCTSTR szIP, LPCTSTR szUrl, int port, LPCTSTR shdr[], int headerCount)
	:CAsyncHttp( szIP,  szUrl,  port )
{
	m_szHttpType = "GET";
	this->onFinish = OnHttpHeaderSend;
}

void CGetHttp::GetFile(LPCTSTR slclfname)
{
	m_strLocalFile = slclfname;
	Connect();
}

INT CGetHttp::RecvData()
{
	int len = GetHttpHeader(m_szRespHeader);
	if (len > 0)
	{
		if (m_szRespHeader.Find(" 200 OK") >= 0)
		{
			const char szContLen[] = { "Content-Length:" };
			int ps = m_szRespHeader.Find(szContLen);
			if (ps > 0)
			{
				int len = 0;
				sscanf((LPCSTR)m_szRespHeader + ps + sizeof(szContLen), "%d", &len);
				char * buf = new char[len + 1];
				recv(m_pSocket->m_hSocket, buf, len, 0);
				CFile of;
				if (of.Open(m_strLocalFile, CFile::modeCreate | CFile::modeReadWrite))
				{
					of.Write(buf, len);
					of.Close();
				}
				delete buf;
			}
		}
	}
	return 0;
}

//<====================CHttpPost class<============================<<<<<<<<<