// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DirPackge.h"
#include "CovtMainDlg.h"
#include "afxdialogex.h"
#include "XZip\XZip.h"
#include "XZip\XUnzip.h"
#include "PackerProj.h"
#include "expat\expat.h"
#include <list>
//#include "ServerModal.h"
#include "libs.h"
static const LPCTSTR stRetLogFName[] = {_T("\\ok.log"), _T("\\error.log")  };

#define CFG_XML_TEMPLATE_FILE _T(".meta_template.xml")

static void XMLCALL ParseXmlRoot(void *userData, const char *name, const char **atts);
static void XMLCALL endXmlRoot(void *userData, const char *name);

#define IfSubSection(p, na, pf_attr, pf_val)        \
	if (strcmp(name, na ) == 0){         \
		p->m_sXmlStrings.Empty();\
		XML_Parser parser = (XML_Parser)p->m_ptrUserData;\
		XML_SetElementHandler(parser, pf_attr, pf_val); }  



INT ListZips(LPCTSTR szDir, CStringArray &sZips)
{
	TCHAR scdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, scdir);
	SetCurrentDirectory(szDir);
	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("*.zip"));

	sZips.RemoveAll();
	CString ssrc = szDir;
	ssrc += _T("\\");

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString str = ssrc + finder.GetFileName();
		sZips.Add(str);
	}
	SetCurrentDirectory(scdir);
	return 0;
}

int FindFile(LPCTSTR sp, CStringArray &ret)
{
	CString str;
	ret.RemoveAll();
	CFileFind finder;
	BOOL bf = finder.FindFile(sp);
	while (bf)
	{
		bf = finder.FindNextFile();
		str = finder.GetFileName();
		ret.Add(str);
	}
	return ret.GetSize();
}


INT UnzipLimitFile(CString &sin, CStringArray * pFiles, int max_size, LPCTSTR sname)
{
	TCHAR * sbuf = sin.GetBuffer();
	HZIP hz = OpenZip(sbuf, 0, ZIP_FILENAME);
	sin.ReleaseBuffer();
	if (hz == NULL)
	{
		return CVT_ERR_OPEN_PACKAGE;
	}
	ZIPENTRYW ze; GetZipItem(hz, -1, &ze);
	int numitems = ze.index;
	CString sext;
	for (int i = 0; i < numitems; i++)
	{
		GetZipItem(hz, i, &ze);
		if (ze.unc_size >= max_size)
		{
			CString s(ze.name);
			if (sname== NULL || (CUtil::GetFileExt(s, sext) && sext.CollateNoCase(sname) == 0) )
			{
				CloseZip(hz);
				return CVT_ERR_HUGE_FLV;
			}
		}
	}

	for (int i = 0; i < numitems; i++)
	{
		GetZipItem(hz, i, &ze);
		if (UnzipItem(hz, i, ze.name, 0, ZIP_FILENAME) != ZR_OK)
		{
			CloseZip(hz);
			return CVT_ERR_UNZIP;
		}
		if (pFiles) pFiles->Add(ze.name);
	}
	CloseZip(hz);
	return TRUE;
}


static void XMLCALL ParseXmlDir(void *userData, const char *name, const char **atts)
{
	CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
	CString str;
	if (atts[0] && atts[1] && atts[3])
	{
		QUtf2Unc(atts[1], str);
		HTREEITEM hpt = pdlg->m_hDirCur;
		if (hpt == NULL) pdlg->m_pProjDir->GetRootItem();
		pdlg->m_hDirCur = pdlg->m_pProjDir->InsertItem(str, pdlg->m_hDirCur);		
	}
}

static void XMLCALL endXmlDir(void *userData, const char *name)
{
	CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
	if (strcmp(name, "item") == 0)
	{
		pdlg->m_hDirCur = pdlg->m_pProjDir->GetNextItem(pdlg->m_hDirCur, TVGN_PARENT);
		if (pdlg->m_hDirCur == NULL)
			pdlg->m_pProjDir->GetRootItem();
	}
	else if (strcmp(name, "textbook") == 0)
	{
		XML_Parser parser = (XML_Parser)pdlg->m_ptrUserData;
		XML_SetElementHandler(parser, ParseXmlRoot, endXmlRoot);
	}
}

static void XMLCALL ParseXmlStrings(void *userData, const char *name, const char **atts)
{
	CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
	pdlg->m_sXmlData.Empty();
}

static void XMLCALL endXmlStrings(void *userData, const char *name)
{
	CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
	if (strcmp(name, "string") == 0 || strcmp(name, "lanstring") == 0)
	{
		if (!pdlg->m_sXmlStrings.IsEmpty())
			pdlg->m_sXmlStrings += (';');
			pdlg->m_sXmlStrings += pdlg->m_sXmlData;
	}
	else 
	{
		CStringA sv = (pdlg->m_sXmlStrings.IsEmpty()) ? (""): pdlg->m_sXmlStrings;
		pdlg->m_mapMetaVal.insert(make_pair(pdlg->m_sXmlCaption, sv));
			
		XML_Parser parser = (XML_Parser)pdlg->m_ptrUserData;
		XML_SetElementHandler(parser, ParseXmlRoot, endXmlRoot);
	}
}
static void XMLCALL ParseXmlDumy(void *userData, const char *name, const char **atts)
{
}


static void XMLCALL endXmlDumy(void *userData, const char *name)
{
	if (strcmp(name, "annotation") == 0)
	{
		CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
		XML_Parser parser = (XML_Parser)pdlg->m_ptrUserData;
		XML_SetElementHandler(parser, ParseXmlRoot, endXmlRoot);
	}
}

static void XMLCALL ParseXmlRoot(void *userData, const char *name, const char **atts)
{	
	CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
	CStringA sorig = pdlg->m_sXmlCaption;
	pdlg->m_sXmlCaption = name;
	XML_Parser parser = (XML_Parser)pdlg->m_ptrUserData;
	if (strcmp(name, "audience") == 0 || strcmp(name, "learning_resource_type") == 0 || 
		strcmp(name, "grade_level") == 0 ||strcmp(name, "grade_group") == 0  ||	
		strcmp(name, "title") == 0 || strcmp(name, "description") == 0 ||
		strcmp(name, "subject_keyword") == 0 || strcmp(name, "language") == 0  ) 
	{
		pdlg->m_sXmlStrings.Empty(); 
		XML_SetElementHandler(parser, ParseXmlStrings, endXmlStrings);
	}
	else if (strcmp(name, "textbook") == 0  )
	{
		pdlg->m_sXmlStrings.Empty(); 		
		XML_SetElementHandler(parser, ParseXmlDir, endXmlDir);
	}
	else if (strcmp(name, "annotation") == 0)
	{
		XML_SetElementHandler(parser, ParseXmlDumy, endXmlDumy);
	}
	else
	{
	}
	pdlg->m_sXmlData.Empty();

}


