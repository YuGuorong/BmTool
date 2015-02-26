// ResManDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ResManDlg.h"
#include "afxdialogex.h"
#include "AsyncHttp.h"

// CResManDlg 对话框
static const INT btnids[] = { IDC_BTN_LOCAL, IDC_BTN_RESUPLOAD, IDC_BTN_ADD,
IDC_BTN_REMOVE, IDC_BTN_SELECT, IDC_BTN_UPLOAD, IDC_BTN_RETURN };

IMPLEMENT_DYNAMIC(CResManDlg, CExDialog)
static LPCTSTR  szColumns[] = { { _T("图书名") }, { _T("图书大小") }, { _T("创建时间") },
{ _T("上传状态") }, { _T("上传进度") }, { _T("位置") }, { _T("ID") } };
static int colum_w[] = { 300, 80, 120, 80, 60, 400, 0 };
enum
{
	IDX_COL_BOOK_NAME,
	IDX_COL_BOOK_SIZE,
	IDX_COL_TM_CREATE,
	IDX_COL_ST_UPLOAD,
	IDX_COL_NU_UPLOAD,
	IDX_COL_BOOK_PATH,
	IDX_COL_BOOK_ID,

	ID_TMR_SCAN_TASK,
};


CResManDlg::CResManDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CResManDlg::IDD, pParent)
{
	m_pbtns[0] = NULL;
	m_proj = NULL;
	m_CurResType = TYPE_LOCAL_RES;
}

CResManDlg::~CResManDlg()
{
}

void CResManDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RES, m_listRes);
	DDX_Control(pDX, IDC_LIST_BOOKS, m_listTask);
}


BEGIN_MESSAGE_MAP(CResManDlg, CExDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_LOCAL, &CResManDlg::OnBnClickedBtnLocal)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_UPLOAD, &CResManDlg::OnBnClickedBtnUpload)
	ON_BN_CLICKED(IDC_BTN_RESUPLOAD, &CResManDlg::OnBnClickedBtnResupload)
	ON_BN_CLICKED(IDC_BTN_SELECT, &CResManDlg::OnBnClickedBtnSelect)
	ON_BN_CLICKED(IDC_BTN_ADD, &CResManDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_REMOVE, &CResManDlg::OnBnClickedBtnRemove)
	ON_BN_CLICKED(IDC_BTN_RETURN, &CResManDlg::OnBnClickedBtnReturn)
	ON_MESSAGE(WM_HTTP_DONE, &CResManDlg::OnHttpFinishMsg)		//自定义HTTP事件
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CResManDlg 消息处理程序


void CResManDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (m_pbtns[0])
	{
		CRect rs, rbt;
		GetWindowRect(rs);
		ScreenToClient(rs);
		rs.InflateRect(-2, -2);
		m_pbtns[0]->GetWindowRect(rbt);
		ScreenToClient(rbt);
		rs.top = rbt.bottom + 8;
		GetDlgItem(IDC_LIST_RES)->MoveWindow(rs);
	}
}


void CResManDlg::OnDestroy()
{
	CExDialog::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		delete m_pbtns[i];
	}
}

