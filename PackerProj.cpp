#include "StdAfx.h"
#include "PackerProj.h"
#include "Util.h"
#include "libs.h"
#include "resource.h"
#include "expat\expat.h"
#include "ServerModal.h"

#pragma comment(lib, ".\\lib\\expat\\libexpat.lib" ) 

CDigest::CDigest(CString &sfile, DIGEST_MODE mode )
{
	CFile of;
	m_len = 0;
	if (of.Open(sfile, CFile::modeRead | CFile::shareDenyNone))
	{
		UINT32 flen = (UINT32)of.GetLength();
		m_len = flen;
		CHAR * ptr = new CHAR[flen];
		CalDigest(ptr, flen, mode);
		delete ptr;
	}
}

LPCTSTR g_szMode[] = { { _T("MD5") }, { _T("SHA1") }, { _T("SHA256") } };
LPCTSTR  CDigest::GetModeString()
{
	return g_szMode[(int)m_mode];
}

void CDigest::CalDigest(const void * ptr, int len, DIGEST_MODE mode )
{
	if (mode == MODE_MD5)
	{
		MD5 md;
		md.update((const char *)ptr, len);
		md.finalize();
		std::string str = md.hexdigest();
		QUtf2Unc(str.c_str(), m_sDigest);
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
//================----------------------================----------------------================----------------------

CResMan::CResMan(CPackerProj * proj)
{
	m_proj = proj;
	pNext = NULL;
	m_icon_id = 0;
}

CResMan::~CResMan()
{
}

CResMan * CResMan::NewRes(const char ** sxml)
{
	if (sxml == NULL || sxml[0] == NULL) return NULL;
	CPackerProj * proj = ::GetPackProj();
	CResMan * pRes = new CResMan(proj);
	;
	for (int i = 0; sxml[i]; i+=2)
	{
		if (strcmp(sxml[i], "id") == 0 )
		{
			QUtf2Unc(sxml[i + 1], pRes->m_strResId );
		}
		else if (strcmp(sxml[i], "relation") == 0 )
		{
			pRes->m_relation = atoi(sxml[i + 1]);
		}
		else if (strcmp(sxml[i], "digestMethod") == 0)
		{
			pRes->m_digest.m_sDigest = sxml[i + 3];
			pRes->m_digest.m_mode = strcmp(sxml[i + 1], "MD5") == 0 ?
				CDigest::DIGEST_MODE::MODE_MD5 : CDigest::DIGEST_MODE::MODE_SHA256;
		}
		else if (strcmp(sxml[i], "srctype") == 0)
		{
			QUtf2Unc(sxml[i + 1], pRes->m_sformat );
		}
		else if (strcmp(sxml[i], "src") == 0)
		{
			QUtf2Unc(sxml[i + 1], pRes->m_sfileName);
		}
	}
	CResMan * pres = proj->m_pRes;
	if (pres == NULL)
		proj->m_pRes = pRes;
	else
	{
		while (pres->pNext)
			pres = pres->pNext;
		pres->pNext = pRes;
	}
	return pRes;
}

CResMan * CResMan::NewRes(CFileDialog &fdlg)
{
	CFile of;
	if (!of.Open(fdlg.GetPathName(), CFile::modeRead | CFile::shareDenyNone))
		return NULL;
	CPackerProj * proj = ::GetPackProj();
	CResMan * pRes = new CResMan(proj);
	pRes->m_sPath = fdlg.GetPathName();
	pRes->m_sfileName = fdlg.GetFileName();
	pRes->m_fsize = of.GetLength();
	CFileStatus fstatus;
	of.GetStatus(fstatus);
	int flen = (int)of.GetLength();
	byte * cbuf = new byte[flen];
	of.Read(cbuf, flen);
	pRes->m_digest.CalDigest(cbuf, flen);
	of.Close();
	delete cbuf;
	pRes->m_strResId = CUtil::GenGuidString();
	pRes->m_tmCreate = fstatus.m_ctime;
	CUtil::GetFileMemiType(fdlg.GetFileExt(), pRes->m_sformat);
	pRes->m_relation = proj->m_nCurPage;

	HICON hIcon = CUtil::GetFileIcon(pRes->m_sPath);
	proj->m_imglist.Add(hIcon);
	int imgid = proj->m_imglist.GetImageCount() - 1;
	pRes->m_icon_id = imgid;
	
	CResMan * pres = proj->m_pRes;
	if (pres == NULL)
		proj->m_pRes = pRes;
	else
	{
		while (pres->pNext)
			pres = pres->pNext;
		pres->pNext = pRes;
	}
	return pRes;
}

INT CResMan::Remove(CResMan * pres)
{
	if (pres == NULL) return 0;
	CPackerProj * proj = ::GetPackProj();
	if (pres == proj->m_pRes)
		proj->m_pRes = pres->pNext;
	else
	{
		CResMan* pit = proj->m_pRes;
		while (pit)
		{
			if (pit->pNext == pres)
			{
				pit->pNext = pres->pNext;
				break;
			}
			pit = pit->pNext;
		}
	}
	delete pres;
	return 1;
}

INT CResMan::Remove(LPCTSTR szResId)
{
	CPackerProj * proj = ::GetPackProj();
	CResMan* pres = proj->m_pRes;
	while ( pres )
	{
		if (pres->m_strResId.Compare(szResId) == 0)
		{
			Remove(pres);
			return 1;
		}
		pres = pres->pNext;
	}
	return 0;
}

BOOL CResMan::SaveRes(CString &strXml)
{
	CString strTemplate;
	strTemplate .Format(_T("<attachment id=\"%d\" src=\"%s\" srctype=\"%s\" ")
		_T("digestMethod=\"%s\" digest=\"%s\" /> \r\n"), 
		m_icon_id, m_sPath, m_sformat, m_digest.GetModeString(), m_digest.m_sDigest);
	return TRUE;
}
//================----------------------================----------------------================----------------------

CPackerProj::CPackerProj(CWnd * pParent)
{
	m_ProjState = -1;
	m_logState = 0;
	m_bLock = 1;
	m_pMetaWnd = NULL;
	m_pRes = NULL;
	m_pMeta = NULL;
	m_type = VIEW_UNKNOWN;
	m_bProjModified = FALSE;

	memset(&m_db_items, 0, sizeof(m_db_items));
	memset(m_ptrDbCol, 0, sizeof(char*) * MAX_BOOK_DB_COL);
	
	for (int i = 0; i < MAX_BOOK_DB_COL; i++)
	{
		if (tblBookColSize[i] != 0)
		{
			m_ptrDbCol[i] = new char[tblBookColSize[i]];
			memset(m_ptrDbCol[i], 0, tblBookColSize[i]);
		}
	}
	m_ptrDbCol[0] = &m_db_items.Id;
	m_ptrDbCol[4] = &m_db_items.BookSize;	

	m_db_items.BookId    = (char *)m_ptrDbCol[1];
	m_db_items.BookName  = (char *)m_ptrDbCol[2];
	m_db_items.BookPath  = (char *)m_ptrDbCol[3];
	m_db_items.tmCreate  = (char *)m_ptrDbCol[5];
	m_db_items.tmModify  = (char *)m_ptrDbCol[6]; 
	m_db_items.tmUpload  = (char *)m_ptrDbCol[7]; 
	m_db_items.BookState = (char *)m_ptrDbCol[8];
	m_db_items.author    = (char *)m_ptrDbCol[9];
	m_db_items.description = (char *)m_ptrDbCol[10];
	
	LoadMetaCol();
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
	
	for (int i = 0; i < MAX_BOOK_DB_COL; i++)
	{
		if (tblBookColSize[i]) delete m_ptrDbCol[i];
	}
}

BOOL CPackerProj::LoadMetaCol()
{	
	CString strf = g_pSet->strCurPath + _T("metadetail.template");
	CString fbuf = CUtil::File2Unc(strf);
	TCHAR * ptr = fbuf.GetBuffer();
	CString strCap;
	AString strVar;
	while (*ptr)
	{
		if (ptr[0] == '!' && ptr[1] == '&')
		{
			TCHAR toc = (*(ptr - 1) == _T('\"')) ? _T('\"') : _T('<');
			TCHAR * pe = _tcschr(ptr + 2, toc);
			if (pe == NULL) break;
			pe[0] = 0; 
			strCap = ptr+2;
			TRACE(_T("%s:"), strCap);
			while ( *--ptr )
			{
				if (ptr[0] == '=' || ptr[0] == '>')
				{
					while (*--ptr == ' ');
					ptr[1] = 0;
					while (ptr[0] != ' ' && ptr[0] != '<') ptr--;
					ptr++;
					QUnc2Utf(ptr, strVar);
					strVar.Trim();
					strCap.Trim();
					MyTracex("%s\r\n", strVar);
					ptr = pe;
					m_mapKeyCaps.insert(make_pair(strVar, strCap));
					break;
				}
			}
		}
		ptr++;
	}
	fbuf.ReleaseBuffer();
	return TRUE;
}

BOOL CPackerProj::SetBookState(LPCTSTR sbookid, LPCTSTR sState)
{
	CString str; str.Format(_T("UPDATE books SET BookState = \'%s\' WHERE BookId=\'%s\' "), sState, sbookid);
	AString sql;
	QUnc2Utf(str, sql);
	execSQL(sql);
	return TRUE;
}

BOOL CPackerProj::ClearResTable()
{
	CResMan * pres = m_pRes;
	while (pres)
	{
		CResMan * pnext = pres->pNext;
		delete pres;
		pres = pnext;
	}
	//m_ProjState = CLOSE_PROJ;
	m_pRes = NULL;
	return TRUE;
}


#include "Link.h"
void LoadGif(CDC * pDC)
{
	//装载gif文件  
	Image* image = Image::FromFile(_T("res//test.gif"));

	//获得有多少个维度，对于gif就一个维度  
	UINT count = image->GetFrameDimensionsCount();
	GUID *pDimensionIDs = (GUID*)new GUID[count];
	image->GetFrameDimensionsList(pDimensionIDs, count);
	TCHAR strGuid[39];
	StringFromGUID2(pDimensionIDs[0], strGuid, 39);
	UINT frameCount = image->GetFrameCount(&pDimensionIDs[0]);

	delete[]pDimensionIDs;

	//获得这个维度有多少个帧  
	UINT   FrameNums = image->GetFrameCount(&pDimensionIDs[0]);


	//获得各帧之间的时间间隔  
	//先获得有多少个时间间隔，PropertyTagFrameDelay是GDI+中预定义的一个GIG属性ID值，表示标签帧数据的延迟时间  
	UINT   FrameDelayNums = image->GetPropertyItemSize(PropertyTagFrameDelay);


	PropertyItem *  lpPropertyItem = new  PropertyItem[FrameDelayNums];
	image->GetPropertyItem(PropertyTagFrameDelay, FrameDelayNums, lpPropertyItem);



	//Guid的值在显示GIF为FrameDimensionTime，显示TIF时为FrameDimensionPage  
	int    FrameCount = 0;
	GUID    Guid = FrameDimensionTime;
	image->SelectActiveFrame(&Guid, FrameCount);
	while (true)
	{
		Graphics   gh(pDC->GetSafeHdc());

		//显示当前帧  
		gh.DrawImage(image, 0, 0, image->GetWidth(), image->GetHeight());

		//时间间隔  
		long lPause = ((long*)lpPropertyItem->value)[FrameCount] * 10;
		Sleep(lPause);


		//设置当前需要显示的帧数  
		if ((FrameCount + 1) == FrameNums)
		{   //如果已经显示到最后一帧，那么重新开始显示  
			FrameCount = 0;
			image->SelectActiveFrame(&Guid, 0);
		}
		else
		{
			image->SelectActiveFrame(&Guid, ++FrameCount);
		}

	}
}

void CPackerProj::LoadMetaImage(CMetaDataItem * pItem, LPCTSTR strV, CRect r)
{
	r.InflateRect(-1, -1);
	CImage img;
	if (img.Load(strV) != S_OK)
	{
		img.LoadFromResource(::AfxGetInstanceHandle(), IDB_BITMAP_UNSHOWN);
	}
	HDC hDC = img.GetDC();
	CDC *pDC = CDC::FromHandle(hDC);
	CBitmap * pbtmp = new CBitmap();
	pbtmp->CreateCompatibleBitmap(pDC, r.Width(), r.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *pOld = memDC.SelectObject(pbtmp);
	CRect rsrc(0, 0, img.GetWidth(), img.GetHeight());
	CRect rc1(r);
	rc1.OffsetRect(-r.left, -r.top);
	::SetStretchBltMode(memDC.m_hDC, HALFTONE);
	::SetBrushOrgEx(memDC.m_hDC, 0, 0, NULL);
	img.StretchBlt(memDC.m_hDC, rc1, rsrc, SRCCOPY);
	memDC.SelectObject(pOld->m_hObject);
	img.ReleaseDC();
	ReleaseDC(pItem->pWnd[1]->GetSafeHwnd(),memDC.GetSafeHdc());
	if (pItem->pimg)
	{
		pItem->pimg->DeleteObject();
		delete pItem->pimg;
	}
	pItem->pimg = pbtmp;

	CLink * pbmp = (CLink *)pItem->pWnd[1];
	pbmp->SetBitmap(pItem->pimg->operator HBITMAP());
	m_bProjModified = TRUE; 
}

INT CPackerProj::DestoryProj()
{
	ClearResTable();
	m_type = VIEW_EMPTY;
	m_szProj.Empty();
	m_szProjPath.Empty();
	m_strTmCreateProj.Empty();
	m_strTmModifyProj.Empty();
	m_strTmCreateSrc.Empty();
	m_strTmModifySrc.Empty();
	m_strUuid.Empty();
	m_mapMetaValue.clear();
	while (m_imglist.Remove(1)); //note: do not remove firset image, that is project icon.
	CMetaDataItem * pit = m_pMeta;
	while (pit)
	{
		pit->pWnd[1]->SetWindowText(_T(""));
		pit->strValue.Empty();
		if (pit->style & META_PICTURE)
		{
			CRect r;
			pit->pWnd[1]->GetWindowRect(r);
			LoadMetaImage(pit, _T("NONE"), r);
		}
		pit = pit->pNext;
	}
	SetProjStatus(CLOSE_PROJ);
	return 0;
}


INT CPackerProj::CreateProj(LPCTSTR szTarget)
{
	//if (szTarget == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.pdf;*.epub"), OFN_HIDEREADONLY,
			_T("PFD文件|*.pdf|EPUB文件|*.epub|所有文件|*.*||"), m_pMetaWnd);
		fdlg.m_ofn.lpstrInitialDir = g_pSet->m_strAppUserPath;
		if (fdlg.DoModal() == IDOK)
		{
			if (fdlg.GetFileExt().CompareNoCase(_T("PDF")) == 0)
				m_type = VIEW_PDF;
			else if (fdlg.GetFileExt().CompareNoCase(_T("epub")) == 0)
				m_type = VIEW_EPUB;
			else
			{
				MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("不识别的扩展名"), _T("打开文件失败"), MB_OK);
				return 0;
			}

			m_szProj.Empty();
			m_szTargetPath = fdlg.GetPathName();
			m_szTargetFileName = fdlg.GetFileName();
			m_strUuid = CUtil::GenGuidString();
			m_mapMetaValue.clear();
			CFile of;
			if (of.Open(m_szTargetPath, CFile::modeRead | CFile::shareDenyNone))
			{
				m_nTargetFLen = (UINT32)of.GetLength();
				CFileStatus ofs;
				if (of.GetStatus(ofs))
				{
					m_strTmCreateSrc = ofs.m_ctime.Format(TIME_FMT);
					m_strTmModifySrc = ofs.m_mtime.Format(TIME_FMT);
				}
				of.Close();
				SetProjStatus(NEW_PROJ);
			}
			else
			{
				strError = _T("书已损坏，不可识别");
				DestoryProj();
			}

			//::CreateDirectory(str, NULL);
			
			//SetProjStatus(NEW_PROJ);
			return 1;
		}
	}
	return 0;
}


