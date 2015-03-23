#pragma once

#define KByte    *1024
#define MByte    *1024*1024

class CPMutex
{
public:	/* Ĭ�ϴ��������Ļ��� */
	CPMutex(const TCHAR* name = NULL);
	~CPMutex();

	BOOL Lock();
	BOOL UnLock();
private:
	void* m_pMutex;
	TCHAR m_cMutexName[30];
};


class CUtil
{
public:
	CUtil(void);
	~CUtil(void);
	static void GetCurPath(CString &strPath);
	static CString GetUserFolder();
	static CString GetFilePath(CString &strFile);
	static CString GetFileName(CString &strFile);
	static CString GetFileType(LPCTSTR szFile);
	static DWORD GetFileSize(LPCTSTR sfile);
	static int GetFileExt(const CString &Sfile, CString &sext);
	static BOOL GetFileMemiType(LPCTSTR szExt, CString &stype);
	static HANDLE FindProcessByName(LPCTSTR szFileName, BOOL bKill = FALSE, INT exit_code = 0);
	static HANDLE CUtil::RunProc(LPCTSTR strcmd, LPCTSTR strparam, LPCTSTR strPath, BOOL bsync = TRUE, BOOL forcehide = TRUE);
	static CSize GetBitmapSize(CBitmap &bmp);
	static void DrawGradientFill(CDC * pdc, CRect &r, COLORREF clt, COLORREF cbt, int mode=GRADIENT_FILL_RECT_H);
	static CStringA File2Asc(LPCTSTR sfilename);
	static int Asc2File(LPCTSTR sfilename, CStringA &sa);
	static CString File2Unc(LPCTSTR sfilename);
	static HICON GetFileIcon(LPCTSTR sfilename);
	static HICON GetFileIcon(CFileDialog &fdlg);
	static CString GenGuidString();
	static BOOL GetFileSize(LPCTSTR  sfile, DWORD &flen, DWORD * pflenh = NULL);
	static CString CUtil::FormatMessageFor(HRESULT hr);
	static HGLOBAL m_hG;
	static LPCTSTR m_szMimeType; //contend type
};

BOOL DelTree(LPCTSTR lpszPath);
void UnzipFile(CString &sin, CStringArray * sFiles = NULL); //sdir is output parameter, indicate what folder exact to
DWORD GetFileSize(LPCTSTR sfile);
int RunCmd(LPCTSTR scmd, CString &sresult, LPCTSTR sdir =NULL, BOOL bshow =FALSE);
#define  AString   CStringA

typedef  CArray<CStringA, CStringA> AStringArray;

CStringA qUnc2Utf(LPCWSTR wstr);

CString qUtf2Unc(LPCSTR astr);

//Utf-8 to Wide char(unicode), not use too much stack 
void Unc2Utf(LPCTSTR   wstr, char  * bstr, int lenw, int lens);
//Wide char(unicode) to Utf-8 , not use too much stack
void Utf2Unc(const char  * bstr, WCHAR  * wstr, int lens, int lenw);
//Wide char(unicode) to Utf-8 , not use too much stack
void QUtf2Unc(LPCSTR  astr, CString &wstr);   //Wide char to Utf-8 , not use too much stack
//Utf-8 to Wide char(unicode), not use too much stack 
void QUnc2Utf(LPCWSTR wstr, AString &astr);  //Utf-8 to Wide char, not use too much stack 

int atox(const char *str);

void QA2W(LPCSTR  astr, CString &wstr);
WCHAR* QA2W(LPCSTR astr);
void QW2A(LPCTSTR wstr, CStringA &astr);

typedef enum
{
    RUN_NORMAL,
    RUN_SHOW_TIP,
    RUN_MINISIZE,
    RUN_EXIT_PROCESS,
    RUN_SHOW_TIP_YES
}; //Program start commands
extern int BaseAppInit();
extern void BaseAppExit();
BOOL IsOsVistaExt();
BOOL PickupFolder(CString &strDir, CWnd * pown, LPCTSTR szTitle = NULL, LPCTSTR szDispDir= NULL);

class CSetting
{
public:
	CString strCurPath;
	CString m_strAppUserPath;
	CString m_strUserName;
	CString m_strUserId;
	CString m_strPassword;
	CString m_strServerIP;
	INT     m_nPort;
	INT     m_bEnProxy;
	INT     m_nLimitPrevSize;
	INT     m_nFlvLimit;
	CString m_strProxyIP;
	INT     m_nProxyPort;
	CString m_strProxyUser;
	CString m_strProxyPwd;
	CString m_strHomePath;
	CString m_strSrcDir;
	CString m_strDstDir;
	CString m_strPdf2SwfParm;

public :
	int operator=(int id){ return 0;};
	CSetting();
	~CSetting();
	void Load();
	void Save();
	
};

CString GetReadableSize(UINT32 size);
#define FreePtr(ptr)  {delete ptr; ptr=NULL;}

extern CSetting * g_pSet;

void HttpPost(LPCTSTR szServer, int nPort, LPCTSTR url, void * postdata, int dlen, CString &strResponse);
BOOL HttpPostEx(LPCTSTR szServer, int nPort, LPCTSTR url, LPCTSTR szCookies[], int nheaders, int ncookies,\
	void * postdata, int dlen, CString &strResponse);
void ReplaceXmlItem(LPCTSTR szKey, CString &strval, CString &szXml);
CString ConvtXmlChars(CString &strval);
void ReplaceXmlItem(LPCSTR szKey, CStringA &strval, CStringA &szXml);
CStringA ConvtXmlChars(CStringA &str);
CStringA ConvtXmlChars(LPCSTR str);