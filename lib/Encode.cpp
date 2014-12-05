#include "stdafx.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

char  HEADERBYTE[32] = { 0x1, 0x2, 0x98, 0x68, 0x32, 0xd9, 0x6f, 0x9f, 0x1d, 0x2a, 0x9e, 0x6d, 0x3c, 0xd9, 0x6a, 0x9e,
	0xed, 0xaa, 0x5e, 0x7d, 0x3c, 0xd3, 0x6a, 0x8e, 0x1d, 0x1a, 0x3e, 0x3d, 0x6c, 0xd6, 0x9a, 0x8a};

const unsigned long DATA_LEN = 1024*1000;

int en_first_1024(char *pSrc, unsigned long src_len)
{
	char tmp = 0;
	unsigned long i = 0;

	if(src_len < 1024){
		return src_len;
	}

	for (i = 0; i < 1024; )
	{
		tmp = pSrc[i];
		pSrc[i] = pSrc[i + 2];
		pSrc[i + 2] = tmp;

		tmp = pSrc[i + 1];
		pSrc[i + 1] = pSrc[i + 3];
		pSrc[i + 3] = tmp;

		i += 4;
	}

	return src_len;
}

int dec_first_1024(char *pSrc, unsigned long src_len)
{
	char tmp = 0;
	unsigned long i = 0;

	if(src_len < 1024){
		return src_len;
	}

	for (i = 0; i < 1024; )
	{
		tmp = pSrc[i];
		pSrc[i] = pSrc[i + 2];
		pSrc[i + 2] = tmp;

		tmp = pSrc[i + 1];
		pSrc[i + 1] = pSrc[i + 3];
		pSrc[i + 3] = tmp;

		i += 4;
	}

	return src_len;
}


int en_data(char *pSrc, unsigned long src_len)
{
	char tmp = 0;
	unsigned long i = 0;
unsigned long handle_len = src_len;

if (src_len < 1024){
	return src_len;
}

handle_len &= ~(1);

for (i = 0; i < handle_len; i++)
{
	tmp = pSrc[i];
	pSrc[i] = pSrc[i + 1];
	pSrc[i + 1] = tmp;
	i++;
}

return src_len;
}


int dec_data(char *pSrc, unsigned long src_len)
{
	char tmp = 0;
	unsigned long i = 0;
	unsigned long handle_len = src_len;

	if (src_len < 1024){
		return src_len;
	}


	if (handle_len % 2 != 0)
		handle_len--;


	for (i = 0; i < handle_len; i++)
	{
		tmp = pSrc[i];
		pSrc[i] = pSrc[i + 1];
		pSrc[i + 1] = tmp;
		i++;
	}

	return src_len;
}

// <0: error  >=0: ok
LPVOID  encode_file(LPCTSTR inPath, int & outLen)
{
	char * ptr = NULL;
	char * fbuf;
	CFile ofin;
	UINT32 flen = 0;
	if (ofin.Open(inPath, CFile::modeRead | CFile::shareDenyNone))
	{
		flen = ofin.GetLength();
		ptr = new char[flen + sizeof(HEADERBYTE)];
		fbuf = ptr + sizeof(HEADERBYTE);
		if (ptr)
		{
			memcpy(ptr, HEADERBYTE, sizeof(HEADERBYTE));
			ofin.Read(fbuf, flen);
		}
		ofin.Close();
	}
	if (ptr)
	{
		if (flen < 1024) return ptr;// do not encode when buffer length less than 1024
		int enlen = flen - 1024;
		en_data(fbuf, 1024);
		en_first_1024(fbuf, 1024);

		en_data(fbuf + 1024, flen - 1024);
		outLen = flen + sizeof(HEADERBYTE);
	}
	return ptr;
}

void free_enc(LPVOID penc)
{
	if (penc && memcmp(penc, HEADERBYTE, 8) == 0)
		delete penc;
}

int enc_file(LPCTSTR inpath, LPCTSTR outpath)
{
	int buflen;
	BYTE * ptr = (BYTE*)encode_file(inpath, buflen);
	CFile ofout;
	if (ofout.Open(outpath, CFile::modeCreate | CFile::modeReadWrite))
	{
		ofout.Write(ptr, buflen);
		ofout.Close();
	}
	delete ptr;
	return buflen;
}

BOOL decode_mem(LPVOID penc_buf, LPCTSTR outpath, INT  flen)
{
	char * ptr = (char *)penc_buf;
	char * fbuf;


	fbuf = ptr + sizeof(HEADERBYTE);
	if (memcmp(ptr, HEADERBYTE, sizeof(HEADERBYTE)) != 0)
	{
		return FALSE;
	}
	flen = flen - sizeof(HEADERBYTE);

	if (flen > 1024)
		dec_first_1024(fbuf, 1024);
	dec_data(fbuf, flen);

	CFile ofout;
	if (ofout.Open(outpath, CFile::modeCreate | CFile::modeReadWrite))
	{
		ofout.Write(fbuf, flen);
		ofout.Close();
	}
	return TRUE;
}

int decode_file(LPCTSTR inpath, LPCTSTR outpath)
{
	char * ptr = 0;
	char * fbuf;
	CFile ofin;
	UINT32 flen = 0;
	if (ofin.Open(inpath, CFile::modeRead | CFile::shareDenyNone))
	{
		flen = ofin.GetLength();
		ptr = new char[flen];
		fbuf = ptr + sizeof(HEADERBYTE);
		ofin.Read(ptr, sizeof(HEADERBYTE));
		if (memcmp(ptr, HEADERBYTE, sizeof(HEADERBYTE)) != 0)
		{
			delete ptr;
			ofin.Close();
			return 0;
		}
		flen = flen - sizeof(HEADERBYTE);
		ofin.Read(ptr, flen );
		if ( flen > 1024 )
			dec_first_1024(ptr, 1024);
		dec_data(ptr, flen);
		ofin.Close();

		CFile ofout;
		if (ofout.Open(outpath, CFile::modeCreate | CFile::modeReadWrite))
		{
			ofout.Write(ptr, flen);
			ofout.Close();
		}
		delete ptr;
	}
	return flen;
}