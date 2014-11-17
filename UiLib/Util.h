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




