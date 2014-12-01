#include "StdAfx.h"
#include "PackerProj.h"
#include "sha256.h"
#include "md5.h"
#include "Util.h"

CDigest::CDigest(CString &sfile, DIGEST_MODE mode )
{
	CFile of;
	if (of.Open(sfile, CFile::modeRead | CFile::shareDenyNone))
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
	m_strResId=CUtil::GenGuidString();
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
	if (!of.Open(m_sPath, CFile::modeRead|CFile::shareDenyNone))
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
	m_proj->GetFileType(fdlg.GetFileExt(), m_sformat);
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
#include "resource.h"

CPackerProj::CPackerProj(CWnd * pParent)
{
	m_ProjState = -1;
	m_logState = 0;
	m_pMetaWnd = NULL;
	m_pRes = NULL;
	m_type = VIEW_UNKNOWN;
	HRSRC hr = ::FindResource(NULL, MAKEINTRESOURCE(IDR_MIME_TYPE), _T("MIME"));
	if (hr)
	{
		ULONG nResSize = ::SizeofResource(NULL, hr);  // Data size/length  
		m_hG = ::LoadResource(NULL, hr);
		m_szMimeType = (LPCTSTR)LockResource(m_hG);    // Data Ptr 
	}
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
	::UnlockResource(m_hG);
}

INT CPackerProj::DestoryProj()
{
	CResMan * pres = m_pRes;
	while (pres)
	{
		CResMan * pnext = pres->pNext;
		delete pres;
		pres = pnext;
	}
	m_ProjState = CLOSE_PROJ;
	m_pRes = NULL;
	m_type = VIEW_UNKNOWN;
	m_szProj.Empty();
	m_szProjPath.Empty();
	m_strTmCreateProj.Empty();
	m_strTmModifyProj.Empty();
	m_strTmCreateSrc.Empty();
	m_strTmModifySrc.Empty();
	m_strUuid.Empty();
	SetProjStatus(CLOSE_PROJ);
	return 0;
}

INT CPackerProj::CreateProj(LPCTSTR szTarget)
{
	if (szTarget == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.pdf;*.epub"), OFN_HIDEREADONLY,
			_T("PFD文件|*.pdf|EPUB文件|*.epub|所有文件|*.*||"), m_pMetaWnd);
		if (fdlg.DoModal() == IDOK)
		{
			if (fdlg.GetFileExt().CompareNoCase(_T("PDF")) == 0)
				m_type = VIEW_PDF;
			else
				m_type = VIEW_EPUB;

			m_szProj.Empty();
			m_szTarget = fdlg.GetPathName();
			m_szTargetFileName = fdlg.GetFileName();
			m_strUuid = CUtil::GenGuidString();

			CFile of;
			if (of.Open(m_szTarget, CFile::modeRead | CFile::shareDenyNone))
			{
				CFileStatus fstatus;
				of.GetStatus(fstatus);
				of.Close();
				m_strTmCreateSrc = fstatus.m_ctime.Format(TIME_FMT);
				m_strTmModifySrc = fstatus.m_mtime.Format(TIME_FMT);
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
	//if (m_szProj.IsEmpty())
	{
		CFileDialog fdlg(FALSE, 0, _T("*.bmproj"), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			_T("编目工程文件|*.bmproj|所有文件|*.*||"), m_pMetaWnd);
		if (fdlg.DoModal() == IDOK)
		{
			m_szProjPath = fdlg.GetFolderPath();
			m_szProj = fdlg.GetFileName();
			CString str = fdlg.GetFolderPath();
			m_szPathRes = str + _T("\\res");
			m_szPathMeta = str + _T("\\meta");
			::CreateDirectory(m_szPathRes, NULL);
			::CreateDirectory(m_szPathMeta, NULL);
			m_strTmCreateProj = CTime::GetCurrentTime().Format(TIME_FMT);
			m_strTmModifyProj = m_strTmCreateProj;
			CString sXml;
			CString strTempXmlFile = g_pSet->strCurPath;
			strTempXmlFile += _T("metadetail.template");
			sXml = CUtil::File2Unc(strTempXmlFile);
			if (sXml.IsEmpty())  return -1;
			sXml.Replace(_T("!&Target"), ConvertXmlString(m_szTargetFileName));
			sXml.Replace(_T("!&id"), ConvertXmlString(m_strUuid));// this->m_strLoginUser);
			sXml.Replace(_T("!&ProjCreateTime"), m_strTmCreateProj);
			sXml.Replace(_T("!&ProjModifyTime"), m_strTmModifyProj);
			sXml.Replace(_T("!&srcCreateTime"), m_strTmCreateSrc);
			sXml.Replace(_T("!&SrcModifyTime"), m_strTmModifySrc);
			sXml.Replace(_T("!&digestMethod"), _T("MD5"));
			sXml.Replace(_T("!&digest"), _T(""));
			sXml.Replace(_T("!&version"), _T("1"));

			m_pMetaWnd->SendMessage(WM_VIEW_PROJ, SAVE_PROJ, (WPARAM)(&sXml));//save meta
			CString sAttachment;
			this->Res2Xml(sAttachment);
			sXml.Replace(_T("!&attachments"), sAttachment); //resource in xml;

			CFile of;
			CString spath = m_szPathMeta + _T("\\__meta.xml");

			if (of.Open(spath, CFile::modeReadWrite | CFile::modeCreate))
			{		
				CStringA astr;
				QU2W(sXml, astr);
				int bg = astr.Find('<');
				if ( bg > 0 )
					astr.Delete(0, bg);
				of.Write(astr, astr.GetLength());
				of.Close();

				CString spath = m_szProjPath + _T("\\");
				spath += m_szProj + _T(".")PROJ_EXT;//_T(".zip");
				if (ZipRes(spath) == FALSE)
					return -1;
			}
		}
	}
	return 0;
}


int CPackerProj::Open(LPCTSTR szProj)
{
	if ( 0 )//!this->m_szProj.IsEmpty())
	{
		if (::AfxGetMainWnd()->MessageBox(_T("旧工程是否要保存？"), _T("保存"), MB_YESNO) == IDYES)
		{
			this->Save();
		}
		this->DestoryProj();
	}
	if (szProj == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.") PROJ_EXT, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			_T("编目工程文件|*.") PROJ_EXT _T("|所有文件|*.*||"), m_pMetaWnd);
		if (fdlg.DoModal() == IDOK)
		{
			m_szProjPath = fdlg.GetFolderPath();
			m_szProj = fdlg.GetFileName();
			UnzipProj();
		}
	}
	return 0;
}



#include "lib\XZip\XZip.h"
#include "lib\XZip\XUnZip.h"
BOOL CPackerProj::Res2Xml(CString &strResXml)
{
	CString strResTemp = _T("<attachment id=\"!&id\" src=\"!&src\" srctype=\"!&srctype\" digestMethod=\"!&digestMethod\" digest=\"!&digest\" />\r\n");
	CResMan * pRes = m_pRes;
	while (pRes)
	{
		CString strxml = strResTemp;
		strxml.Replace(_T("!&id"), pRes->m_strResId);
		strxml.Replace(_T("!&src"), ConvertXmlString(pRes->m_sfileName));
		strxml.Replace(_T("!&srctype"), ConvertXmlString(pRes->m_sformat));
		strxml.Replace(_T("!&digestMethod"), pRes->m_digest.GetModeString());
		strxml.Replace(_T("!&digest"), ConvertXmlString(pRes->m_digest.m_sDigest));
		strResXml += strxml;
		pRes = pRes->pNext;
	}
	return TRUE;
}

BOOL CPackerProj::ZipRes(LPCTSTR szZipFile)
{
	if (m_pRes)
	{
		CString strZip = szZipFile;
		TCHAR * pzip = strZip.GetBuffer(strZip.GetLength());
		HZIP hz = CreateZip(pzip, 0, ZIP_FILENAME);
		if ( hz == 0) return FALSE;

		try {
			strZip.ReleaseBuffer();
			CString spath = this->m_szTarget;
			if (ZipAdd(hz, m_szTargetFileName, spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME) != ZR_OK)
				throw _T("压缩图书文件失败！");
			spath.ReleaseBuffer();

			spath = m_szPathMeta + _T("\\__meta.xml");
			if (ZipAdd(hz, _T("__meta.xml"), spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME) != ZR_OK)
				throw _T("压缩编目文件失败！");
			spath.ReleaseBuffer();
			spath.ReleaseBuffer();

			spath = m_szCoverPath;// m_szPathRes + _T("\\__cover.jpg");
			ZipAdd(hz, _T("__cover.jpg"), spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME);
			spath.ReleaseBuffer();

			CResMan * pRes = m_pRes;
			while (pRes)
			{
				CString src = pRes->m_sPath;
				TCHAR * ptr = src.GetBuffer(src.GetLength());
				ZRESULT ret = ZipAdd(hz, pRes->m_sfileName, ptr, 0, ZIP_FILENAME);
				src.ReleaseBuffer();
				if (ret != ZR_OK)
					throw _T("压缩资源文件失败！");
				pRes = pRes->pNext;
			}
		}
		catch (...) //TODO: 压缩失败？
		{
		}
		CloseZip(hz);
	}
	return TRUE;
}

BOOL CPackerProj::UnzipProj()
{
	CString strResPath = m_szProjPath + _T("\\res");
	CString strZipFile = m_szProjPath + _T("\\") + m_szProj;
	TCHAR * pbuf = strZipFile.GetBuffer(strZipFile.GetLength());

	SetCurrentDirectory(m_szProjPath);
	HZIP hz = OpenZip(pbuf, 0, ZIP_FILENAME);
	ZIPENTRYW ze; GetZipItem(hz, -1, &ze); 
	int numitems = ze.index;
	//SetCurrentDirectory(strResPath);
	for (int i = 0; i<numitems; i++)
	{
		//GetZipItem(hz, i, &ze);
		{
			/*int len = ze.unc_size;
			char * buf = new char[len + 1];
			UnzipItem(hz, i, buf, len, ZIP_MEMORY);
			buf[len] = 0;*/
			UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME);
			//delete buf;
		}
	}
	CloseZip(hz);
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

BOOL CPackerProj::GetFileType(LPCTSTR szExt, CString &stype)
{
	if (szExt && *szExt != 0)
	{
		CString strext = _T("\n");
		strext += szExt;
		strext += _T(":");
		LPCTSTR ptr = _tcsstr(m_szMimeType, strext);
		if (ptr)
		{
			LPCTSTR pstart = ptr + strext.GetLength();
			LPCTSTR pend = _tcschr(pstart, _T('\r'));
			if (pend)
			{
				int cplen = pend - pstart;
				TCHAR * pdst = stype.GetBuffer(cplen + 1);
				memcpy(pdst, pstart, cplen*sizeof(TCHAR));
				pdst[cplen] = 0;
				stype.ReleaseBuffer();
				return TRUE;
			}
		}
	}
	stype = _T("application/octet-stream");
	return FALSE;

}

BOOL CPackerProj::UpLoadProj()
{
	// 外发的header


	return true;
}