#include "..\inc\swilib.h"

#ifndef NEWSGOLD
extern TREGEXPLEXT *EXT2_REALLOC(void);
extern TREGEXPLEXT *EXT2_AREA;
extern int EXT2_CNT;

// В этот файл включены некоторые функции которых нет в прошивке для SGOLD

// ========================================== Reg Files =============================================
__arm void RegFile(WSHDR*ext,int unical_id,int menu_flag,unsigned int* icon1,int obex_path_id,int enabled_options,void *proc1,void *proc2,unsigned int *icon2)
{
  TREGEXPLEXT *reg;
  unsigned int *icon2new;
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
  icon2new[0]=*icon1>>28==0xA?*icon2:*icon1+1;
  icon2new[1]=0;
  reg->icon2=icon2new;
  UnlockSched();
}

__arm int GetBigIcon(const unsigned int icon, int uid)
{
  asm("mov r1, r8");
  TREGEXPLEXT *reg;
  if ((icon>>28)!=0xA) return (icon+1);
  reg=get_regextpnt_by_uid(uid);
  if (!reg) return (0);
  return (*(reg->icon2));
}

__arm void UnregExplExt_impl(REGEXPLEXT const * reg_orig)
{
  char ext[16];
  TREGEXPLEXT *reg=(TREGEXPLEXT*)EXT2_AREA;
  for (int i=0;i!=EXT2_CNT;i++)
  {
    if (reg_orig->unical_id==reg[i].unical_id)
    {
      ws_2str(reg[i].ext,ext,15);
      if (!strcmp(ext,reg_orig->ext))
      {
        FreeWS(reg[i].ext);
        mfree((void*)reg[i].icon2);
        EXT2_CNT--;
        memcpy(&reg[i],&reg[i+1],sizeof(TREGEXPLEXT)*(EXT2_CNT-i));
        return;
      }
    }
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
  if (proc==NULL) return (0);
  PLIST *ptop=(PLIST*)&KEY_TOP;
  PLIST *plist=malloc(sizeof(PLIST));
  if (plist==NULL) return (0);
  LockSched();
  plist->next=0;
  plist->proc=proc;
  plist->mfree=(void(*)(void*))mfree_adr();
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
  PLIST *plist=malloc(sizeof(PLIST));
  if (plist==NULL) return (0);
  LockSched();
  plist->next=ptop;
  plist->proc=proc;
  plist->mfree=(void(*)(void*))mfree_adr();
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
      ptop->mfree(ptop);
      return;
    }
    prev=ptop;
  }
}


int PatchKeybMsg(int submsg, int msg)
{
  PLIST *ptop=(PLIST*)(&KEY_TOP);
  int result;
  int mode=0;
  while ((ptop=ptop->next))
  {
    if (!ptop->proc) continue;
    result=ptop->proc(submsg,msg);
    switch(result)
    {
    case 0:
      continue;
    case 1:
      mode=1;
      continue;
    case 2:
      return (2);
    case 3:
      return (mode==1?2:0);
    }
  }
  return (mode);
}

// ========================================= fread/fwrite ===========================================
extern int (*FReadOld)(int FileHandler, void *cBuffer, int iByteCount, unsigned int *ErrorNumber);
extern unsigned int(*FWriteOld)(int FileHandler, char * cBuffer, int iByteCount, unsigned int *ErrorNumber);

int fread32(int fh, void *buf, int len, unsigned int *err)
{
  int clen;
  int rlen;
  int total=0;
  while(len)
  {
    clen=len>=0x400?0x400:len;
    total+=(rlen=FReadOld(fh, buf, clen, err));
    if (rlen!=clen) break;
    buf=(char *)buf+rlen;
    len-=clen;
  }
  return(total);
}


unsigned int fwrite32(int fh, void *buf, int len, unsigned int *err)
{
  int clen;
  unsigned int wlen;
  int total=0;
  while(len)
  {
    clen=len>=0x400?0x400:len;
    total+=(wlen=FWriteOld(fh, buf, clen, err));
    if (wlen!=clen) break;
    buf=(char *)buf+wlen;
    len-=clen;
  }
  return(total);
}



#pragma diag_suppress=Pe177
__root static const int SWILIB_FUNC00B @ "SWILIB_FUNC00B" = (int)fread32;
__root static const int SWILIB_FUNC00C @ "SWILIB_FUNC00C" = (int)fwrite32;
__root static const int SWILIB_FUNC095 @ "SWILIB_FUNC095" = (int)UnregExplExt_impl;
__root static const int SWILIB_FUNC12B @ "SWILIB_FUNC12B_12D" = (int)AddKeybMsgHook_impl;
__root static const int SWILIB_FUNC12C @ "SWILIB_FUNC12B_12D" = (int)AddKeybMsgHook_end_impl;
__root static const int SWILIB_FUNC12D @ "SWILIB_FUNC12B_12D" = (int)RemoveKeybMsgHook_impl;
#pragma diag_default=Pe177



#endif
