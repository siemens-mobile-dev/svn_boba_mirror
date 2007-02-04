#include "..\inc\swilib.h"

#ifndef NEWSGOLD
extern void *EXT2_REALLOC(void);
extern int *EXT2_AREA;
extern int EXT2_CNT;

// В этот файл включены некоторые функции которых нет в прошивке для SGOLD

// ========================================== Reg Files =============================================
__arm void RegFile(WSHDR*ext,int unical_id,int menu_flag,unsigned int* icon1,int obex_path_id,int enabled_options,void *proc1,void *proc2,unsigned int *icon2)
{
  REGEXPLEXT_ARM_NEW* reg;
  unsigned int * icon2new;
  reg=EXT2_REALLOC();
  LockSched();
  reg->zero=0;
  reg->icon1=icon1;
  reg->unical_id=unical_id;
  reg->obex_path_id=obex_path_id;
  reg->menu_flag=menu_flag;
  reg->enabled_options=enabled_options;
  reg->ext=ext;
  reg->proc=proc1;
  reg->altproc=proc2;
   
  icon2new=malloc(sizeof(int)*2);
  if ((*icon1>>28)==0xA)
    icon2new[0]=*icon2;
  else
    icon2new[0]=*icon1+1;
  icon2new[1]=0;
  reg->icon2=icon2new;
  UnlockSched();
}

__arm int GetBigIcon(const unsigned int icon, int uid)
{
  int num=EXT2_CNT;
  REGEXPLEXT_ARM_NEW* reg=(REGEXPLEXT_ARM_NEW*)EXT2_AREA;
  if ((icon>>28)!=0xA) return (icon+1);
  for (int i=0;i!=num;i++)
  {
    if (reg[i].unical_id==uid)
      return (*(reg[i].icon2));
  }
  return (icon+1);
}
__arm void UnregExplExt_impl(REGEXPLEXT const * reg_orig)
{
  char ext[16];
  REGEXPLEXT_ARM_NEW *reg=(REGEXPLEXT_ARM_NEW*)EXT2_AREA;
  for (int i=0;i!=EXT2_CNT;i++)
  {
    if (reg_orig->unical_id!=reg[i].unical_id) continue;
    ws_2str(reg[i].ext,ext,16);
    if (strcmp(ext,reg_orig->ext)) continue;
    FreeWS(reg[i].ext);
    mfree((void*)reg[i].icon2);
    EXT2_CNT--;
    memcpy(&reg[i],&reg[i+1],sizeof(REGEXPLEXT_ARM_NEW)*(EXT2_CNT-i));
    return;  
  }
}

// ========================================= Keyb Hooks ===========================================
typedef struct
{
  void *next;
  int (*proc)(int submsg,int msg);
  void (*mfree)(void*data);
}PLIST;

__no_init void *KEY_TOP;


__arm int AddKeybMsgHook_impl(int (*proc)(int submsg,int msg))
{
  void *_mfree_adr;
  if (proc==NULL) return (0);
  PLIST *ptop=(PLIST*)&KEY_TOP;
  PLIST *plist=malloc(sizeof(PLIST));
  if (plist==NULL) return (0);
  LockSched();
  _mfree_adr=mfree_adr();
  plist->next=0;
  plist->proc=proc;
  plist->mfree=(void(*)(void*))_mfree_adr;
  while(ptop->next)
    ptop=ptop->next;
  ptop->next=plist;
  UnlockSched();
  return (1);
}


__arm int AddKeybMsgHook_end_impl(int (*proc)(int submsg,int msg))
{
  if (proc==NULL) return (0);
  PLIST *ptop=KEY_TOP;
  void *_mfree_adr;
  PLIST *plist=malloc(sizeof(PLIST));
  if (plist==NULL) return (0);
  LockSched();
  _mfree_adr=mfree_adr();
  plist->next=ptop;
  plist->proc=proc;
  plist->mfree=(void(*)(void*))_mfree_adr;
  KEY_TOP=plist;
  UnlockSched();
  return (1);
}  

extern BXR1(void *, void (*)(void *));

__arm void RemoveKeybMsgHook_impl(int (*proc)(int submsg,int msg))
{
  PLIST *ptop=(PLIST*)(&KEY_TOP);
  PLIST *prev=ptop;
  while ((ptop=ptop->next))
  {
    if (ptop->proc==proc)
    {
      LockSched();
      prev->next=ptop->next;
      UnlockSched();
      BXR1(ptop,ptop->mfree);
      return;
    }
    prev=ptop;
  }
}

extern void(*SendKeybMsg)(int msg, int submsg);
extern BXR2(int, int, void(*)(int,int));


int PatchKeybMsg(int msg, int submsg)
{
  PLIST *ptop=(PLIST*)(&KEY_TOP);
  while ((ptop=ptop->next))
  {
    if (!ptop->proc) return (0);
    if (ptop->proc(submsg,msg)!=0) return (1);
  }
  return (0);
}


#pragma diag_suppress=Pe177
__root static const int SWILIB_FUNC095 @ "SWILIB_FUNC095" = (int)UnregExplExt_impl;

__root static const int SWILIB_FUNC12B @ "SWILIB_FUNC12B_12D" = (int)AddKeybMsgHook_impl;
__root static const int SWILIB_FUNC12C @ "SWILIB_FUNC12B_12D" = (int)AddKeybMsgHook_end_impl;
__root static const int SWILIB_FUNC12D @ "SWILIB_FUNC12B_12D" = (int)RemoveKeybMsgHook_impl;
#pragma diag_default=Pe177
#endif