static void XMLCALL endXmlRoot(void *userData, const char *name)
{
	CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
	if (pdlg->m_sXmlData.IsEmpty() || pdlg->m_sXmlData.GetLength() < 1)
		pdlg->m_mapMetaVal.insert(make_pair(name, ("")));
	else
	{
		pdlg->m_sXmlData.Trim();
		pdlg->m_mapMetaVal.insert(make_pair(name, pdlg->m_sXmlData));
		pdlg->m_sXmlCaption.Empty();
		pdlg->m_sXmlData.Empty();
	}
}

static void XMLCALL parsedata(void *userData, const XML_Char *s, int len)//Combine element data 
{
	if (len)
	{
		CCovtMainDlg * pdlg = (CCovtMainDlg*)userData;
		if (len > BUFSIZ)
			len = BUFSIZ;
		char buf[BUFSIZ + 1];
		memcpy(buf, s, len);
		buf[len] = 0;
		pdlg->m_sXmlData += buf;

		//delete buf;
	}
}

int FindSpecFile(CStringArray& sfiles, CString &sf, BOOL bRemove)
{
	for (int i = 0; i < sfiles.GetCount(); i++)
	{
		if (sfiles[i].Compare(sf) == 0)
		{
			if (bRemove)
				sfiles.RemoveAt(i);
			return i;
		}
	}
	return -1;
}

CString SplidFileName(CString &sf, CString &sext)
{
	CString str = sf;
	int p = str.ReverseFind('.');
	str = sf.Left(p);
	sext = sf.Right(sf.GetLength() - p - 1);
	return str;
}



int CCovtMainDlg::AddCover(CStringA &sxmlCover, CStringA &asfile, CStringArray &sfiles)
{/* !&cover*/	
	CString strF;
	CString strCover = CFG_COVER_FILE;
	if (asfile.IsEmpty() || asfile.GetLength() <= 1)
	{
		CStringArray  sfind;
		if (FindFile(_T("cover.*"), sfind))
		{
			strF = sfind[0];
			strCover = _T("__") + strF;
			FindSpecFile(sfiles, strF, true);
		}
		else
		{
			int nctype = m_nClassType;
			if (nctype >= m_fCovers.GetCount()) nctype = m_fCovers.GetCount() - 1;
			if (nctype <= 0) nctype = 1;
				
			CStringArray * spf = (CStringArray *)m_fCovers[nctype];
			if (spf == NULL || spf->GetCount() == 0) return CVT_ERR_OVER_COVERS;
			srand(time(NULL));
			INT nr = rand() % spf->GetCount();
			CString ss;
			strF = spf->GetAt(nr);
			Logs(_T("+...%s"), strF);
			asfile = qUnc2Utf(strF);
			ss = g_pSet->strCurPath + _T("covers\\") + strF;
			//CopyFile(ss, strF, FALSE);
			strF = ss;
		}
	}
	else
	{
		strF = qUtf2Unc(asfile);
		FindSpecFile(sfiles, strF, true);
	}

	CDigest  d(strF);	
	char * ptmp = "<item type=\"cover\" id=\"\" caption=\"cover\" size=\"%d\" hashType=\"%S\" hashValue=\"%S\" file=\"%S\"  preview=\"\"/>";
	sxmlCover.Format(ptmp, d.m_len, d.GetModeString(), d.m_sDigest, ConvtXmlChars(strCover));
	ZRESULT rt = ZipAdd(m_hz, strCover, strF.GetBuffer(), 0, ZIP_FILENAME);
	strF.ReleaseBuffer();
	if (rt != ZR_OK)
		return CVT_ERR_ADD_COVER;
	//ramdom assign a cover from picturs
	//add file to package rename to __cover.???
	return CVT_OK;
}


