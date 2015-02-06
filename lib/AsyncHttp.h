#pragma once
#include <vector>
using namespace std;
#ifndef WM_HTTP_DONE
#define WM_HTTP_DONE  (WM_APP + 137)
#endif /*WM_HTTP_DONE*/

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
	unsigned int threadID;

protected:
	virtual void * run(void *) = 0;//����ʵ���߳�����̺߳�������

protected:
	HANDLE threadHandle;
	bool started;
	bool detached;
	void * param;
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
	INT Send(void * buf, int len);
	INT Recv(void * buf, int len);
	void GetRecvStatus(INT * p_ncur, INT * p_nTot);
public: //for display progressing
	INT m_RecvTotLen;
	INT m_CurRecvBytes;
	INT m_SendTotLen;
	INT m_curTxBytes;
};


class CAsyncHttp : public Thread
{
protected:
	CAsyncHttp();           
	CAsyncHttp(LPCTSTR szIP, LPCTSTR szUrl ,CWnd * pmsgWnd, int port = 80);
	INT GetChunk(int size);
	HANDLE m_hFile;
	HANDLE m_hFileMap;

public:
	CStringA  m_szHttpType;
	CStringA  m_szRespHeader;    //
	void SetProxy(LPCTSTR szProxyIp, int nProxyPort, LPCTSTR szUser = NULL, LPCTSTR pwd = NULL);
	INT   Connect();
	void  CancelHttp();
	INT   Disconnect();	
	void  AppendHeader(LPCWSTR szheader);
	void  AppendHeader(LPCSTR szheader);
	DWORD GetHttpHeader(CStringA &strResp);
	INT GetBody(LPCTSTR szLocalFile );
	INT GetBody();
	void * m_pBody;
	int    m_nBodyLen;
	int    m_nTotolLen;
	CDealSocket * m_pSocket;

public:
	static INT ParseUrl(CString &url, CString &sIp, int &port);
	static INT ParseUrlFile(CString url, CString &sfile);
	void setOnFinish(void(*func)());
	virtual void * run(void *);
	virtual INT SendData() ;
	virtual ~CAsyncHttp();
	virtual INT OnHttpHeaderSend() { return 1; };
	virtual INT OnHttpSend() = 0;

protected:
	BOOL  SendHttpHeader();
	vector<CStringA> m_vstrHeaders;  //
	CStringA  m_hostIP;
	int       m_nport;
	CStringA  m_url;
	DWORD     m_bProxyMode;      // ����ģ̬. 
	CStringA  m_szProxyIp;       // �������������ַ.
	int       m_nProxyPort;      // �������˿ں�.
	CStringA  m_szProxyUser;     // ������������û��� Ϊ�ձ�ʾ����Ҫ��¼
	CStringA  m_szProxyPwd;      // ���������������.
	CWnd  * m_pMsgWnd;
	INT (*onFinish)(void * parm, INT http_send_status);//��ɺ���õĺ���. Status < 0 means fail ; return < 0 ,keep connection

	INT ConnectHttp();
	INT ConnectProxyHttp();
	INT GetContentLen(CStringA &sheader);
	BYTE * m_pBuff;

	INT GetChunckSize();

};

class CHttpPost : public CAsyncHttp
{
	CString m_strLocalFile;
public:
	CHttpPost(LPCTSTR szIP, LPCTSTR szUrl, CWnd * pmsgWnd , int port = 80, LPCTSTR shdr[] = NULL, int headerCount = 0);
	~CHttpPost();
	INT SendFile(LPCTSTR slclfname, void * param = NULL);
	INT SendFile(const void * ptr, int len, LPCTSTR sztype=NULL);//ptr need keep contend till finish
	virtual INT OnHttpHeaderSend();
	virtual INT OnHttpSend();
};

class CGetHttp : public CAsyncHttp
{
public:
	CString m_strLocalFile;
	CGetHttp(LPCTSTR szIP, LPCTSTR szUrl, CWnd * pmsgWnd = NULL, int port = 80, LPCTSTR shdr[] = NULL, int headerCount = 0);
	~CGetHttp();
	void GetFile(LPCTSTR slclfname);
	void GetFile();//buffer is m_pBody, length is m_nBodyLen;
	void GetFile(const void * ptr, int len, LPCTSTR sztype = NULL);
	virtual INT OnHttpSend();
};



class CHttpResMan : public CWnd
{
	DECLARE_DYNAMIC(CHttpResMan)

public:
	CHttpResMan();
	virtual ~CHttpResMan();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnHttpFinishMsg(WPARAM wParam, LPARAM lParam);
};



// CAsyncHttp

