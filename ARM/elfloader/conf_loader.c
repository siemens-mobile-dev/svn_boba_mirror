#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

__no_init const char *successed_config_filename;

#pragma segment="CONFIG_DATA_ID"
#pragma segment="CONFIG_DATA_I"
__arm int LoadConfigData(const char *fname)
{
  int f;
  unsigned int ul;
  int result=0;
  void *cfg;
  void *cfg_init;
  unsigned int rlen;

  cfg=(void *)__segment_begin("CONFIG_DATA_I");
  cfg_init=(void *)__segment_begin("CONFIG_DATA_ID");
  
  unsigned int len=(char *)__segment_end("CONFIG_DATA_ID")-(char *)__segment_begin("CONFIG_DATA_ID");
  if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
  {
    rlen=fread(f,cfg,len,&ul);
    if ((rlen==lseek(f,0,S_END,&ul,&ul)) && rlen==len)
    {
      fclose(f,&ul);
    }
    else
    {
      fclose(f,&ul);
      goto L_SAVENEWCFG;
    }
  }
  else
  {
  L_SAVENEWCFG:
    memcpy(cfg,cfg_init,len);
    if ((f=fopen(fname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=-1)
    {
      if (fwrite(f,cfg,len,&ul)!=len) result=-1;
      fclose(f,&ul);
    }
    else
      result=-1;
  }
  if (result>=0) successed_config_filename=fname;
  return(result);
}

void InitConfig()
{
  if (LoadConfigData("4:\\ZBin\\etc\\ElfPack.bcfg")<0)
  {
    LoadConfigData("0:\\ZBin\\etc\\ElfPack.bcfg");
  }
}