int CCovtMainDlg::Addfile(CStringA &sxml, CStringArray &files)
{
	/*  !&files
	
	<item type="类型，除系统预定义cover之外，可以根据业务需要，增加预定义类型"
	id="" caption="" size=""
	hashType="sha1" hashValue="经过hashType指定的算法，HASH后的值" file="" preview=""/>
	<item type="" id="" caption="" size="" hashType="sha1" hashValue="经过hashType指定的算法，HASH后的值" file="" preview=""/>
	*/
	CStringA asxml_files;
	CStringArray syulan;
	FindFile(_T("yulan*.Pdf"), syulan);
	
	for (int i = 0; i < files.GetCount(); i++)
	{
		CString sfi = files[i];
		CDigest  d(sfi);
		CString sf = CUtil::GetFileName(sfi);
		int p = sf.ReverseFind(_T('.'));
		CString snm = p<0? sf : sf.Left(p);
		CString sext = sfi.Right(sfi.GetLength() - p - 1);
		CString sft = CUtil::GetFileType(sfi);
		if (p >= 0)
		{
			//already create preview? pdf ?
			if (m_strPrevw.GetLength()<2 && (!sext.IsEmpty() && sext.CompareNoCase(_T("pdf")) == 0))
			{ //convert yulan_*.pdf , other wise covert first pdf
				if (syulan.GetCount() ==0  || syulan[0].Compare(sfi) == 0)
				{
					Logs(_T("\r\n    pdf to \"preview\"..."));
					CString spath = m_strTmpDir + _T("\\")CFG_PREVIEW_FILE;
					CString sparm;
					sparm.Format(_T("-p %d-%d \""),m_nMin, m_nMax);
					sparm += sfi;
				
					CString sexe = g_pSet->strCurPath + CFG_PDF2SWF_EXE;
					sparm += _T("\" -o \"") + spath +_T("\" ") + g_pSet->m_strPdf2SwfParm;
					HANDLE hproc = ::CUtil::RunProc(sexe, sparm, m_strTmpDir);
					DWORD ret;
					BOOL bOK = GetExitCodeProcess(hproc, &ret);
					Logs(_T("\r\n    打包:\"preview\"..."));

					if ((ret = CUtil::GetFileSize(spath)) <= 0 )
						return CVT_ERR_PDF_2_SWF;

					if (ret > m_nLimitPrevSize)
						return CVT_ERR_HUGE_SWF;

					ZipAdd(m_hz, CFG_PREVIEW_FILE, spath.GetBuffer(spath.GetLength()), 0, ZIP_FILENAME);
					Logs(_T("done"));
					spath.ReleaseBuffer();
					m_strPrevw = CFG_PREVIEW_FILE;
					if (syulan.GetCount())
					{
						continue;
						ZipAdd(m_hz, snm, sfi.GetBuffer(sfi.GetLength()), 0, ZIP_FILENAME);
						sfi.ReleaseBuffer();
					}
				}
			}
#if 1
			if ( m_bEnEncrypt && m_nClassType == 1 && (!sext.IsEmpty()) &&
				( sext.CompareNoCase(_T("pdf")) == 0 || sext.CompareNoCase(_T("epub")) == 0  ) )
			{
#ifdef OPEN_ENC
				CString sen_in = sfi;
				sfi += _T(".s.tmp");
				Logs(_T("\r\n    加密文件：\"%s\"... "), sfi);
				int enc_ret = enc_file(sen_in, sfi);
#else
				CStringA asin, asEnc;
				QW2A(sfi, asin);
				sfi += _T(".s.tmp");
				asEnc = asin + (".s.tmp");
				int enc_ret = Encrypt((LPCTSTR)(LPCSTR)asin, (LPCTSTR)(LPCSTR)asEnc, 0, 0);
#endif
				if( enc_ret <= 0 )
				{
					Logs(_T("\r\n    \"%s\"加密失败[>0], %d... "), sfi, enc_ret);
					return CVT_ERR_ENC_FAIL_0 - enc_ret;
				}
			}
#endif
		}

		char * ptmp = "\t\t\t<item type=\"%S\" id=\"%d\" caption=\"%s\" size=\"%d\" hashType=\"md5\" hashValue=\"%S\" file=\"%s\"  preview=\"!&preview\"/>\r\n";
		CStringA s;
		s.Format(ptmp, sft, i, ConvtXmlChars(qUnc2Utf(snm)), d.m_len, d.m_sDigest, ConvtXmlChars(qUnc2Utf(sf)));
		asxml_files += s;
		
		Logs(_T("\r\n    打包:%s..."), files[i]);
		ZipAdd(m_hz, sf, sfi.GetBuffer(), 0, ZIP_FILENAME);
		Logs(_T("done"));
		sfi.ReleaseBuffer();
	}
	if (!m_strPrevw.IsEmpty())
		asxml_files.Replace("!&preview", qUnc2Utf(m_strPrevw));
	else
		asxml_files.Replace("!&preview", "");
	sxml.Replace("!&files", asxml_files);
	return 0;
}

static void EndDirToXml(CStringA &sxml, int sublevel)
{
	CString str;
	for (int i = 0; i < sublevel; i++)
		str += ("\t");
	sxml += str;
	sxml += ("\t</item>\r\n");
}

void CCovtMainDlg::SaveDirToXml(HTREEITEM hit, CStringA &sxml, int sublevel)
{
	CStringA sdir = qUnc2Utf(m_pProjDir->GetItemText(hit));
	//UINT32 npg = m_pProjDir->GetItemData(hit);
	CStringA str;
	for (int i = 0; i < sublevel; i++)
		str += ("\t");
	sxml += str;
	str.Format(("\t<item id=\"\" caption=\"%s\">\r\n"), ConvtXmlChars(sdir));
	sxml += str;
}

