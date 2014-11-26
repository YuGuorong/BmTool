#include "StdAfx.h"
#include "PackerProj.h"


CPackerProj::CPackerProj(CWnd * pParent)
{
	m_Parent = pParent;
}


CPackerProj::~CPackerProj()
{
}


INT CPackerProj::CreateProj(LPCTSTR szTarget)
{
	if (szTarget == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.pdf;*.epub"), OFN_HIDEREADONLY,
			_T("PFD文件|*.pdf|EPUB文件|*.epub|所有文件|*.*||"), m_Parent);
		if (fdlg.DoModal() == IDOK)
		{
			m_state = NEW_PROJ;
			if (fdlg.GetFileExt().CompareNoCase(_T("PDF")) == 0)
				m_state |= TYPE_PDF;
			else
				m_state |= TYPE_EPUB;

			m_szProj.Empty();
			m_szTarget = fdlg.GetPathName();
			//::CreateDirectory(str, NULL);
			m_Parent->PostMessage(WM_VIEW_PROJ, NEW_PROJ, (WPARAM)(LPCTSTR)m_szTarget); //m_Parent == BMDlg
			return 1;
		}
	}
	return 0;
}


int CPackerProj::Save()
{
	if (m_szProj.IsEmpty())
	{
		CFileDialog fdlg(FALSE, 0, _T("*.bmproj"), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			_T("编目工程文件|*.bmproj|所有文件|*.*||"), m_Parent);
		if (fdlg.DoModal() == IDOK)
		{
			m_szProjPath = fdlg.GetPathName();
			m_szProj = fdlg.GetFileName();
			CString str = fdlg.GetFolderPath();
			m_szPathRes = str + _T("\\res");
			m_szPathMeta = str + _T("\\meta");
			::CreateDirectory(m_szPathRes, NULL);
			::CreateDirectory(m_szPathMeta, NULL);

			m_Parent->SendMessage(WM_VIEW_PROJ, SAVE_PROJ, (WPARAM)this);
		}
	}
	return 0;
}


int CPackerProj::Open(LPCTSTR szProj)
{
	if (szProj == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.bmproj"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			_T("编目工程文件|*.bmproj|所有文件|*.*||"), m_Parent);
		if (fdlg.DoModal() == IDOK)
		{
			m_szProj.Empty();
			m_szTarget = fdlg.GetPathName();
			//::CreateDirectory(str, NULL);
		}
	}
	return 0;
}
