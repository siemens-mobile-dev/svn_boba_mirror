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
#include "inc\fn_list.h"

#include "zlib\minizip\unzip.h"

#include "inc\7zslib.h"
#include "inc\arch.h"


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

// for type in FILEINF
#define TYPE_COMMON_FILE	0
#define TYPE_COMMON_DIR		1
#define TYPE_ZIP_FILE	        2
#define TYPE_ZIP_DIR	        3
#define TYPE_7Z_FILE	        4
#define TYPE_7Z_DIR	        5





typedef struct
{
	short ccFiles;
	short sort;
	short CurDrv;
	short iIndex[MAX_DRV];
	short iBase[MAX_DRV];
	wchar_t  szDirs[MAX_DRV][MAX_PATH];
	wchar_t  szFilter[MAX_PATH];
	ARCHINFO* zipInfo;
} TABINFO;

typedef struct
{
	wchar_t path[8];
	int  enabled;
} DRVINFO;




typedef struct
{
	int sig;
	int ver;
	int size;
	int offset;
} mccfg_hdr;

extern FILEINF* scfile;
void SortFiles(int tab);

typedef void (*KEY_PROC) ();
void win12512unicode(wchar_t *ws, const char *s, int len);
void dos2utf16(wchar_t *ws, const char* s);
int utf8_to_utf16(char *utf8, int cc, wchar_t *unicode16);

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
extern volatile int RedrawGUI;



extern wchar_t STD_ICONS[];
enum S_ICONS {
  ICN_FOLDER=0,
  ICN_CHMARK,
  ICN_CH_RB,
  ICN_RB,
  ICN_CH_CB,
  ICN_CB,
  TOTAL_ICONS 
};

#define L_ICN_FOLDER        L"DB_LIST_FOLDER_ICN"
#define L_ICN_CHMARK        L"CHECKMARK_ICN"
#define L_ICN_CH_RB         L"CHECKED_RADIOBUTTON_ICN"
#define L_ICN_RB            L"RADIOBUTTON_ICN"
#define L_ICN_CH_CB         L"CHECKMARK_IN_BOX_ICN"
#define L_ICN_CB            L"CHECKBOX_ICN"

extern MyBOOK * MCBook;
extern FN_LIST buffer;

extern DISP_OBJ *main_obj;
int cd(int tab, wchar_t *dname);

void S_Delit(void);
void S_Paste(void);
int M_MoveCopy(FILEINF *file, int param);
void _NewDir(wchar_t *wsname);
int cdsys(wchar_t *dname);

int isdir(wchar_t *name);
void UseBM(wchar_t* filename);
void DoBMAdd();
void DoBMList();

void MC_NewDir(void);
#endif