void CCovtMainDlg::SaveDirs(CStringA &sxml)
{	/* !&dirs
	<item id="目录ID" caption="目录名称">
	<item id="子目录ID" caption="子目录名称"/>
	</item>
	*/
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

int CCovtMainDlg::AddMetaInfo(CStringA &sxml)
{//!&operator : <uploader id="上传者ID" caption="上传者姓名" time="上传时间"/>
	char * ptmp = "<uploader id=\"%s\" caption=\"%s\" time=\"%S\"/>";
	CStringA smeta;
	CStringA su = qUnc2Utf(g_pSet->m_strUserName);
	CStringA sid = qUnc2Utf(g_pSet->m_strUserId);
	CTime tm(CTime::GetCurrentTime());
	CString strtm = tm.Format(TIME_FMT);
	smeta.Format(ptmp, ConvtXmlChars(sid), ConvtXmlChars(su), strtm);
	sxml.Replace("!&operator", smeta);
	return 0;
}



void CCovtMainDlg::CleanXmlUnknown(CStringA &strxml)
{
	int bf;
	while ((bf = strxml.Find("!&")) >= 0)
	{
		LPCSTR p0 = (LPCSTR)strxml + bf;
		LPCSTR ptr = p0 + 2;
		while (*ptr && isalpha(*ptr) != 0)
			ptr++;
		CStringA s;
		int count = ptr - p0;
		s = strxml.Mid(bf+2, count-2);
		Logs(_T("\r\n-->warning: \"%s\" is missing!"), qUtf2Unc(s));
		strxml.Delete(bf, count);
		strxml.Insert(bf, "0");
	}
}

int CCovtMainDlg::ParseXmlMeta(CStringArray &sfiles)
{
	CFile of;
	CString sxmlf = g_pSet->strCurPath + CFG_XML_TEMPLATE_FILE;
	if (of.Open(sxmlf, CFile::modeRead | CFile::shareDenyNone) == FALSE) return CVT_ERR_OPEN_TEMPLATE;
	CStringA stemplate;
	int flen = of.GetLength();
	of.Read(stemplate.GetBuffer(flen), flen);
	of.Close();
	stemplate.ReleaseBuffer();

	CStringA  grades, scover, assubject, asterm = "0";
	for (multimap<CStringA, CStringA>::iterator it = m_mapMetaVal.begin(); it != m_mapMetaVal.end(); it++)
	{
		CString s; QUtf2Unc(it->second, s);	MyTracex("%s :", it->first );	TRACE(_T("%s\n"), s);
		if (it->first.Compare("coverage") == 0)
		{
			scover = it->second;
			continue;
		}
		else if (it->first.Compare("contributor") == 0)
		{
			const char * ps = strstr(it->second, "\nFN:");			
			if (ps == NULL) return CVT_ERR_SECTION_AUTHOR;
			it->second.Delete(0, ps - it->second + 4);
			const char * px = strchr(it->second, '\r');
			if (px == NULL)px = strchr(it->second, '\n');
			if (px == NULL) return CVT_ERR_SECTION_AUTHOR;
			INT p2 = px - it->second;
			it->second.Delete(p2, it->second.GetLength() - p2);
		}
		else  if (it->first.Compare("metadataclass") == 0)
		{
			const int nclstbl[] = {1,8,2,6,5,3,7,4}; 
			int icls = atoi(it->second);
			if( icls <= 0 || icls >= sizeof(nclstbl)/sizeof(int)+1 )
			{
				Logs(_T("\r\n--->不可解析的\"metadataclass\": %s [1-8]!!\r\n"), it->second);
				return CVT_ERR_SECTION_METACLASS;
			}
			m_nClassType = nclstbl[icls -1];
			it->second.Format("%02d",m_nClassType);
		}
		else if (it->first.Compare("audience") == 0)
		{
			int c = 0;
			if (it->second.Find(("01")) >= 0) c |= 1;
			if (it->second.Find(("02")) >= 0) c |= 2;
			if (c == 3) c = 0;
			it->second.Format(("%d"), c);
		}
		else if (it->first.Compare("m_szUuid") == 0)
		{
			m_szUuid = it->second;
		}
		else if (it->first.Compare("textbook_release") == 0)
		{
			it->second.Trim();
			if (it->second.IsEmpty() || it->second.GetLength() <= 0)
			{
				Logs(_T(" ?+\"北京版\"..."));
				it->second = qUnc2Utf(_T("北京版"));
			}
		}
		else if (it->first.Compare("subject") == 0)
		{
			assubject = it->second;
			continue;
		}
		else if (it->first.Compare("volume") == 0)
		{
			CString svol = qUtf2Unc(it->second);
			svol.Trim();
			if (svol.Compare(_T("全册")) == 0)
			{
				asterm = "0";
			}
			else if (svol.Compare(_T("上册")) == 0)
			{
				asterm = "1";
			}
			else if (svol.Compare(_T("下册")) == 0)
			{			
				asterm = "2";
			}
			else
			{
				asterm = "0";
				Logs(_T("\r\n--->不可解析的\"volume\": %s [全册|上册|下册]!!\r\n"), svol);
				//return CVT_ERR_SECTION_VOLUME;
			}
		}
		else if (it->first.Compare("price") == 0) //replace "!&bookprice" and "!&price" both
		{
			stemplate.Replace("!&bookprice", ConvtXmlChars(it->second));
		}
		else if (it->first.Compare("grade_level") == 0)
		{
			const char grade_tmplt[] = "\t\t\t<item id = \"\" value = \"!&grade_level\" caption=\"!&grade_caption\" term=\"!&term\"/>\r\n";
			if ( it->second.IsEmpty()) continue;
			char * pval = it->second.GetBuffer(it->second.GetLength()+8);
			while (pval)
			{
				CStringA stmp = grade_tmplt;
				int ngrade = atoi(pval);
				
				if (ngrade <= 0 || ngrade > 13)
				{
					Logs(_T("\r\n--->不可解析的\"grade_level\": %d [1-13]!!\r\n"), ngrade);
					it->second.ReleaseBuffer();
					return CVT_ERR_SECTION_GRADE;
				}
				if (ngrade == 11) strcat(pval, ";12;13");
				LPCWSTR sgrades_tmp[] = { { _T("学前") }, { _T("一年级") }, { _T("二年级") }, { _T("三年级") }, { _T("四年级") },
				{ _T("五年级") }, { _T("六年级") }, { _T("初一") }, { _T("初二") },
				{ _T("初三") }, { _T("高一") }, { _T("高二") }, { _T("高三") } };
				ngrade--;
				CStringA sv;sv.Format("%02d", ngrade);
				stmp.Replace("!&grade_level", sv);
				CStringA sc = qUnc2Utf(sgrades_tmp[ngrade]);
				stmp.Replace("!&grade_caption", sc);

				grades += stmp;
				pval = strchr(pval, ';');
				if (pval) pval++;
			}
			it->second.ReleaseBuffer();
			continue;
		}
		
		CStringA skey = ("!&") + it->first;
		stemplate.Replace((LPCSTR)skey, ConvtXmlChars(it->second));
	}
	
	{
		CStringA scvr = scover;
		Logs(_T(" 封面..."));

		AddCover(scover, scvr, sfiles);
	}
	grades.Replace("!&term", asterm);
	stemplate.Replace("!&cover",scover);
	stemplate.Replace("!&grades", grades);
	stemplate.Replace("!&subject_caption", assubject);

	int ret = Addfile(stemplate, sfiles);
	if (ret < 0) return ret;

	CStringA  sdir;  //save directory information
	SaveDirs(sdir);
	stemplate.Replace(("!&dirs"), sdir);
	AddMetaInfo(stemplate);
	CleanXmlUnknown(stemplate);

	CString sf_xml = CFG_META_FILE;
	if (of.Open(CFG_META_FILE, CFile::modeCreate | CFile::modeWrite) == FALSE) return CVT_ERR_CREATE_META_FILE;
	of.Write(stemplate, stemplate.GetLength());
	of.Close();
	ZipAdd(m_hz, CFG_META_FILE, sf_xml.GetBuffer(), 0, ZIP_FILENAME);
	sf_xml.ReleaseBuffer();
	return 0;
}

int CCovtMainDlg::ParseXmlField(CString &sbook)
{
	INT ret = S_OK;
	XML_Parser parser = XML_ParserCreate(NULL);
	m_ptrUserData = parser;
	CFile of;
	if (of.Open(sbook, CFile::modeRead | CFile::shareDenyNone) == FALSE)
		return CVT_ERR_OPEN_XML;
	char buf[BUFSIZ];
	
	int done;
	XML_SetUserData(parser, this);
	XML_SetElementHandler(parser, ParseXmlRoot, endXmlRoot);
	XML_SetCharacterDataHandler(parser, parsedata);
	do {
		int len = of.Read(buf, BUFSIZ);
		done = len < sizeof(buf);
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
			m_strCvtError.Format(_T("%S at line %u"),
				XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser) );
			Logs(_T("\r\n-->Xml Error: %s\r\n"), m_strCvtError);
			ret = CVT_ERR_PARSE_XML;
			break;
		}
	} while (!done);
	of.Close();
	XML_ParserFree(parser);

	return ret;
}


