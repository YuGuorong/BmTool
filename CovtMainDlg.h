#pragma once

#include "ExDialog.h"
#include "AsyncHttp.h"
#include "XZip\XZip.h"
#include "tables.h"
// CCovtMainDlg 对话框
#define WM_BURN_DONE (WM_APP+179)


class CDelegate : public Thread
{
public:
	CDelegate(CWnd * pmsgWnd);
	void * prom;
	CWnd  * m_pMsgWnd;
public:
	virtual void * run(void * pobj );
	INT(*onFinish)(void * parm, INT burn_status);//完成后调用的函数. Status < 0 means fail ; return < 0 ,keep connection
};

class CCovtMainDlg;
typedef int (CCovtMainDlg::*pfnDelegate)(int);
#define CFG_ORIG_PREF   _T("__old_meta.xml")
#define CFG_PREVW_FLV   _T("__preview.flv")
#include <map>
#include "afxwin.h"
#include "afxcmn.h"


using namespace std;

enum
{
	CVT_ERR_BASE = -500,
	CVT_ERR_UNZIP,
	CVT_ERR_HUGE_ZIP,
	CVT_ERR_OPEN_PACKAGE,
	CVT_ERR_HUGE_FLV,
	CVT_ERR_HUGE_SWF,
	CVT_ERR_PDF_2_SWF,
	CVT_ERR_ADD_COVER,
	CVT_ERR_OVER_COVERS,
	CVT_ERR_NO_XML,
	CVT_ERR_PARSE_XML,
	CVT_ERR_OPEN_XML,
	CVT_ERR_OPEN_TEMPLATE,
	CVT_ERR_CREATE_META_FILE,
	CVT_ERR_SECTION_VOLUME,
	CVT_ERR_SECTION_METACLASS,
	CVT_ERR_SECTION_GRADE,
	CVT_ERR_SECTION_AUTHOR,
	CVT_ERR_ENC_FAIL_0,
	CVT_ERR_ENC_FAIL_1,
	CVT_ERR_ENC_FAIL_2,
	CVT_ERR_ENC_FAIL_3,
	CVT_ERR,
	CVT_OK = 0
};

#define CVT_ERR_STRINGS  			\
		{ _T("Error unzip package") },  \
		{ _T("Error open zip package") },  \
		{ _T("Error huge zip package") },  \
		{ _T("Error huge flv file(>10MB) ") },  \
		{ _T("Error huge swf file(>10MB) ") },  \
		{ _T("Error convert pdf to swf  ") },  \
		{ _T("Error add a cover  ") },  \
		{ _T("Error class type over cover number") },  \
		{ _T("Error no xml ") },				\
		{ _T("Error parse xml") },				\
		{ _T("Error open xml") },				\
		{ _T("Error open_template") },			\
		{ _T("Error create _meta_file") },		\
		{ _T("Error Section volume") },			\
		{ _T("Error Section metadataclass") },	\
		{ _T("Error Section grade") },			\
		{ _T("Error Section author") },			\
		{ _T("Error 加密文件大小为0") },		\
		{ _T("Error 目标文件加密失败") },		\
		{ _T("Error 加密目标文件已存在") },		\
		{ _T("Error 加密目标文件创建失败") },	\
		{ _T("Error unkown") }			 

int AddTaskToDb(CString &szip);
#define MAX_CLASS_NUMBER  8

class CCovtMainDlg : public CExDialog
{
	DECLARE_DYNAMIC(CCovtMainDlg)

public:
	CCovtMainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCovtMainDlg();
	virtual void OnOK();

	void * m_ptrUserData;  //parser
	CStringA m_sXmlStrings;
	CStringA m_sXmlCaption;
	CStringA m_sXmlData;  //for combine element data
	HTREEITEM m_hDirCur;
	CTreeCtrl * m_pProjDir;
	CLCheckBox m_EnEncrypt;
	BOOL   m_bEnEncrypt;
	CString m_strPrevw;

	CGdipButton	*m_cFolderBtns[2];
	
	CStringA m_szUuid;
	CFont m_ftEdit;

	multimap<CStringA, CStringA> m_mapMetaVal;  //all xml value here
	multimap<CStringA, CStringA> m_mapMultiVal;  //all xml value here
	map <CStringA, CStringA> m_mapKeyCaps;
	CStringArray m_Files;
	HZIP m_hz;

	// Dialog Data
	enum { IDD = IDD_CONVT_MAIN_DLG };
	CSkinBtn * m_pbtns[2];
	pfnDelegate m_pfnAsynRun;
	pfnDelegate m_pfnDone;
	CDelegate * m_pDeleGate;
	int AsyncConvertDir(int v);
	int OnConvertDone(int v);
	int AddCover(CStringA &sxml, CStringA &sfile, CStringArray &sPackageFiles);
	int Addfile(CStringA &sxml, CStringArray &sPackageFiles);
	int AddMetaInfo(CStringA &sxml);

	CString m_slog;
	void SetCurProgPos(int cp, LPCTSTR  sinf);
	void SetProgWndLimit(int max, int min=0);
	void SetProgInfo(LPCSTR strInfo);
	void AddLog(LPCTSTR slog);
	void Logs(LPCTSTR fmt, ...);
	void LogToFile(int result, LPCTSTR spckFile);

	void CCovtMainDlg::SaveDirToXml(HTREEITEM hit, CStringA &sxml, int sublevel);
	void CCovtMainDlg::SaveDirs(CStringA &sxml);
	CString m_strCvtError;
	
	CPtrArray m_fCovers;
	CStringArray m_strSrcPacks;
	int ConvertBook(CString &sbook);
	int ParseXmlField(CString &sbook);
	int ParseXmlMeta(CStringArray &sPackageFiles);
	BOOL m_bCancel;
	void ProcMsg();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void EnableProxy(BOOL benProxy);
	CLCheckBox m_ProxyBox;
	int CheckLastTaskBroken();
	int m_nClassType;
	void CleanXmlUnknown(CStringA &strxml);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnOk2();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	int m_nMin;
	int m_nMax;
	CString m_strSrcDir;
	CString m_strDstDir;
	CString m_strTmpDir;
	int m_nLimitPrevSize;
	int m_nFlvLimit;
	afx_msg void OnBnClickedBtnSrcDir();
	afx_msg void OnBnClickedBtnDstDir();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_oLog;
	CProgressCtrl m_oProg;
	CLink m_oInf;
	afx_msg void OnBnClickedBtnCvtErrs();
	afx_msg void OnEnChangeEditSrcDir();
};
