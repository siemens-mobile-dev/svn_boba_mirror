#include "..\inc\swilib.h"
#include "mc.h"

typedef void (*KEY_PROC) ();

typedef struct{
  int key;
  char* name;
} KEYNM_MAP;

const KEYNM_MAP keynames[] = 
{
 {LEFT_SOFT, "lsoft"},
 {RIGHT_SOFT, "rsoft"},
 {GREEN_BUTTON, "green"},
 {RED_BUTTON, "red"},
 {ENTER_BUTTON, "enter"},
 {UP_BUTTON, "up"},
 {DOWN_BUTTON, "down"},
 {LEFT_BUTTON, "left"},
 {RIGHT_BUTTON, "right"},
 {VOL_UP_BUTTON, "volup"},
 {VOL_DOWN_BUTTON, "voldown"},
 {'0', "0"},
 {'1', "1"},
 {'2', "2"},
 {'3', "3"},
 {'4', "4"},
 {'5', "5"},
 {'6', "6"},
 {'7', "7"},
 {'8', "8"},
 {'9', "9"},
 {'*', "*"},
 {'#', "#"},
};

KEY_PROC procmap[] = 
{
 DoMenu,
 (KEY_PROC)DoBack,
 DoChk,
 NULL,
 DoOpen,
 DoUp,
 DoDwn,
 DoPrvDrv,
 DoNxtDrv,
 NULL,
 NULL,
 NULL,
 DoSwapTab,
 DoUp,
 DoPgUp,
 DoPrvDrv,
 DoOpen,
 DoNxtDrv,
 NULL,
 DoDwn,
 DoPgDwn,
 DoInvChk,
 NULL,
};
#define KEYS_COUNT (sizeof(procmap)/sizeof(procmap[0])


void DoKey(int key)
{
  for(int cc=0;cc<KEYS_COUNT);cc++)
   if(keynames[cc].key==key)
   {
     if(procmap[cc])procmap[cc]();
     return;
   }
}

typedef struct{
  char* name;
  KEY_PROC proc;
} PROC_MAP;

const PROC_MAP keyprocs[]=
{
 {"none", NULL},
 {"open", DoOpen},
 {"rename", DoRen},
 {"past", DoPast},
 {"cancel", DoCancel},
 {"copy", DoCopy},
 {"move", DoMove},
 {"del", DoDel},
 {"newdir", DoNewDir},
 {"back", (KEY_PROC)DoBack},
 {"menu", DoMenu},
 {"fileprop", (KEY_PROC)DoFileProp},
 {"drvinf", DoDrvInf},
 {"swaptab", DoSwapTab},
 {"prvdrv", DoPrvDrv},
 {"nxtdrv", DoNxtDrv},
 {"invchk", DoInvChk},
 {"chk", DoChk},
 {"chkall", DoChkAll},
 {"unchall", DoUnCAll},
 {"up", DoUp},
 {"dwn", DoDwn},
 {"pgup", DoPgUp},
 {"pgdn", DoPgDwn},
 {"begin", DoBegin},
 {"bmlist", DoBMList},
 {"bmadd", DoBMAdd},
 {"filter", DoFilter},
 {"refresh", (KEY_PROC)DoRefresh},
 {"root", DoRoot},
 {"showpath", DoShowPath},
 {"exit", DoExit},
 {"showpath", DoShowPath},
 {"sortn", DoSortN},
 {"sorte", DoSortE},
 {"sorts", DoSortS},
 {"sortd", DoSortD},
 {"sortr", DoSortR},
 {"setsort", DoSetSort},
};

void KeysProc(char *name, char *value)
{
  strtolower(name, name, -1);
  strtolower(value, value, -1);

  int inkey=-1;
  KEY_PROC keyproc = DoErrKey;
  
  for(int cc=0;cc<KEYS_COUNT);cc++)
   if(strcmp(keynames[cc].name,name)==0)
   {
     inkey=cc;
     break;
   }
  if(inkey==-1) return;

  for(int cc=0;cc<sizeof(keyprocs)/sizeof(keyprocs[0]);cc++)
   if(strcmp(keyprocs[cc].name,value)==0)
   {
     keyproc=keyprocs[cc].proc;
     break;
   }
  
  procmap[inkey] = keyproc;    
}

void LoadKeys()
{
  if(LOAD_KEYS)
  EnumIni(1,(char*)&keys_file,KeysProc);
}
