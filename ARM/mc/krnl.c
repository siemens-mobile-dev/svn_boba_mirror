#include "..\inc\swilib.h"
#include "mc.h"
#include "mui.h"
#include "lng.h"
#include "ColorMap.h"
#include "config.h"

char in_open_path[MAX_PATH];
char mcpath[MAX_PATH];
int back_tab;

int progr_stop  = 0;
int progr_act   = 0;

int progr_max   = 0;
int progr_cur   = 0;
int progr_start  = 0;

int progrsp_max = 0;
int progrsp_cur = 0;
int progrsp_start = 0;

DRVINFO Drives[MAX_DRV] = {
 {0, "0:", psz_drv_0},
 {1, "1:", psz_drv_1},
 {2, "2:", psz_drv_2},
 {4, "4:", psz_drv_4}
};

int curtab=0;
TABINFO* tabs[MAX_TABS+1];
FILEINF* FileListBase[MAX_TABS+1];

const char ext_file[] = "mcfileico.cfg";
const char keys_file[] = "mckeys.cfg";
const char cfg_file[] = "mc.bcfg";
char etc_path[] = DEFAULT_DISK":\\ZBin\\etc";



void SetTabIndex(int tab, int num, int slide)
{
 TABINFO *tmp = tabs[tab];
 
 if(tmp->ccFiles==0)     num=-1; else
 if(slide)
 {
   if(num>=tmp->ccFiles) num=0; else
   if(num<0)             num=tmp->ccFiles-1;
 }
 else
 {
   if(num>=tmp->ccFiles) num=tmp->ccFiles-1; else
   if(num<0)             num=0;
 }
 tmp->iIndex[tmp->CurDrv] = num;
}

void SetCurTabIndex(int num, int slide)
{
  SetTabIndex(curtab, num, slide);
}

int GetTabIndex(int tab)
{
  return tabs[tab]->iIndex[ tabs[tab]->CurDrv ];
}

char* GetTabPath(int tab)
{
  return (char*)&tabs[tab]->szDirs[tabs[tab]->CurDrv];
}

void FullInit(FILEINF *file)
{
 if(!file || file->inited)return;
 char* buf=malloc(128);
 if(!file->ws_short)
 {
   file->uccnt = getLVC(&file->ws_name->wsbody[1], file->ws_name->wsbody[0]);
   if(file->uccnt)
   {
     file->ws_short = AllocWS(file->uccnt+1);
     cutname(file->ws_name, file->ws_short, file->uccnt);
   }
 }
 if(!file->ws_attr)
 {
   attr2s(file->attr, buf);
   file->ws_attr = AllocWS(strlen(buf)+1);
   str_2ws(file->ws_attr, buf, 64);
 }
 if(!file->ws_size)
 {
   sz2s(file->size, buf);
   file->ws_size = AllocWS(strlen(buf)+1);
   wsprintf(file->ws_size, _t, buf);
 }
 if(!file->ws_time)
 {
   fdt2s(file->time, buf);
   file->ws_time = AllocWS(strlen(buf)+1);
   str_2ws(file->ws_time, buf, 64);
 }
 if (!file->ext)
 {
   file->ext = GetExt(file->sname, file->attr);
 }
 mfree(buf);
 file->inited = 1;
}

void AddFile(int tab, int findex, char *fname, WSHDR *fwsname, unsigned int fsize, short fattr, unsigned int ftime)
{
  FILEINF *file = malloc(sizeof(FILEINF));
  file->id     = findex;
  file->sname  = fname;
  file->attr   = fattr;
  file->size   = fsize;
  file->time   = ftime;
  file->ext    = 0;
  file->inited = 0;
  file->ws_name  = fwsname;
  file->uccnt    = 0;
  file->ws_short = 0;
  file->ws_attr  = 0;
  file->ws_size  = 0;
  file->ws_time  = 0;
  

  LockSched();

  file->next  = FileListBase[tab]->next;
  FileListBase[tab]->next = file;

  UnlockSched();
}

void DelFiles(int tab)
{
 if(tabs[tab]->ccFiles)
 {
  LockSched();
  scfile=NULL;
  while(FileListBase[tab]->next!=FileListBase[tab])
  {
   FILEINF *file = FileListBase[tab]->next;                        // Второй элемент
   FileListBase[tab]->next = file->next;                           // Следующий у FileListBase - на третий
   if(file)
   {
    if(file->sname) mfree(file->sname);
    if(file->ws_name) FreeWS(file->ws_name); 
    if(file->ws_short) FreeWS(file->ws_short);
    if(file->ws_attr) FreeWS(file->ws_attr);
    if(file->ws_size) FreeWS(file->ws_size);
    if(file->ws_time) FreeWS(file->ws_time);
    mfree(file);
   } 
   tabs[tab]->ccFiles--;
  }
  UnlockSched();
 }
}

