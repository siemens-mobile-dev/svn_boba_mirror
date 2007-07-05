#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

const char *successed_config_filename="";

#pragma segment="CONFIG_C"
static int LoadConfigData(const char *fname)
{
  int f;
  unsigned int ul;
  char *buf;
  int result=0;
  void *cfg;
  unsigned int rlen, end;

  cfg=(char *)__segment_begin("CONFIG_C");
  unsigned int len=(char *)__segment_end("CONFIG_C")-(char *)__segment_begin("CONFIG_C");

  if (!(buf=malloc(len))) return -1;
  if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
  {
    rlen=fread(f,buf,len,&ul);
    end=lseek(f,0,S_END,&ul,&ul);
    fclose(f,&ul);
    if (rlen==end && rlen==len)
    {
      memcpy(cfg,buf,len);
    }
    else
    {
      goto L_SAVENEWCFG;
    }
  }
  else
  {
  L_SAVENEWCFG:
    if ((f=fopen(fname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=-1)
    {
      if (fwrite(f,cfg,len,&ul)!=len) result=-1;
      fclose(f,&ul);
    }
    else
      result=-1;
  }
  mfree(buf);
  if (result>=0) successed_config_filename=fname;
  return(result);
}


// Инициализация конфигурации
// Надо вызвать в начале работы для загрузки конфигурации
// Поиск конфиг-файла согласно стандарту
static char config_name[128];


void InitConfig()
{
  extern unsigned int prev_cc;
  extern unsigned int prev_nc;
  if (prev_cc==0xFFF && prev_nc==0xFF) return; // Если нет сети то ждем пока появится
  sprintf(config_name,"4:\\ZBin\\etc\\CallCenter_%03X-%02X.bcfg",prev_cc,prev_nc);
  //ShowMSG(1,(int)config_name);
  if(LoadConfigData(config_name)<0)
  {
    config_name[0] = '0';
    LoadConfigData(config_name);
  }
}