CDelegate::CDelegate(CWnd * pmsgWnd)
	: Thread()
{
	m_pMsgWnd = pmsgWnd;
}

void * CDelegate::run(void * pobj )
{
	int iret = 0;
	CCovtMainDlg * pdlg = (CCovtMainDlg*)pobj;
	if (pdlg->m_pfnAsynRun)
		iret = (pdlg->*(pdlg->m_pfnAsynRun))(0);

	if (pdlg->m_pfnDone)
		(pdlg->*(pdlg->m_pfnDone))(iret);
	else
		pdlg->PostMessage(WM_BURN_DONE, (WPARAM)iret, 0);
	return 0;
}

// CCovtMainDlg dialog

IMPLEMENT_DYNAMIC(CCovtMainDlg, CExDialog)


CCovtMainDlg::CCovtMainDlg(CWnd* pParent /*=NULL*/)
	: CExDialog(CCovtMainDlg::IDD, pParent)
	, m_nMin(1)
	, m_nMax(20)
	, m_strSrcDir(_T(""))
	, m_strDstDir(_T(""))
{
	m_pDeleGate = NULL;
	m_pProjDir = NULL;
	m_pfnDone = m_pfnAsynRun = NULL;
	m_hDirCur = NULL;
	for (int i = 0; i<sizeof(m_cFolderBtns) / sizeof(CGdipButton*); i++)
	{
		m_cFolderBtns[i] = NULL;
	}
}

CCovtMainDlg::~CCovtMainDlg()
{
}

void CCovtMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CExDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROXY_PORT, m_nMax);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nMin);
	DDX_Text(pDX, IDC_EDIT_SRC_DIR, m_strSrcDir);
	DDX_Text(pDX, IDC_EDIT_DST_DIR, m_strDstDir);
	DDX_Control(pDX, IDC_EDIT_LOG, m_oLog);
	DDX_Control(pDX, IDC_PROGRESS, m_oProg);
	DDX_Control(pDX, IDC_ST_INFO, m_oInf);
}


BEGIN_MESSAGE_MAP(CCovtMainDlg, CExDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &CCovtMainDlg::OnBnClickedBtnOk)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_CANCLE, &CCovtMainDlg::OnBnClickedBtnOk2)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SRC_DIR, &CCovtMainDlg::OnBnClickedBtnSrcDir)
	ON_BN_CLICKED(IDC_BTN_DST_DIR, &CCovtMainDlg::OnBnClickedBtnDstDir)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CVT_ERRS, &CCovtMainDlg::OnBnClickedBtnCvtErrs)
	ON_EN_CHANGE(IDC_EDIT_SRC_DIR, &CCovtMainDlg::OnEnChangeEditSrcDir)
END_MESSAGE_MAP()


// CCovtMainDlg message handlers
static const INT btnids[] = { IDC_BTN_OK, IDC_BTN_CVT_ERRS };
static const int btnbtmp[] = { IDB_BITMAP_SLIVE_BTN, IDB_BITMAP_RND_BTN};
static const SIZE szBtn[] = { { 102, 26 }, { 91, 20 } };

#define CFG_COVER_FOLDER  _T("covers")

INT FindCovers(CPtrArray & pCvrs)
{
	TCHAR scdir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, scdir);
	CString spath = g_pSet->strCurPath + CFG_COVER_FOLDER;
	SetCurrentDirectory(spath);
	
	int i = 0;
	CStringArray * sfs = new CStringArray();
	pCvrs.Add(sfs);
	for (i = 1; i < 10; i++)
	{
		CString str;
		str.Format(_T("%d*.*"), i);
		CStringArray * sfs = new CStringArray();
		FindFile(str, *sfs);
		for (int k = 0; k < sfs->GetCount(); k++)
		{
			CString sext;			
			CUtil::GetFileExt(sfs->GetAt(k), sext);
			sext = sext.MakeLower();
			if (sext.Compare(_T("jpg")) != 0 && sext.Compare(_T("png")) != 0 && sext.Compare(_T("bmp")) != 0)
			{
				sfs->RemoveAt(k);
				k--;
			}
		}
		if (sfs->GetCount() == 0)
		{
			delete sfs;
			break;
		}
		pCvrs.Add(sfs);
	}
	
	SetCurrentDirectory(scdir);
	return TRUE;
}

