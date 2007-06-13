#include "..\inc\swilib.h"

void main()
{
  char path[]="0:\\ZBin\\etc\\start_elf.bcfg";
  unsigned int err;
  int f;
  
  if((f=fopen(path,A_ReadOnly+A_BIN,P_READ,&err))==-1)
  {
    MsgBoxError(1,(int)"can't open config file!");
    return;
  }
  char* buf=malloc(64);
  lseek(f,44,0,&err,&err);
  fread(f,buf,64,&err);
  fclose(f,&err);  
  WSHDR *ws=AllocWS_old(128);
  wsprintf_old(ws,"%t",buf);
  ExecuteFile(ws,0,0);
  FreeWS_old(ws);
  mfree(buf);
}

#pragma diag_suppress=Pe177
__root static const int PATCH_START_ELF @ "PATCH_START_ELF" = (int)main;
#pragma diag_default=Pe177