//>>>>==============save functions=================================saving=========================>

BOOL CPackerProj::SaveProjToDb()
{
	char sql[64 * 1024];
	Unc2Utf(g_pSet->m_strUserName, m_db_items.author, -1, 255);
	Unc2Utf(this->m_szZipPath, m_db_items.BookPath, -1, 255);
	Unc2Utf(this->m_strUuid, m_db_items.BookId, -1, 63);
	Unc2Utf(this->m_strTmCreateProj, m_db_items.tmCreate, -1, 31);
	Unc2Utf(this->m_strTmModifyProj, m_db_items.tmModify, -1, 31);
	Unc2Utf(this->m_szProj, m_db_items.BookName, -1, 255);
	Unc2Utf(L"未上传", m_db_items.BookState, -1, 16);
	DWORD flen = 0;
	if (CUtil::GetFileSize(m_szZipPath, flen) == FALSE)
		return FALSE;
	m_db_items.BookSize = flen;
	
	if (tbl_books.write(sql, 64 * 1024, &m_db_items, NULL) == 19)//constraint violation ,unique issue repeat saving ?
	{
		AString condit;
		condit.Format(" WHERE BookId=\'%s\'", m_db_items.BookId); //update
		return tbl_books.write(sql, 64 * 1024, &m_db_items, condit);
	}
	return TRUE;
}