// Заполняет список файлов текущей директории
int FillFiles(int tab, char *dname)
{
  if(tabs[tab]->ccFiles) DelFiles(tab);

  DIR_ENTRY de;
  int num=0;
  char *fname;
  WSHDR *wsname;
  if(pathbuf)
  {
   sprintf(pathbuf, _s_stars, dname);
//   ShowMSG(1, (int) pathbuf);
   if (FindFirstFile(&de,pathbuf,&err))
   {
    do
    {
      int len = strlen(de.file_name);
      fname = malloc(len+1);
      wsname = AllocWS(len+1);
      str_2ws(wsname,de.file_name,len);
      strcpy(fname,de.file_name);
      if(!tabs[tab]->szFilter[0] || de.file_attr & FA_DIRECTORY)
      {
       if(SHOW_HIDDEN || !(de.file_attr & (FA_HIDDEN | FA_SYSTEM)))
         AddFile(tab, num++, fname, wsname, de.file_size, de.file_attr, de.create_date_time);
      } 
    }
    while(FindNextFile(&de,&err));
    FindClose(&de,&err);
   }
   

   if (tabs[tab]->szFilter[0])
   {
     sprintf(pathbuf, _s_s, dname, tabs[tab]->szFilter);
     if (FindFirstFile(&de,pathbuf,&err))
     {
      do
      {
        int len = strlen(de.file_name);
        fname = malloc(len+1);
        wsname = AllocWS(len+1);
        str_2ws(wsname,de.file_name,len);
        strcpy(fname,de.file_name);
        if(!(de.file_attr & FA_DIRECTORY))
        {
         if(SHOW_HIDDEN || !(de.file_attr & (FA_HIDDEN | FA_SYSTEM)))
           AddFile(tab, num++, fname, wsname, de.file_size, de.file_attr, de.create_date_time);
        } 
      }
      while(FindNextFile(&de,&err));
      FindClose(&de,&err);
     }
   }
  }
  
  tabs[tab]->ccFiles = num;
  SortFiles(tab);
  
  return tabs[tab]->ccFiles;
}

int SetTabDrv(int tab, int num)
{
 if(num>=MAX_DRV) num=0; else
 if(num<0)        num=MAX_DRV-1;
 tabs[tab]->CurDrv=num;
 UpdateCSMname();
 return FillFiles(tab, tabs[tab]->szDirs[tabs[tab]->CurDrv]);
}

int SetCurTabDrv(int num)
{
  return SetTabDrv(curtab, num);
}

int RefreshTab(int tab)
{
  FILEINF *cfile=_CurTabFile(tab);
  char *lpname;
  if(cfile)
  {
   lpname  = malloc(strlen(cfile->sname)+1);
   strcpy(lpname, cfile->sname);
  } 

  int res = FillFiles(tab, GetTabPath(tab));

  int ind;
  if(cfile)
  {
   ind = GetFileIndex(tab, lpname);
   mfree(lpname);
  }
  else
    ind = 0;

  SetTabIndex(tab, ind, 0);
  return res;
}

int DoRefresh()
{
  return RefreshTab(curtab);
}

void UpdateAll()
{
  for(int ii=0;ii<MAX_TABS+1;ii++)
   RefreshTab(ii);
}

void _cd_tab(int tab,int drv,char *dname)
{
 if(strcmp(tabs[tab]->szDirs[drv], dname))
 {
  tabs[tab]->iBase[drv]=0;
  tabs[tab]->iIndex[drv]=0;
  strcpy(tabs[tab]->szDirs[drv], dname);
 } 
}

int InitTab(int tab)
{
 tabs[tab] = malloc(sizeof(TABINFO));
 {
  tabs[tab]->sort = ST_FIRST;
  tabs[tab]->szFilter[0]=0;
 } 
 
 FileListBase[tab] = malloc(sizeof(FILEINF));
 {
  zeromem(FileListBase[tab], sizeof(FILEINF));
  FileListBase[tab]->id = -1;
  FileListBase[tab]->sname = psz_errfile;
  FileListBase[tab]->next = FileListBase[tab];
 }
 
// strcpy(tabs[tab]->szFilter, "a*.*");
 for(int num=0;num<4;num++)
 {
   _cd_tab(tab, num, Drives[num].path);
 }
 return SetTabDrv(tab, 0);
}

