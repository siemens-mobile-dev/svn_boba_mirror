#include "..\inc\swilib.h"
#include "mc.h"
#include "config.h"

const char mccf_config[]="mcconfig.cfg";
MC_CFG MCConfig;

void InitCfg()
{
  zeromem(&MCConfig, sizeof(MCConfig));
  if(SAVE_PATH)
  {
   if(curtab==systab) 
    MCConfig.curtab = back_tab<MAX_TABS?back_tab:0;
   else
    MCConfig.curtab = curtab;
   
   for(int ii=0;ii<MAX_TABS;ii++)
   {
     int dr = tabs[ii]->CurDrv;
     MCConfig.tabs[ii].sort = tabs[ii]->sort;
     MCConfig.tabs[ii].LastInd = tabs[ii]->iIndex[dr];
     strcpy(MCConfig.tabs[ii].LastPath, tabs[ii]->szDirs[dr]);
   }
  } 
}

void SaveCfg()
{
  if(pathbuf)
  {
   MCFilePath(mccf_config);

   int f;
   if ((f=fopen(pathbuf,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&err))!=-1)
   {
    MCCF_hdr hdr;
    hdr.sig=mccf_sig;
    hdr.ver=mccf_ver;
    hdr.size=mccf_size;
    hdr.offset=mccf_offset;

    InitCfg();

    fwrite(f,&hdr,sizeof(hdr),&err);
    fwrite(f,&MCConfig,sizeof(MCConfig),&err);
    fclose(f,&err);
   }
  } 
}

int LoadCfg()
{
  int res = 0;

  if(pathbuf)
  {
   MCFilePath(mccf_config);

   int f;
   if((f=fopen(pathbuf,A_ReadOnly+A_BIN,P_READ,&err))!=-1)
   {
     MCCF_hdr hdr;
     if(fread(f, &hdr, sizeof(hdr), &err)==sizeof(hdr) &&
       (hdr.sig==mccf_sig) && (hdr.size>=mccf_size))
     {
       lseek(f,hdr.offset,S_SET,&err,&err);
       res = fread(f, &MCConfig, sizeof(MCConfig), &err)==sizeof(MCConfig);
     } 
     fclose(f, &err);
   }
  }
  if(!res)zeromem(&MCConfig, sizeof(MCConfig));
  return res;
}