CMetaDataItem *  CPackerProj::PrecheckContent()
{
	CMetaDataItem * pit = m_pMeta;
	CString str;
	while (pit)
	{
		if (pit->bUniq)
		{
			if (pit->style & META_READWRITE)
			{
				pit->pWnd[1]->GetWindowText(str);
				CString strt = str.Trim();
				if (strt.IsEmpty())
				{
					pit->pWnd[1]->SetFocus();
					return pit;
				}
			}
			else if (pit->style & META_COMBOBOX)
			{
				CComboBox * pbox = (CComboBox*)(pit->pWnd[1]);
				int nsel = pbox->GetCurSel();
				if (nsel == -1)
				{
					pit->pWnd[1]->SetFocus();
					return pit;
				}
			}
			else if (pit->style & META_PICTURE)
			{
				if (pit->strValue.IsEmpty())
					return pit;
			}
		}
		pit = pit->pNext;
	}
	return NULL;
}

void CPackerProj::SaveMeta(CString &sxml)
{
	CMetaDataItem * pit = m_pMeta;
	while (pit)
	{
		//pit->strKey
		CString strval;
		if (pit->style & META_COMBOBOX)
		{
			CComboBox * pbox = (CComboBox*)(pit->pWnd[1]);
			int nsel = pbox->GetCurSel();
			if (nsel >= 0)
				pbox->GetLBText(nsel, strval);
		}
		else if (pit->style & META_PICTURE)
		{
			strval = pit->strValue;
		}
		else
		{
			pit->pWnd[1]->GetWindowText(strval);
		}
		strval.TrimLeft();
		if (strval.GetLength() < 1 && (!pit->style & META_COMBOBOX))
			strval = _T("1");
		CString strpos = _T("!&");
		strpos += pit->strKey;
		strpos.TrimRight();
		sxml.Replace(strpos, ConvertXmlString(strval));
		pit = pit->pNext;
	}
}
//----save resources-----
BOOL CPackerProj::Res2Xml(CString &strResXml)
{
	CString strResTemp = _T("<attachment id=\"!&id\" src=\"!&src\" srctype=\"!&srctype\" relation=\"!&relation\" digestMethod=\"!&digestMethod\" digest=\"!&digest\" />\r\n");
	CResMan * pRes = m_pRes;
	CString strpage;
	while (pRes)
	{
		CString strxml = strResTemp;
		strxml.Replace(_T("!&id"), pRes->m_strResId);
		strxml.Replace(_T("!&src"), ConvertXmlString(pRes->m_sfileName));
		strxml.Replace(_T("!&srctype"), ConvertXmlString(pRes->m_sformat));
		strxml.Replace(_T("!&digestMethod"), pRes->m_digest.GetModeString());
		strxml.Replace(_T("!&digest"), ConvertXmlString(pRes->m_digest.m_sDigest));
		strpage.Format(_T("%d"), pRes->m_relation);
		strxml.Replace(_T("!&relation"), strpage);
		strResXml += strxml;
		pRes = pRes->pNext;
	}
	return TRUE;
}