BOOL CCovtMainDlg::OnInitDialog()
{
	m_bTransprent = TRUE;
	CExDialog::OnInitDialog();


	int txtids[] = { IDC_ST_PROXY_IP, IDC_ST_PROXY_PORT, IDC_ST_PROXY_USER, IDC_ST_PROXY_PWD, IDC_ST_PROXY_PWD2,\
		IDC_TXT_FROMTO, IDC_TEXT_LIMITE, IDC_TEXT_LIMITE2, IDC_TEXT_MB, IDC_TEXT_MB2, IDC_TEXT_PDFSWF_PARAM, IDC_ST_PROXY_PWD3 };
	SubTextItems(txtids, sizeof(txtids) / sizeof(int), NULL, NULL);

	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		m_pbtns[i] = new CSkinBtn();
		m_pbtns[i]->SubclassDlgItem(btnids[i], this);
		m_pbtns[i]->SetImage(btnbtmp[i], szBtn[i].cx, szBtn[i].cy);
	}
	m_pProjDir = new CTreeCtrl();
	m_pProjDir->SubclassDlgItem(IDC_TREE_DIR, this);
	m_strSrcDir = g_pSet->m_strSrcDir;
	m_strDstDir = g_pSet->m_strDstDir;
	UpdateData(0); 

	int txtBlake[] = { IDC_TEXT_LIMITE, IDC_TEXT_LIMITE2, IDC_TEXT_MB, IDC_TEXT_MB2,IDC_TEXT_PDFSWF_PARAM,
		IDC_TXT_FROMTO, IDC_ST_PROXY_PORT, IDC_ST_PROXY_PWD2 };
	LOGFONT lf = { 0 };
	lf.lfHeight = 12;
	lf.lfWeight = FW_THIN;
	_tcscpy_s(lf.lfFaceName, _T("宋体"));
	
	VERIFY(m_ftEdit.CreateFontIndirect(&lf));

	for (int i = 0; i < sizeof(txtBlake) / sizeof(int); i++)
	{
		CLink * pctrl = (CLink*)GetDlgItem(txtBlake[i]);
		pctrl->SetFontStyle(&m_ftEdit, RGB(0, 0, 0));
	}

	CString strlmt;
	strlmt.Format(_T("%d"), g_pSet->m_nLimitPrevSize/(1 MByte));
	CWnd * pwnd = GetDlgItem(IDC_EDIT_LIMIT_SIZE);
	pwnd->SetWindowText(strlmt);

	strlmt.Format(_T("%d"), g_pSet->m_nFlvLimit / (1 MByte));
	pwnd = GetDlgItem(IDC_EDIT_FLV_LIMIT);
	pwnd->SetWindowText(strlmt);

	const INT btnid[] = { IDC_BTN_SRC_DIR, IDC_BTN_DST_DIR };
	for (int i = 0; i<sizeof(btnid) / sizeof(int); i++)
	{
		m_cFolderBtns[i] = new CGdipButton;
		m_cFolderBtns[i]->SubclassDlgItem(btnid[i], this);
		m_cFolderBtns[i]->LoadStdImage(IDB_PNG_FOLDER_16, _T("PNG"));
	}

	CString stip(_T("浏览选择转换源文件夹"));
	m_cFolderBtns[0]->SetToolTipText(stip);
	stip = (_T("浏览选择转换目标文件夹"));
	m_cFolderBtns[1]->SetToolTipText(stip);


	m_EnEncrypt.SubclassDlgItem(IDC_CHK_ENCRPT, this);
	m_EnEncrypt.SetCheck(FALSE);
	
	NewProcessWnd(100, 0);
	CEdit * pedit = (CEdit *)GetDlgItem(IDC_EDIT_SRC_DIR);
	pedit->SetCueBanner(_T("转换源文件夹"));

	pedit = (CEdit *)GetDlgItem(IDC_EDIT_DST_DIR);
	pedit->SetCueBanner(_T("转换到目标文件夹"));

	m_pbtns[0]->SetWindowText(_T("转    换"));
	m_oInf.ShowWindow(SW_HIDE);


	pwnd = GetDlgItem(IDC_EDIT_PDF2SWF_PARAM);
	pwnd->SetWindowText(g_pSet->m_strPdf2SwfParm);

	FindCovers(m_fCovers);
	OnEnChangeEditSrcDir();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCovtMainDlg::OnDestroy()
{
	CExDialog::OnDestroy();
	for (int i = 0; i < sizeof(btnids) / sizeof(int); i++)
	{
		delete m_pbtns[i];
	}
	if (m_pDeleGate) delete m_pDeleGate;
	m_pDeleGate = NULL;
	if (m_pProjDir) delete m_pProjDir;
	m_pProjDir = NULL;

	for (int i = 0; i<sizeof(m_cFolderBtns) / sizeof(CGdipButton*); i++)
	{
		if (m_cFolderBtns[i])
			delete m_cFolderBtns[i] ;
		m_cFolderBtns[i] = NULL;
	}

	for (int i = 0; i < m_fCovers.GetCount(); i++)
	{
		CStringArray * ptr = (CStringArray *)m_fCovers[i];;
		delete ptr;
	}
	m_fCovers.RemoveAll();
	FreeProcessWnd();
}

LPCTSTR err_text[] = { CVT_ERR_STRINGS };

LPCTSTR GetErrorString(int v)
{
	if (v == 0) return _T("成功！");
	else if (v<CVT_ERR && v> CVT_ERR_BASE)
		return err_text[v - CVT_ERR_BASE - 1];
	else return _T("未知错误！");
}

int CCovtMainDlg::CheckLastTaskBroken( )
{
	CString str;
	CStdioFile   of;
	for (int f = 0; f < sizeof(stRetLogFName) / sizeof(LPCTSTR); f++)
	{
		CString strF = m_strSrcDir + stRetLogFName[f];
		if (of.Open(strF, CFile::modeRead | CFile::shareDenyNone) == FALSE) continue;
		while (of.ReadString(str))
		{
			if (str.IsEmpty()) continue;

			int p = str.Find(_T("] \""));
			if (p < 0) continue;
			str.Delete(0, p + 3);
			p = str.Find(_T('\"'));
			if (p < 0) continue;
			str.Delete(p, str.GetLength());

			for (int i = 0; i < m_strSrcPacks.GetCount(); i++)
			{
				if (str.Compare(m_strSrcPacks[i]) == 0)
				{
					Logs(_T("    略过:\"%s\" 已转换。\r\n"), str);
					m_strSrcPacks.RemoveAt(i);
					break;
				}
			}
			str.Empty();
		}
		of.Close();
	}
	return 1;
}

void MarkTaskDone(CString &stasklog_f, CString &stask)
{
	CStdioFile  of;
	if (of.Open(stasklog_f, CFile::modeWrite) == FALSE)
		if (of.Open(stasklog_f, CFile::modeCreate|CFile::modeWrite) == FALSE)
			return;
	of.SeekToEnd();
	of.WriteString(stask);
	of.WriteString(_T("\n"));
	of.Close();
}

void CCovtMainDlg::LogToFile(int result, LPCTSTR spckFile)
{
	CString sflog = m_strSrcDir;
	sflog += result<0 ? stRetLogFName[1] : stRetLogFName[0];
	CTime tm = CTime::GetCurrentTime();
	CString stmlog = tm.Format(TIME_FMT);
	CStdioFile of;
	if (!of.Open(sflog, CFile::modeReadWrite))
		if (!of.Open(sflog, CFile::modeCreate | CFile::modeWrite))
			return;
	of.SeekToEnd();
	CString str;
	CStringA sa;
	if (result < 0)
		str.Format(_T("[%s] \"%s\" 转换失败，错误: %s"), stmlog, spckFile, GetErrorString(result) );
	else
		str.Format(_T("[%s] \"%s\" 转换成功"), stmlog, spckFile);
	m_strCvtError.Remove(_T('\n'));
	m_strCvtError.Remove(_T('\r'));
	if (!m_strCvtError.IsEmpty())
		str += _T("(")+m_strCvtError + _T(").\n");
	else
		str += _T(".\n");
	QW2A(str, sa);
	of.Write(sa, sa.GetLength());
	of.Close();
	m_strCvtError.Empty();
}