void FreeTab(int tab)
{
  mfree(FileListBase[tab]);
  mfree(tabs[tab]);
}

void CB_CS(int id)
{
  if((id==IDYES) && pathbuf) 
    if(!LoadCS(pathbuf))
      MsgBoxError(1, (int) muitxt(ind_err_badformat));
    else
      SaveCS(NULL);
}

void CB_LG(int id)
{
  if((id==IDYES) && pathbuf) 
    if(!LoadMUI(pathbuf))
      MsgBoxError(1, (int) muitxt(ind_err_badformat));
    else
    {
      SaveMUI(NULL);
      sprintf(msgbuf, _ss, muitxt(ind_lng), muitxt(ind_lngname));
      ShowMSG(1, (int) msgbuf);
    }  
}
//####### Commands #######

void DoOpen()
{
  if(_CurIndex<0) return;
  
  FILEINF *file = _CurFile();
  if (file && pathbuf && strlen(file->sname))
  {
    CurFullPath(file->sname);
    if(file->attr & FA_DIRECTORY) 
       cd(curtab, pathbuf);
    else
    {
      char *sz = GetFileExt(file->sname);
      if ((int) sz) 
      {
        char szext[MAX_EXT];
        strtolower(sz, szext, MAX_EXT);
  
        if(!strncmp(szext, mcbm_ext, MAX_EXT))
        {
          UseBM(pathbuf);
          return;
        }
        else
        if(!strncmp(szext, mccs_ext, MAX_EXT))
        {
          MsgBoxYesNo(1, (int) muitxt(ind_pmt_impcs), CB_CS);
          return;
        }
        else
        if(!strncmp(szext, mclg_ext, MAX_EXT))
        {
          MsgBoxYesNo(1, (int) muitxt(ind_pmt_implg), CB_LG);
          return;
        }
      }
      
      if(wsbuf)
      {
       str_2ws(wsbuf,pathbuf,MAX_PATH);
       ExecuteFile(wsbuf,0,0);
      }
    }  
  }
}

void CB_Exit(int id)
{
  if(id==IDYES)
  {
   Terminate=progr_stop=1;
   if(!Busy)GeneralFuncF1(1);
  }
}

void DoExit()
{
  if(CONFIRM_EXIT)
   MsgBoxYesNo(1, (int) muitxt(ind_pmt_exit), CB_Exit);
  else
   CB_Exit(IDYES);
}

int DoBack()
{
  int res = 0;
  if(pathbuf)
  {
    if(curtab==systab)
    {
      curtab = back_tab<MAX_TABS?back_tab:0;
    }
    else
    {
     GetFileDir(_CurPath, pathbuf);
     if(pathbuf[0])
     {
       //Сохраняем имя тек. папки
       char *lpsname = GetFileName(_CurPath);
       char *lpname  = malloc(strlen(lpsname)+1);
       strcpy(lpname,lpsname);
      
       res = cd(curtab, pathbuf);
      
       //Ищим папку из которой вышли
       int ind = GetFileIndex(curtab, lpname);
       SetTabIndex(curtab, ind, 0);
      
       mfree(lpname);
     }  
     else if(BACK_EXIT) DoExit();
    }  
  }  
  return res;
}

void DoSwapTab()
{
  curtab++;
  if(curtab>=MAX_TABS) curtab=0;
  UpdateCSMname();
}

int cordrv(int num, int inc)
{
  num=num+inc;
  
  if(num>=MAX_DRV) num=0; else
  if(num<0)        num=MAX_DRV-1;
  
  if(num==3 && !SHOW_MMC)
    num=inc>=0?0:num-1;
    
  if(num==2 && !SHOW_SYSDRV)
    num=inc>=0?(SHOW_MMC?3:0):0;
    
  if(num==1 && !SHOW_SYSDRV)
    num=inc>=0?(SHOW_MMC?3:0):0;
    
  return num;
}

void DoPrvDrv()
{
  if(curtab!=systab)
  SetCurTabDrv(cordrv(_CurDrv, -1));
}

void DoNxtDrv()
{
  if(curtab!=systab)
  SetCurTabDrv(cordrv(_CurDrv, +1));
}