//>----save direcotries----->
static void EndDirToXml(CString &sxml, int sublevel)
{
	CString str;
	for (int i = 0; i < sublevel; i++)
		str += _T("\t");
	sxml += str;
	sxml += _T("\t</direntry>\r\n");
}

void CPackerProj::SaveDirToXml(HTREEITEM hit, CString &sxml, int sublevel)
{
	CString sdir = m_pProjDir->GetItemText(hit);
	UINT32 npg = m_pProjDir->GetItemData(hit);
	CString str;
	for (int i = 0; i < sublevel; i++)
		str += _T("\t");
	sxml += str;
	str.Format(_T("\t<direntry name=\"%s\" pageNo=\"%d\">\r\n"), ConvertXmlString(sdir), npg);
	sxml += str;
}

void CPackerProj::SaveDirs(CString &sxml)
{
	HTREEITEM hroot = m_pProjDir->GetRootItem();
	HTREEITEM hpt = hroot;// m_pProjDir->GetNextItem(hroot, TVGN_CHILD);
	int sub = 0;
	while (hpt)
	{
		SaveDirToXml(hpt, sxml, sub);
		HTREEITEM hnext = m_pProjDir->GetNextItem(hpt, TVGN_CHILD);
		if (hnext == NULL)
		{
			hnext = m_pProjDir->GetNextItem(hpt, TVGN_NEXT);
			EndDirToXml(sxml, sub);
		}
		else
			sub++;
		while (hnext == NULL) //no sblin and child ,track back
		{
			if (hpt == hroot) break; //root no parent
			EndDirToXml(sxml, --sub);
			hpt = m_pProjDir->GetNextItem(hpt, TVGN_PARENT);
			hnext = m_pProjDir->GetNextItem(hpt, TVGN_NEXT);
			if (hnext == NULL && hpt == NULL)
				return;
		}
		hpt = hnext;
	}
}
//<----save direcotries-----<
int QUnc2Gbk(LPCTSTR lwstr, CStringA &sgbk)
{
	int needlen = WideCharToMultiByte(936, 0, lwstr, -1, 0, 0, NULL, NULL);
	if (needlen <= 0)
	{
		return 0;
	}
	else
	{
		WideCharToMultiByte(936, 0, lwstr, -1, sgbk.GetBuffer(needlen), needlen, NULL, NULL);
		sgbk.ReleaseBuffer();
	}
	return needlen;
}
void CPackerProj::GenPreview(void * hhz)
{
	HZIP hz = (HZIP)hhz;
	CString spath = m_szPathRes + _T("\\")CFG_PREVIEW_FILE;
	CString sparm = _T("-p 1-20 \"");
	if (m_type == VIEW_EPUB)
	{
		CString epub_tool_path = g_pSet->strCurPath + _T("\\")CFG_EPUB2PDF_TOOL_DIR;
		
		//CStringA sapdf; QUnc2Gbk(m_szPathRes, sapdf);//gbk???
		//CStringA sini = CUtil::File2Asc(epub_tool_path + CFG_EPUB2PDF_INF_FILE _T(".temp"));
		//sapdf.Replace("\\", "\\\\");
		//sini += sapdf + "\n\n";
		//CUtil::Asc2File(epub_tool_path + CFG_EPUB2PDF_INF_FILE, sini);
		CString sfname = m_szTargetFileName;
		int np = sfname.ReverseFind(_T('.'));
		if (np >= 0) sfname.Delete(np, sfname.GetLength() - np);
		CString srespdf = m_szPathRes + _T("\\") + sfname + _T(".pdf");
		CString stpdf = g_pSet->m_strHomePath + _T("\\") + sfname + _T(".pdf");

		CString sexe = epub_tool_path + CFG_EPUB2PDF_EXE;
		CString sparm_epubpdf = m_szTargetPath;		
		::CUtil::RunProc(sexe, sparm_epubpdf, epub_tool_path);
		CopyFile(stpdf, srespdf, FALSE);
		DeleteFile(stpdf);
		sparm += srespdf + _T("\"");
	}
	else
	{
		sparm += m_szTargetPath + _T("\"");
	}
	CString sexe = g_pSet->strCurPath + _T("\\")CFG_PDF2SWF_EXE;
	sparm += _T(" -o \"") + spath + _T("\"");
	::CUtil::RunProc(sexe, sparm, m_szPathRes);

	ZipAdd(hz, CFG_PREVIEW_FILE, spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME);
	spath.ReleaseBuffer();
}