BOOL CResManDlg::OnInitDialog()
{
	CExDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_setQueueId.clear();
	m_mapBookid_listIdx.clear();

	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		m_pbtns[i] = new CSkinBtn();
		m_pbtns[i]->SubclassDlgItem(btnids[i], this);
		m_pbtns[i]->SetImage(IDB_BITMAP_BTN,106,28);
	}
	m_proj = ::GetPackProj();
	int nSmallCx = ::GetSystemMetrics(SM_CXSMICON);
	int nSmallCy = ::GetSystemMetrics(SM_CYSMICON);
	if (m_proj->m_imglist.m_hImageList == NULL)
		m_proj->m_imglist.Create(nSmallCx, nSmallCy, ILC_MASK | ILC_COLOR32, 0, 1);
	m_listRes.SetImageList(&m_proj->m_imglist, LVSIL_SMALL);
	m_listRes.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_JUSTIFYCOLUMNS | LVS_EX_SINGLEROW | LVS_EX_SNAPTOGRID);
	
	m_listRes.DeleteAllItems();
	while (m_listRes.DeleteColumn(0)){};
	m_nBookResIdCol = sizeof(colum_w) / sizeof(int) - 1;

	static const LPCTSTR  szBookColumns[] = { { _T("ID") }, { _T("上传状态") }, { _T("上传进度") } };
	static const int book_colum_w[] = { 300, 80, 120, 80, 400, 0 };
	for (int i = 0; i<sizeof(szBookColumns) / sizeof(LPCTSTR); i++)
	{
		m_listTask.InsertColumn(i, szBookColumns[i], LVCFMT_CENTER);
		m_listTask.SetColumnWidth(i, book_colum_w[i]);
	}

	LoadBooks();
	SetTimer(ID_TMR_SCAN_TASK, 5000, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

static int cb_Show_Record(void * param, int argc, char ** argv, char ** aszColName)
{
	CListCtrl * plist = (CListCtrl*)param;
	int row = plist->GetItemCount();
	CString str;
	QUtf2Unc(argv[2], str);
	plist->InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_IMAGE, row, str,
		LVIS_SELECTED | LVIF_IMAGE, LVIS_SELECTED | LVIF_IMAGE, 0, 0); //BookName; /* 2*/

	CString strsize = GetReadableSize((UINT32)(atoi(argv[4])) );
	plist->SetItemText(row, IDX_COL_BOOK_SIZE, strsize);  //BookSize; /*4*/
	QUtf2Unc(argv[5], str);	plist->SetItemText(row, IDX_COL_TM_CREATE, str);//tmCreate;   /* 5 */
	QUtf2Unc(argv[8], str); plist->SetItemText(row, IDX_COL_ST_UPLOAD, str);//BookState; /* 8*/
	QUtf2Unc(argv[3], str); plist->SetItemText(row, IDX_COL_BOOK_PATH, str);//BookPath; /*3*/
	QUtf2Unc(argv[1], str); plist->SetItemText(row, IDX_COL_BOOK_ID, str);//ID; /*1*/
	plist->SetItemText(row, IDX_COL_NU_UPLOAD, _T("..."));  //BookSize; /*4*/
	return 0;
}

void CResManDlg::LoadBooks()
{
	m_CurResType = TYPE_BOOKS_RES;
	m_listRes.DeleteAllItems();
	while (m_listRes.DeleteColumn(0)){};
	m_nBookResIdCol = sizeof(colum_w) / sizeof(int) - 1;
	for (int i = 0; i<sizeof(szColumns) / sizeof(LPCTSTR); i++)
	{
		m_listRes.InsertColumn(i, szColumns[i], LVCFMT_CENTER);
		m_listRes.SetColumnWidth(i, colum_w[i]);
	}
	char *pErrMsg;
	CString str = _T("SELECT * FROM books WHERE BookState<>\'删除\' ORDER BY BookState ");
	AString sql_show;
	QUnc2Utf(str, sql_show);
	execSQL(sql_show, cb_Show_Record, &m_listRes, &pErrMsg);
	GetDlgItem(IDC_BTN_UPLOAD)->EnableWindow(TRUE);
	CString stritem;
	m_mapBookid_listIdx.clear();
	for (int i = 0; i < m_listRes.GetItemCount(); i++)
	{
		stritem = m_listRes.GetItemText(i, IDX_COL_BOOK_ID);
		m_mapBookid_listIdx.insert(m_mapBookid_listIdx.end(), make_pair(stritem, i));
	}
}

