#pragma once
class CUtil
{
public:
	CUtil(void);
	~CUtil(void);
	static void GetCurPath(CString &strPath);
	static HANDLE FindProcessByName(LPCTSTR szFileName, BOOL bKill = FALSE, INT exit_code = 0);
	static void CUtil::RunProc(LPCTSTR strcmd, LPCTSTR strparam, LPCTSTR strPath);
	static CSize GetBitmapSize(CBitmap &bmp);
	static void DrawGradientFill(CDC * pdc, CRect &r, COLORREF clt, COLORREF cbt, int mode=GRADIENT_FILL_RECT_H);
};

#define  AString   CStringA
typedef  CArray<CStringA, CStringA> AStringArray;
void U2W(LPCTSTR   wstr, char  * bstr,  int lenw,int lens);
void W2U(const char  * bstr, WCHAR  * wstr,  int lens,int lenw);
void QW2U(LPCSTR  astr, CString &wstr);   //Wide char to Utf-8 , not use too much stack
void QU2W(LPCTSTR wstr, AString &astr );  //Utf-8 to Wide char, not use too much stack 

void QA2W(LPCSTR  astr, CString &wstr);
WCHAR* QA2W(LPCSTR astr);
typedef enum
{
    RUN_NORMAL,
    RUN_SHOW_TIP,
    RUN_MINISIZE,
    RUN_EXIT_PROCESS,
    RUN_SHOW_TIP_YES
};
extern int BaseAppInit();
extern void BaseAppExit();
class CSetting
{
public:
	CString strCurPath;
public :
	CSetting();
	~CSetting();
	
};

#define FreePtr(ptr)  {delete ptr; ptr=NULL;}

extern CSetting * g_pSet;

