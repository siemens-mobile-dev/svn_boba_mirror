#ifndef _MC_H
#define _MC_H

#include "..\\include\Lib_Clara.h"
#include "..\\include\var_arg.h"
#include "inc\slib.h"
#include "inc\fslib.h"
#include "inc\krnl.h"
#include "inc\log.h"
#include "inc\main.h"
#include "inc\config_data.h"
#include "inc\menus.h"


#include "zlib\zlib.h"
#include "zlib\minizip\unzip.h"

#define LGP_NULL 0x6FFFFFFF

#define IDYES 1
#define IDNO 0

void MsgBoxError(char *err);
void MsgBoxError(int lgind, char* str);
void MsgBoxError(int lgind, wchar_t* str);
void MsgBoxYesNo(char *qv, void(*f)(int));
void MsgBoxError(char *err, int a);

void DoErrKey();

#define MAX_DRV			4

#define MAX_TABS		2
#define systab			MAX_TABS

#define FNT_NONE		1
#define FNT_COPY		1
#define FNT_MOVE		2

#define ST_REVERS		0x0100
#define ST_NAME			0x0
#define ST_EXT			0x1
#define ST_SIZE			0x2
#define ST_DATE			0x3
#define STD_MASK		0x0100
#define STV_MASK		0x000F
#define ST_FIRST		ST_NAME
#define ST_LAST			ST_DATE

#define FA_RIGHTS_MASK          0x1FF
#define FA_CHECK		0x1000
#define FA_DIRECTORY            0x4000
#define FA_HIDDEN               0x20000

// for type in FILEINF
#define TYPE_COMMON_FILE	0
#define TYPE_COMMON_DIR		1
#define TYPE_ZIP_FILE		2
#define TYPE_ZIP_DIR		3



typedef struct
{
	unzFile uf;
	wchar_t szZipPath[MAX_PATH];
	wchar_t szCurDir[MAX_PATH];
	unz_global_info gi;
	unz_file_info** pfi;
	wchar_t** pszNames;
	char* password;
} ZIPINF;

typedef struct
{
	short ccFiles;
	short sort;
	short CurDrv;
	short iIndex[MAX_DRV];
	short iBase[MAX_DRV];
	wchar_t  szDirs[MAX_DRV][MAX_PATH];
	wchar_t  szFilter[MAX_PATH];
	ZIPINF* zipInfo;
} TABINFO;

typedef struct
{
	wchar_t path[8];
	int  enabled;
} DRVINFO;


typedef struct
{
	int ftype;
	int pname;
	wchar_t* full;
	void* next;
	wchar_t* zipPath;
} FN_ITM;

typedef struct
{
	int count;
	int type;
	FN_ITM *items;
} FN_LIST;

typedef struct
{
	int sig;
	int ver;
	int size;
	int offset;
} mccfg_hdr;

void fn_zero(FN_LIST *list);
void fn_fill(FN_LIST *list, char* path);
void fn_add(FN_LIST *list, int type, int ftype, int pname, wchar_t* full, wchar_t* zipPath);
void fn_rev(FN_LIST *list);
void fn_free(FN_LIST *list);
void fn_log(FN_LIST *list);

extern FILEINF* scfile;
void SortFiles(int tab);

typedef void (*KEY_PROC) ();
void win12512unicode(wchar_t *ws, const char *s, int len);
void dos2utf16(wchar_t *ws, const char* s);

extern "C" long  strtol (const char* nptr,char* *endptr,int base);
KEY_PROC GetKeyprocByKey(char key);
KEY_PROC GetKeyprocLongByKey(char key);
void DoKey(int isLongPress, int key);

void LoadKeys();

extern int back_tab;
#define _CurTab   tabs[curtab]
#define _CurCount _CurTab->ccFiles
#define _CurDrv   _CurTab->CurDrv
#define _CurPath  _CurTab->szDirs[_CurDrv]
#define _CurIndex _CurTab->iIndex[_CurDrv]
#define _CurBase  _CurTab->iBase[_CurDrv]
extern int curtab;
extern TABINFO* tabs[];
extern FILEINF* FileListBase[MAX_TABS+1];
extern DRVINFO Drives[MAX_DRV];

extern const wchar_t mcbm_ext[];
extern volatile int Busy;
extern volatile int Terminate;
extern wchar_t folder_icn;
extern wchar_t chmark_icn;
extern wchar_t ch_rb_icn, rb_icn, ch_cb_icn, cb_icn;
extern MyBOOK * MCBook;
extern FN_LIST buffer;

extern DISP_OBJ *main_obj;
int cd(int tab, wchar_t *dname);

void S_Delit(void);
void S_Paste(void);
int M_MoveCopy(FILEINF *file, int param);

int isdir(wchar_t *name);
void UseBM(wchar_t* filename);

void MC_NewDir(void);
#endif
