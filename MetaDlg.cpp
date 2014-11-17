// MetaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iMobleAgent.h"
#include "MetaDlg.h"
#include "afxdialogex.h"


// CMetaDlg dialog

IMPLEMENT_DYNAMIC(CMetaDlg, CExDialog)

CMetaDlg::CMetaDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CMetaDlg::IDD, pParent)
{

}

CMetaDlg::~CMetaDlg()
{
}

void CMetaDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMetaDlg, CExDialog)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CMetaDlg message handlers


BOOL CMetaDlg::OnInitDialog()
{
	this->SetWndStyle(0, RGB(255,255,255), RGB(220,220,240));
	CExDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#define SW_STYLE  SW_SCROLLCHILDREN|SW_INVALIDATE|SW_ERASE
#define TORP(x)  (x*12)

void CMetaDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

  // TODO: Add your message handler code here and/or call default
  SCROLLINFO scrollinfo;
  GetScrollInfo(SB_VERT,&scrollinfo);  
  int& nPosFrom = scrollinfo.nPos;
  int nPosTo = nPos;

  switch (nSBCode)  
  {  
  case SB_BOTTOM:  
    nPosTo = scrollinfo.nMax;
    goto label;

  case SB_TOP:  
    nPosTo = scrollinfo.nMin;
    goto label;

  case SB_LINEUP:  
    nPosTo = scrollinfo.nPos - TORP(5);
    if (scrollinfo.nPos == scrollinfo.nMin)
      break;
    else if (nPosTo < scrollinfo.nMin)
      nPosTo = scrollinfo.nMin;
    goto label;

  case SB_LINEDOWN:
    nPosTo = scrollinfo.nPos + TORP(5);
    if (scrollinfo.nPos == scrollinfo.nMax)
      break;
    else if (nPosTo > scrollinfo.nMax)
      nPosTo = scrollinfo.nMax;
    goto label;

  case SB_PAGEUP:  
    nPosTo = scrollinfo.nPos - TORP(130);
    if (scrollinfo.nPos == scrollinfo.nMin)
      break;
    else if (nPosTo < scrollinfo.nMin)
      nPosTo = scrollinfo.nMin;
    goto label;

  case SB_PAGEDOWN:  
    nPosTo = scrollinfo.nPos + TORP(130);
    if (scrollinfo.nPos == scrollinfo.nMax)
      break;
    else if (nPosTo > scrollinfo.nMax)
      nPosTo = scrollinfo.nMax;
    goto label;

  case SB_THUMBPOSITION:  
    if(scrollinfo.nPos==scrollinfo.nMax && ((int)nPos>scrollinfo.nMax) )
      break;
    goto label;

  case SB_THUMBTRACK:  
    if(scrollinfo.nPos==scrollinfo.nMax && ((int)nPos>scrollinfo.nMax) )
      break;
    goto label;

label:
    SetScrollPos(SB_VERT, nPosTo);
    ScrollWindowEx(0, nPosFrom - nPosTo, NULL, NULL, NULL, NULL, SW_STYLE);
    UpdateWindow();
    break;  

  case SB_ENDSCROLL:  
    break;  
  }

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
