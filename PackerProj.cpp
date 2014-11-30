#include "StdAfx.h"
#include "PackerProj.h"
#include "sha256.h"
#include "md5.h"
#include "Util.h"

CDigest::CDigest(CString &sfile, DIGEST_MODE mode )
{
	CFile of;
	if (of.Open(sfile, CFile::modeRead))
	{
		UINT32 flen = (UINT32)of.GetLength();
		CHAR * ptr = new CHAR[flen];
		CalDigest(ptr, flen, mode);
	}
}

LPCTSTR g_szMode[] = { { _T("MD5") }, { _T("SHA1") }, { _T("SHA256") } };
LPCTSTR  CDigest::GetModeString()
{
	return g_szMode[(int)m_mode];
}

void CDigest::CalDigest(LPVOID ptr, int len, DIGEST_MODE mode )
{
	if (mode == MODE_MD5)
	{
		MD5 md;
		md.update((char *)ptr, len);
		md.finalize();
		std::string str = md.hexdigest();
		QW2U(str.c_str(), m_sDigest);
	}
	else if (mode == MODE_SHA256)
	{
		byte sharet[32];
		SHA256 sha;
		sha.init();
		sha.update((byte*)ptr, len);
		sha.final(sharet);
		TCHAR sbuf[65];
		for (int i = 0; i < 32; i++)
		{
			_stprintf_s(sbuf + i * 2, 3, _T("%02X"), sharet[i]);
		}
		sbuf[64] = 0;
		m_sDigest = sbuf;
	}
	m_mode = mode;
}

CResMan::CResMan(CPackerProj * proj)
{
	CTime tm = CTime::GetCurrentTime();
	m_id.tick = ::GetTickCount();
	m_id.curtime = tm.GetTime();
	m_proj = proj;
	pNext = NULL;
}

CResMan::~CResMan()
{
}

BOOL CResMan::NewRes(RES_RELATION rel)
{
	CFileDialog fdlg(TRUE, 0, _T("*.*"), OFN_HIDEREADONLY,
		_T("所有文件|*.*||"), ::AfxGetMainWnd());

	if (fdlg.DoModal() != IDOK) return FALSE;
	m_sPath = fdlg.GetPathName();
	m_sfileName = fdlg.GetFileName();
	CFile of;
	if (!of.Open(m_sPath, CFile::modeRead))
		return FALSE;
	m_fsize = of.GetLength();
	CFileStatus fstatus;
	of.GetStatus(fstatus);
	int flen = (int)of.GetLength();
	byte * cbuf = new byte[flen];
	of.Read(cbuf, flen);
	m_digest.CalDigest(cbuf, flen);
	of.Close();
	delete cbuf;
	
	m_tmCreate = fstatus.m_ctime;
	m_sformat = fdlg.GetFileExt();
	m_relation = m_proj->m_nCurPage;
	
	CResMan * pres = m_proj->m_pRes;
	if (pres == NULL)
		m_proj->m_pRes = this;
	else
	{
		while (pres->pNext)
			pres = pres->pNext;
		pres->pNext = this;
	}
	return TRUE;
}

BOOL CResMan::SaveRes(CString &strXml)
{
	CString strTemplate;
	strTemplate .Format(_T("<attachment id=\"%d\" src=\"%s\" srctype=\"%s\" ")
		_T("digestMethod=\"%s\" digest=\"%s\" /> \r\n"), 
		m_icon_id, m_sPath, m_sformat, m_digest.GetModeString(), m_digest.m_sDigest);
	return TRUE;
}


CPackerProj::CPackerProj(CWnd * pParent)
{
	m_logState = 0;
	m_pMetaWnd = NULL;
	m_pRes = NULL;
	m_type = VIEW_UNKNOWN;
}


CPackerProj::~CPackerProj()
{
	CResMan * pres = m_pRes;
	while (pres)
	{
		CResMan * pnext = pres->pNext;
		delete pres;
		pres = pnext;
	}
}