void CResManDlg::LoadBookResList()
{
	m_CurResType = TYPE_LOCAL_RES;
	m_listRes.DeleteAllItems();

	while (m_listRes.DeleteColumn(0)){};
	LPCTSTR  szColumns[] = { { _T("资源文件包") }, { _T("对应书所在页面") }, { _T("文件大小") }, { _T("修改时间") }, { _T("文件格式") }, { _T("ID") } };
	int colum_w[] = { 300, 120, 180, 200, 200 , 0};
	m_nBookResIdCol = sizeof(colum_w) / sizeof(int) - 1;
	for (int i = 0; i<sizeof(szColumns) / sizeof(LPCTSTR); i++)
	{
		m_listRes.InsertColumn(i, szColumns[i], LVCFMT_CENTER);
		m_listRes.SetColumnWidth(i, colum_w[i]);
	}

	int row = 0;
	CResMan * pRes = m_proj->m_pRes;
	while (pRes)
	{
		int col = 1;
		int imgid = pRes->m_icon_id;
		m_listRes.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_IMAGE, row, pRes->m_sfileName,
			LVIS_SELECTED | LVIF_IMAGE, LVIS_SELECTED | LVIF_IMAGE, imgid, 0);
		CString spg; spg.Format(_T("%d"), pRes->m_relation);
		m_listRes.SetItemText(row, col++, spg);
		CString strsize = GetReadableSize((UINT32)pRes->m_fsize);
		m_listRes.SetItemText(row, col++, strsize);
		CString stime = pRes->m_tmCreate.Format(TIME_FMT);
		m_listRes.SetItemText(row, col++, stime);
		m_listRes.SetItemText(row, col++, pRes->m_sformat);
		m_listRes.SetItemText(row, col++, pRes->m_strResId);
		pRes = pRes->pNext;
		row++;
	}
	GetDlgItem(IDC_BTN_UPLOAD)->EnableWindow(FALSE);
}

void CResManDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CExDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		LoadBookResList();
	}
}

void * CResManDlg::UploadBook(LPCTSTR book)
{
	CPackerProj * proj = ::GetPackProj();
	//::CreateDirectory(str, NULL);
	TCHAR headerLanguage[] = _T("Accept-Language: zh-cn\r\n");
	TCHAR headerEncoding[] = _T("Accept-Encoding: gzip, deflate\r\n");
	TCHAR headerContentType[] = _T("Content-Type: application/zip\r\n");//text/xml
	CString strSession;
	strSession.Format(_T("Cookie: sessionId=%s;tooken=11111\r\n"), proj->m_strSession);
	LPCTSTR * cookies = new LPCTSTR[4];
	int hdrs = 0;
	cookies[hdrs++] = headerLanguage;
	cookies[hdrs++] = headerEncoding;
	cookies[hdrs++] = headerContentType;
	cookies[hdrs++] = strSession;

	
	CFile of;
	int flen = 0;
	CHttpPost * pTask = new CHttpPost(g_pSet->m_strServerIP, _T("/books/streamUpload.whtml"),\
		this, g_pSet->m_nPort, cookies, hdrs);
	
	pTask->SendFile(book);
	SetTimer(ID_TMR_SCAN_TASK, 500, NULL);
	return pTask;
}

BOOL CResManDlg::UploadItem(CString &sid, int row)
{
	if (row == -1)
	{
		std::map<CString, int>::iterator fit = m_mapBookid_listIdx.find(sid);
		if (fit != m_mapBookid_listIdx.end())
		{
			row = fit->second;
		}
		else
			return FALSE;
	}

	if (m_listTask.GetItemCount() < CFG_MAX_TASK_LIMIT)
	{
		CString str = m_listRes.GetItemText(row, IDX_COL_BOOK_PATH);
		m_proj->SetBookState(sid, _T("上传中"));
		m_listRes.SetItemText(row, IDX_COL_ST_UPLOAD, _T("上传中"));
		m_listRes.SetItemText(row, IDX_COL_NU_UPLOAD, _T("0%"));
		void * ptask = UploadBook(str);
		int row_task = m_listTask.GetItemCount();
		m_listTask.InsertItem(row_task, sid);
		m_listTask.SetItemText(row_task, 1, _T("上传中"));
		m_listTask.SetItemData(row_task, (DWORD_PTR)ptask);
	}
	else //full
	{
		m_listRes.SetItemText(row, IDX_COL_ST_UPLOAD, _T("等待上传"));
		m_listRes.SetItemText(row, IDX_COL_NU_UPLOAD, _T("0%"));
		set<CString>::iterator fit = m_setQueueId.find(sid);
		if (fit == m_setQueueId.end())
			m_setQueueId.insert(m_setQueueId.end(), sid);
	}
}

