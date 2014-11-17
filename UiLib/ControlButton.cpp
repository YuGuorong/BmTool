// ControlButton.cpp : implementation file
//

#include "stdafx.h"
#include "ControlButton.h"
//#include ".\controlbutton.h"
#include <GdiPlus.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlButton

CControlButton::CControlButton()
	: m_IsDrawText(FALSE)
	, m_bIsChecked(FALSE)
{
	m_BoundryPen.CreatePen(PS_INSIDEFRAME | PS_SOLID, 1, RGB(0, 0, 0));
	m_InsideBoundryPenLeft.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(250, 196, 88)); 
	m_InsideBoundryPenRight.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(251, 202, 106));
	m_InsideBoundryPenTop.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(252, 210, 121));
	m_InsideBoundryPenBottom.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(229, 151, 0));
	
	m_FillActive.CreateSolidBrush(RGB(223, 222, 236));
	m_FillInactive.CreateSolidBrush(RGB(222, 223, 236));
	
	m_InsideBoundryPenLeftSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(153, 198, 252)); 
	m_InsideBoundryPenTopSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(162, 201, 255));
	m_InsideBoundryPenRightSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 3, RGB(162, 189, 252));
	m_InsideBoundryPenBottomSel.CreatePen(PS_INSIDEFRAME | PS_SOLID, 2, RGB(162, 201, 255));
	
	m_bOver = m_bSelected = m_bTracking = m_bFocus = FALSE;
    IsMask = FALSE;
	IsMaskRgn = FALSE;
	IsBackBmp = FALSE;
	 m_Style = ROUNDRECT;

	 m_IsDrawText=3;

}

CControlButton::~CControlButton()
{
	m_BoundryPen.DeleteObject();
	m_InsideBoundryPenLeft.DeleteObject();
	m_InsideBoundryPenRight.DeleteObject();
	m_InsideBoundryPenTop.DeleteObject();
	m_InsideBoundryPenBottom.DeleteObject();
	
	m_FillActive.DeleteObject();
	m_FillInactive.DeleteObject();
	m_rgn.DeleteObject();

	m_InsideBoundryPenLeftSel.DeleteObject();
	m_InsideBoundryPenTopSel.DeleteObject();
	m_InsideBoundryPenRightSel.DeleteObject();
	m_InsideBoundryPenBottomSel.DeleteObject();
}


BEGIN_MESSAGE_MAP(CControlButton, CButton)
	//{{AFX_MSG_MAP(CControlButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlButton message handlers

LRESULT CControlButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bOver = FALSE;         
	m_bTracking = FALSE;           //����뿪��ť       
	InvalidateRect(NULL, FALSE);  //�ð�ť�ػ�
	return 0;
}

LRESULT CControlButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bOver = TRUE;  //��������ڰ�ť�Ͽ�
	InvalidateRect(NULL); //�ػ���
	return 0;
}


void CControlButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	//��lpDrawItemStruct��ȡ�ؼ��������Ϣ
	CRect rect =  lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->SetBkColor(RGB(255,0,255));  

	int nSaveDC=pDC->SaveDC();
	UINT state = lpDrawItemStruct->itemState;

	if (state & ODS_DISABLED)
	{
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		
		CBitmap *pOldBmp;
		pOldBmp = MemDC.SelectObject(&m_DisableBitamp);		
		if (!pOldBmp)
		{
			return;
		}
		BITMAP bmp; 
		m_PressedBitmap.GetObject(sizeof(bmp),&bmp);
		
		if (IsMask==TRUE)
		{
			CDC MaskDC;
			MaskDC.CreateCompatibleDC(pDC);
			if (IsBackBmp==TRUE)
			{
				CBitmap *pOldBmp;
				CDC BackDC;
				BackDC.CreateCompatibleDC(pDC);
				pOldBmp = MaskDC.SelectObject(&m_MaskBitmap);
				if (!pOldBmp)
				{
					return;
				}
				BackDC.SelectObject(&m_BackBitmap);
				//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&BackDC,BackRect.left,BackRect.top,SRCCOPY);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
				pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);//2011-7-5 lyl\k

			}

			CBitmap *pOldBmp;
			pOldBmp = MaskDC.SelectObject(&m_MaskBitmap);
			if (!pOldBmp)
			{
				return;
			}
			BITMAP bmp;
			m_MaskBitmap.GetObject(sizeof(bmp),&bmp);
			/*pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MaskDC,0,0,MERGEPAINT);
			pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCAND);*/
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,MERGEPAINT);//2011-7-5 lyl\k
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCAND);//2011-7-5 lyl\k

		}
		else
		{
			//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);//2011-7-5 lyl\k

		}

		pDC->RestoreDC(nSaveDC);

		return;
	}
	
	//��ȡ��ť��״̬
	if (state & ODS_FOCUS)
	{
		m_bFocus = TRUE;
		m_bSelected = TRUE;
	}
	else
	{
		m_bFocus = FALSE;
		m_bSelected = FALSE;
	}
	
	
	if (state & ODS_SELECTED || state & ODS_DEFAULT)
	{
		m_bFocus = TRUE;
	}
	
	//���ݰ�ť��״̬��ͼ
	if (m_bOver)
	{
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		CBitmap *pOldBmp;
		pOldBmp = MemDC.SelectObject(&m_OverBitmap);
		if (!pOldBmp)
		{
			return;
		}
		BITMAP bmp; 
		m_OverBitmap.GetObject(sizeof(bmp),&bmp);
		//pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
		if (IsMask==TRUE) //ֵΪ����ȥ��ͼƬ����
		{
			CDC MaskDC;
			
			MaskDC.CreateCompatibleDC(pDC);
			
			if (IsBackBmp==TRUE)//ʹ�ú���������ͬ�ı���ͼƬ
			{
				CBitmap *pOldBmp;
				CDC BackDC;
				BackDC.CreateCompatibleDC(pDC);
				pOldBmp = MaskDC.SelectObject(&m_MaskBitmap);
				if (!pOldBmp)
				{
					return;
				}
				BackDC.SelectObject(&m_BackBitmap);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
				//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&BackDC,BackRect.left,BackRect.top,SRCCOPY);
				pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);//2011-7-5 lyl\k
			}
			
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);

			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,MERGEPAINT);//2011-7-5 lyl\k
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCAND);//2011-7-5 lyl\k
			/*pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MaskDC,0,0,MERGEPAINT);
			pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCAND);*/
			ReleaseDC(&MaskDC);
		}
		else
		{
			//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);//2011-7-5 lyl\k
		}
		
		pDC->RestoreDC(nSaveDC);
	}
	else
	{
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		CBitmap *pOldBmp;
		pOldBmp = MemDC.SelectObject(&m_NormalBitmap);
		if(pOldBmp==NULL)
			return;
		BITMAP bmp; 
		m_NormalBitmap.GetObject(sizeof(bmp),&bmp);
		pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
		
		if (IsMask==TRUE)
		{
			CDC MaskDC;
			MaskDC.CreateCompatibleDC(pDC);
			if (IsBackBmp==TRUE)
			{
				CBitmap *pOldBmp;
				CDC BackDC;
				BackDC.CreateCompatibleDC(pDC);
				pOldBmp = MaskDC.SelectObject(&m_MaskBitmap);
				if (!pOldBmp)
				{
					return;
				}
				BackDC.SelectObject(&m_BackBitmap);
				//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&BackDC,BackRect.left,BackRect.top,SRCCOPY);
				pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
			}

			CBitmap *pOldBmp;
			pOldBmp = MaskDC.SelectObject(&m_MaskBitmap);
			if (!pOldBmp)
			{
				return;
			}
			//BITMAP bmp;
			m_MaskBitmap.GetObject(sizeof(bmp),&bmp);
			/*pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MaskDC,0,0,MERGEPAINT);
			pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCAND);*/
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,MERGEPAINT);//2011-7-5 lyl\k
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCAND);//2011-7-5 lyl\k

		}
		else
		{
			//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);//2011-7-5 lyl\k

		}
		pDC->RestoreDC(nSaveDC);
	}
	
	//��ť������(ѡ��)
	if (state & ODS_SELECTED)
	{
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		CBitmap *pOldBmp;
		pOldBmp = MemDC.SelectObject(&m_PressedBitmap);	
		if (!pOldBmp)
		{
			return;
		}
		BITMAP bmp; 
		m_PressedBitmap.GetObject(sizeof(bmp),&bmp);
		pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
		if (IsMask==TRUE)
		{
			CDC MaskDC;
			MaskDC.CreateCompatibleDC(pDC);
			if (IsBackBmp==TRUE)
			{
				CBitmap *pOldBmp;
				CDC BackDC;
				BackDC.CreateCompatibleDC(pDC);
				pOldBmp = MaskDC.SelectObject(&m_MaskBitmap);
				if (!pOldBmp)
				{
					return;
				}
				BackDC.SelectObject(&m_BackBitmap);
				//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&BackDC,BackRect.left,BackRect.top,SRCCOPY);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
				pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);//2011-7-5 lyl\k

			}

			CBitmap *pOldBmp;
			pOldBmp = MaskDC.SelectObject(&m_MaskBitmap);
			if (!pOldBmp)
			{
				return;
			}
			//BITMAP bmp;
			m_MaskBitmap.GetObject(sizeof(bmp),&bmp);
			/*pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MaskDC,0,0,MERGEPAINT);
			pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCAND);*/
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,MERGEPAINT);//2011-7-5 lyl\k
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCAND);//2011-7-5 lyl\k

		}
		else
		{
			//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetStretchBltMode(/*HALFTONE*/STRETCH_HALFTONE);
			//pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);
			pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);//2011-7-5 lyl\k

		}
		pDC->RestoreDC(nSaveDC);

	}

	if(m_IsDrawText==1)
	{
		CFont* newFont=new CFont();
		newFont->CreateFont(20, 0, 0, 0, FW_NORMAL , FALSE, FALSE,
	                0,GB2312_CHARSET , OUT_DEFAULT_PRECIS,
	                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	                 DEFAULT_PITCH  | FF_SCRIPT, _T("����"));
		CFont* oldFont=pDC->SelectObject(newFont);

		Gdiplus::Graphics graphics(lpDrawItemStruct->hDC);	
		CString cszText;
		GetWindowText(cszText);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0,0,0));	
		CRect rcText;
		rcText.CopyRect(&(lpDrawItemStruct->rcItem));
		rcText.left+=40;

		DrawText(lpDrawItemStruct->hDC,cszText,-1,rcText,
			DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		pDC->SelectObject(oldFont);
		delete newFont;
	}
	else if (m_IsDrawText==2)
	{
		CFont* newFont=new CFont();
		newFont->CreateFont(14, 0, 0, 0, 400 , FALSE, FALSE,
			0,GB2312_CHARSET , OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH  | FF_SCRIPT,  _T("����"));
		CFont* oldFont=pDC->SelectObject(newFont);

		Gdiplus::Graphics graphics(lpDrawItemStruct->hDC);	
		CString cszText;
		GetWindowText(cszText);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255,255,255));	
		CRect rcText;
		rcText.CopyRect(&(lpDrawItemStruct->rcItem));
		rcText.left+=17;

		DrawText(lpDrawItemStruct->hDC,(cszText),-1,rcText,
			DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		pDC->SelectObject(oldFont);
		delete newFont;
	}

	else if (m_IsDrawText==3)
	{
		CFont* newFont=new CFont();
		newFont->CreateFont(12, 0, 0, 0, 400 , FALSE, FALSE,
			0,GB2312_CHARSET , OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH  | FF_SCRIPT,  _T("����"));
		CFont* oldFont=pDC->SelectObject(newFont);

		Gdiplus::Graphics graphics(lpDrawItemStruct->hDC);	
		CString cszText;
		GetWindowText(cszText);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255,255,255));	
		CRect rcText;
		rcText.CopyRect(&(lpDrawItemStruct->rcItem));
		rcText.left+=15;

		DrawText(lpDrawItemStruct->hDC,(cszText),-1,rcText,
			DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		pDC->SelectObject(oldFont);
		delete newFont;
	}
}

void CControlButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}

//���ư�ť�ĵ�ɫ
void CControlButton::DoGradientFill(CDC *pDC, CRect* rect)
{
	CBrush brBk[64];
	int nWidth = rect->Width();	
	int nHeight = rect->Height();
	CRect rct;
	int i;
	for (i = 0; i < 64; i ++)
	{
		if (m_bOver)
		{
			if (m_bFocus)
				brBk[i].CreateSolidBrush(RGB(255 - (i / 4), 255 - (i / 4), 255 - (i / 3)));
			else
				brBk[i].CreateSolidBrush(RGB(255 - (i / 4), 255 - (i / 4), 255 - (i / 5)));
		}
		else
		{
			if (m_bFocus)
				brBk[i].CreateSolidBrush(RGB(255 - (i / 3), 255 - (i / 3), 255 - (i / 4)));
			else
				brBk[i].CreateSolidBrush(RGB(255 - (i / 3), 255 - (i / 3), 255 - (i / 5)));
		}
	}
	
	for (i = rect->top; i <= nHeight + 2; i ++) 
	{
		rct.SetRect(rect->left, i, nWidth + 2, i + 1);
		pDC->FillRect(&rct, &brBk[((i * 63) / nHeight)]);
	}
	
	for (i = 0; i < 64; i ++)
		brBk[i].DeleteObject();
}


