// BMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirPackge.h"
#include "BMDlg.h"
#include "afxdialogex.h"
#include "PdfReaderDlg.h"
#include "EpubReaderDlg.h"


#define GetMetaWnd() ((CMetaDlg*)(m_pViews[VIEW_META_DATA]))
// CBMDlg dialog

IMPLEMENT_DYNAMIC(CBMDlg, CExDialog)

CBMDlg::CBMDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CBMDlg::IDD, pParent)
{
	memset(m_pViews, 0, sizeof(m_pViews));
}

CBMDlg::~CBMDlg()
{
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		if (m_pViews[i]) FreePtr(m_pViews[i]);		
	} 
}

void CBMDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAME_META_DETAIL, m_FrameMeta);
	DDX_Control(pDX, IDC_TREE_META, m_trMeta);
	DDX_Control(pDX, IDC_TREE_DIR, m_trDir);
	DDX_Control(pDX, IDC_LIST_RES, m_listRes);
}


BEGIN_MESSAGE_MAP(CBMDlg, CExDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_VIEW_PROJ, &CBMDlg::OnViewProjMsg)
	ON_MESSAGE(WM_PDF_PAGE, &CBMDlg::OnPdfPageEvent)
	ON_BN_CLICKED(IDC_BTN_ADD_DIR, &CBMDlg::OnBnClickedBtnAddDir)
	ON_BN_CLICKED(IDC_BTN_REMOVE_DIR, &CBMDlg::OnBnClickedBtnRemoveDir)
	ON_NOTIFY(NM_CLICK, IDC_TREE_META, &CBMDlg::OnNMClickTreeMeta)
	ON_NOTIFY(NM_CLICK, IDC_TREE_DIR, &CBMDlg::OnNMClickTreeDir)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DIR, &CBMDlg::OnNMDblclkTreeDir)
	ON_BN_CLICKED(IDC_BTN_ADD_RES, &CBMDlg::OnBnClickedBtnAddRes)
	ON_BN_CLICKED(IDC_BTN_REMOVE_RES, &CBMDlg::OnBnClickedBtnRemoveRes)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIR, &CBMDlg::OnTvnSelchangedTreeDir)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_DIR, &CBMDlg::OnTvnEndlabeleditTreeDir)
END_MESSAGE_MAP()

// CBMDlg message handlers
view_type CBMDlg::ChangeView(view_type vtype)  //view_type
{
	view_type oldview = m_cur_view;

	m_pViews[vtype]->ShowWindow(SW_SHOW);

	
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		if (i != vtype)
			m_pViews[i]->ShowWindow(SW_HIDE);
	}
	m_cur_view = vtype;
	m_proj->m_nCurPage = -1;
	m_proj->m_nBookPageCount = -1;
	UpdateInfoText();
	return oldview;
}

