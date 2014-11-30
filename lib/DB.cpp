#include "stdafx.h"
#include "lib/sqlite3/sqlite3.h"
#include "lib\tables.h"

#pragma comment(lib, ".\\lib\\sqlite3\\sqlite3.lib" ) 

char g_DbName[MAX_PATH];
sqlite3 * g_DbApp = 0; 


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
    if( g_benDebug)
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
		if( ret != SQLITE_OK )
			MyTracex("exec SQL error ");
		else
			MyTracex("SQL:");
        MyTracex(("%s"), (sql));
        MyTracex("\n");
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
        int ret = sqlite3_exec(g_DbApp,sql,callback,arg,&zErrMsg);
		if( ret != SQLITE_OK )
			MyTracex("exec SQL error ");
		else
			MyTracex("SQL:");

        MyTracex(("%s"), (sql));
        MyTracex("\n");
        return ret;
    }
    return -1;
}

int ConnectDb(const char * db_name) 
{ 
    int ret = 0; 
    sqlite3 * db = 0; 
    
    strcpy_s(g_DbName,MAX_PATH, db_name);
    ret = sqlite3_open(g_DbName, &db); 

    if ( ret != SQLITE_OK ) 
    { 
        MyTracex( ("Could not open database: %s"), sqlite3_errmsg(db)); 
        exit(1); 
    } 

    MyTracex(("Database Connecteted\n")); 

    char * pErrMsg = 0; 
    int err = 0;
    g_DbApp = db;

    try
    {
        //for(int i=0; i<EEXP_TBL_TOTALS; i++)
        //{
        //    CreateTable(db, g_eExpressTbls[i] );
        //}

    }
    catch(  char * pMsg)
    {
        err = 1;
        pErrMsg = pMsg;
    }


    if(err && pErrMsg )
    {
        USES_CONVERSION;
        MyTracex(("%ws"), A2W(pErrMsg)); 
        MyTracex(("\n")); 
        sqlite3_free(pErrMsg); 
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

