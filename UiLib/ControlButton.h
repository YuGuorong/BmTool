#if !defined(AFX_CONTROLBUTTON_H__651F14BF_BEBC_4595_90ED_CD4B920F1A0A__INCLUDED_)
#define AFX_CONTROLBUTTON_H__651F14BF_BEBC_4595_90ED_CD4B920F1A0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlButton window
/**********************************************
 简介：CControlButton 控制按钮类，继承于CButton
	实现控制按钮的绘制
 功能：可加载按钮上四种状态图片，可任意改变其形状和响应区域
 原作者：李其宋
 修改：邹英斌
 修改内容：增加2个函数SetMaskBitmapId(int mask,bool action),设置一张蒙板图片，对背景透明
            SetRgnMask(int nMaskBmpId,bool nAction)，设置和蒙板相对应的区域


************************************************/

class CControlButton : public CButton
{
// Construction
public:
	CControlButton();
	enum SYTLE{ROUNDRECT,ELLIPSE,UPTRIANGLE,DOWNTRIANGLE}m_Style; // 按钮的外观样式
	void SetRgnStyle(SYTLE nStyle); // 设置按钮有效区域的样式
	void SetBitmapId(int nOver,int nNormal,int nPressed,int nFocus); // 设定按钮各种状态的图片ID
	void SetBitmap(int buttns_btmp_id, int w, int h, int count = 4, COLORREF cmask=RGB(255,0,255));
	void SetDisableBmpId(int nDisabled);

// Attributes
protected:
	//按钮的外边框
	CPen m_BoundryPen;
	CRgn m_rgn;
	//鼠标指针置于按钮之上时按钮的内边框
	CPen m_InsideBoundryPenLeft;
	CPen m_InsideBoundryPenRight;
	CPen m_InsideBoundryPenTop;
	CPen m_InsideBoundryPenBottom;
	
	//按钮获得焦点时按钮的内边框
	CPen m_InsideBoundryPenLeftSel;
	CPen m_InsideBoundryPenRightSel;
	CPen m_InsideBoundryPenTopSel;
	CPen m_InsideBoundryPenBottomSel;
	
	//按钮的底色，包括有效和无效两种状态
	CBrush m_FillActive;
	CBrush m_FillInactive;
	
	//按钮的状态
	BOOL m_bOver;	//鼠标位于按钮之上时该值为true，反之为flase
	BOOL m_bTracking;	//在鼠标按下没有释放时该值为true
	BOOL m_bSelected;	//按钮被按下是该值为true
	BOOL m_bFocus;	//按钮为当前焦点所在时该值为true


	CBitmap m_NormalBitmap;
	CBitmap m_PressedBitmap;
	CBitmap m_FocusBitmap;
	CBitmap m_OverBitmap;
    CBitmap m_MaskBitmap;
	CBitmap m_BackBitmap;
	CBitmap m_DisableBitamp;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CControlButton();
    virtual void DoGradientFill(CDC *pDC, CRect* rect);
	virtual void DrawInsideBorder(CDC *pDC, CRect* rect);
	// Generated message map functions
protected:
	//{{AFX_MSG(CControlButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	// 设置蒙板图片，显示不规则图片
	void SetMaskBitmapId(int mask, bool action);
	static HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance = RGB(0,0,0));//位图转换成区域
	// 是否使用蒙板
	bool IsMask;
	bool IsMaskRgn;
	bool IsBackBmp;
	CRect BackRect;
	// 设置位图转成有效区域
	void SetRgnMask(int nMaskBmpId, bool nAction);
	void SetBackBmp(int nBgdBmpId,CRect rect);
	int m_IsDrawText;
	void IsDrawText(int btntype);
	BOOL m_bIsChecked;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLBUTTON_H__651F14BF_BEBC_4595_90ED_CD4B920F1A0A__INCLUDED_)