void DoUp()
{
  SetCurTabIndex(--_CurIndex, 1);
}

void DoDwn()
{
  SetCurTabIndex(++_CurIndex, 1);
}

void DoPgUp()
{
  _CurIndex-=LONG_SCRL;
  if(_CurIndex<0) _CurIndex=0;        
  SetCurTabIndex(_CurIndex, 0);
}

void DoPgDwn()
{
  _CurIndex+=LONG_SCRL;
  _CurBase  = _CurIndex;
  if(_CurIndex>=_CurCount)_CurIndex=_CurCount-1;
  SetCurTabIndex(_CurIndex, 0);
}

void DoBegin()
{
  SetCurTabIndex(0, 0);
}

void DoChk()
{
  ChkFile(_CurFile());
  if(SC_ON_CHK)SetCurTabIndex(++_CurIndex, 1);
}

void DoChkAll()
{
  ChkAll(1);
}

void DoUnCAll()
{
  ChkAll(0);
}

void DoInvChk()
{
  ChkAll(-1);
}

int DoFileProp()
{
  if(_CurIndex<0) return 0;
  EnumSel(M_FileProp, 0);
  return 1;
}

void DoNewDir()
{
  if(wsbuf)
  {
   wsprintf(wsbuf, def_new_dir);
   TextInput(muitxt(ind_newdir), muitxt(ind_name), 1, wsbuf, _NewDir);
  } 
}

void _Filter(WSHDR *wsname)
{
 ws_2str(wsname, _CurTab->szFilter, MAX_PATH);
 DoRefresh();
 GeneralFuncF1(1);
}

void DoFilter()
{
  if(wsbuf)
  {
   str_2ws(wsbuf, _CurTab->szFilter, MAX_PATH);
   TextInput(muitxt(ind_filter), muitxt(ind_name), 0, wsbuf, _Filter);
  } 
}

void DoRen()
{
  FILEINF *cfile = _CurFile();
  if(cfile)
    TextInput(muitxt(ind_rename), muitxt(ind_name), 1, cfile->ws_name, _Rename);
}

void CB_Past(int id)
{
  if(id==IDYES)SUBPROC((void *)S_Past);
}

void DoPast()
{
 if(buffer.count)
 {
  int pmt = buffer.type==FNT_MOVE?ind_pmt_move:ind_pmt_copy;
  pmt=(int)muitxt(pmt);
  MsgBoxYesNo(1, pmt, CB_Past);
 } 
}

void CB_Cancel(int id)
{
  if(id==IDYES)fn_free(&buffer);
}

void DoCancel()
{
 if(buffer.count)
  MsgBoxYesNo(1,(int)muitxt(ind_pmt_cancel), CB_Cancel);
}

void DoCopy()
{
  EnumSel(M_MoveCopy, FNT_COPY);
}

void DoMove()
{
  EnumSel(M_MoveCopy, FNT_MOVE);
}

void CB_Del(int id)
{
  if(id==IDYES)SUBPROC((void *)S_Delit);
}

void DoDel()
{
  MsgBoxYesNo(1, (int) muitxt(ind_pmt_del), CB_Del);
}

void DoRoot()
{
  if(curtab!=systab)
  cd(curtab, Drives[_CurDrv].path);
}

void DoShowPath()
{
  ShowMSG(0, (int) _CurPath);
}

void DoSortN()
{
  _CurTab->sort=(_CurTab->sort & STD_MASK) | ST_NAME;
  DoRefresh();
}

void DoSortE()
{
  _CurTab->sort=(_CurTab->sort & STD_MASK) | ST_EXT;
  DoRefresh();
}

void DoSortS()
{
  _CurTab->sort=(_CurTab->sort & STD_MASK) | ST_SIZE;
  DoRefresh();
}

void DoSortD()
{
  _CurTab->sort=(_CurTab->sort & STD_MASK) | ST_DATE;
  DoRefresh();
}

void DoSetSort()
{
  int srt = (_CurTab->sort & STV_MASK);
  srt=(srt>=ST_LAST)?srt=ST_FIRST:srt+1;
  _CurTab->sort=(_CurTab->sort & STD_MASK) | srt;
  DoRefresh();
}

void DoSortR()
{
  _CurTab->sort=(_CurTab->sort & STV_MASK) | (~_CurTab->sort & STD_MASK);
  DoRefresh();
}

void DoErrKey()
{
  MsgBoxError(1, (int) muitxt(ind_err_badkey));
}


