#include "md5.h"
#include "sha256.h"
#include "XZip\XZip.h"
#include "XZip\XUnzip.h"

#include "deelx.h"

//º”√‹, Ω‚√‹
BOOL decode_mem(LPVOID penc_buf, LPCTSTR outpath, INT  flen);
int decode_file(LPCTSTR inpath, LPCTSTR outpath);
int enc_file(LPCTSTR inpath, LPCTSTR outpath);

LPVOID  encode_file(LPCTSTR inPath, int & outLen);
void free_enc(LPVOID penc);


BOOL SetCurProgPos(int cur, LPCTSTR szinfo = NULL);
void SetProgWndLimit(int max, int min = -1);
void EndProgWnd();
BOOL OffsetPorgPos(int offset, LPCTSTR szinfo = NULL);