BOOL CResManDlg::CheckTask(LPCTSTR bookid, BOOL bremove)
{
	
	for (int row = 0; row < m_listTask.GetItemCount(); row++)
	{
		CString strtaskid = m_listTask.GetItemText(row, 0);
		if (strtaskid.Compare(bookid) == 0)
		{
			if (bremove)
			{
				CAsyncHttp* phttp = (CAsyncHttp*)m_listTask.GetItemData(row);
				phttp->Disconnect();
				m_listTask.DeleteItem(row);
				delete phttp;
			}
			return   TRUE;
		}
	}
	return FALSE;
}

void CResManDlg::OnBnClickedBtnUpload()
{
	if (m_CurResType == TYPE_LOCAL_RES)
	{
		MessageBox(_T("请选择上传资源"),_T("没选择上传资源"));
		LoadBooks();
	}
	else
	{
		for (int j = 0; j<m_listRes.GetItemCount(); j++)
		{
			if (m_listRes.GetCheck(j))
			{
				CString strid = m_listRes.GetItemText(j, IDX_COL_BOOK_ID);
				BOOL bAlreadyUpload = CheckTask(strid);
				if (bAlreadyUpload == FALSE) //not in task ,then put into task 
				{
					UploadItem(strid, j);
				}//bAlreadyUpload true: no process, nothing todo!
			}
		}
		this->UpdateWindow();
	}
}

void CResManDlg::OnBnClickedBtnLocal()
{
	LoadBookResList(); 
}

void CResManDlg::OnBnClickedBtnResupload()
{
	// TODO:  在此添加控件通知处理程序代码
	LoadBooks();
}


void CResManDlg::OnBnClickedBtnSelect()
{
	BOOL bToSelall = FALSE;
	for (int j = 0; j < m_listRes.GetItemCount(); j++)
	{
		if (m_listRes.GetCheck(j) == FALSE)
		{
			m_listRes.SetCheck(j);
			bToSelall = TRUE;
		}
	}

	if (bToSelall == FALSE)
	{
		for (int j = 0; j < m_listRes.GetItemCount(); j++)
		{
			m_listRes.SetCheck(j, FALSE);
		}
	}
}


void CResManDlg::OnBnClickedBtnAdd()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CResManDlg::OnBnClickedBtnRemove()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_CurResType == TYPE_LOCAL_RES)
	{
		for (int j = 0; j < m_listRes.GetItemCount(); )
		{
			if (m_listRes.GetCheck(j))
			{
				CString sid = m_listRes.GetItemText(j, m_nBookResIdCol);
				if (CResMan::Remove(sid))
				{
					m_listRes.DeleteItem(j);
				}
			}
			else
				j++;
		}
	}
	else
	{
		if (MessageBox(_T("确认要删除所选择的书籍吗？\r\n（正在上传的会自动中止上传并删除）"), _T("删除上传资料"), MB_YESNO)
			== IDYES)
		{
			int row = 0;
			while (row < m_listRes.GetItemCount())
			{
				if (m_listRes.GetCheck(row))
				{
					CString sid = m_listRes.GetItemText(row, IDX_COL_BOOK_ID);
					CString strwsql; strwsql.Format(_T("DELETE FROM books WHERE BookId == \"%s\" "), sid);
					AString sql_del;
					QUnc2Utf(strwsql, sql_del);
					execSQL(sql_del);
					if (CheckTask(sid, TRUE) == FALSE)
					{
						m_setQueueId.erase(sid);
						m_listRes.DeleteItem(row);
						continue;
					}
				}
				row++;
			}
		}
	}
}


