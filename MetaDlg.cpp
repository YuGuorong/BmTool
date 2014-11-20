// MetaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirPackge.h"
#include "MetaDlg.h"
#include "afxdialogex.h"
using namespace std;

// CMetaDlg dialog

IMPLEMENT_DYNAMIC(CMetaDlg, CExDialog)

CMetaDlg::CMetaDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CMetaDlg::IDD, pParent)
{
	m_pItem = NULL ;
}

CMetaDlg::~CMetaDlg()
{
	std::map<CString, CMetaExtend *>::iterator it ;
	for(it= m_MetaExtMap.begin(); it!= m_MetaExtMap.end(); it++)
	{
		FreePtr(it->second);
	}
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


 
INT  CMetaExtend::NewExtendItem(CString sFile, CWnd * pParent)
{
	CFile of ;
	if( !of.Open(sFile, CFile::modeRead) )
	{
		sFile.Format(_T("�Ҳ���Ԫ�����ļ�: %s ") , sFile);
		pParent->MessageBox(sFile, _T("���ļ�ʧ��!"));
		return -1;
	}
	int flen = (int)of.GetLength();
	char * fbuf = new char[flen+1];
	of.Read(fbuf,flen);
	of.Close();
	fbuf[flen] = 0;
	TCHAR * sbuf = QA2W(fbuf);
	delete fbuf;
	//strFile = sFile;
	pszMetaDetail = sbuf;
	strKey.Empty();
	return 1;
}
// CMetaDlg message handlers
CMetaExtend * CMetaDlg::ParseExtMeta(CString strExt, int &nSubIndex)
{
	CMetaExtend * pExt = NULL;
	CString sFile;  
	CRegexpT <WCHAR> regexp(_T("\\\"([^\\\"]*)\\\"\\s*(\\d+)"));//("course.h" 1) match group1, name ,group2 number 
	MatchResult result = regexp.Match(strExt);
	if( result.MaxGroupNumber() >= 1 )
	{
		sFile = strExt.Mid(result.GetGroupStart(1), result.GetGroupEnd(1)-result.GetGroupStart(1));
		if( result.MaxGroupNumber() == 2 )
		{
			CString stridx = strExt.Mid(result.GetGroupStart(2), result.GetGroupEnd(2)-result.GetGroupStart(2));
			nSubIndex = _tstoi(stridx);
		}
		std::map<CString, CMetaExtend *>::iterator it = m_MetaExtMap.find(sFile);

		if(  it == m_MetaExtMap.end() )
		{
			pExt = new CMetaExtend();
			if( pExt->NewExtendItem( sFile, this) > 0 )
				m_MetaExtMap.insert(make_pair(sFile,pExt));
			else
				FreePtr(pExt);
		}
		else
		{
			pExt = it->second;
		}
	}
	return pExt;
}

INT CMetaDlg::ParseMetaItem(CString &strRawMeta )
{
	CString str;
	int count = 0;
	m_nMaxCapLen = 0;
	while( AfxExtractSubString(str, strRawMeta, count++, _T('\n')) )
	{
		int nSubIndex = -1;
		if( str.IsEmpty() ) break;
		INT style = META_READWRITE;
		CMetaExtend *pext = NULL;
		str.Replace(_T('��'), _T('('));
		int istart = 0;
		CString strCap = str.Tokenize(_T("("),istart);
		str.Delete(0,strCap.GetLength());
		if( str.Find(_T('\\'))>= 0) 
		{
			istart = 0;
			str.Replace(_T('��'), _T(')'));
			int ifd = str.ReverseFind(_T(')'));
			ifd = ( ifd > 1 )? ifd-1 : str.GetLength();
			str = str.Mid(1, ifd);	
			style =  META_COMBOBOX;
		}
		else if( str.Find(_T("ͼƬ"))>= 0 ) 
			style = META_PICTURE; 
		else if( str.Find(_T("����"))>= 0 ) 
			style = META_READWRITE|META_MULTLINE; 
		else if( str.Find(_T("ʱ��"))>= 0 ) 
			style = META_DATETIME; 
		else if( str.Find(_T("\""))>=0 ) 
		{
			pext = ParseExtMeta(str, nSubIndex);
			if( pext == NULL )
			{
				FreePtr(pext);
				//return -1;
			}
			style =  META_COMBOBOX;
		}
		CMetaDataItem * pit =NewMetaItem(style, strCap,  str);	
		if( m_nMaxCapLen < strCap.GetLength() ) m_nMaxCapLen = strCap.GetLength();
		if( pext )
		{
			pit->nSubIdx = nSubIndex;
			pit->pExt = pext;
			LoadExtMetaValue(pit, pit->pExt->strKey);
		}
	}
	return count;
}

INT  CMetaDlg::LoadExtMetaValue(CMetaDataItem * pit, CString &szKeyIn )
{
	int count = 0;
	CString str, strVal;
	TCHAR * praw = pit->pExt->pszMetaDetail;
	BOOL keyMatched = FALSE;
	while( AfxExtractSubString(str, praw, count++, _T('\n')) ) //split to line
	{		
		str.Remove(_T('\r'));
		int nItem = pit->nSubIdx-1;
		if( nItem > 0 && !szKeyIn.IsEmpty() )
		{
			int nkey = nItem <1 ?  0 : nItem-1;
			CString strKey;
			if( AfxExtractSubString(strKey, str, nkey, _T('\t') ))
			{
				if( !strKey.IsEmpty()  ) 
				{
					if(strKey.Compare(szKeyIn) != 0 )
					{
						if( keyMatched ) 
							break;
						else 
							continue;		
					}
				}
				else if( !keyMatched ) continue;
				keyMatched = true;
			}
		}
		
		CString strSub;
		if( AfxExtractSubString(strSub, str, nItem, _T('\t') ) )
		{
			if( !strSub.IsEmpty() )
			{
				strSub.Replace(_T('��'), _T('\\'));
				strSub.Replace(_T('��'),_T('\\'));
				if(!strVal.IsEmpty()) strVal += _T("\\");
				strVal += strSub;
			}
		}
	}
	pit->strDefVal = strVal;
	AfxExtractSubString(pit->pExt->strKey, strVal, 0, _T('\\'));
	return 0;
}

INT CMetaDlg::LoadMetaData(LPCTSTR szMetaFile)
{
	CFile of;
	if( of.Open(szMetaFile, CFile::modeRead ) == FALSE ) return -1;
	INT flen = (int)of.GetLength();
	char *fraw = new char[flen+1];
	of.Read(fraw, flen);
	fraw[flen] = 0;
	of.Close();

	USES_CONVERSION;
	CString strRaws ;
	QA2W(fraw, strRaws);
	int ret = 0;
	ret =  ParseMetaItem(strRaws) ;
	delete fraw;
	return ret;
}

BOOL CMetaDlg::OnInitDialog()
{
	this->SetWndStyle(0, RGB(255,255,255), RGB(220,220,240));
	CExDialog::OnInitDialog();
	LOGFONT lf={0};
	lf.lfHeight = 16;
	_tcscpy_s(lf.lfFaceName, _T("����"));
	
	VERIFY(m_ftCaption.CreateFontIndirect(&lf));
	VERIFY(m_ftEdit.CreateFontIndirect(&lf));

	if(  LoadMetaData(_T("MetaTable.txt")) > 0 )
	{
		//LoadXmlMetaValues();
		CRect r(0,0, 300,ITEM_HIGHT);
		r.MoveToXY(4,4);	
		CMetaDataItem * pit = m_pItem;
		while( pit )
		{
			CreateItem( pit, _T(""), r);
			pit = pit->pNext;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#define SW_STYLE  SW_SCROLLCHILDREN|SW_INVALIDATE|SW_ERASE
#define TORP(x)  (x*12)

void CMetaDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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

	if( m_pItem == NULL ) 
		m_pItem= pit;
	else
	{
		CMetaDataItem * pits = m_pItem;
		while( pits->pNext ) pits = pits->pNext;
		pits->pNext = pit;
	}
	return pit;
}

void CMetaDlg::LoadMetaImage( CMetaDataItem * pItem, LPCTSTR strV, CRect r)
{
	r.InflateRect(-1,-1);
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
	::SetStretchBltMode(memDC.m_hDC, HALFTONE);
	::SetBrushOrgEx(memDC.m_hDC, 0, 0, NULL);
	img.StretchBlt(memDC.m_hDC,rc1,rsrc,SRCCOPY);
	memDC.SelectObject(pOld->m_hObject);
	img.ReleaseDC();
	ReleaseDC(&memDC);
	if( pItem->pimg ) 
	{
		pItem->pimg->DeleteObject();
		delete pItem->pimg;
	}
	pItem->pimg = pbtmp;

	CLink * pbmp = (CLink * )pItem->pWnd[1] ;
	pbmp->SetBitmap(pItem->pimg->operator HBITMAP());
}

void CMetaDlg::CreateTitle(PCWnd * pWnd , LPCTSTR szKey, CRect &rs)
{
	CRect r(rs);
	r.right = r.left + m_nMaxCapLen * 16;
	pWnd[0] =(CWnd *)new CLink;
	CString str = szKey;
	str = str.TrimRight();
	str +=_T(" ");
    ((CStatic*)pWnd[0])->Create(str,WS_CHILD|WS_VISIBLE|SS_RIGHT|WS_BORDER|SS_CENTERIMAGE, r, this, IDC_STATIC);
	pWnd[0]->SetFont(&m_ftCaption);
	rs.OffsetRect(r.Width(),0);
}

void CMetaDlg::CreateItem( CMetaDataItem * pItem, LPCTSTR strV, CRect &rs  )
{
	CRect rc;
	this->GetClientRect(rc);
	this->ScreenToClient(rc);
	CRect r(rs);
	int left = r.left;
	if(pItem->style & (META_PICTURE|META_MULTLINE) ) r.bottom+=60;
	if( r.bottom >= rc.bottom - 2)  
	{
		r.MoveToXY(r.left + m_nMaxCapLen * 16 + r.Width() + 8, 4);
		rs.MoveToXY(r.TopLeft());
	}
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

		pbox->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|CBS_DROPDOWNLIST|WS_VSCROLL, rb, /*pItem->pWnd[2]*/this,  pItem->nCtrlID );
        CString str = pItem->strDefVal;
		int count = 0;
		while( AfxExtractSubString(str,pItem->strDefVal,count++, _T('\\')) )
		{
			pbox->AddString(str);
			if( str.Compare(strV) == 0 )
				pbox->SetCurSel(pbox->GetCount() -1 );
		}
	}
	else if(pItem->style & META_PICTURE )
	{
		pItem->pWnd[2] =(CWnd *)new CStatic;
		((CStatic*)pItem->pWnd[2])->Create(_T(""),WS_CHILD|WS_VISIBLE|WS_BORDER|SS_USERITEM, r, this, IDC_STATIC); //frame border
		CLink * pbmp = new CLink;
		r.InflateRect(-1,-1); 
		pItem->pWnd[1] = pbmp;
		pbmp->Create(_T("static"), WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_NOTIFY, r, this,  pItem->nCtrlID);
		LoadMetaImage(pItem, strV, r);		
		HCURSOR hCurHand  =  LoadCursor( NULL  , IDC_HAND ) ;
		pbmp->SetLinkCursor(hCurHand);
			
	}
	else if(pItem->style & META_DATETIME )
	{
		pItem->pWnd[2] =(CWnd *)new CStatic;
		((CStatic*)pItem->pWnd[2])->Create(_T(""),WS_CHILD|WS_VISIBLE|WS_BORDER|SS_USERITEM, r, this, IDC_STATIC); //frame border
        CDateTimeCtrl *pWnd =new CDateTimeCtrl;
		CRect rdt(r);
		rdt.InflateRect(-1,-1);
		rdt.MoveToXY(1,1);
		if(rdt.Width()>200) rdt.left = rdt.right-200;
		pWnd->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | DTS_SHOWNONE | DTS_SHORTDATEFORMAT, rdt, pItem->pWnd[2],  pItem->nCtrlID );   
		pItem->pWnd[1] = pWnd;
	}
	else
	{
		DWORD dwstyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER;
        if( pItem->style & META_PASSWORD )  dwstyle |= ES_PASSWORD;
        if( pItem->style & META_READONLY )  dwstyle |= ES_READONLY;
		if( pItem->style & META_MULTLINE )  dwstyle |= ES_WANTRETURN|ES_MULTILINE|WS_VSCROLL;
        CWnd *pWnd =(CWnd *)new CEdit;
        ((CEdit*)pWnd)->CreateEx(/*WS_EX_CLIENTEDGE|*/WS_EX_LEFT|ES_AUTOHSCROLL, _T("EDIT"),NULL, dwstyle , r, this,  pItem->nCtrlID );       
        pWnd->SetWindowText(strV);
		pWnd->SetFont(&m_ftEdit);
		pItem->pWnd[1] = pWnd;
	}
	
	rs.OffsetRect(0, r.Height()-1);	 


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
			FreePtr( pit->pimg);
		}
		if( pit->pExt) FreePtr(pit->pExt);
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