//���ư�ť���ڱ߿�
void CControlButton::DrawInsideBorder(CDC *pDC, CRect* rect)
{
	CPen *pLeft, *pRight, *pTop, *pBottom;
	
	if (m_bSelected && !m_bOver)
	{
		pLeft = & m_InsideBoundryPenLeftSel;
		pRight = &m_InsideBoundryPenRightSel;
		pTop = &m_InsideBoundryPenTopSel;
		pBottom = &m_InsideBoundryPenBottomSel;
	}
	else
	{
		pLeft = &m_InsideBoundryPenLeft;
		pRight = &m_InsideBoundryPenRight;
		pTop = &m_InsideBoundryPenTop;
		pBottom = &m_InsideBoundryPenBottom;
	}
	
	CPoint oldPoint = pDC->MoveTo(rect->left, rect->bottom - 1);
	CPen* pOldPen = pDC->SelectObject(pLeft);
	pDC->LineTo(rect->left, rect->top + 1);
	pDC->SelectObject(pRight);
	pDC->MoveTo(rect->right - 1, rect->bottom - 1);
	pDC->LineTo(rect->right - 1, rect->top);
	pDC->SelectObject(pTop);
	pDC->MoveTo(rect->left - 1, rect->top);
	pDC->LineTo(rect->right - 1, rect->top);
	pDC->SelectObject(pBottom);
	pDC->MoveTo(rect->left, rect->bottom);
	pDC->LineTo(rect->right - 1, rect->bottom);
	pDC->SelectObject(pOldPen);
	pDC->MoveTo(oldPoint);

	if (m_bSelected && !m_bOver)
		DrawFocusRect(pDC->m_hDC,rect);
}

void CControlButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);

}

BOOL CControlButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

// ���ð�ť��Ч�������ʽ
void CControlButton::SetRgnStyle(SYTLE nStyle)
{
	m_Style = nStyle;

	if( m_rgn.GetSafeHandle() != NULL )
	{
		m_rgn.DeleteObject();
	}

	//���ð�ť����Ч����(ʵ�����Ͱ�ť)
	CRect rc;
	GetClientRect(&rc);


	// ������Ч����
	// ROUNDRECT,ELLIPSE,UPTRIANGLE,DOWNTRIANGLE
    
	if(m_Style==ROUNDRECT) // Բ�Ǿ���
	{
		m_rgn.CreateRoundRectRgn(rc.left,rc.top,rc.right,rc.bottom,6,6);
	}
	else if(m_Style==ELLIPSE)// ��Բ
	{
		m_rgn.CreateEllipticRgn(rc.left,rc.top,rc.right,rc.bottom);	  
	}
	else if(m_Style==UPTRIANGLE)// ������
	{
		CPoint ptArray[] ={CPoint(rc.left+rc.Width()/2,rc.top),
			               CPoint(rc.left,rc.bottom),
                           CPoint(rc.right,rc.bottom)};

		VERIFY(m_rgn.CreatePolygonRgn(ptArray, 3, ALTERNATE));
        
	}
	else if(m_Style==UPTRIANGLE)// ������
	{
      CPoint ptArray[] ={CPoint(rc.left,rc.top),
			               CPoint(rc.right,rc.top),
                           CPoint(rc.left+rc.Width()/2,rc.bottom)};

	  VERIFY(m_rgn.CreatePolygonRgn(ptArray, 3, ALTERNATE));
	}

	int nRes = SetWindowRgn(m_rgn,TRUE);
    if (nRes==0)
    {
		MessageBox(_T("������״���ɹ���"));
    }
	
}

#define FreeObj(bitmap)  {if( bitmap.m_hObject )  bitmap.DeleteObject();}