INT CPackerProj::CreateProj(LPCTSTR szTarget)
{
	if (szTarget == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.pdf;*.epub"), OFN_HIDEREADONLY,
			_T("PFD文件|*.pdf|EPUB文件|*.epub|所有文件|*.*||"), m_pMetaWnd);
		if (fdlg.DoModal() == IDOK)
		{
			m_state = NEW_PROJ;
			if (fdlg.GetFileExt().CompareNoCase(_T("PDF")) == 0)
				m_type = VIEW_PDF;
			else
				m_type = VIEW_EPUB;

			m_szProj.Empty();
			m_szTarget = fdlg.GetPathName();
			m_szTargetFileName = fdlg.GetFileName();

			CFile of;
			if (of.Open(m_szTarget, CFile::modeRead))
			{
				CFileStatus fstatus;
				of.GetStatus(fstatus);
				of.Close();
				m_strTmCreateSrc = fstatus.m_ctime.Format(_T("%Y/%m/%d %H:%M"));
				m_strTmModifySrc = fstatus.m_mtime.Format(_T("%Y/%m/%d %H:%M"));
			}
			else
			{
				//TODO: OPEN fail!
			}
			//::CreateDirectory(str, NULL);
			m_pMetaWnd->PostMessage(WM_VIEW_PROJ, NEW_PROJ, (WPARAM)(LPCTSTR)m_szTarget); //m_Parent == BMDlg
			SetProjStatus(NEW_PROJ);
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
			_T("编目工程文件|*.bmproj|所有文件|*.*||"), m_pMetaWnd);
		if (fdlg.DoModal() == IDOK)
		{
			m_szProjPath = fdlg.GetPathName();
			m_szProj = fdlg.GetFileName();
			CString str = fdlg.GetFolderPath();
			m_szPathRes = str + _T("\\res");
			m_szPathMeta = str + _T("\\meta");
			::CreateDirectory(m_szPathRes, NULL);
			::CreateDirectory(m_szPathMeta, NULL);
			m_strTmCreateProj = CTime::GetCurrentTime().Format(_T("%Y/%m/%d %H:%M"));
			m_strTmModifyProj = m_strTmCreateProj;
			CString sXml;
			CString strTempXmlFile = g_pSet->strCurPath;
			strTempXmlFile += _T("metadetail.template");
			sXml = CUtil::File2Unc(strTempXmlFile);
			if (sXml.IsEmpty())  return -1;
			sXml.Replace(_T("!&Target"), m_szTargetFileName);
			sXml.Replace(_T("!&id"), this->m_strSession);// this->m_strLoginUser);
			sXml.Replace(_T("!&ProjCreateTime"), m_strTmCreateProj);
			sXml.Replace(_T("!&ProjModifyTime"), m_strTmModifyProj);
			sXml.Replace(_T("!&srcCreateTime"), m_strTmCreateSrc);
			sXml.Replace(_T("!&SrcModifyTime"), m_strTmModifySrc);

			m_pMetaWnd->SendMessage(WM_VIEW_PROJ, SAVE_PROJ, (WPARAM)(&sXml));//save meta

			CFile of;
			CString spath = m_szPathMeta + _T("\\__meta.xml");

			if (of.Open(spath, CFile::modeReadWrite | CFile::modeCreate))
			{
				CStringA astr;
				QU2W(sXml, astr);
				of.Write(astr, astr.GetLength());
				of.Close();

				CString spath = m_szPathRes + _T("\\");
				spath += m_szProj + _T(".zip");
				ZipRes(spath);
			}
		}
	}
	return 0;
}


int CPackerProj::Open(LPCTSTR szProj)
{
	if (szProj == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.bmproj"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			_T("编目工程文件|*.bmproj|所有文件|*.*||"), m_pMetaWnd);
		if (fdlg.DoModal() == IDOK)
		{
			m_szProj.Empty();
			//::CreateDirectory(str, NULL);
		}
	}
	return 0;
}

#include "lib\XZip\XZip.h"
BOOL CPackerProj::ZipRes(LPCTSTR szZipFile)
{
	if (m_pRes)
	{
		CString strZip = szZipFile;
		TCHAR * pzip = strZip.GetBuffer(strZip.GetLength());
		HZIP hz = CreateZip(pzip, 0, ZIP_FILENAME);
		strZip.ReleaseBuffer();
		CString spath = this->m_szTarget;
		ZipAdd(hz, m_szTargetFileName, spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME);
		spath.ReleaseBuffer();

		 spath = m_szPathMeta + _T("\\__meta.xml");
		ZipAdd(hz, _T("__meta.xml"), spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME);
		spath.ReleaseBuffer();

		spath = m_szPathRes + _T("\\__cover.jpg");
		ZipAdd(hz, _T("__cover.jpg"), spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME);
		spath.ReleaseBuffer();

		CResMan * pRes = m_pRes;
		while (pRes)
		{
			CString src = pRes->m_sPath;
			TCHAR * ptr = src.GetBuffer(src.GetLength());
			ZRESULT ret = ZipAdd(hz, pRes->m_sfileName, ptr, 0, ZIP_FILENAME);
			src.ReleaseBuffer();
			pRes = pRes->pNext;
		}
		CloseZip(hz);
	}
	return TRUE;
}

void CPackerProj::SetProjStatus(int proj_st)
{
	if (m_ProjState != proj_st)
	{
		::AfxGetMainWnd()->SendMessage(WM_VIEW_PROJ, proj_st, m_ProjState);
		m_ProjState = proj_st;
	}
}