BOOL CBMDlg::OnInitDialog()
{
	CExDialog::OnInitDialog();
	m_proj = ::GetPackProj();

	CREATE_SUB_WND(m_pViews[VIEW_META_DATA], CMetaDlg, &m_FrameMeta);
	CREATE_SUB_WND(m_pViews[VIEW_EPUB], CEpubReaderDlg, &m_FrameMeta);
	CREATE_SUB_WND(m_pViews[VIEW_PDF], CPdfReaderDlg, &m_FrameMeta);
	CREATE_SUB_WND(m_pViews[VIEW_EMPTY], CReaderView, &m_FrameMeta);
	m_pViews[VIEW_EPUB]->SetWndStyle(0, RGB(188, 186, 186), RGB(210, 212, 214));
	m_pViews[VIEW_PDF]->SetWndStyle(0, RGB(255, 255, 255), RGB(230, 232, 234));
	m_pViews[VIEW_EMPTY]->SetWndStyle(0, RGB(255, 255, 255), RGB(230, 232, 234));
	::SetForegroundWindow(m_pViews[VIEW_EPUB]->GetSafeHwnd());
	CRect r;
	m_FrameMeta.GetWindowRect(r);
	m_FrameMeta.ScreenToClient(r);
	r.bottom += 100;
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		m_pViews[i]->MoveWindow(r, FALSE);
	}
	int txtids[] = { IDC_ST_INFO };
	SubTextItems(txtids, sizeof(txtids) / sizeof(int), NULL, NULL);
	
	//ChangeView(VIEW_META_DATA);
	UINT btnids[][2] = {
		{ IDC_BTN_ADD_DIR, IDB_PNG_ADD24 },
		{ IDC_BTN_REMOVE_DIR, IDB_PNG_REMOVE24 },
		{ IDC_BTN_ADD_META, IDB_PNG_ADD24 },
		{ IDC_BTN_REMOVE_META, IDB_PNG_REMOVE24 },
		{ IDC_BTN_ADD_RES, IDB_PNG_ADD24 },
		{ IDC_BTN_REMOVE_RES, IDB_PNG_REMOVE24 } };
	
	for (int i = 0; i<MAX_COLOR_BTNS; i++)
	{
		m_cBtns[i] = new CGdipButton;
		m_cBtns[i]->SubclassDlgItem(btnids[i][0], this);
		m_cBtns[i]->LoadStdImage(btnids[i][1], _T("PNG"));
		//m_cTabBtns[i]->EnableButton(FALSE);
	}
	m_trMeta.ModifyStyle(NULL, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT );
	HTREEITEM hRootMeta = m_trMeta.InsertItem(_T("元数据"), NULL);
	// TODO:  Add extra initialization here
	CMetaDataItem * pit = GetMetaWnd()->FindNextMetaItem(NULL);
	while (pit)
	{
		m_trMeta.InsertItem(pit->strKey, hRootMeta);
		pit = GetMetaWnd()->FindNextMetaItem(pit);
	}
	m_trMeta.Expand(hRootMeta, TVE_EXPAND);

	m_trDir.ModifyStyle(NULL, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS);
	HTREEITEM hRootDir = m_trDir.InsertItem(_T("根目录"), NULL);

	
	LPCTSTR  szColumns[] = { { _T("资源文件包") }, { _T("文件大小") }, { _T("修改时间") }, { _T("文件格式") }, { _T("id") } };
	int colum_w[] = { 300, 180, 200, 200 ,0};
	m_nResIdCol = sizeof(colum_w) / sizeof(int)-1;
	for (int i = 0; i<sizeof(szColumns) / sizeof(LPCTSTR); i++)
	{
		m_listRes.InsertColumn(i, szColumns[i], LVCFMT_CENTER);
		m_listRes.SetColumnWidth(i, colum_w[i]);
	}

	int nSmallCx = ::GetSystemMetrics(SM_CXSMICON);
	int nSmallCy = ::GetSystemMetrics(SM_CYSMICON);
	if (m_proj->m_imglist.m_hImageList == NULL) //Create,renew this 
	{
		m_proj->m_imglist.Create(nSmallCx, nSmallCy, ILC_MASK | ILC_COLOR32, 0, 1);
		
		HICON hIcon = CUtil::GetFileIcon(_T(".zip"));
		m_proj->m_imglist.Add(hIcon);
	}

	m_proj->m_pProjDir = &m_trDir;
	m_listRes.SetImageList(&m_proj->m_imglist, LVSIL_SMALL);
	m_listRes.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_JUSTIFYCOLUMNS | LVS_EX_SINGLEROW | LVS_EX_SNAPTOGRID );
	m_proj->m_type = VIEW_EMPTY;
	ChangeView(m_proj->m_type);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBMDlg::MoveCtrlRect(int id, int right, int top, CRect &r)
{
	CWnd * pwnd = GetDlgItem(id);
	pwnd->GetWindowRect(r); ScreenToClient(r);
	r.MoveToXY(right - r.Width() - 2, top);
	pwnd->MoveWindow(r);
}

void CBMDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);
	if( ::IsWindow(m_trDir.GetSafeHwnd() ) )
	{
		CRect r;

		m_trDir.GetWindowRect(r);  ScreenToClient(r);
		r.bottom = cy -1;
		m_trDir.MoveWindow(r);

		m_listRes.GetWindowRect(r); ScreenToClient(r);
		r.right = cx -1;
		r.MoveToY(cy - r.Height() -1);
		m_listRes.MoveWindow(r);

		int restop = r.top;
		INT resleft = r.left;

		CWnd * pwnd = GetDlgItem(IDC_BTN_SCAN_RES);
		pwnd->GetWindowRect(r); ScreenToClient(r);
		r.MoveToXY(cx - r.Width() - 1, restop-r.Height()-1);
		pwnd->MoveWindow(r);

		int ftop = r.top;
		//MoveCtrlRect(IDC_BTN_SCAN_RES, cx, ftop, r);
		MoveCtrlRect(IDC_BTN_REMOVE_RES, r.left, ftop, r);
		MoveCtrlRect(IDC_BTN_ADD_RES, r.left, ftop, r);

		r.right = r.left-4;
		r.left = resleft;
		GetDlgItem(IDC_ST_INFO)->MoveWindow(r);
		r.SetRect(resleft, 1, cx - 1, r.top - 1);
		m_FrameMeta.MoveWindow(r);
		r.OffsetRect(-r.left, -r.top );	
		for (int i = 0; i < (int)VIEW_MAX; i++)
		{
			m_pViews[i]->MoveWindow(r, TRUE);
		}
	}
}


void CBMDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	
	for (int i = 0; i < (int)VIEW_MAX; i++)
	{
		if (m_pViews[i] && ::IsWindow(m_pViews[i]->GetSafeHwnd()))
		{
			m_pViews[i]->DestroyWindow();
			FreePtr(m_pViews[i]);
		}
	}
	for (int i = 0; i < MAX_COLOR_BTNS; i++)
	{
		if (m_cBtns[i] )
			FreePtr(m_cBtns[i]);
	}
	// TODO: Add your message handler code here
}

CReaderView * CBMDlg::GetProjView()
{
	if (VIEW_UNKNOWN == m_proj->m_type)
	{
		return m_pViews[VIEW_EMPTY];
	}
	for (int i = VIEW_EPUB; i < (int)VIEW_MAX; i++)
	{
		if (m_pViews[i]->m_vType == m_proj->m_type)
			return m_pViews[i];
	}
	return NULL; //m_pViews[0] 
}

LRESULT CBMDlg::OnViewProjMsg(WPARAM wParam, LPARAM lParam)
{
	CString * pxml = (CString *)lParam;
	switch (wParam)
	{
	case OPEN_PROJ:
		{
			CString str = m_proj->m_szTargetPath;
			int np = str.ReverseFind(_T('\\'));
			if (np >= 0) str.Delete(0, np + 1);
			GetProjView()->ViewFile(m_proj->m_szTargetPath);
			ChangeView(m_proj->m_type);
			GetMetaWnd()->SetMetaValues();
			GetMetaWnd()->LoadCoverImage();
			
		}
		break;
	case NEW_PROJ:
		{ 
			CString str = m_proj->m_szTargetPath;
			int np = str.ReverseFind(_T('\\'));
			if (np >= 0) str.Delete(0, np + 1);
			GetProjView()->ViewFile(m_proj->m_szTargetPath);
			ChangeView(m_proj->m_type);

			np = str.ReverseFind(_T('.'));
			if (np >= 0) str.Delete(np, str.GetLength() - np);
			GetMetaWnd()->SetItemValue(_T("图书名称"), str);

			GetMetaWnd()->SetItemValue(_T("电子书籍类别"), m_proj->m_type == VIEW_PDF ?  _T("Pdf") : _T("Epub"));
			str.Format(_T("%d"), m_proj->m_nTargetFLen);
			GetMetaWnd()->SetItemValue(_T("电子书籍文件大小"), str);
			GetMetaWnd()->SetDefComboxVal();
			//GetMetaWnd()->SetItemValue(_T("学科门类"), _T("工学"));
			//GetMetaWnd()->SetItemValue(_T("专业类"), _T("计算机科学与技术"));
			//GetMetaWnd()->SetItemValue(_T("专业名称"), _T("计算机软件与理论"));
		//	GetMetaWnd()->SetItemValue(_T("教材分类"), _T("公共基础课"));
			str.Format(_T("%d"), m_proj->m_nBookPageCount);
			GetMetaWnd()->SetItemValue(_T("页数"), str);
			GetProjView()->GetBookInfo(_T("Keywords"), str);
			GetMetaWnd()->SetItemValue(_T("关键字 "), str);
			GetProjView()->GetBookInfo(_T("Author"), str);
			if (str.IsEmpty())
				GetProjView()->GetBookInfo(_T("Creator"), str);
			GetMetaWnd()->SetItemValue(_T("作 者 "), str);

			MyTracex("new_proj message\r\n");
		}
		//GetMetaWnd()->SetItemValue(_T("页数"), _T("Pdf"));
		break;

	case CLOSE_PROJ:
		m_trDir.DeleteAllItems();
		m_trDir.InsertItem(_T("根目录"), NULL);
		ChangeView(m_proj->m_type);
		break;
	
	case SAVE_PROJ:
	{
	}
	default:
		break;
	}
	return 0;
}

