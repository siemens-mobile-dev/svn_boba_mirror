#include "..\inc\swilib.h"
#include "mc.h"
#include "mui.h"

void ShowErr1(int lgind, char* str)
{
  sprintf(msgbuf, muitxt(lgind), str);
  MsgBoxError(1, (int) msgbuf);
}

int cd(int tab, char *dname)
{
  if(!isdir(dname, &err))
  {
    char* buf=malloc(MAX_PATH);
    sprintf(buf, "%t\\", dname);
    int r=isdir(buf, &err);
    mfree(buf);
    if(!r)
    {
      ShowErr1(ind_err_badname_t, dname);
      return 0;
    }
  }
  unsigned int drv=dname[0]-'0';
  if(drv==4)drv=3;
  if(drv>=MAX_DRV)
  {
    ShowErr1(ind_err_baddrv_t, dname);
    return 0;
  }

  _cd_tab(tab, drv, dname);
  int res = SetTabDrv(tab, drv);
  if(!res) SetTabIndex(tab, 0, 0);

  return res;
}

int cdsys(char *dname)
{
  if(pathbuf2)
  {
    if(cd(systab, dname))
    {
      back_tab = curtab;
      curtab = systab;
      return 1;
    }
  }
  return 0;
}


int _DirSize(DIR_ENTRY *de, int param)
{
  if(param)
  {
    *(int*)param += de->file_size;
    return 1;
  } 
  return 0;
}

int M_DirSize(FILEINF *file, int param)
{
  if(file && pathbuf && strlen(file->sname))
  {
   CHKDS_PARAM *p = (CHKDS_PARAM *)param;
   CurFullPath(file->sname);
   if(file->attr & FA_DIRECTORY)
     p->count+=EnumFiles(pathbuf, _DirSize, (int)&p->gsize)+0x10000;
   else
   {
     p->gsize+=file->size;
     p->count++;
   }  
   return 1;
  } 
 return 0;
}

int _SetAttr(DIR_ENTRY *de, int param)
{
  if(pathbuf2)
  {
    sprintf(pathbuf2, _s_s, de->folder_name, de->file_name);
    SetFileAttrib(pathbuf2, param, &err);
    return 1;
  }  
 return 0;
}

int M_SetAttr(FILEINF *file, int param)
{
  if (file && pathbuf && strlen(file->sname))
  {
    CurFullPath(file->sname);
    int res=SetFileAttrib(pathbuf, fa_attr, &err);
    if(fa_sb)
     EnumFiles(pathbuf, _SetAttr, fa_attr);
     
    if(!res)
     *(int*) param = 0;

    return 1;
  }
  return 0;
}

int M_FileProp(FILEINF *file, int param)
{
  _FileProp(file);
  return 0; //Only first :)))
}

void MC_FileProp(GUI *data)
{
  if( DoFileProp() ) 
    CloseMenu();
}

void MC_DrvInf(GUI *data)
{
  DoDrvInf();
//  GeneralFuncF1(1);
}

void _NewDir(WSHDR *wsname)
{
 if(pathbuf && pathbuf2)
 {
   ws_2str(wsname, pathbuf2, MAX_PATH);
   CurFullPath(pathbuf2);
   if(!mkdir(pathbuf, &err))
     MsgBoxError(1, (int) muitxt(ind_err_makedir));
   else
   {
    DoRefresh();
    //»щим папку которую создали
    int ind = GetFileIndex(curtab, pathbuf2);
    SetCurTabIndex(ind, 0);
   } 
 }
 GeneralFuncF1(1);
}

void MC_NewDir(GUI *data)
{
  DoNewDir();
  CloseAllMenu();
}