void CResManDlg::OnBnClickedBtnReturn()
{
	// TODO:  在此添加控件通知处理程序代码
	::SwitchBackMainDlg();
}


LRESULT CResManDlg::OnHttpFinishMsg(WPARAM wParam, LPARAM lParam)
{
	CAsyncHttp * pHttp = (CAsyncHttp*)wParam;
	BOOL  stat = (BOOL)lParam;
	int taskidx = 0;
	if (stat > 0)
	{
		int len = pHttp->GetHttpHeader(pHttp->m_szRespHeader);
		if (pHttp->m_szRespHeader.Find(" 200 OK") >= 0)
		{
			stat = TRUE;
		}
		else
			stat = FALSE;
	}
	for (int i = 0; i < m_listTask.GetItemCount(); i++)
	{
		if (m_listTask.GetItemData(i) == (DWORD_PTR)pHttp)
		{
			LPCTSTR strRet = _T("上传成功");// (stat > 0 || stat <= -500) ? _T("上传成功") : _T("上传失败");
			CString strid = m_listTask.GetItemText(i, 0);

			m_proj->SetBookState(strid, strRet);
			std::map<CString, int>::iterator fit = m_mapBookid_listIdx.find(strid);
			if (fit != m_mapBookid_listIdx.end())
			{
				int row = fit->second;
				m_listRes.SetItemText(row, IDX_COL_ST_UPLOAD, strRet);
				m_listRes.SetItemText(row, IDX_COL_NU_UPLOAD, _T("100%"));
			}
			m_listTask.DeleteItem(i);//delete from task
			pHttp->stop();
			delete pHttp;
			break;
		}
	}

	EndWaitCursor();
	::AfxGetMainWnd()->EndWaitCursor();

	return 0;
}


BOOL CResManDlg::ScanTask()
{
	int i;
	if (m_CurResType == TYPE_LOCAL_RES) return FALSE;
	for (i = 0; i < m_listTask.GetItemCount(); i++) //update process indications
	{
		CString sid = m_listTask.GetItemText(i, 0);
		std::map<CString, int>::iterator fit = m_mapBookid_listIdx.find(sid);
		if (fit != m_mapBookid_listIdx.end())
		{
			int row = fit->second;
			CAsyncHttp * phttp = (CAsyncHttp *)m_listTask.GetItemData(i);
			if (phttp != NULL && phttp->m_pSocket != NULL && phttp->m_pSocket->m_SendTotLen>100)
			{
				int per = (phttp->m_pSocket->m_curTxBytes )/ (phttp->m_pSocket->m_SendTotLen/100);
				CString sprog;
				sprog.Format(_T("%d%%"), per);
				CString snow = m_listRes.GetItemText(row, IDX_COL_NU_UPLOAD);
				if (snow.Compare(sprog) != 0 )
					m_listRes.SetItemText(row, IDX_COL_NU_UPLOAD, sprog);
			}
		}
	}
	if (m_listTask.GetItemCount() < CFG_MAX_TASK_LIMIT) //update queue 
	{
		if (m_setQueueId.size() != 0)
		{
			CString sid = *m_setQueueId.begin();
			m_setQueueId.erase(m_setQueueId.begin());
			UploadItem(sid);
		}
	}
	return (i!= 0);
}

void CResManDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case ID_TMR_SCAN_TASK:
		KillTimer(ID_TMR_SCAN_TASK);
		if( ScanTask() )
			SetTimer(ID_TMR_SCAN_TASK, 500, NULL);
		else
			SetTimer(ID_TMR_SCAN_TASK, 3000, NULL);
		break;
	}

	CExDialog::OnTimer(nIDEvent);
}