LRESULT CBMDlg::OnPdfPageEvent(WPARAM wParam, LPARAM lParam)
{
	m_proj->m_nCurPage = wParam;
	m_proj->m_nBookPageCount = lParam;
	
	ResOnPageChange();
	return 0;
}

void CBMDlg::InsertRes(CResMan* pRes)
{
	int row = m_listRes.GetItemCount();
	m_listRes.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_IMAGE, \
		row, pRes->m_sfileName, LVIS_SELECTED | LVIF_IMAGE, \
		LVIS_SELECTED | LVIF_IMAGE, pRes->m_icon_id, 0);
	CString strsize = GetReadableSize((UINT32)pRes->m_fsize);
	m_listRes.SetItemText(row, 1, strsize);
	CString stime = pRes->m_tmCreate.Format(TIME_FMT);
	m_listRes.SetItemText(row, 2, stime);
	m_listRes.SetItemText(row, 3, pRes->m_sformat);
	m_listRes.SetItemText(row, 4, pRes->m_strResId);
}


void CBMDlg::UpdateInfoText()
{
	const LPCTSTR sViewText[] = { { TEXT("元数据编辑") }, { TEXT("Epub 当前页：%d， 总页数:%d") }, \
			{ TEXT("PDF  当前页：%d ，总页数: %d") }, { TEXT("未打开书籍") } };
	m_strInfo.Format(sViewText[m_cur_view], m_proj->m_nCurPage, m_proj->m_nBookPageCount);
	CWnd * pwnd = GetDlgItem(IDC_ST_INFO);
	pwnd->SetWindowText(m_strInfo);
	pwnd->ShowWindow(SW_HIDE);
	CRect r;
	pwnd->GetWindowRect(r);
	this->ScreenToClient(r);
	//GetParent()->Invalidate();
	InvalidateRect(r);
	pwnd->ShowWindow(SW_SHOW);
	//GetParent()->Invalidate();
	InvalidateRect(r);

}
void CBMDlg::ResOnPageChange()
{
	int npg = m_proj->m_nCurPage;
	m_listRes.DeleteAllItems();

	CResMan * pRes = m_proj->m_pRes;
	while (pRes)
	{
		if (pRes->m_relation == npg)
		{
			InsertRes(pRes);
		}
		pRes = pRes->pNext;
	}
	UpdateInfoText();
}

void CBMDlg::OnBnClickedBtnAddDir()
{
	// TODO:  在此添加控件通知处理程序代码
	INT page = GetProjView()->GetPageInfo(NULL, NULL);
	if (m_proj->m_type == VIEW_EMPTY)
	{
		MessageBox(_T("请打开书对应页后添加该页目录！"), _T("还没有打开书！"));
		return;
	}
	
	HTREEITEM hroot = m_trDir.GetSelectedItem();
	if (hroot == NULL) hroot = m_trDir.GetRootItem();

	CString str;
	HTREEITEM hpt = m_trDir.GetChildItem(hroot);
	while (hpt)
	{
		HTREEITEM hnext = m_trDir.GetNextItem(hpt, TVGN_NEXT);
		if (hnext == NULL)break;
		hpt = hnext;
	}
	if (hpt == NULL)
	{
		str = m_trDir.GetItemText(hroot);
		if (str.Compare(_T("根目录")) == 0)
		{
			str = _T("目录");
		}
		str += _T("_1");
	}
	else
	{
		str = m_trDir.GetItemText(hpt);
		int rx = str.ReverseFind(_T('_'));
		if (rx == -1)
			rx = str.GetLength();
		CString sv = str.Right(str.GetLength() - rx - 1);
		int idx = _wtoi(sv) + 1;
		str = str.Left(rx + 1);
		if (str.IsEmpty())
			str = _T("目录x");
		CString strx;
		strx.Format(_T("%s%d"), str, idx);
		str = strx;
	}

	hpt = m_trDir.InsertItem( str, hroot);
	m_trDir.SetItemData(hpt, page);
	m_trDir.Expand(hroot, TVE_EXPAND);

}