//####### ######## #######















FILEINF *_CurFile()
{
  return _CurTabFile(curtab);
}

FILEINF *_CurTabFile(int tab)
{
  int ind = GetTabIndex(tab);
  if(ind<0) return NULL;
  
  FILEINF *file = FileListBase[tab];
  for(int ii=0;ii<=ind;ii++) 
   if (file)
    file = file->next;
   else
    return NULL;
  
  return file;
}

int GetFileIndex(int tab, char *fname)
{
 if(tabs[tab]->ccFiles) 
 {
  int ind=0;
  FILEINF *file = FileListBase[tab]->next;
  while(file!=FileListBase[tab])
  {
    if(!strcmp(fname, file->sname)) 
      return ind;
    file = file->next;
    ind++;
  }
 } 
 return -1;
}

int ___IsMultiChk(FILEINF *file, int param){ return 1;}
int IsMultiChk()
{
  return EnumChk(___IsMultiChk, 0);
}

int EnumChk(ENUM_SEL_PROC EnumProc, int param)
{
 int cc=0;
 if(tabs[curtab]->ccFiles) 
 {
  FILEINF *file = FileListBase[curtab]->next;
  while(file!=FileListBase[curtab])
  {
    if(file->attr & FA_CHECK)
    {
     cc++;
     if(EnumProc)
     if(EnumProc(file, param)==0) break;
    }
    file = file->next;
  }
 }
 return cc;
}

int EnumSel(ENUM_SEL_PROC EnumProc, int param)
{
 int cc=EnumChk(EnumProc, param);
 if(cc==0) 
 {
   if(EnumProc)
   {
     FILEINF *cfile = _CurFile();
     if (cfile)
     {
       EnumProc(cfile, param);
       cc++;
     }  
   }  
 }
 return cc;
}

/*
int IsChkAll()
{
 if(tabs[curtab]->ccFiles) 
 {
  FILEINF *file = FileListBase[curtab]->next;
  while(file!=FileListBase[curtab])
  {
    if(!(file->attr & FA_CHECK)) return 0;
    file = file->next;
  }
  return 1;
 }
 return 0;
}
*/

int ChkAll(int chk)
{
 if(tabs[curtab]->ccFiles) 
 {
  FILEINF *file = FileListBase[curtab]->next;
  while(file!=FileListBase[curtab])
  {
    if(chk>0 || (chk<0 && !(file->attr & FA_CHECK)))
      file->attr = file->attr | FA_CHECK;
    else
      file->attr = file->attr & ~FA_CHECK;
    
    file = file->next;
  }
  return 1;
 }
 return 0;
}

void ChkFile(FILEINF *file)
{
 if(file)
 {
  file->attr = file->attr & FA_CHECK?
    file->attr & ~FA_CHECK:
    file->attr | FA_CHECK;
 }   
}




void initprogr(int act)
{
  progr_start=1;
  progr_max=0;
  progr_stop=0;
  progr_act=act; 
}

void incprogr(int inc)
{
  if(inc>0) progr_cur+=inc; else 
  if(!inc)  progr_cur=0;

  if(IsGuiOnTop(MAINGUI_ID))REDRAW();
}

void incprogrsp(int inc)
{
  if(inc) progrsp_cur+=inc;
  else    progrsp_cur=0;
  
  if(IsGuiOnTop(MAINGUI_ID))REDRAW();
}

void endprogr()
{
  progr_start=0;
  if(IsGuiOnTop(MAINGUI_ID))REDRAW();
}

void endprogrsp()
{
  progrsp_max=progrsp_cur=0;
  if(IsGuiOnTop(MAINGUI_ID))REDRAW();
}

char *CurFullPath(char* sfile)
{
 if(pathbuf)
   sprintf(pathbuf, _s_s, _CurPath, sfile);
 return pathbuf;
}

char *CurFullPath2(char* buff, char* sfile)
{
 if(buff)
   sprintf(buff, _s_s, _CurPath, sfile);
 return buff;
}

char *TmpFullPath2(char* buff, char* sfile)
{
 if(buff)
 {
  TTime t;
  TDate d;
  GetDateTime(&d,&t);
  sprintf(buff, _s_s_d, _CurPath, sfile, *(int*)&t);
 }  
 return buff;
}

char *MCFilePath(const char* sfile)
{
 if(pathbuf)
   sprintf(pathbuf, _s_s, mcpath, sfile);
 return pathbuf;
}
