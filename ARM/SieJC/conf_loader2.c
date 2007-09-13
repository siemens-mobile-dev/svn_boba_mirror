#include "..\inc\swilib.h"
#include "main.h"
#include "conf_loader2.h"

#ifdef NEWSGOLD
#define DEFAULT_DISK_CFG "4"
#else
#define DEFAULT_DISK_CFG "0"
#endif

void LoadTempSetting()
{
  int f;
  char h[3];
  unsigned int err;
 extern int Is_Sounds_Enabled;
 extern int Is_Vibra_Enabled;
 extern char Display_Offline;
 char fname[255]= DEFAULT_DISK_CFG ":\\Zbin\\var\\SieJC.cfg";
  if ((f=fopen(fname, A_ReadOnly+A_MMCStream, P_READ, & err ))!=1)
  {
    fread(f,h,3,&err);
    fclose(f,&err);    
   Is_Sounds_Enabled=h[0];
   Is_Vibra_Enabled=h[1];
   Display_Offline=h[2];
  }
}

void SaveTempSetting()
{
  int f;
  char g[3];
  char fname[255]= DEFAULT_DISK_CFG ":\\Zbin\\var\\SieJC.cfg";
  unsigned int ul;
 extern int Is_Sounds_Enabled;
 extern int Is_Vibra_Enabled;
 extern char Display_Offline; 
   if ((f=fopen(fname,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=1)
   {
     g[0]=Is_Sounds_Enabled;
     g[1]=Is_Vibra_Enabled;
     g[2]=Display_Offline;
     fwrite(f,g,3,&ul);
     fclose(f,&ul);
   };
}
