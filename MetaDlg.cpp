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
	m_pItem = NULL ;
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
	ON_WM_DESTROY()
	ON_CONTROL_RANGE(CBN_SELCHANGE, ID_META_CTRL_START, ID_META_CTRL_END, OnSelchangeCombboxs)
	ON_CONTROL_RANGE(STN_CLICKED, ID_META_CTRL_START, ID_META_CTRL_END, OnPictureClick)
END_MESSAGE_MAP()


// CMetaDlg message handlers

BOOL CMetaDlg::OnInitDialog()
{
	this->SetWndStyle(0, RGB(255,255,255), RGB(220,220,240));
	CExDialog::OnInitDialog();
	LOGFONT lf={0};
	lf.lfHeight = 16;
	_tcscpy(lf.lfFaceName, _T("宋体"));
	
	VERIFY(m_ftCaption.CreateFontIndirect(&lf));
	VERIFY(m_ftEdit.CreateFontIndirect(&lf));

	// TODO:  Add extra initialization here
	CRect r(0,0, 200,ITEM_HIGHT);
	r.MoveToXY(4,4);	
	CMetaDataItem * pit = NewMetaItem(META_COMBOBOX,  _T("caption ygr "),  _T("test;my val;yours;lover"));
	CreateItem( pit, _T("lover"), r);

	pit = NewMetaItem(META_READWRITE,_T("caption 2 "),  _T(""));
	CreateItem( pit, _T("edit test"), r);

	pit = NewMetaItem(META_DATETIME,_T("caption Datetime "),  _T(""));
	CreateItem( pit, _T(""), r);

	pit = NewMetaItem(META_PICTURE, _T("caption Datetime "),  _T(""));
	CreateItem( pit, _T(""), r);
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

CMetaDataItem * CMetaDlg::NewMetaItem(int style, LPCTSTR szKey, LPCTSTR strDefV)
{
	CMetaDataItem * plast = m_pItem;
	CMetaDataItem * pit = new CMetaDataItem();	
	if( plast != NULL )
	{
		while( plast->pNext ) plast = plast->pNext;
		pit->nCtrlID = plast->nCtrlID + 4;
	}
	pit->style = style;
	pit->strDefVal = strDefV;
	pit->strKey = szKey;
	return pit;
}

void CMetaDlg::CreateTitle(PCWnd * pWnd , LPCTSTR szKey, CRect &r)
{
	pWnd[0] =(CWnd *)new CLink;
    ((CStatic*)pWnd[0])->Create(szKey,WS_CHILD|WS_VISIBLE|SS_RIGHT|WS_BORDER, r, this, IDC_STATIC);
	pWnd[0]->SetFont(&m_ftCaption);
	r.OffsetRect(r.Width(),0);
}

CRect CreateMatchRect(CRect &rwnd, CRect rImg)
{
	CRect r;
	return r;
}

void CMetaDlg::LoadImage( CMetaDataItem * pItem, LPCTSTR strV, CRect &r)
{
	CImage img ;
	if( img.Load(strV) != S_OK )
	{
		img.LoadFromResource(::AfxGetInstanceHandle(), IDB_BITMAP_UNSHOWN );
	}		
	HDC hDC = img.GetDC();
	CDC *pDC = CDC::FromHandle(hDC);
	CBitmap * pbtmp = new CBitmap();
	pbtmp->CreateCompatibleBitmap(pDC,r.Width(),r.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *pOld = memDC.SelectObject(pbtmp);
	CRect rsrc(0,0, img.GetWidth(), img.GetHeight());
	CRect rc1(r);
	rc1.OffsetRect(-r.left, -r.top);
	img.StretchBlt(memDC.m_hDC,rc1,rsrc,SRCCOPY);
	memDC.SelectObject(pOld->m_hObject);
	img.ReleaseDC();
	ReleaseDC(&memDC);
	pItem->pimg = pbtmp;

	CLink * pbmp = (CLink * )pItem->pWnd[1] ;
	pbmp->SetBitmap(pItem->pimg->operator HBITMAP());
}


void CMetaDlg::CreateItem( CMetaDataItem * pItem, LPCTSTR strV, CRect &r  )
{
	int left = r.left;
	if(pItem->style & META_PICTURE ) r.bottom+=60;
	CreateTitle(pItem->pWnd, pItem->strKey, r);	
	if( pItem->style & META_COMBOBOX )
	{
		pItem->pWnd[2] =(CWnd *)new CStatic;
		((CStatic*)pItem->pWnd[2])->Create(_T(""),WS_CHILD|WS_VISIBLE|WS_BORDER|SS_USERITEM, r, this, IDC_STATIC);
		CRect rcb(r);
		rcb.InflateRect(-1,-1);
		rcb.MoveToXY(1,1);

		CComboBox * pbox = (CComboBox *)new CComboBox;
		pItem->pWnd[1] = pbox;
		CRect rb = r;//rcb;
		rb.top += 1;
        rb.bottom += 60;        

		pbox->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|CBS_DROPDOWNLIST, rb, /*pItem->pWnd[2]*/this,  pItem->nCtrlID );
        CString str = pItem->strDefVal;
		int count = 0;
		while( AfxExtractSubString(str,pItem->strDefVal,count++, _T(';')) )
		{
			pbox->AddString(str);
			if( str.Compare(strV) == 0 )
				pbox->SetCurSel(pbox->GetCount() -1 );
		}
	}
	else if(pItem->style & META_PICTURE )
	{
		CLink * pbmp = new CLink;
		pItem->pWnd[1] = pbmp;
		pbmp->Create(_T("static"), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_NOTIFY, r, this,  pItem->nCtrlID);
		LoadImage(pItem, strV, r);		
		HCURSOR hCurHand  =  LoadCursor( NULL  , IDC_HAND ) ;
		pbmp->SetLinkCursor(hCurHand);
		r.OffsetRect(0, r.Height()-ITEM_HIGHT);	
	}
	else if(pItem->style & META_DATETIME )
	{
		pItem->pWnd[2] =(CWnd *)new CStatic;
		((CStatic*)pItem->pWnd[2])->Create(_T(""),WS_CHILD|WS_VISIBLE|WS_BORDER|SS_USERITEM, r, this, IDC_STATIC); //frame border
        CDateTimeCtrl *pWnd =new CDateTimeCtrl;
		CRect rdt(r);
		rdt.InflateRect(-1,-1);
		rdt.MoveToXY(1,1);
		pWnd->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | DTS_SHOWNONE | DTS_SHORTDATEFORMAT, rdt, pItem->pWnd[2],  pItem->nCtrlID );   
		pItem->pWnd[1] = pWnd;
	}
	else
	{
		DWORD dwstyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER;
        if( pItem->style & META_PASSWORD )  dwstyle |= ES_PASSWORD;
        if( pItem->style & META_READONLY )  dwstyle |= ES_READONLY;
        CWnd *pWnd =(CWnd *)new CEdit;
        ((CEdit*)pWnd)->CreateEx(/*WS_EX_CLIENTEDGE|*/WS_EX_LEFT|ES_AUTOHSCROLL, _T("EDIT"),NULL, dwstyle , r, this,  pItem->nCtrlID );       
        pWnd->SetWindowText(strV);
		pWnd->SetFont(&m_ftEdit);
		pItem->pWnd[1] = pWnd;
	}
	
	r.OffsetRect((0-(r.left-left)), ITEM_HIGHT-1);	 

	

	if( m_pItem == NULL ) 
		m_pItem= pItem;
	else
	{
		CMetaDataItem * pit = m_pItem;
		while( pit->pNext ) pit = pit->pNext;
		pit->pNext = pItem;
	}
}

