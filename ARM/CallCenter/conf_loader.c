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
  unsigned int rlen;

  cfg=(char *)__segment_begin("CONFIG_C");
  unsigned int len=(char *)__segment_end("CONFIG_C")-(char *)__segment_begin("CONFIG_C");

  if (!(buf=malloc(len))) return -1;
  if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
  {
    rlen=fread(f,buf,len,&ul);
    if ((rlen==lseek(f,0,S_END,&ul,&ul)) && rlen==len)
    {
      memcpy(cfg,buf,len);
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

/*
  Возвращает строку вида CC-NC - параметры текущей сети в буфере.
  возвращает 0, Если сети не обнаружено:)
*/
static char get_net_id(char *buf)
{
  char *x = (char*)Get_CC_NC();
  char *y = x+1;
  char *z = x+2;
  char cc_2 = *x;
  char cc_1 = *y;
  // Немного жёсткого секса с BCD...
  if(cc_1>=0xF0){cc_1 = cc_1 && 0x0F>>4;}
  cc_2 = (cc_2<<4) + (cc_2>>4);
  char nc = *z;
  nc = (nc>>4) + (nc<<4);
  if((nc == 0xFF)&&(cc_2==0xFF))
  {
    return 0;
  }
  // ... и золотой ключик у нас в кармане )
  snprintf(buf, 15, "%X%X-%02X", cc_2, cc_1, nc);
  return 1;
}


// Инициализация конфигурации
// Надо вызвать в начале работы для загрузки конфигурации
// Поиск конфиг-файла согласно стандарту
static char config_name[128];

void InitConfig()
{
  char buf[16];
  if(!get_net_id(buf))
  {
    return;
  }
  sprintf(config_name,"4:\\ZBin\\etc\\CallCenter_%s.bcfg", buf);
  //ShowMSG(1,(int)config_name);
  if(LoadConfigData(config_name)<0)
  {
    config_name[0] = '0';
    LoadConfigData(config_name);
  }
}