int CCovtMainDlg::AsyncConvertDir(int param)
{
	CString strTmpRoot = g_pSet->strCurPath + _T("_tmp\\");
	AddLog(_T("开始转换：\r\n"));

	CreateDirectory(strTmpRoot, NULL);
	ListZips(m_strSrcDir,  m_strSrcPacks);
	CheckLastTaskBroken();
	CTime dts = CTime::GetCurrentTime();
	SetCurProgPos(0, _T("正在转换..."));
	SetProgWndLimit(m_strSrcPacks.GetSize(), 0);
	CArray<int, int> m_aRsut;
	m_aRsut.SetSize(m_strSrcPacks.GetSize());
	for (int i = 0; i < m_strSrcPacks.GetSize(); i++)
	{
		m_strCvtError.Empty();

		SetCurProgPos(i, NULL);
		Logs(_T("转换: %s\r\n"), m_strSrcPacks[i]);
		CString sf = CUtil::GetFileName(m_strSrcPacks[i]);
		int p = sf.ReverseFind(_T('.'));
		if (p)  sf.Delete(p, sf.GetLength() - p);
		m_strTmpDir = strTmpRoot + sf;
		DelTree(m_strTmpDir);
		CreateDirectory(m_strTmpDir, NULL);
		::SetCurrentDirectory(m_strTmpDir);

		m_aRsut[i] = ConvertBook(m_strSrcPacks[i]);
		Logs(_T("\r\n    清理临时文件..."));

		Logs(_T("\r\n完成: %s"), GetErrorString(m_aRsut[i]));
		LogToFile(m_aRsut[i], m_strSrcPacks[i]);
		::SetCurrentDirectory(g_pSet->strCurPath);
		DelTree(m_strTmpDir);
		Logs(_T("\r\n"));
	}
	

	CString sol;
	AddLog(_T("\r\n-------------------------------\r\n\r\n"));

	CTime dte = CTime::GetCurrentTime();
	CTimeSpan ts = dte - dts;
	DWORD dsec = ts.GetTotalSeconds();
	sol.Format(_T("结束时间：%s, 总耗时：%d分钟%d秒"), dte.Format(TIME_FMT), dsec / 60, dsec % 60);

	sol  += _T("\r\n    源文件夹：") + m_strSrcDir;
	sol += _T("\r\n    目标文件夹：") + m_strDstDir + _T("\r\n    --------------\r\n");
	for (int i = 0; i < m_aRsut.GetSize() ; i++)
	{
		if (m_aRsut[i] < 0)
		{
			CString str;
			str.Format(_T("    \"%s\" 转换错误: %s\r\n"), m_strSrcPacks[i], GetErrorString(m_aRsut[i]));
			sol += str;
		}
	}
	AddLog(sol);
	AddLog(_T("\r\n-------------------------------\r\n"));
	return 0;
}

