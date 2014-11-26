// ResManDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ResManDlg.h"
#include "afxdialogex.h"


CManifestMan::CManifestMan()
{
}

CManifestMan::~CManifestMan()
{
}

int CManifestMan::AddRes(CString strRes, void * prelation)
{
	return  0;
}

int CManifestMan::Save()
{
	const char * pxml =
	{ "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
	"<manifest>\r\n"
	"<metainfo>\r\n"
	"<times createTime = \"&ProjCreateTime\" modifyTime=\"&ProjModifyTime\" />\r\n"
	"<author name = \"&AutherName\" />\r\n"
	"</metainfo>\r\n"
	"<main id = \"&id\" src=\"&Target\" srctype=\"&pdf|epub|chm|html|plain\" isbn=\"&isbn\" digestMethod=\"&md5|sha1|sha256\" digest=\"&digest\">\r\n"
	"<times createTime = \"&srcCreateTime\" modifyTime=\"&SrcModifyTime\" />\r\n"
	"<version value = \"&version\" publisher=\"&publisher\" time=\"&publishTime\" />\r\n"
	"<pageSizes>\r\n"
	"&PageSizes"
	"</pageSizes>\r\n"
	"</main>\r\n"
	"<attachments>\r\n"
	"</attachments>\r\n"
	"<relations id = \"id\" src=\"relations.sqlite\" srctype=\"database|xml|ini\" />\r\n"
	"<tags id = \"id\" src=\"tags.xml\" srctype=\"database|xml\" />\r\n"
	"<comments id = \"id\" src=\"comments.sqlite\" srctype=\"database|xml|ini\" />\r\n"
	"<regions id = \"id\" src=\"regions.sqlite\" srctype=\"database|xml\" />\r\n"
	"</manifest>\r\n"
	};

	return 0;
}
// CResManDlg 对话框
static const INT btnids[] = { IDC_BTN_LOCAL, IDC_BTN_RESUPLOAD, IDC_BTN_ADD,
						IDC_BTN_REMOVE, IDC_BTN_SELECT, IDC_BTN_UPLOAD, IDC_BTN_SEARCH };

IMPLEMENT_DYNAMIC(CResManDlg, CExDialog)

CResManDlg::CResManDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CResManDlg::IDD, pParent)
{
	m_pbtns[0] = NULL;
}

CResManDlg::~CResManDlg()
{
}

void CResManDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResManDlg, CExDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_LOCAL, &CResManDlg::OnBnClickedBtnLocal)
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

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CResManDlg::OnBnClickedBtnLocal()
{
	// TODO:  在此添加控件通知处理程序代码
}