UINT32 GetFileSize(LPCTSTR sf)
{
	CFileStatus status;
	if (CFile::GetStatus(sf, status))
	{
		return (UINT32)status.m_size;
	}
	return 0;
}

UINT32 CPackerProj::PreCountZipSize()
{
	UINT32 tlen = 0;
	tlen += m_nTargetFLen;
	tlen += m_nTargetFLen;// PREVEW_SIZE
	
	tlen += GetFileSize(m_szCoverPath + _T("\\__cover.jpg"));
	tlen += GetFileSize(m_szPathRes + _T("\\")CFG_META_FILE);
	CResMan * pRes = m_pRes;
	while(pRes)
	{
		tlen += (UINT32)pRes->m_fsize;
		pRes = pRes->pNext;
	}
	return tlen;
}

BOOL CPackerProj::EncZip(HANDLE hzip, LPCTSTR szIn, LPCTSTR szZipName, BOOL bEnc)
{
	HZIP hz = (HZIP)hzip;
#ifdef MEM_ENC
	void * ptr = NULL;
	int flen;
	ptr = encode_file(szIn, flen);
	ZRESULT ret = ZipAdd(hz, (LPCTSTR)szZipName, ptr, flen, ZIP_MEMORY);
	free_enc(ptr);
#else
	CString szenc = m_szPathRes + _T(".s.tmp");
	CStringA sin, senc;
	QW2A(szIn, sin);
	QW2A(szenc, senc);
	Encrypt((LPCTSTR)(LPCSTR)sin, (LPCTSTR)(LPCSTR)senc, 0, 0);
	ZRESULT ret = ZipAdd(hz, (LPCTSTR)szZipName, szenc.GetBuffer(), 0, ZIP_FILENAME);
	szenc.ReleaseBuffer();
#endif
	return ret;
}

BOOL CPackerProj::ZipRes(LPCTSTR szZipFile)
{
	if (m_szTargetPath)
	{
		CString strZip = szZipFile;
		TCHAR * pzip = strZip.GetBuffer(strZip.GetLength());
		HZIP hz = CreateZip(pzip, 0, ZIP_FILENAME);
		if (hz == 0) return FALSE;
		BOOL ret = TRUE;

		void * ptr = NULL;
		try {
			strZip.ReleaseBuffer();
			CString spath = this->m_szTargetPath;
			::SetProgWndLimit(PreCountZipSize(), 0);
			CString sinfo; sinfo.Format(_T("保存书本:%s"), m_szTargetFileName);
			if (!OffsetPorgPos(0, sinfo)) throw _T("用户中止");

			EncZip(hz, m_szTargetPath, m_szTargetFileName, TRUE);

			sinfo.Format(_T("保存编目元数据")); 
			if (!OffsetPorgPos(0, sinfo)) throw _T("用户中止");
			spath = m_szPathRes + _T("\\")CFG_META_FILE;
			if (ZipAdd(hz, CFG_META_FILE, spath.GetBuffer(), 0, ZIP_FILENAME) != ZR_OK)
				throw _T("压缩编目文件失败！");
			spath.ReleaseBuffer();

			sinfo.Format(_T("保存封面")); 
			if (!OffsetPorgPos(GetFileSize(m_szPathRes + _T("\\")CFG_META_FILE), sinfo)) throw _T("用户中止");
			spath = m_szCoverPath;// m_szPathRes + _T("\\__cover.jpg");
			ZipAdd(hz, _T("__cover.jpg"), spath.GetBuffer(), 0, ZIP_FILENAME);
			spath.ReleaseBuffer();

			sinfo.Format(_T("保存预览"));
			if (!OffsetPorgPos(0, sinfo)) throw _T("用户中止");
			GenPreview(hz);

			CResMan * pRes = m_pRes;
			sinfo.Format(_T("保存附件资源")); 
			if (!OffsetPorgPos(0, sinfo)) throw _T("用户中止");
			while (pRes)
			{
				if (!OffsetPorgPos((UINT32)pRes->m_fsize) ) throw _T("用户中止");
				EncZip(hz, pRes->m_sPath, pRes->m_sfileName, TRUE);
				if (ret != ZR_OK)		throw _T("压缩资源文件失败！");
				pRes = pRes->pNext;
			}
		}
		catch (...) //TODO: 压缩失败？
		{
			ret = FALSE;
		}
		CloseZip(hz);
		EndProgWnd();
	}
	return TRUE;
}