int AddTaskToDb(CString &szip)
{
	char sql[64 * 1024];
	CTime tm(CTime::GetCurrentTime());
	CString strtm = tm.Format(TIME_FMT);
	CString suid = CUtil::GenGuidString();
	SQL_DB_BOOK  m_db_items;
	char pbuf [255 * 8];
	int ip = 0, is = 255, im = 64;
	m_db_items.BookId = (char *)&pbuf[ip];
	m_db_items.BookName = (char *)&pbuf[ip += 64];
	m_db_items.BookPath = (char *)&pbuf[ip += 256];
	m_db_items.tmCreate = (char *)&pbuf[ip += 256];
	m_db_items.tmModify = (char *)&pbuf[ip += 32];
	m_db_items.tmUpload = (char *)&pbuf[ip += 32];
	m_db_items.BookState = (char *)&pbuf[ip += 32];
	m_db_items.author = (char *)&pbuf[ip += 32];
	m_db_items.description = (char *)&pbuf[ip += 64];


	Unc2Utf(g_pSet->m_strUserName, m_db_items.author, -1, 255);
	Unc2Utf(szip, m_db_items.BookPath, -1, 255);
	Unc2Utf(suid, m_db_items.BookId, -1, 64);
	Unc2Utf(strtm, m_db_items.tmCreate, -1, 31);
	Unc2Utf(strtm, m_db_items.tmModify, -1, 31);
	Unc2Utf(szip, m_db_items.BookName, -1, 255);
	Unc2Utf(L"未上传", m_db_items.BookState, -1, 16);
	DWORD flen = 0;
	if (CUtil::GetFileSize(szip, flen) == FALSE)
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



int CCovtMainDlg::ConvertBook(CString &sbook)
{
	CStringArray sfiles;
	m_strPrevw.Empty();

	Logs(_T("    解包..."));
	int ret = CVT_ERR_UNZIP;
	DWORD flen, flenh;
	if (!CUtil::GetFileSize(sbook, flen, &flenh))
		return ret;
	if (flenh > 0)
		return CVT_ERR_HUGE_ZIP;
	if ((ret = UnzipLimitFile(sbook, &sfiles, (m_nFlvLimit), _T("flv"))) < 0)
		return ret;
	
	CStringArray sxmls, sflvs;
	if (FindFile(_T("*.xml"), sxmls) == 0)
		return CVT_ERR_NO_XML;

	CString sfxml = CFG_ORIG_PREF;
	MoveFile(sxmls[0], sfxml);
	FindSpecFile(sfiles, sxmls[0], true);

	if (FindFile(_T("*.flv"), sflvs))
	{
		MoveFile(sflvs[0], CFG_PREVW_FLV);
		FindSpecFile(sfiles, sflvs[0], true);
		m_strPrevw = CFG_PREVW_FLV;
		//sfiles.Add(CFG_PREVW_FLV);
	}

	CString strZip = m_strDstDir + _T("\\") + CUtil::GetFileName(sbook);
	TCHAR * pzip = strZip.GetBuffer(strZip.GetLength());
	m_hz = CreateZip(pzip, 0, ZIP_FILENAME);
	m_pProjDir->DeleteAllItems();
	m_mapMultiVal.clear();
	m_mapMetaVal.clear();

	ret = S_OK;
	Logs(_T("done\r\n    解析XML..."));
	ret = ParseXmlField(sfxml);
	Logs(_T("done\r\n    解析元数据..."));
	if (ret ==S_OK && ( (ret =ParseXmlMeta(sfiles)) >= 0) )
	{
		if(sflvs.GetCount()!=0)
		{
			Logs(_T("\r\n    打包preview.flv..."));
			CString sprev = CFG_PREVW_FLV;
			ZipAdd(m_hz, sprev, sprev.GetBuffer(), 0, ZIP_FILENAME);
			sprev.ReleaseBuffer();		
		}
		ZipAdd(m_hz, sfxml, sfxml.GetBuffer(), 0, ZIP_FILENAME);
		sfxml.ReleaseBuffer();
		CloseZip(m_hz);
		//AddTaskToDb(strZip);
		m_hz = NULL;
	}
	if (m_hz != NULL)
	{
		CloseZip(m_hz);
		DeleteFile(strZip);
	}
	return ret;
}

void CCovtMainDlg::OnBnClickedBtnOk()
{
	CString str;
	m_pbtns[0]->GetWindowText(str);
	if (str.Compare(_T("转    换")) == 0)
	{
		m_pfnAsynRun = &CCovtMainDlg::AsyncConvertDir;
		m_pfnDone = &CCovtMainDlg::OnConvertDone;
		UpdateData();
		m_bEnEncrypt = m_EnEncrypt.GetCheck();
		g_pSet->m_strSrcDir = this->m_strSrcDir;
		g_pSet->m_strDstDir = m_strDstDir;
		GetDlgItemText(IDC_EDIT_LIMIT_SIZE, str);
		m_nLimitPrevSize = _ttoi(str) MByte;
		if (m_nLimitPrevSize == 0) m_nLimitPrevSize = 1 MByte;
		g_pSet->m_nLimitPrevSize = m_nLimitPrevSize;

		GetDlgItemText(IDC_EDIT_FLV_LIMIT, str);
		m_nFlvLimit = _ttoi(str) MByte;
		if (m_nFlvLimit == 0) m_nFlvLimit = 1 MByte;
		g_pSet->m_nFlvLimit = m_nFlvLimit;


		GetDlgItemText(IDC_EDIT_PDF2SWF_PARAM, g_pSet->m_strPdf2SwfParm);
		g_pSet->Save();
		m_slog.Empty();
		m_oLog.SetWindowText(_T(""));
		m_oLog.ShowWindow(SW_SHOW);
		if (m_pDeleGate == NULL)
			m_pDeleGate = new CDelegate(this);
		m_pDeleGate->start(this);
		m_pbtns[0]->SetWindowText(_T("中止转换"));
		
		ProcMsg();
	}
	else
	{
		if (MessageBox(_T("是否要中止当前的转换？"), _T("中止转换"), MB_YESNO) == IDYES)
			SetTimer(0x111, 50, NULL)	;
	}
}


int CCovtMainDlg::OnConvertDone(int param)
{
	SetTimer(0x111,50, NULL);
	return 0;
}


void CCovtMainDlg::OnOK()
{
}

void CCovtMainDlg::ProcMsg()
{
	MSG msg;
	m_bCancel = FALSE;
	while (m_bCancel == FALSE && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

void CCovtMainDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	CExDialog::OnPaint();
}


void CCovtMainDlg::OnBnClickedBtnOk2()
{
	this->OnOK();
}


void CCovtMainDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CExDialog::OnShowWindow(bShow, nStatus);

	// TODO:  在此处添加消息处理程序代码
}


void CCovtMainDlg::OnBnClickedBtnSrcDir()
{
	if (PickupFolder(m_strSrcDir, this, _T("选择源文件夹"), m_strSrcDir))
	{
		UpdateData(FALSE);
	}
	
}


void CCovtMainDlg::OnBnClickedBtnDstDir()
{
	// TODO:  在此添加控件通知处理程序代码
	if (PickupFolder(m_strDstDir, this, _T("选择目标文件夹"), m_strDstDir))
	{
		UpdateData(FALSE);
	}
}

void InvalidateMainRect();
void CCovtMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0x111)
	{
		KillTimer(0x111);
		m_bCancel = TRUE;
		EndProgWnd();
		if (m_pDeleGate)
		{
			m_pDeleGate->stop();
			delete m_pDeleGate;
			m_pDeleGate = NULL;
		}
		m_pbtns[0]->SetWindowText(_T("转    换"));
		AddLog(_T("\r\n转换结束！"));
		//m_oLog.ShowWindow(SW_HIDE);
		m_oInf.ShowWindow(SW_HIDE);
		m_oProg.ShowWindow(SW_HIDE);
		InvalidateMainRect();
	}

	CExDialog::OnTimer(nIDEvent);
}

void CCovtMainDlg::SetCurProgPos(int pos, LPCTSTR sinf)
{
	if( sinf )
	{
		m_oInf.SetWindowText(sinf);
		m_oInf.ShowWindow(SW_SHOW);
	}
	m_oProg.SetPos(pos);
}
void CCovtMainDlg::SetProgWndLimit(int max, int min)
{
	m_oProg.SetRange32(min, max);
	if (min != -1 )
		m_oProg.SetPos(min);
	m_oProg.ShowWindow(SW_SHOW);
}

void CCovtMainDlg::SetProgInfo(LPCSTR strInfo)
{

}
void CCovtMainDlg::AddLog(LPCTSTR slog)
{
	m_slog += slog;
	m_oLog.SetWindowText(m_slog);
	m_oLog.LineScroll(m_oLog.GetLineCount(), 0);
}

void CCovtMainDlg::Logs(LPCTSTR fmt, ...)
{
	if (1)//g_benDebug)
	{
		TCHAR buf[4096], *p = buf;
		va_list args;
		va_start(args, fmt);
		p += _vsntprintf_s(p, 4096, sizeof buf - 1, fmt, args);
		va_end(args);
		AddLog(buf);
	}
}

void CCovtMainDlg::OnBnClickedBtnCvtErrs()
{
	CString str;
	GetDlgItemText(IDC_EDIT_SRC_DIR, str);
	str += stRetLogFName[1];
	DeleteFile(str);
	GetDlgItem(IDC_BTN_CVT_ERRS)->EnableWindow(FALSE);
}


void CCovtMainDlg::OnEnChangeEditSrcDir()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CExDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	GetDlgItemText(IDC_EDIT_SRC_DIR, str);
	str += stRetLogFName[1];
	if (CUtil::GetFileSize(str) != 0)
	{
		GetDlgItem(IDC_BTN_CVT_ERRS)->EnableWindow(TRUE);
	}
	else
		GetDlgItem(IDC_BTN_CVT_ERRS)->EnableWindow(FALSE);
		
}
