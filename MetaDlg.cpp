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
	ON_WM_SIZE()
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


void CMetaDlg::CreateTitle(PCWnd * pWnd , LPCTSTR szKey, CRect &r)
{
	pWnd[0] =(CWnd *)new CLink;
    ((CStatic*)pWnd[0])->Create(szKey,WS_CHILD|WS_VISIBLE|SS_RIGHT, r, this, IDC_STATIC);
    r.OffsetRect(ITEM_CAPTION_SIZE,0);
}

void CMetaDlg::CreateItem(PCWnd * pWnd, META_ITEM * pItem, CString &strV, CRect &r,  int nID)
{
	CreateTitle(pWnd, pItem->strKey, r);
	if( pItem->style & META_COMBOBOX )
	{
		CComboBox * pbox = (CComboBox *)new CComboBox;
        pWnd[1] = pbox;
		CRect rb = r;
        rb.bottom += 60;        

        pbox->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|CBS_DROPDOWNLIST, rb, this,  nID );
        CString str = pItem->strDefVal;
        TCHAR * pn, * ptr = str.GetBuffer(str.GetLength());
        BOOL bfind = FALSE;
        {
            pn = _tcschr(ptr,_T(';'));
            if( pn )
                *pn = _T('\0');
            pbox->AddString (ptr);

            if( bfind ==FALSE && strV.Compare( ptr ) == 0 )
            {
                bfind = TRUE;
                pbox->SetCurSel(pbox->GetCount()-1);
            }
                
            ptr = pn+1;            
        }while( pn );

        str.ReleaseBuffer();
	}
	else if(pItem->style & META_PICTURE )
	{
		pItem->pimg = new CImage;
		pItem->pimg->Load(pItem->strDefVal);
		CStatic * pbmp = new CStatic;
		pbmp->Create(_T("my static"), WS_CHILD|WS_VISIBLE|SS_BITMAP, CRect(10,10,150,50), this);
		pbmp->SetBitmap(pItem->pimg->operator HBITMAP());
		r.OffsetRect(0, pItem->pimg->GetHeight()-ITEM_HIGHT);	
	}
	else
	{
		DWORD dwstyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER;
        if( pItem->style & META_PASSWORD )  dwstyle |= ES_PASSWORD;
        if( pItem->style & META_READONLY )  dwstyle |= ES_READONLY;
        pWnd[1] =(CWnd *)new CEdit;
        ((CEdit*)pWnd[1])->Create(dwstyle , r, this,  nID );
        pWnd[1]->ModifyStyleEx(0, WS_EX_CLIENTEDGE ); 
        pWnd[1]->SetWindowText(strV);
	}
	r.OffsetRect(-ITEM_CAPTION_SIZE, ITEM_HIGHT);	 
}

void CMetaDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}
