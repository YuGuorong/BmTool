#pragma once
#include <vector>
using namespace std;

static unsigned int __stdcall threadFunction(void *);

class Thread {
	friend unsigned int __stdcall threadFunction(void *);
public:
	Thread();
	virtual ~Thread();
	int start(void * = NULL);//�߳����������������������������ָ�롣
	void stop();
	void* join();//�ȴ���ǰ�߳̽���
	void detach();//���ȴ���ǰ�߳�
	void Reset();//�õ�ǰ�߳����߸���ʱ�䣬��λΪ����

protected:
	virtual void * run(void *) = 0;//����ʵ���߳�����̺߳�������

protected:
	HANDLE threadHandle;
	bool started;
	bool detached;
	void * param;
	unsigned int threadID;
};

#define MAX_RECV_LEN           100   // ÿ�ν�������ַ�������.
#define MAX_PENDING_CONNECTS   4     // �ȴ����еĳ���.
class CDealSocket
{
public:
	CDealSocket();
	virtual ~CDealSocket();
public:
	SOCKET GetConnect(LPCSTR host, int port);
	SOCKET Listening(int port);
	SOCKET  m_hSocket;
	void Close();
	CStringA GetResponse();
	BOOL Send(CStringA sdata);
};


class CAsyncHttp : public Thread
{
protected:
	CAsyncHttp();           
	CAsyncHttp(LPCTSTR szIP, LPCTSTR szUrl, int port = 80);

public:
	CStringA  m_szHttpType;
	CStringA  m_szRespHeader;    //
	void SetProxy(LPCTSTR szProxyIp, int nProxyPort, LPCTSTR szUser = NULL, LPCTSTR pwd = NULL);
	INT   Connect();
	INT   Disconnect();
	void  AppendHeader(LPCTSTR szheader);
	void  HttpProcess();

public:
	void setOnFinish(void(*func)());
	virtual void * run(void *);
	virtual INT SendData() ;
	virtual ~CAsyncHttp();

protected:
	BOOL  SendHttpHeader();
	DWORD GetHttpHeader(CStringA &strResp);
	vector<CStringA> m_vstrHeaders;  //
	CStringA  m_hostIP;
	int       m_nport;
	CStringA  m_url;
	DWORD     m_bProxyMode;      // ����ģ̬. 
	CStringA  m_szProxyIp;       // �������������ַ.
	int       m_nProxyPort;      // �������˿ں�.
	CStringA  m_szProxyUser;     // ������������û��� Ϊ�ձ�ʾ����Ҫ��¼
	CStringA  m_szProxyPwd;      // ���������������.
	CDealSocket * m_pSocket;	
	INT (*onFinish)(void * parm);//��ɺ���õĺ���

	INT ConnectHttp();
	INT ConnectProxyHttp();

	void * m_pBody;
	int    m_nBodyLen;
};

class CHttpPost : public CAsyncHttp
{
public:
	CHttpPost(LPCTSTR szIP, LPCTSTR szUrl, int port = 80, LPCTSTR shdr[] = NULL, int headerCount = 0);
	~CHttpPost();
	INT SendFile(LPCTSTR slclfname);
};

class CGetHttp : public CAsyncHttp
{
	CString m_strLocalFile;
public:
	CGetHttp(LPCTSTR szIP, LPCTSTR szUrl, int port = 80, LPCTSTR shdr[] = NULL, int headerCount = 0);
	~CGetHttp();
	void GetFile(LPCTSTR slclfname);
	INT RecvData();
};

// CAsyncHttp