int CPackerProj::Save()
{
	CString strErr;
	INT ret = PR_OK;
	try{
		if (m_type != VIEW_EMPTY)
		{
			CMetaDataItem * pit = PrecheckContent();
			if (pit != NULL)
			{
				CString strcap;
				pit->pWnd[0]->GetWindowText(strcap);
				strErr.Format(_T("请完整填写元数据内容！\r\n \"%s\" 为必填项"), strcap);
				throw (LPCTSTR)strErr;
			}
			CFileDialog fdlg(FALSE, _T("*.") PROJ_EXT, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				_T("编目工程文件|*.") PROJ_EXT _T("|所有文件|*.*||"), ::AfxGetMainWnd());
			fdlg.m_ofn.lpstrInitialDir = g_pSet->m_strAppUserPath;
			if (fdlg.DoModal() == IDOK)
			{
				::AfxGetMainWnd()->BeginWaitCursor();
				m_szProj = fdlg.GetFileName();
				CString strpath = fdlg.GetPathName();

				m_szProjPath = CUtil::GetFilePath(strpath);//fdlg.GetFolderPath();
				CString str = m_szProjPath;
				m_szPathRes = str + _T("\\") CFG_RES_FOLDER;
				::CreateDirectory(m_szPathRes, NULL);
				m_strTmCreateProj = CTime::GetCurrentTime().Format(TIME_FMT);
				m_strTmModifyProj = m_strTmCreateProj;
				CString sXml;
				CString strTempXmlFile = g_pSet->strCurPath;
				strTempXmlFile += _T("metadetail.template");
				sXml = CUtil::File2Unc(strTempXmlFile);
				if (sXml[0] == 0xFFFE || sXml[0] == 0xFEFF) sXml.Delete(0, 1);
				if (sXml.IsEmpty())  throw _T("保存模板文件打开失败！");
				sXml.Replace(_T("!&Target"), ConvertXmlString(m_szTargetFileName));
				sXml.Replace(_T("!&id"), ConvertXmlString(m_strUuid));// this->m_strLoginUser);
				sXml.Replace(_T("!&ProjCreateTime"), m_strTmCreateProj);
				sXml.Replace(_T("!&ProjModifyTime"), m_strTmModifyProj);
				sXml.Replace(_T("!&srcCreateTime"), m_strTmCreateSrc);
				sXml.Replace(_T("!&SrcModifyTime"), m_strTmModifySrc);
				sXml.Replace(_T("!&digestMethod"), _T("MD5"));
				sXml.Replace(_T("!&digest"), _T("1"));
				sXml.Replace(_T("!&version"), _T("1"));

				SaveMeta(sXml); //m_pMetaWnd->SendMessage(WM_VIEW_PROJ, SAVE_PROJ, (WPARAM)(&sXml));//save meta

				CString  sdir;  //save directory information
				SaveDirs(sdir);
				sXml.Replace(_T("!&directory"), sdir);

				CString sAttachment;
				this->Res2Xml(sAttachment);
				sXml.Replace(_T("!&attachments"), sAttachment); //resource in xml;

				CFile of;
				CString spath = m_szPathRes + _T("\\__meta.xml");

				if (of.Open(spath, CFile::modeReadWrite | CFile::modeCreate))
				{
					CStringA astr;
					QUnc2Utf(sXml, astr);
					of.Write(astr, astr.GetLength());
					of.Close();

					CString spath = m_szProjPath + _T("\\");
					spath += m_szProj;// +_T(".")PROJ_EXT;//_T(".zip");
					if (ZipRes(spath) == FALSE) throw _T("压缩资源失败！");
					m_szZipPath = spath;
				}
				else
					throw _T("生成元数据xml失败");
				SaveProjToDb();
			}
		}
	}
	catch (LPCTSTR serr)
	{
		::MessageBox(NULL, serr, _T("保存失败"), MB_OK);
		ret = PR_ERR_CONTENT;
	}
	catch (...)
	{
		ret = PR_ERR_UNKOWN_TYPE;
	}
	::AfxGetMainWnd()->EndWaitCursor(); 
	return ret;
}
//<==============save functions=================================saving========================<<<<<<

//>>>>>>==============open functions=================================opening========================>

static void XMLCALL endXmlRoot(void *userData, const char *name);
static void XMLCALL ParseXmlRoot(void *userData, const char *name, const char **atts);

int CPackerProj::Open(LPCTSTR szProj)
{
	strError.Empty();
	if ( !this->m_szTargetPath.IsEmpty())
	{
		if (::AfxGetMainWnd()->MessageBox(_T("旧工程是否要保存？"), _T("保存"), MB_YESNO) == IDYES)
		{
			Save();
		}
	}
	if (szProj == NULL)
	{
		CFileDialog fdlg(TRUE, 0, _T("*.") PROJ_EXT, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			_T("编目工程文件|*.") PROJ_EXT _T("|所有文件|*.*||"), ::AfxGetMainWnd());
		fdlg.m_ofn.lpstrInitialDir = g_pSet->m_strAppUserPath;
		if (fdlg.DoModal() == IDOK)
		{
			if (!m_szTargetPath.IsEmpty())
				DestoryProj();
			m_szProj = fdlg.GetFileName();
			CString strpath = fdlg.GetPathName();
			m_szProjPath = CUtil::GetFilePath(strpath); //fdlg.GetFolderPath();
			ClearResTable();
			if (m_pProjDir) m_pProjDir->DeleteAllItems();
			if (UnzipProj() == FALSE)
			{
				DestoryProj();
				MessageBox(::AfxGetMainWnd()->GetSafeHwnd(), _T("工程文件内容不可识别，文件内容被损坏！"), _T("打开工程失败"), MB_OK);
				return PR_ERR_UNKOWN_TYPE;
			}
			map<CStringA, CString >::iterator l_it;
			l_it = m_mapMetaValue.find(CStringA("ebook_type"));
			if (l_it != m_mapMetaValue.end())
			{
				if (l_it->second.CompareNoCase(_T("PDF")) == 0)
					m_type = VIEW_PDF;
				else if (l_it->second.CompareNoCase(_T("EPUB")) == 0)
					m_type = VIEW_EPUB;
				else
				{
					strError = _T("不识别的类型");
					DestoryProj();
					return PR_ERR_UNKOWN_TYPE;
				}
			}
			CFile of;
			if (of.Open(m_szTargetPath, CFile::modeRead|CFile::shareDenyNone))
			{
				m_nTargetFLen = (UINT32)of.GetLength();
				CFileStatus ofs;
				if (of.GetStatus(ofs))
				{
					m_strTmCreateSrc = ofs.m_ctime.Format(TIME_FMT);
					m_strTmModifySrc = ofs.m_mtime.Format(TIME_FMT);
				}
				of.Close();
				SetProjStatus(OPEN_PROJ);
			}
			else
			{
				strError = _T("书已损坏，不可识别");
				DestoryProj();
				return PR_ERR_OPEN_BOOK;
			}
		}
		//SaveProjToDb();
	}
	return PR_OK;
}