void CMetaDlg::OnSize(UINT nType, int cx, int cy)
{
	CExDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


void CMetaDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	CMetaDataItem *pit = m_pItem;
	while( pit )
	{
		CMetaDataItem *pn = pit->pNext;
		if( pit->style & META_PICTURE )
		{
			pit->pimg->DeleteObject();
			delete pit->pimg;
			pit->pimg = NULL;
		}
		delete pit;

		pit = pn;
	}
	m_pItem = NULL;
	// TODO: Add your message handler code here
}

CMetaDataItem * CMetaDlg::GetCtrlItem(int nid)
{
	CMetaDataItem * pit = m_pItem;
	while( pit )
	{
		if( pit->nCtrlID == nid) 
		{
			return pit;
		}
		pit = pit->pNext;
	}
	return NULL;
}

void CMetaDlg::OnSelchangeCombboxs(UINT id)
{
	if( CMetaDataItem * pit = GetCtrlItem(id) )
	{
		if( pit->style & META_COMBOBOX )
		{
			
		}
	}
}


void CMetaDlg::OnPictureClick(UINT id)
{
	if( CMetaDataItem * pit = GetCtrlItem(id) )
	{
		if( pit->style & META_PICTURE )
		{
			CFileDialog dlg(TRUE,0,0,OFN_HIDEREADONLY,_T(" 位图文件|*.bmp| 所有文件|*.*||"), this);
			if( dlg.DoModal() == IDOK )
			{
				CRect r;
				pit->pWnd[1]->GetWindowRect(r);
				LoadImage(pit, dlg.GetPathName(), r);
			}
		}
	}
}

