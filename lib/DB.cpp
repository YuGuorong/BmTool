#include "stdafx.h"
#include "lib/sqlite3/sqlite3.h"
#include "lib\tables.h"

//#pragma comment(lib, ".\\lib\\sqlite3\\sqlite3.lib" ) 

char g_DbName[MAX_PATH];
sqlite3 * g_DbApp = 0; 


#define CHAR_INTEGER "INTEGER" 
#define CHAR_VARCHAR "VARCHAR"
#define CHAR_PRIMKEY "INTEGER PRIMARY KEY"
#define CHAR_NOTNULL " NOT NULL"
#define CHAR_UNIQUE  " UNIQUE"


static const char g_primkey[] = { CHAR_PRIMKEY };
static const char g_integer[] = { CHAR_INTEGER };
static const char g_varchar[] = { CHAR_VARCHAR };
static const char g_unqestr[] = { CHAR_VARCHAR CHAR_NOTNULL CHAR_UNIQUE };
static const char g_keychar[] = { CHAR_VARCHAR CHAR_NOTNULL };

const SQL_TABLE_COL book_col[] =
{
	{ "Id",       g_primkey, e_key, _T("唯一") },
	{ "BookId",   g_unqestr, e_str, _T("BookID") },
	{ "BookName", g_keychar, e_str, _T("Book名称") },
	{ "ZipPath",  g_keychar, e_str, _T("Book位置") },
	{ "ZipSize",  g_integer, e_int, _T("Book大小") },
	{ "tmCreate", g_varchar, e_str, _T("创建时间") },
	{ "tmModify", g_varchar, e_str, _T("修改时间") },
	{ "tmUpload", g_varchar, e_str, _T("最后上传时间") },
	{ "BookState", g_keychar, e_str, _T("上传状态") },//未上传，已上传，重传，正上传， 删除
	{ "author", g_varchar, e_str, _T("上传者") },
	{ "description", g_varchar, e_str, _T("备注") }
};

const int tblBookColSize[] = { 0, 64, 256, 256, 0, 32, 32, 32, 16, 256, 256 };
int SQL_INS_book(char * sql, int len, const void * pval, const char * pCondition)
{
	if (Sql_Write_DB(SQL_TBL_BOOK_LIST, sql, len, pval, pCondition) == 0)
	{
		int ret = execSQL(sql, 0, 0, NULL);
		return ret;
	}
	return -1;

}

const SQL_TABLE tbl_books =
{
	SQL_TBL_BOOK_LIST,
	"books",
	sizeof(book_col) / sizeof(SQL_TABLE_COL),
	book_col,
	SQL_INS_book
};

const SQL_TABLE * g_sqlTbls[] = {
	&tbl_books,
};


#if  1
#include <stdio.h>

BOOL g_benDebug = 0;
void InitDebugFun()
{
   char path[1024];
   size_t path_len = 0;

   if(getenv_s(&path_len,path,1024,"EXP_DEBUG_SQL") == 0
       && path_len )
   {
        if( path && path[0] != '\0' )
            g_benDebug = 1;
   }

}

void MyTracex(const char * fmt, ...) 
{
    if( 1)//g_benDebug)
    {
        char buf[4096], *p = buf;
        va_list args;
        va_start(args, fmt);
        p += _vsnprintf_s(p, 4096, sizeof buf - 1, fmt, args);
        va_end(args);
        USES_CONVERSION;
        OutputDebugString( A2W(buf));
    }
}
#endif 


int CreateTable(sqlite3* db, const SQL_TABLE * ptbl)
{
    ASSERT(ptbl);
    
    int bflen = STR_LEN_BUFF;
    int sz_len;
    char sql[STR_LEN_BUFF];
    sprintf_s(sql,bflen, "CREATE TABLE IF NOT EXISTS %s (", ptbl->table);
    sz_len = (int)strlen(sql);
    char * psql = sql + sz_len;
    bflen -= sz_len;

    for(int i=0; i<ptbl->cols;i++)
    {
        sprintf_s(psql,bflen, "%s %s",ptbl->pCol[i].name,ptbl->pCol[i].type);
        sz_len = (int)strlen(psql);
        psql += sz_len;
        bflen -= sz_len;

        strcat_s(psql,bflen, ", ");
        psql += 2;
        bflen -= 2;
    }

    psql -= 2;
    bflen += 2;
    strcpy_s(psql,bflen, ")");

    execSQL(sql);
    MyTracex("Table ");
    MyTracex(("%s"), (sql));
    MyTracex("\n");
    return 0;

}
 

int execSQL(const char* sql)
{
    if( g_DbApp )
    {
        char* zErrMsg = 0;
        int ret = sqlite3_exec(g_DbApp,sql,0,0,&zErrMsg);
		if (ret != SQLITE_OK)
		{
			MyTracex("exec SQL(%s) error\n%s\n", (sql), zErrMsg);
			sqlite3_free(zErrMsg);
		}
        return ret;
    }
    return -1;
}


