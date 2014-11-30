// ResManDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ResManDlg.h"
#include "afxdialogex.h"

// CResManDlg 对话框
static const INT btnids[] = { IDC_BTN_LOCAL, IDC_BTN_RESUPLOAD, IDC_BTN_ADD,
						IDC_BTN_REMOVE, IDC_BTN_SELECT, IDC_BTN_UPLOAD, IDC_BTN_SEARCH };

IMPLEMENT_DYNAMIC(CResManDlg, CExDialog)

CResManDlg::CResManDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CResManDlg::IDD, pParent)
{
	m_pbtns[0] = NULL;
	m_proj = NULL;
}

CResManDlg::~CResManDlg()
{
}

void CResManDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RES, m_listRes);
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
	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		m_pbtns[i] = new CSkinBtn();
		m_pbtns[i]->SubclassDlgItem(btnids[i], this);
		m_pbtns[i]->SetImage(IDB_BITMAP_BTN,106,28);
	}


	LPCTSTR  szColumns[] = { { _T("资源文件包") }, { _T("文件大小") }, { _T("修改时间") }, { _T("文件格式") } };
	int colum_w[] = { 300, 180, 200, 200 };
	for (int i = 0; i<sizeof(szColumns) / sizeof(LPCTSTR); i++)
	{
		m_listRes.InsertColumn(i, szColumns[i], LVCFMT_CENTER);
		m_listRes.SetColumnWidth(i, colum_w[i]);
	}

	m_proj = ::GetPackProj();
	int nSmallCx = ::GetSystemMetrics(SM_CXSMICON);
	int nSmallCy = ::GetSystemMetrics(SM_CYSMICON);
	if (m_proj->m_imglist.m_hImageList == NULL)
		m_proj->m_imglist.Create(nSmallCx, nSmallCy, ILC_MASK | ILC_COLOR32, 0, 1);
	m_listRes.SetImageList(&m_proj->m_imglist, LVSIL_SMALL);
	m_listRes.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_JUSTIFYCOLUMNS | LVS_EX_SINGLEROW | LVS_EX_SNAPTOGRID);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CResManDlg::OnBnClickedBtnLocal()
{
	// TODO:  在此添加控件通知处理程序代码
}

void CResManDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CExDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		m_listRes.DeleteAllItems();
		int row = 0;
		CResMan * pRes = m_proj->m_pRes;
		while (pRes)
		{
			int imgid = pRes->m_icon_id;
			m_listRes.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_IMAGE, row, pRes->m_sfileName,
				LVIS_SELECTED | LVIF_IMAGE, LVIS_SELECTED | LVIF_IMAGE, imgid, 0);
			CString strsize = GetReadableSize((UINT32)pRes->m_fsize);
			m_listRes.SetItemText(row, 1, strsize);
			CString stime = pRes->m_tmCreate.Format(_T("%Y/%m/%d %H:%M"));
			m_listRes.SetItemText(row, 2, stime);
			m_listRes.SetItemText(row, 3, pRes->m_sformat);
			pRes = pRes->pNext;
			row++;
		}
	}
}



void CResManDlg::OnBnClickedBtnUpload()
{
}


void CResManDlg::OnBnClickedBtnResupload()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CResManDlg::OnBnClickedBtnSelect()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CResManDlg::OnBnClickedBtnAdd()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CResManDlg::OnBnClickedBtnRemove()
{
	// TODO:  在此添加控件通知处理程序代码
}