void CB_RenEx(int id)
{
  if(id==IDYES || id==-1 || id==-2)
  {
    char* src=malloc(MAX_PATH);
    FILEINF *file = _CurFile();
    sprintf(src, _s_s, _CurPath, file->sname);
    if(id==IDYES)unlink(pathbuf, &err);
    int res;
    if(id==-2)
    {
      char* tmp=malloc(MAX_PATH);
      TmpFullPath2(tmp, file->sname);
      res = fmove(src, tmp, &err) && fmove(tmp, pathbuf, &err);
      mfree(tmp);
    }
    else
    res = fmove(src, pathbuf, &err);
    
    DoRefresh();
    if(res)
    {
     //»щим
     int ind = GetFileIndex(curtab, pathbuf2);
     SetCurTabIndex(ind, 0);
    } 
    else
     MsgBoxError(1, (int) muitxt(ind_err_rename));
    mfree(src);
  }
}
void _Rename(WSHDR *wsname)
{
  if(pathbuf && pathbuf2)
  {
    ws_2str(wsname, pathbuf2, MAX_PATH);
    CurFullPath(pathbuf2);
    
    if(wstrcmpi(wsname, _CurFile()->ws_name))
    {
      if(fexists(pathbuf))
       MsgBoxYesNo(1, (int) muitxt(ind_pmt_exists), CB_RenEx);
      else
       CB_RenEx(-1);
    }
    else
      CB_RenEx(-2);
  } 
}

void MC_Rename(GUI *data)
{
  DoRen();
  CloseAllMenu();
}

int M_Delit(FILEINF *file, int param)
{
  if (file && pathbuf && strlen(file->sname))
  {
    CurFullPath(file->sname);
    if(fsrm(pathbuf, 1)==0)
      *(int*) param = 0;

    return 1;
  }
  return 0;
}


int M_FileCnt(FILEINF *file, int param)
{
 if(file && pathbuf)
 {
   CurFullPath(file->sname);
   *(int*)param+=GetFilesCnt(pathbuf);
   return 1;
 } 
 return 0;
}

void S_Delit(void)
{
    if (_CurIndex < 0) return;
    Busy = 1;
    int res = 1;

    initprogr(ind_deling);
    EnumSel(M_FileCnt, (int)&progr_max);
    incprogr(0);

    // Store current file index
    int ind = _CurIndex;

    EnumSel(M_Delit, (int)&res);

    if (!res)
    {
        MsgBoxError(1, (int) muitxt(ind_err_delete));
    }
    else
    {
        // List is not refreshed yet, so move index to the next file if there are no errors
        SetCurTabIndex(ind + 1, 0);
    }

    DoRefresh();

    endprogr();
    Busy = 0;
}

void MC_Open(GUI *data)
{
  DoOpen();
  CloseMenu();
}

int M_MoveCopy(FILEINF *file, int param)
{
  if (file && pathbuf && strlen(file->sname))
  {
    int isfile = file->attr & FA_DIRECTORY?0:1;
    int pname = strlen(_CurPath)+1;
    CurFullPath(file->sname);
    fn_add(&buffer, param, isfile, pname, pathbuf);
    return 1;
  }
  return 0;
}

//Buffer
FN_LIST buffer;

void S_Past(void)
{
 if(buffer.count)
 {
  if(pathbuf)
  {
   int res=1;
   Busy = 1;

   if(buffer.type==FNT_MOVE) progr_act = ind_moveing; else
   if(buffer.type==FNT_COPY) progr_act = ind_copying;
   
   initprogr(progr_act);

   FN_ITM *itm = buffer.items;
   while(itm)
   {
//     progr_max+=itm->full[0]!=_CurPath[0] || buffer.type!=FNT_MOVE?GetFilesCnt(itm->full):1;
     progr_max+=GetFilesCnt(itm->full);
     itm=itm->next;
   }
   incprogr(0);

   itm = buffer.items;
   while(itm && !progr_stop)
   {
     char *sname = itm->full + itm->pname;
     sprintf(pathbuf, _s_s, _CurPath, sname);
     if(buffer.type==FNT_MOVE)
     {
        progr_act = ind_moveing;
        res &= fsmv(itm->full, pathbuf);
     }else
     if(buffer.type==FNT_COPY)
     {
        progr_act = ind_copying;
        res &= fscp(itm->full, pathbuf, 1);
     }      

     itm=itm->next;
   }
  
   if(!res)
     MsgBoxError(1, (int) muitxt(ind_err_resnok));

   fn_free(&buffer);

   UpdateAll();
   SetCurTabIndex(0, 0);

   endprogr();
   Busy = 0;
  }
 }
 else
   ShowMSG(1, (int) muitxt(ind_err_nofiles));
}