CMetaDataItem * CMetaDlg::FindSubItem(CMetaDataItem * pit)
{
	CMetaDataItem * psub = m_pItem;
	while( psub )
	{
		if( psub->pExt == pit->pExt ) 
		{
			if( psub->nSubIdx == pit->nSubIdx+1 ) return psub;
		}
		psub = psub->pNext;
	}
	return NULL;
}

CMetaDataItem * CMetaDlg::ChangeSubComboBox(CMetaDataItem * pit)
{
	CMetaDataItem * psub = FindSubItem(pit);
	if( psub )
	{
		CString str ;
		GetDlgItem(pit->nCtrlID)->GetWindowText(str);
		this->LoadExtMetaValue(psub, str);
		CComboBox* pbox = (CComboBox*)GetDlgItem(psub->nCtrlID);
		pbox->ResetContent();
		int count = 0;
		while( AfxExtractSubString(str,psub->strDefVal,count++, _T('\\')) )
		{
			pbox->AddString(str);
		}
		pbox->SetCurSel(0);
	}
	return psub;
}

void CMetaDlg::OnSelchangeCombboxs(UINT id)
{
	if( CMetaDataItem * pit = GetCtrlItem(id) )
	{
		if( pit->style & META_COMBOBOX )
		{
			do{
				pit = ChangeSubComboBox(pit);
			}while(pit);
		}
	}
}


void CMetaDlg::OnPictureClick(UINT id)
{
	if( CMetaDataItem * pit = GetCtrlItem(id) )
	{
		if( pit->style & META_PICTURE )
		{
			CFileDialog dlg(TRUE,0,_T("*.jpg;*.bmp;*.png"),OFN_HIDEREADONLY,
				_T(" jpg�ļ�|*.jpg|λͼ�ļ�|*.bmp|png�ļ�|*.png|�����ļ�|*.*||"), this);
			if( dlg.DoModal() == IDOK )
			{
				CRect r;
				pit->pWnd[2]->GetWindowRect(r);
				LoadMetaImage(pit, dlg.GetPathName(), r);
			}
		}
	}
}