static void XMLCALL ParseXmlDir(void *userData, const char *name, const char **atts)
{
	CPackerProj * proj = (CPackerProj*)userData;
	CString str;
	if (atts[0] && atts[1] && atts[3])
	{
		QUtf2Unc(atts[1], str);
		int pg = 0;
		pg = atoi(atts[3]);
		HTREEITEM hpt = proj->m_hDirCur;
		proj->m_hDirCur = proj->m_pProjDir->InsertItem(str, proj->m_hDirCur);
		proj->m_pProjDir->SetItemData(proj->m_hDirCur, pg);
	}
}

static void XMLCALL endXmlDir(void *userData, const char *name)
{
	CPackerProj * proj = (CPackerProj*)userData;
	if (strcmp(name, "direntry") == 0)
	{
		proj->m_hDirCur = proj->m_pProjDir->GetNextItem(proj->m_hDirCur, TVGN_PARENT);
		if (proj->m_hDirCur == NULL)
			proj->m_pProjDir->GetRootItem();		
	}
	else if (strcmp(name, "directory") == 0)
	{
		XML_Parser parser = (XML_Parser)proj->m_ptrUserData;
		XML_SetElementHandler(parser, ParseXmlRoot, endXmlRoot);
	}
}

static void XMLCALL ParseXmlAttachment(void *userData, const char *name, const char **atts)
{
	CPackerProj * proj = (CPackerProj*)userData;
	proj->m_pRes->NewRes(atts);
}

static void XMLCALL endXmlAttachment(void *userData, const char *name)
{
	if (strcmp(name, "attachments") == 0)
	{
		CPackerProj * proj = (CPackerProj*)userData;
		XML_Parser parser = (XML_Parser)proj->m_ptrUserData;
		XML_SetElementHandler(parser, ParseXmlRoot, endXmlRoot);
	}
}

static void DecMainSection(CPackerProj * proj, const char ** atts)
{
	if (atts == NULL) return;
	const char * * pxml = atts;
	CString strVal;
	while ( pxml[0] )
	{
		if (strcmp(pxml[0], "src") == 0)
			QUtf2Unc(pxml[1], proj->m_szTargetFileName);
		else if (strcmp(pxml[0], "id") == 0)
			QUtf2Unc(pxml[1], proj->m_strUuid);
		QUtf2Unc(pxml[1], strVal);
		MyTracex("%s=", pxml[0]);
		TRACE(_T("%s\r\n"), strVal);
		proj->m_mapMetaValue.insert(make_pair(pxml[0], strVal));
		pxml += 2;
	}
}

static void XMLCALL ParseXmlRoot(void *userData, const char *name, const char **atts)
{
	CPackerProj * proj = (CPackerProj*)userData;
	if (strcmp(name, "directory") == 0)
	{
		proj->m_hDirCur = NULL;

		XML_Parser parser = (XML_Parser)proj->m_ptrUserData;
		XML_SetElementHandler(parser, ParseXmlDir, endXmlDir);
	}
	else if (strcmp(name, "attachments") == 0)
	{
		XML_Parser parser = (XML_Parser)proj->m_ptrUserData;
		XML_SetElementHandler(parser, ParseXmlAttachment, endXmlAttachment);
	}
	else if ((strcmp(name, "main") == 0) || (strcmp(name, "Book_info") == 0) || (strcmp(name, "EBook") == 0) ||
		(strcmp(name, "version") == 0) || strcmp(name, "BookClass") == 0 || strcmp(name, "author") == 0  )
	{
		DecMainSection(proj, atts);
	}
	else if (strcmp(name, "times") == 0)
	{
		if (proj->m_strTmCreateProj.IsEmpty() &&(strcmp(atts[0], "createTime") == 0) )
		{
			QUtf2Unc(atts[1], proj->m_strTmCreateProj);
			if (strcmp(atts[2], "modifyTime") == 0 )
				QUtf2Unc(atts[2], proj->m_strTmModifyProj);
		}
	}
	proj->m_sXmlData.Empty();

}

static void XMLCALL endXmlRoot(void *userData, const char *name)
{
	CPackerProj * proj = (CPackerProj*)userData;
	LPCTSTR ptr = proj->m_sXmlData;
	while (*ptr)
	{
		if (ptr[0] != '\r' && ptr[0] != '\n' && ptr[0] != ' ' && ptr[0] != '\t')
		{
			proj->m_sXmlData.Trim();
			proj->m_mapMetaValue.insert(make_pair(name, proj->m_sXmlData));
		}
		ptr++;
	}
	/*
	if (strcmp(name, "content") == 0) { proj->m_mapMetaValue.insert(make_pair(name, proj->m_sXmlData)); }
	else if (strcmp(name, "author_introduce") == 0){ proj->m_mapMetaValue.insert(make_pair(name, proj->m_sXmlData)); }
	else if (strcmp(name, "force_word") == 0 ){ proj->m_mapMetaValue.insert(make_pair(name, proj->m_sXmlData)); }
	else if (strcmp(name, "book_recomend") == 0 ){ proj->m_mapMetaValue.insert(make_pair(name, proj->m_sXmlData)); }
	else if (strcmp(name, "notes") == 0 ){ proj->m_mapMetaValue.insert(make_pair(name, proj->m_sXmlData)); }
	else if (strcmp(name, "cover") == 0){ proj->m_mapMetaValue.insert(make_pair(name, proj->m_sXmlData)); }
*/
}

static void XMLCALL parsedata(void *userData, const XML_Char *s, int len)//Combine element data 
{
	if (len)
	{
		CPackerProj * proj = (CPackerProj*)userData;
		if (len > BUFSIZ)
			len = BUFSIZ;
		char buf[BUFSIZ+1];
		memcpy(buf, s, len);
		buf[len] = 0;
		TCHAR bufx[BUFSIZ];
		Utf2Unc(buf, bufx, -1, BUFSIZ);
		proj->m_sXmlData += bufx;
		
		//delete buf;
	}
}

