
#ifndef EEPRESS_TABLE_H
#define EEPRESS_TABLE_H

#define STR_LEN_BUFF (64*1024)


typedef int     INTEGER;
typedef char *  VARCHAR;
typedef int (*SQL_InstUpdate)(char * sql, int len,const void *  pval, const char * pCondition);

typedef enum 
{
	SQL_TBL_BOOK_LIST,
    SQL_TBL_ID_MAX
}SQL_TBL_ID;

typedef enum
{
    e_key,
    e_int,
    e_str
}SQL_TXT_TYPE;

typedef struct tag_SQL_TABLE_COL
{
    const char *  name;
    const char *  type;
    const int     type_id;
    const LPCTSTR alias;
    int    width;
}SQL_TABLE_COL;

typedef struct tag_EEXP_TABLE
{
    SQL_TBL_ID tbl_id; 
    const char * table;
    int   cols;
    const SQL_TABLE_COL* pCol;
    SQL_InstUpdate write;
}SQL_TABLE;


typedef struct tag_SQL_DB_BOOK
{
	INTEGER Id;       /*Ψһ*/
	VARCHAR BookId;   /* 1 */
	VARCHAR BookName; /* 2*/
	VARCHAR BookPath; /*3*/
	INTEGER BookSize; /*4*/
	VARCHAR tmCreate;   /* 5 */
	VARCHAR tmModify;   /* 6 */
	VARCHAR tmUpload;   /* 7 */
	VARCHAR BookState; /* 8*/
	VARCHAR author;     /*9 */
	VARCHAR description;/* 10 */
}SQL_DB_BOOK;
#define MAX_BOOK_DB_COL 11
extern const int tblBookColSize[MAX_BOOK_DB_COL];
extern const SQL_TABLE tbl_books;


int UpdateDB(void);
int DisconnetDB(void);
int ConnectDb(LPCTSTR db_name) ;
int execSQL(const char *sql,                           /* SQL to be evaluated */
            int (*callback)(void*,int,char**,char**),  /* Callback function */  
            void * arg,                                /* 1st argument to callback */
            char **errmsg       ) ;                    /* Error msg written here */
int execSQL(const char* sql);

//pCondition == NULL --> Call insert function, otherwise call modify function
int Sql_Write_DB(SQL_TBL_ID  tbl_type, char * sql, int len, const void * pParam, const  char * pCondition);

void MyTracex(const char * fmt, ...) ;
#endif /*EEPRESS_TABLE_H*/