// �趨��ť����״̬��ͼƬID
void CControlButton::SetBitmapId(int nOver,int nNormal,int nPressed,int nFocus)
{
	FreeObj(m_NormalBitmap);
	FreeObj(m_PressedBitmap);
	FreeObj(m_FocusBitmap);
	FreeObj(m_OverBitmap);
	m_NormalBitmap.LoadBitmap(nNormal);
	m_PressedBitmap.LoadBitmap(nPressed);
	m_FocusBitmap.LoadBitmap(nFocus);
	m_OverBitmap.LoadBitmap(nOver);
}
#include "resource.h"

void GetListImage(CImageList &ImageList,CBitmap &Bitmap ,int nImageIdx)
{   
	CClientDC   dcClient(NULL);
	int nWidth, nHeight;
	ImageList_GetIconSize(ImageList.GetSafeHandle(), &nWidth, &nHeight);  
	Bitmap.CreateCompatibleBitmap(&dcClient, nWidth, nHeight);

	CDC            dcMemory;
	dcMemory.CreateCompatibleDC((CDC*)NULL) ;
	CBitmap  *   pBmpOld = dcMemory.SelectObject(&Bitmap);
	dcMemory.FillSolidRect(0, 0, nWidth, nHeight, RGB(255,255,255));
	ImageList.Draw(&dcMemory, nImageIdx, CPoint(0,0), ILD_TRANSPARENT);
	dcMemory.SelectObject(pBmpOld);
	dcMemory.DeleteDC();
}

void CControlButton::SetBitmap(int buttns_btmp_id, int w, int h,  int count, COLORREF cmask)
{
	FreeObj(m_NormalBitmap);
	FreeObj(m_PressedBitmap);
	FreeObj(m_FocusBitmap);
	FreeObj(m_OverBitmap);

	CImageList imglist ;
	imglist.Create(w,h,ILC_MASK|ILC_COLOR32, 0, 4);
	CBitmap bmt;
    bmt.LoadBitmap(buttns_btmp_id);
    imglist.Add(& bmt,cmask);

	if( count <= 0 ) return ;

	GetListImage(imglist, m_OverBitmap, 0 );
	GetListImage(imglist, m_NormalBitmap, 1);
	GetListImage(imglist, m_PressedBitmap, 2 );
	
	GetListImage(imglist, m_FocusBitmap, 0 );
}



void CControlButton::SetDisableBmpId(int nDisabled)
{
	if (m_DisableBitamp.m_hObject)
	{
		m_DisableBitamp.DeleteObject();
	}

	m_DisableBitamp.LoadBitmap(nDisabled);
}

// �����ɰ�ͼƬ����ʾ������ͼƬ
void CControlButton::SetMaskBitmapId(int mask, bool action)
{
	IsMask = action;
    m_MaskBitmap.LoadBitmap(mask);
}

HRGN CControlButton::BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance)
{
	HRGN hRgn = NULL;

	if (hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if (hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {
				sizeof(BITMAPINFOHEADER),	// biSize 
					bm.bmWidth,					// biWidth; 
					bm.bmHeight,				// biHeight; 
					1,							// biPlanes; 
					32,							// biBitCount 
					BI_RGB,						// biCompression; 
					0,							// biSizeImage; 
					0,							// biXPelsPerMeter; 
					0,							// biYPelsPerMeter; 
					0,							// biClrUsed; 
					0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC,
				(BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if (hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if (hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					GetObject(hbm32, sizeof(bm32), &bm32);
					while (bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for (int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for (int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while (x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if (b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if (b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if (b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if (x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if (pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if (x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if (y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if (x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if (y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if (pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if (hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if (hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}
	}

	return hRgn;

}
// ����λͼת����Ч����
void CControlButton::SetRgnMask(int nMaskBmpId, bool nAction)
{
	CBitmap bmp;
	HRGN rgn;
	IsMaskRgn = nAction;
	bmp.LoadBitmap(nMaskBmpId);
	rgn = BitmapToRegion((HBITMAP)bmp,RGB(255,255,255));
	
	if (IsMaskRgn==TRUE)
	{
		SetWindowRgn(rgn,TRUE);
	}
}
void CControlButton::SetBackBmp(int nBgdBmpId,CRect rect)
{
	IsBackBmp = TRUE;
	BackRect = rect;
	m_BackBitmap.LoadBitmap(nBgdBmpId);
}


void CControlButton::IsDrawText(int btntype)
{
	m_IsDrawText=btntype;
}