BOOL CPackerProj::ParseXml()
{
	char buf[BUFSIZ];
	XML_Parser parser = XML_ParserCreate(NULL);
	CFile of;
	if (of.Open(CFG_RES_FOLDER _T("\\") CFG_META_FILE, CFile::modeRead | CFile::shareDenyNone) == FALSE)
		return FALSE;
	m_ptrUserData = parser;
	
	int done;
	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, ParseXmlRoot, endXmlRoot);
	XML_SetCharacterDataHandler(parser, parsedata);
	do {
		int len = of.Read(buf, BUFSIZ);
		done = len < sizeof(buf);
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
			MyTracex( "%s at line %""u\n",
				XML_ErrorString(XML_GetErrorCode(parser)),
				XML_GetCurrentLineNumber(parser));
			return 1;
		}
	} while (!done);
	of.Close();
	XML_ParserFree(parser);
	return 0;
}

int UnZipFile(HZIP hz, ZIPENTRYW *pze, LPCTSTR szFile )
{
	int index = -1;
	ZRESULT zret = FindZipItem(hz, szFile, 0, &index, pze);
	if (zret == ZR_OK)
	{
		
		CString strres =  CFG_RES_FOLDER _T("\\")  ;
		strres += szFile;
		UnzipItem(hz, index, strres.GetBuffer(strres.GetLength()), 0, ZIP_FILENAME);
		strres.ReleaseBuffer();
	}
	return index;
}

int CPackerProj::UnzipEncFile(HANDLE hzip, void * pz, LPCTSTR sfile, BOOL bres)
{
	HZIP hz = (HZIP)hzip;
	ZIPENTRYW * pze = (ZIPENTRYW *)pz;
	int index = -1;
	ZRESULT zret = FindZipItem(hz, sfile, 0, &index, pze);
	if (zret == ZR_OK)
	{
#ifdef DEC_MEM
		int flen = pze->unc_size ;
		char * pbuf = new char[flen];
		zret = UnzipItem(hz, index, pbuf, flen , ZIP_MEMORY);//bres not using
		if (zret == ZR_OK)
		{
			CString strF = bres ? CFG_RES_FOLDER : _T("");
			strF += sfile;
			if (decode_mem(pbuf, strF, flen))
				index = -2;
		}
		else
			index = -3;
		delete pbuf;
#else
		CString szZip = m_szPathRes + _T(".z.tmp");
		CStringA szip, sdec;
		zret = UnzipItem(hz, index, szZip.GetBuffer(), 0, ZIP_FILENAME);//bres not using
		szZip.ReleaseBuffer();
		
		QW2A(szZip, szip);
		QW2A(sfile, sdec);
		Decrypt((LPCTSTR)(LPCSTR)szip, (LPCTSTR)(LPCSTR)sdec, 0, 0);
#endif
	}
	return index;
}

BOOL CPackerProj::UnzipProj()
{
	CString strResPath = m_szProjPath + _T("\\") CFG_RES_FOLDER;
	CString strZipFile = m_szProjPath + _T("\\") + m_szProj;
	TCHAR * pbuf = strZipFile.GetBuffer(strZipFile.GetLength());

	SetCurrentDirectory(m_szProjPath);
	HZIP hz = OpenZip(pbuf, 0, ZIP_FILENAME);
	if (hz == NULL)
	{
		return FALSE;
	}
	ZIPENTRYW ze; GetZipItem(hz, -1, &ze); 
	int numitems = ze.index;
	int idx_xml, idx_cover, idx_book;
	//SetCurrentDirectory(strResPath);
	if (numitems > 2)
	{
		m_mapMetaValue.clear();
		idx_xml = UnZipFile(hz, &ze, CFG_META_FILE);
		idx_cover = UnZipFile(hz, &ze, CFG_COVER_FILE);
		m_szCoverPath = m_szProjPath + _T("\\")CFG_RES_FOLDER _T("\\") CFG_COVER_FILE;		
		ParseXml();	
		idx_book = UnzipEncFile(hz, &ze, m_szTargetFileName);
		m_szTargetPath = m_szProjPath + _T("\\") + m_szTargetFileName;
		m_szPathRes = m_szProjPath + _T("\\")CFG_RES_FOLDER _T("\\");
		
	}
	for (int i = 0; i<numitems; i++)
	{
		if (i != idx_xml && i != idx_cover && i != idx_book)
		{
			GetZipItem(hz, i, &ze);

			CResMan * pres = m_pRes;

			while (pres)
			{
				if (pres->m_sfileName.Compare(ze.name) == 0)
				{
					pres->m_sPath = m_szPathRes + ze.name;
					if (GetFileAttributes(pres->m_sPath) == -1)
					{
						CString strres = pres->m_sPath;
						UnzipItem(hz, i, strres.GetBuffer(strres.GetLength()), 0, ZIP_FILENAME);
						strres.ReleaseBuffer();
					}
					pres->m_fsize = ze.unc_size;
					pres->m_tmCreate = CTime(ze.ctime);
					m_imglist.Add(CUtil::GetFileIcon(ze.name));
					pres->m_icon_id = m_imglist.GetImageCount() - 1;
					//no break since maybe other same file attachments just different page relation also need set res structure.
				}

				pres = pres->pNext;
			}
		}
	}
	CloseZip(hz);
	return TRUE;
}

//<==============open functions=================================opening========================<<<<<<

void CPackerProj::SetProjStatus(int proj_st)
{
	if (m_ProjState != proj_st)
	{
		::AfxGetMainWnd()->SendMessage(WM_VIEW_PROJ, proj_st, m_ProjState); //User interface change, main dialog ui change
		m_pMetaWnd->SendMessage(WM_VIEW_PROJ, proj_st, (WPARAM)(LPCTSTR)m_szTargetPath); //m_Parent == BMDlg , Meta & Dir & Res & views change
		m_ProjState = proj_st;
	}
}



BOOL CPackerProj::UpLoadProj()
{
	// 外发的header


	return true;
}