void CBMDlg::OnBnClickedBtnRemoveDir()
{
	if (m_proj->m_type == VIEW_EMPTY)
	{
		MessageBox(_T("请打开书后才能删除目录！"), _T("还没有打开书！"));
		return;
	}

	HTREEITEM hroot = m_trDir.GetSelectedItem();
	if (hroot != NULL)
	{
		if (hroot != m_trDir.GetRootItem())
			m_trDir.DeleteItem(hroot);
	}
}


void CBMDlg::OnNMClickTreeMeta(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_proj->m_type != VIEW_EMPTY)
	{
		if (GetMetaWnd()->IsWindowVisible() == FALSE)
			ChangeView(VIEW_META_DATA);
	}
	*pResult = 0;
}


void CBMDlg::OnNMClickTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	if (GetProjView()->IsWindowVisible() == FALSE)
		ChangeView(m_proj->m_type);

	*pResult = 0;
}

void CBMDlg::OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}

void CBMDlg::OnNMDblclkTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	HTREEITEM hpt = m_trDir.GetSelectedItem();
	if (hpt != NULL)
	{
		UINT pg = m_trDir.GetItemData(hpt);
		GetProjView()->GotoPage(pg);
	}

	*pResult = 0;
}


void CBMDlg::OnBnClickedBtnAddRes()
{
	if (m_proj->m_type == VIEW_EMPTY)
	{
		MessageBox(_T("请打开书对应页后添加该页资源！"), _T("还没有打开书！"));
		return;
	}

	CFileDialog fdlg(TRUE, 0, _T("*.*"), OFN_HIDEREADONLY,
		_T("所有文件|*.*||"), ::AfxGetMainWnd());
	if (fdlg.DoModal() != IDOK) return ;

	CResMan * pRes = CResMan::NewRes(fdlg);
	if (pRes)
	{		
		InsertRes(pRes);
	}
}


void CBMDlg::OnBnClickedBtnRemoveRes()
{
	// TODO:  在此添加控件通知处理程序代码
	INT row = -1;
	if (m_proj->m_type == VIEW_EMPTY)
	{
		MessageBox(_T("请打开书后才可删除资源！"), _T("还没有打开书！"));
		return;
	}
	while ( (row =m_listRes.GetNextItem(row, LVNI_ALL | LVNI_SELECTED)) != -1)
	{
		CString strid = m_listRes.GetItemText(row, m_nResIdCol);
		if (CResMan::Remove(strid))
		{
			m_listRes.DeleteItem(row);
			row--;
		}		
	}

	//POSITION  pos = m_listRes.GetFirstSelectedItemPosition();
	//if (pos != NULL)
	//{
	//	int row = m_listRes.GetNextSelectedItem(pos);
	//	CString strid = m_listRes.GetItemText(row, -1);
	//	if (CResMan::Remove(strid))
	//	{
	//		m_listRes.DeleteItem(row);
	//	}
	//}

}

BOOL CBMDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN &&
		pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
	{
		//if (pMsg->hwnd != this->GetSafeHwnd())
		CEdit* edit = m_trDir.GetEditControl();
		if (edit)
		{
			edit->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
	}
	return CExDialog::PreTranslateMessage(pMsg);
}

void CBMDlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类
	CExDialog::OnOK();
}


void CBMDlg::OnTvnEndlabeleditTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 1;
}