int execSQL(const char *sql,                           /* SQL to be evaluated */
            int (*callback)(void*,int,char**,char**),  /* Callback function */  
            void * arg,                                /* 1st argument to callback */
            char **errmsg       )                      /* Error msg written here */
{
    if( g_DbApp )
    {
        char* zErrMsg = 0;		
		if (errmsg == NULL)
			errmsg = &zErrMsg;

		int ret = sqlite3_exec(g_DbApp, sql, callback, arg, errmsg);
		if (ret != SQLITE_OK)
		{
			MyTracex("exec SQL error %d: %s \n", ret, *errmsg);
			MyTracex(("SQL:%s\n"), (sql));
		}
        return ret;
    }
    return -1;
}
#include "Util.h"
int ConnectDb(LPCTSTR db_name)
{ 
    int ret = 0; 
    sqlite3 * db = 0; 
	AString sdb;
	QUnc2Utf(db_name, sdb);
	strcpy_s(g_DbName, MAX_PATH, sdb);
	MyTracex("dg: %s\n", g_DbName);


    ret = sqlite3_open(g_DbName, &db); 

    if ( ret != SQLITE_OK ) 
    { 
        MyTracex( ("Could not open database: %s"), sqlite3_errmsg(db)); 
        //exit(1); 
    } 

    MyTracex(("Database Connecteted\n")); 

    char * pErrMsg = 0; 
    int err = 0;
    g_DbApp = db;

    try
    {
		for (int i = 0; i<sizeof(g_sqlTbls) / sizeof(SQL_TABLE *); i++)
        {
            CreateTable(db, g_sqlTbls[i] );
        }
    }
    catch(  char * pMsg)
    {
        err = 1;
        pErrMsg = pMsg;
    }


    if(err && pErrMsg )
    {
        USES_CONVERSION;
        MyTracex("%s\n", pErrMsg); 
    }

    return 0; 
} 


int DisconnetDB(void)
{
    int ret = sqlite3_close(g_DbApp); 
    if( ret == SQLITE_OK ) 
    {
        g_DbApp = 0; 
    }
    return ret;
}

int UpdateDB(void)
{
    DisconnetDB();
    
    int ret = 0; 
    sqlite3 * db = 0; 
    
    ret = sqlite3_open(g_DbName, &db); 
    if ( ret != SQLITE_OK ) 
    { 
        MyTracex( ("Could not open database: %s"), sqlite3_errmsg(db)); 
        
    } 
    else
    {
        g_DbApp = db;
    }

    return ret;

}


static int cb_Show_Record(void * notused,int argc, char ** argv, char ** aszColName) 
{ 
    int i; 
    for ( i=0; i<argc; i++ ) 
    { 
        MyTracex( "%s = %s\n", aszColName[i], argv[i] == 0 ? "NUL" : argv[i] ); 
    } 
 
    return 0; 
} 

#define SZFMT_PRIMKEY "NULL"
#define SZFMT_INTEGER "%d"
#define SZFMT_VARCHAR "'%s'"

int Sql_Write_DB(SQL_TBL_ID  tbl_type, char * sql, int len, const void * pParam, const  char * pCondition)
{
	if (pParam == NULL) return -1;
	const SQL_TABLE * pTblHandle = g_sqlTbls[tbl_type];
	const SQL_TABLE_COL* pCol = pTblHandle->pCol;
	if (pCondition == NULL)
	{
		const char * * pv1 = (const char * *)pParam;

		sprintf_s(sql, len, "INSERT INTO %s VALUES(", pTblHandle->table);
		for (int i = 0; i<pTblHandle->cols; i++)
		{
			char tmp[2048];
			switch (pCol[i].type_id)
			{
			case e_str:
				sprintf_s(tmp, 2048, ","SZFMT_VARCHAR, pv1[i]);
				strcat_s(sql, len, tmp);
				break;
			case e_int:
				sprintf_s(tmp, 2048, ","SZFMT_INTEGER, (INT_PTR)pv1[i]);
				strcat_s(sql, len, tmp);
				break;
			case e_key:
				strcat_s(sql, len, ","SZFMT_PRIMKEY);
				break;
			default:
				break;
			}
		}
		char * pwrg_comma = strchr(sql, ',');
		if (pwrg_comma) *pwrg_comma = ' ';
		strcat_s(sql, len, ")");
		return 0;
	}
	else
	{
		const char * * pv1 = (const char * *)pParam;
		sprintf_s(sql, len, "UPDATE %s  SET ", pTblHandle->table);
		for (int i = 0; i<pTblHandle->cols; i++)
		{
			char tmp[2048];
			switch (pCol[i].type_id)
			{
			case e_str:
				sprintf_s(tmp, 2048, "%s="SZFMT_VARCHAR",", pCol[i].name, pv1[i]);
				strcat_s(sql, len, tmp);
				break;
			case e_int:
				sprintf_s(tmp, 2048, "%s="SZFMT_INTEGER",", pCol[i].name, (INT_PTR)(pv1[i]));
				strcat_s(sql, len, tmp);
				break;
			case e_key:
				break;
			default:
				break;
			}
		}
		sql[strlen(sql) - 1] = '\0';
		strcat_s(sql, len, pCondition);

		return 0;
	}
}
