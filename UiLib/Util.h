#pragma once
class CUtil
{
public:
	CUtil(void);
	~CUtil(void);
	static void GetCurPath(CString &strPath);
	static CString GetFilePath(CString &strFile);
	static CString GetFileName(CString &strFile);
	static HANDLE FindProcessByName(LPCTSTR szFileName, BOOL bKill = FALSE, INT exit_code = 0);
	static HANDLE CUtil::RunProc(LPCTSTR strcmd, LPCTSTR strparam, LPCTSTR strPath);
	static CSize GetBitmapSize(CBitmap &bmp);
	static void DrawGradientFill(CDC * pdc, CRect &r, COLORREF clt, COLORREF cbt, int mode=GRADIENT_FILL_RECT_H);
	static CStringA File2Asc(LPCTSTR sfilename);
	static CString File2Unc(LPCTSTR sfilename);
	static HICON GetFileIcon(LPCTSTR sfilename);
	static HICON GetFileIcon(CFileDialog &fdlg);
	static CString GenGuidString();
	static BOOL GetFileSize(LPCTSTR  sfile, DWORD &flen);
};

#define  AString   CStringA
typedef  CArray<CStringA, CStringA> AStringArray;
//Utf-8 to Wide char(unicode), not use too much stack 
void Unc2Utf(LPCTSTR   wstr, char  * bstr, int lenw, int lens);
//Wide char(unicode) to Utf-8 , not use too much stack
void Utf2Unc(const char  * bstr, WCHAR  * wstr, int lens, int lenw);
//Wide char(unicode) to Utf-8 , not use too much stack
void QUtf2Unc(LPCSTR  astr, CString &wstr);   //Wide char to Utf-8 , not use too much stack
//Utf-8 to Wide char(unicode), not use too much stack 
void QUnc2Utf(LPCWSTR wstr, AString &astr);  //Utf-8 to Wide char, not use too much stack 

void QA2W(LPCSTR  astr, CString &wstr);
WCHAR* QA2W(LPCSTR astr);
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

class CSetting
{
public:
	CString strCurPath;
	CString m_strUserName;
	CString m_strPassword;
	CString m_strServerIP;
	INT     m_nPort;
	INT     m_bEnProxy;
	CString m_strProxyIP;
	INT     m_nProxyPort;
	CString m_strProxyUser;
	CString m_strProxyPwd;

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
CString ConvertXmlString(CString &strval);
