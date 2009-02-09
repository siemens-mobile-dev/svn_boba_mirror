#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "inc\mc.h"
#include "inc\lng.h"
#include "inc\zslib.h"
#include "inc\krnl.h"
#include "inc\ColorMap.h"
#include "inc\mui.h"
#include "inc\exts.h"
#include "inc\gui.h"

char msgbuf[256];
wchar_t mcpath[MAX_PATH];
wchar_t pathbuf[MAX_PATH];
wchar_t zippathbuf[MAX_PATH];
wchar_t szLastNewFile[MAX_PATH];
wchar_t szLastNewDir[MAX_PATH];
wchar_t wsbuf[MAX_PATH*2];
int back_tab;

const wchar_t def_filter[] = L"*.*";

int progr_stop  = 0;
int progr_act   = 0;

int progr_max   = 0;
int progr_cur   = 0;
int progr_start = 0;

int progrsp_max = 0;
int progrsp_cur = 0;
int progrsp_start = 0;

wchar_t in_open_path[MAX_PATH];

const wchar_t keys_file[] = L"mckeys.cfg";



DRVINFO Drives[MAX_DRV] = {
	{L"/tpa",    1},
	{L"/card",   1},
	{L"/system", 1},
	{L"/ifs",    1}
};

int curtab=0;
TABINFO* tabs[MAX_TABS+1];
FILEINF* FileListBase[MAX_TABS+1];


wchar_t* MCFilePath(const wchar_t* sfile)
{
  snwprintf(pathbuf,MAXELEMS(pathbuf)-1, _ls_ls, mcpath, sfile);
  return pathbuf;
}

void SetTabIndex(int tab, int num, int slide)
{
  TABINFO *tmp = tabs[tab];
  
  if (tmp->ccFiles == 0)
  {
    num = -1;
  }
  else
  {
    if (slide)
    {
      if ( num >= tmp->ccFiles) num = 0;
      else if (num < 0) num = tmp->ccFiles - 1;
    }
    else
    {
      if (num >= tmp->ccFiles) num = tmp->ccFiles - 1;
      else if (num < 0) num = 0;
    }
  }
  tmp->iIndex[tmp->CurDrv] = num;
}

void SetCurTabIndex(int num, int slide)
{
  DisableScroll();
  SetTabIndex(curtab, num, slide);
}

int GetTabIndex(int tab)
{
  return tabs[tab]->iIndex[ tabs[tab]->CurDrv ];
}

wchar_t* GetTabPath(int tab)
{
  if (IsZipOpened(tab))
    return tabs[tab]->zipInfo->szCurDir;
  else
    return tabs[tab]->szDirs[tabs[tab]->CurDrv];
}

void FillFileInfo(FILEINF *file)
{
  STRID str;
  if (!file || file->inited) return;
  
  wchar_t buf[MAX_PATH/2];
  if (file->ws_attr==LGP_NULL)
  {
    attr2s(file->attr, buf);
    str=Str2ID(buf,0,SID_ANY_LEN);
    file->ws_attr=str;
  }
  if (file->ws_size==LGP_NULL)
  {
    sz2s(file->size, buf);
    str=Str2ID(buf,0,SID_ANY_LEN);
    file->ws_size=str;
  }
  if (file->ws_time==LGP_NULL)
  {
    unixt2s(file->time, buf);
    str=Str2ID(buf,0,SID_ANY_LEN);
    file->ws_time=str;
  }
  if (!file->icon)
  {
    file->icon = GetIcon(file->ws_name, file->attr);
  }
  if (file->ftype == TYPE_ZIP_FILE && file->ws_ratio==LGP_NULL)
  {
    if (file->size != 0 && file->csize != 0)
    {
      char cbuf[32];
      float ratio = (float)file->csize * (float)100 / (float)file->size;
      sprintf(cbuf, "%.0f%%", ratio); // без цифр после точки
      win12512unicode(buf,cbuf,MAXELEMS(buf));
      file->ws_ratio=Str2ID(buf,0,SID_ANY_LEN);
    }
  }
  wchar_t* sext = GetFileExt(file->ws_name);
  if (sext && !file->ws_showname && wstrcmpi(sext, mcbm_ext) == 0)
  {
    // Откидываем расширение
    wstrcpy(buf, file->ws_name);
    buf[sext - file->ws_name - 1] = '\0';
    file->ws_showname = new wchar_t[wstrlen(buf) + 1];
    wstrcpy(file->ws_showname,buf);
  }
  
  if (!file->ws_short)
  {
    wchar_t *ws;
    if (file->ws_showname) ws=file->ws_showname;
    else ws=file->ws_name;
    file->uccnt = getLVC(ws, FONT_E_24B);
    if (file->uccnt)
    {
      cutname(ws, buf, file->uccnt>MAX_PATH/2?MAX_PATH/2:file->uccnt);
      file->ws_short=Str2ID(buf,0,SID_ANY_LEN);
    }
  }
  wchar_t *ws;
  if (curtab == systab && file->ws_showname) ws=file->ws_showname;
  else ws=file->ws_name;
  file->sid_name=Str2ID(ws,0,SID_ANY_LEN);
  file->inited = 1;
}

FILEINF* CreateFileInfo(int findex, const wchar_t* fpathOriginal,
				unsigned int fsize, short fattr, unsigned int ftime,
				int fcsize, int ftype)
{
	FILEINF *file = new FILEINF;
        memset(file,0,sizeof(FILEINF));
        
        wchar_t *name=new wchar_t[wstrlen(fpathOriginal)+1];
        wstrcpy(name,fpathOriginal);

	file->id		= findex;
	file->attr		= fattr;
	file->size		= fsize;
	file->time		= ftime;
        file->ws_name           = name;
	file->csize		= fcsize;
	file->ftype		= ftype;
        
        file->ws_attr=LGP_NULL;
        file->ws_size=LGP_NULL;
        file->ws_time=LGP_NULL;
        file->ws_ratio=LGP_NULL;
        file->ws_short=LGP_NULL;
        file->sid_name=LGP_NULL;
	return file;
}

void AddFile(int tab, int findex, const wchar_t *fname, unsigned int fsize, short fattr,
				  unsigned int ftime, int fcsize, int ftype)
{
	FILEINF *file = CreateFileInfo(findex, fname, fsize, fattr, ftime, fcsize, ftype);
	file->next = FileListBase[tab]->next;
	FileListBase[tab]->next = file;
}

void AddFileFromDE(int tab, int findex, const wchar_t *name, W_FSTAT *fs)
{
  AddFile(tab, findex, name , fs->st_size, fs->attr, fs->unk_0x1C, 0, fs->attr&FA_DIRECTORY ? TYPE_COMMON_DIR : TYPE_COMMON_FILE);
}


void FreeFileInfo(FILEINF* file)
{
  if (file)
  {
    if (file->ws_name)	delete(file->ws_name);
    if (file->ws_showname) delete(file->ws_showname);
    TextFree(file->ws_attr);
    TextFree(file->ws_size);
    TextFree(file->ws_time);
    TextFree(file->ws_ratio);
    TextFree(file->ws_short);
    TextFree(file->sid_name);
    delete (file);
  }
}

void DelFiles(int tab)
{
  if (tabs[tab]->ccFiles)
  {
    //scfile=NULL;
    while(FileListBase[tab]->next!=FileListBase[tab])
    {
      FILEINF *file = (FILEINF *) FileListBase[tab]->next;	// Второй элемент
      FileListBase[tab]->next = (FILEINF *) file->next;		// Следующий у FileListBase - на третий
      FreeFileInfo(file);
      tabs[tab]->ccFiles--;
    }
  }
}

extern "C" void DirHandle_EnableHiddenFiles(DIR_HANDLE *, int);

int FillRealPathFiles(int tab, wchar_t* dname)
{
  FILELISTITEM *fli=new FILELISTITEM;
  W_FSTAT fs;
  int num = 0;
  {
    snwprintf(pathbuf,MAX_PATH-1, _ls_stars, dname);
    DIR_HANDLE *handle=AllocDirHandle(dname);
    DirHandle_SetFilterStr(handle, pathbuf);
    DirHandle_EnableHiddenFiles(handle, 1);
    while(GetFname(handle,fli))
    {
      if (!w_chdir(fli->path))
        w_fstat(fli->fname,&fs);
      if (!tabs[tab]->szFilter[0] || fs.attr&FA_DIRECTORY)
      {
        AddFileFromDE(tab, num++, fli->fname, &fs);
      }
    }
    DestroyDirHandle(handle);
    if (tabs[tab]->szFilter[0])
    {
      snwprintf(pathbuf, MAX_PATH-1,_ls_ls, dname, tabs[tab]->szFilter);
      DIR_HANDLE *handle=AllocDirHandle(GetDir(DIR_OTHER));
      DirHandle_SetFilterStr(handle, pathbuf);
      while(GetFname(handle,fli))
      {
        if (!w_chdir(fli->path))
        {
          w_fstat(fli->fname,&fs);
          if (!(fs.attr&FA_DIRECTORY))
          {
            AddFileFromDE(tab, num++, fli->fname, &fs);
          }
        }
      }
      DestroyDirHandle(handle);
    }
  }
  delete fli;
  return num;
}

// Заполняет список файлов текущей директории
int FillFiles(int tab, wchar_t* dname)   // Только в ММИ!!!
{
  if (tabs[tab]->ccFiles) DelFiles(tab);
  if (IsZipOpened(tab))
    tabs[tab]->ccFiles = FillZipFiles(tab, dname);
  else
    tabs[tab]->ccFiles = FillRealPathFiles(tab, dname);
  SortFiles(tab);
  return tabs[tab]->ccFiles;
}

int SetTabDrv(int tab, int num)
{
  if (num >= MAX_DRV) num = 0;
  else if (num < 0) num = MAX_DRV - 1;
  tabs[tab]->CurDrv = num;
  return FillFiles(tab, GetTabPath(tab));
}

int SetCurTabDrv(int num)
{
  return SetTabDrv(curtab, num);
}


FILEINF* _CurTabFile(int tab)
{
  int ind = GetTabIndex(tab);
  if (ind < 0) return NULL;
  
  FILEINF* file = FileListBase[tab];
  for(int ii=0; ii<=ind; ii++)
    if (file)
      file =  (FILEINF *)file->next;
    else
      return NULL;
  return file;
}

int GetFileIndex(int tab, wchar_t* fname)
{
  if (tabs[tab]->ccFiles)
  {
    int ind=0;
    FILEINF* file =  (FILEINF *)FileListBase[tab]->next;
    while(file != FileListBase[tab])
    {
      if (!wstrcmp(fname, file->ws_name))
        return ind;
      file = (FILEINF *)file->next;
      ind++;
    }
  }
  return -1;
}

int GetCurTabFileIndex(wchar_t* fname)
{
  return GetFileIndex(curtab, fname);
}

int RefreshTab(int tab)
{
  FILEINF* cfile = _CurTabFile(tab);
  wchar_t* lpname;
  if (cfile)
  {
    lpname = new wchar_t[wstrlen(cfile->ws_name) + 1];
    wstrcpy(lpname, cfile->ws_name);
  }
  int res = FillFiles(tab, GetTabPath(tab));
  
  int ind;
  if (cfile)
  {
    ind = GetFileIndex(tab, lpname);
    delete(lpname);
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
  for(int ii=0; ii < MAX_TABS + 1; ii++)
    RefreshTab(ii);
}

void _cd_tab(int tab, int drv, wchar_t *dname)
{
  if (IsZipOpened(tab))
  {
    if (wstrcmp(tabs[tab]->zipInfo->szCurDir, dname))
    {
      tabs[tab]->iBase[drv] = 0;
      tabs[tab]->iIndex[drv] = 0;
      wstrcpy(tabs[tab]->zipInfo->szCurDir, dname);
    }
  }
  else
  {
    if (wstrcmp(tabs[tab]->szDirs[drv], dname))
    {
      tabs[tab]->iBase[drv] = 0;
      tabs[tab]->iIndex[drv] = 0;
      wstrcpy(tabs[tab]->szDirs[drv], dname);
    }
  }
}


int InitTab(int tab)
{
  tabs[tab] = new TABINFO;
  {
    tabs[tab]->sort = ST_FIRST;
    tabs[tab]->szFilter[0]=0;
  }
  tabs[tab]->zipInfo = new ZIPINF;
  {
    memset(tabs[tab]->zipInfo, 0, sizeof(ZIPINF));
  }
  FileListBase[tab] = new FILEINF;
  {
    memset(FileListBase[tab],0, sizeof(FILEINF));
    FileListBase[tab]->id = -1;
    FileListBase[tab]->next = FileListBase[tab];
  }
  for(int num = 0; num < MAX_DRV; num++)
  {
    _cd_tab(tab, num, Drives[num].path);
  }
  return SetTabDrv(tab, 0);
}

void FreeTab(int tab)
{
  delete (FileListBase[tab]);
  CloseTabZip(tab);
  delete (tabs[tab]->zipInfo);
  delete (tabs[tab]);
}

void DoErrKey()
{
  //MessageBox
  //MsgBoxError(1, (int) muitxt(ind_err_badkey));
}



FILEINF* _CurFile()
{
  return _CurTabFile(curtab);
}

wchar_t* CurFullPath(wchar_t* sfile)
{
  if (IsInZip())
    snwprintf(pathbuf,MAXELEMS(pathbuf)-1, _ls_ls, _CurTab->zipInfo->szCurDir, sfile);
  else
    snwprintf(pathbuf,MAXELEMS(pathbuf)-1,_ls_ls, _CurPath, sfile);
  return pathbuf;
}

void initprogr(int act)
{
  progr_start = 1;
  progr_max = 0;
  progr_stop = 0;
  progr_act = act;
}


void incprogr(int inc)
{
  if (inc > 0) progr_cur += inc;
  else if (!inc) progr_cur = 0;
  RedrawGUI=1;
}

void incprogrsp(int inc)
{
  if (inc) progrsp_cur += inc;
  else progrsp_cur = 0;
  RedrawGUI=1;
}

void endprogr()
{
  progr_start = 0;
  RedrawGUI=1;
}

void endprogrsp()
{
  progrsp_max = progrsp_cur = 0;
  RedrawGUI=1;
}


void CB_Paste(int id)
{
  if (id == IDYES) PasteFindFiles();
}

void DoPaste()
{
  if (IsInZip()) return; // Пока не обрабатывается
  
  if (buffer.count)
  {
    if (buffer.type == FNT_MOVE)
    {
      if (CONFIG_CONFIRM_MOVE)
        MsgBoxYesNo(muitxt(ind_pmt_move), CB_Paste);
      else
        CB_Paste(IDYES);
    }
    else if (buffer.type == FNT_COPY)
    {
      if (CONFIG_CONFIRM_COPY)
        MsgBoxYesNo(muitxt(ind_pmt_copy), CB_Paste);
      else
        CB_Paste(IDYES);
    }
  }
}




void CB_CS(int id)
{
  if (id==1)
  {
    if (!LoadCS(pathbuf))
      MsgBoxError(muitxt(ind_err_badformat));
    else
      SaveCS(NULL);
  }
}

void CB_LG(int id)
{
  if (id==1)
  {
    if (!LoadMUI(pathbuf))
      MsgBoxError(muitxt(ind_err_badformat));
    else
    {
      SaveMUI(NULL);
      sprintf(msgbuf, _ss, muitxt(ind_lng), muitxt(ind_lngname));
      MsgBoxError(msgbuf);
    }
  }
}


void S_ZipOpen(void)
{
  if (zippathbuf[0])
  {
    int zerr = OpenTabZip(curtab, zippathbuf);
    if (zerr == UNZ_OK)
    {
      cd(curtab, (wchar_t *)str_empty);
      SetCurTabIndex(0, 0);
    }
    else if (zerr != -11111) // ignore propr_stop
    {
      sprintf(msgbuf, "OpenZip error %i", zerr);
      MsgBoxError(msgbuf);
    }
    RedrawGUI=1;
  }
}

void ExecuteFile(wchar_t *fname, int, int)
{
  SUB_EXECUTE *data;
  wchar_t *name;
  wchar_t *w=wstrrchr(fname, '/');
  if (w)
  {
    name=w+1;
    *w=0;
    FILEITEM *fi=FILEITEM_Create();
    FILEITEM_SetPathAndContentType(fi,fname);
    FILEITEM_SetFnameAndContentType(fi,name);
    data=DataBrowser_CreateSubExecute(BOOK_GetBookID(&MCBook->book), fi);
    DataBrowser_ExecuteSubroutine(data,1,0);
    DataBrowser_ExecuteSubroutine(data,0x2D,0);
    FILEITEM_Destroy(fi);
  }
}

void _Open(int isSysOpen)
{
  if (_CurIndex < 0) return;
  
  FILEINF* file = _CurFile();
  if (file && pathbuf && wstrlen(file->ws_name))
  {
    CurFullPath(file->ws_name);
    if (file->attr & FA_DIRECTORY)
      cd(curtab, pathbuf);  
    else
    {
      wchar_t* sz = GetFileExt(file->ws_name);
      if (sz)
      {
        if (!wstrcmpni(sz, mcbm_ext, MAX_EXT))
        {
          UseBM(pathbuf);
          return;
        }
        else if (!wstrcmpni(sz, mccs_ext, MAX_EXT))
        {
          MsgBoxYesNo(muitxt(ind_pmt_impcs), CB_CS);
          return;
        }
        else if (!wstrcmpni(sz, mclg_ext, MAX_EXT))
        {
          MsgBoxYesNo(muitxt(ind_pmt_implg), CB_LG);
          return;
        }
      }
      if (!isSysOpen && CONFIG_ZIP_ENABLE && !IsInZip() && IsItZipFile(pathbuf))
      {
        wstrcpy(zippathbuf, pathbuf);
        SUBPROC((void*)S_ZipOpen);
        return;
      }
      if (IsInZip())
      {
        //SUBPROC((void*)S_ZipOpenFile);
        return;
      }
      else
      {
        ExecuteFile(pathbuf, 0, 0);
      }
    }
  }
}

void DoOpen()
{
	_Open(0);
}

void DoSysOpen()
{
	_Open(1);
}

u16 ExitTimer;

void DoExitIdYes(u16 , LPARAM );
void CB_Exit(int id)
{
  if (id == IDYES)
  {
    Terminate = progr_stop = 1;
    if (!Busy)
    {
      // Выходим из зипа на текущем табе с восстановлением позиции
      ExitFromZip();
      
      // Закрываем остальные зипы при выходе
      for (int i = 0; i < MAX_TABS; i++)
        if (i != curtab && IsZipOpened(i)) CloseTabZip(i);
  
  /*
  // Очищаем темп, если разрешено в конфиге
  if (CONFIG_DELETE_TEMP_FILES_ON_EXIT && tmp_files.count)
  {
    DeleteTempFiles();
    fn_free(&tmp_files);
  }*/
      //FREE_GUI(MCBook->main_gui);
      FreeBook(&MCBook->book);
    }
    else ExitTimer=Timer_Set(3000,DoExitIdYes,0);
  }
}

void DoExitIdYes(u16 id, LPARAM f)
{
  CB_Exit(IDYES);
}

void DoExit()
{
  if (CONFIG_CONFIRM_EXIT)
    MsgBoxYesNo(muitxt(ind_pmt_exit), CB_Exit);
  else
    CB_Exit(IDYES);
}

void DoCopy()
{
  EnumSel(M_MoveCopy, FNT_COPY);
}

void DoSwapTab()
{
  curtab++;
  if (curtab >= MAX_TABS) curtab = 0;
  
  // Останавливаем скроллинг при смене таба
  scfile = NULL;
}

int cordrv(int num, int inc)
{
  do
  {
    num = (MAX_DRV + num + inc) % MAX_DRV;
  }
  while (!Drives[num].enabled);
  return num;
}

void DoPrvDrv()
{
  if (curtab < MAX_TABS && !IsInZip())
    SetCurTabDrv(cordrv(_CurDrv, -1));
}

void DoNxtDrv()
{
  if (curtab < MAX_TABS && !IsInZip())
    SetCurTabDrv(cordrv(_CurDrv, +1));
}

void DoUp()
{
  if (_CurIndex > 0 || CONFIG_LOOP_NAVIGATION_ENABLE)
    SetCurTabIndex(--_CurIndex, 1);
}

void DoDwn()
{
  if (CONFIG_LOOP_NAVIGATION_ENABLE || _CurIndex < _CurCount - 1)
    SetCurTabIndex(++_CurIndex, 1);
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

void ExitFromZip()
{
  if (IsInZip())
  {
    //Сохраняем имя тек. зипа
    wchar_t lpname[MAX_PATH/2];
    wstrncpy(lpname, GetFileName(_CurTab->zipInfo->szZipPath), MAXELEMS(lpname));
    
    CloseTabZip(curtab);
    RefreshTab(curtab);
    
    //Ищем файл из которого вышли
    int ind = GetCurTabFileIndex(lpname);
    SetCurTabIndex(ind, 0);
  }
}

int DoBack()
{
  int res = 0;
  if (curtab < MAX_TABS)
  {
    wchar_t lpname[MAX_PATH/2];
    wchar_t* path = IsInZip() ? _CurTab->zipInfo->szCurDir : _CurPath;
    
    // Читаем родительскуюя папку в pathbuf
    GetFileDir(path, pathbuf);
    // Проверка на корень
    int goOut = IsInZip() ? *path == '\0' : *pathbuf == '\0';
    
    if (!goOut)
    {
      	//Сохраняем имя тек. папки
      wstrncpy(lpname, GetFileName(path),MAXELEMS(lpname)-1);
      res = cd(curtab, pathbuf);
      
      //Ищем папку из которой вышли
      int ind = GetCurTabFileIndex(lpname);
      SetCurTabIndex(ind, 0);
    }
    else if (IsInZip()) ExitFromZip();
    else if (CONFIG_BACK_EXIT) DoExit();
  }
  else
  {
    curtab = back_tab < MAX_TABS ? back_tab : 0;
  }
  return res;
}

void DoBackK()
{
  DoBack();
}

int EnumChk(ENUM_SEL_PROC EnumProc, int param)
{
  int cc = 0;
  if (tabs[curtab]->ccFiles)
  {
    FILEINF *file = (FILEINF *)FileListBase[curtab]->next;
    while(file != FileListBase[curtab])
    {
      if (file->attr & FA_CHECK)
      {
        cc++;
        if (EnumProc)
          if (EnumProc(file, param)==0) break;
      }
      file = (FILEINF *)file->next;
    }
  }
  return cc;
}

int EnumSel(ENUM_SEL_PROC EnumProc, int param)
{
  int cc = EnumChk(EnumProc, param);
  if (cc == 0)
  {
    if (EnumProc)
    {
      FILEINF* cfile = _CurFile();
      if (cfile)
      {
        EnumProc(cfile, param);
        cc++;
      }
    }
  }
  return cc;
}


int ChkAll(int chk)
{
  if (tabs[curtab]->ccFiles)
  {
    FILEINF *file = (FILEINF *)FileListBase[curtab]->next;
    while(file!=FileListBase[curtab])
    {
      if (chk>0 || (chk<0 && !(file->attr & FA_CHECK)))
        file->attr = file->attr | FA_CHECK;
      else
        file->attr = file->attr & ~FA_CHECK;
      file = (FILEINF *)file->next;
    }
    return 1;
  }
  return 0;
}


void ChkFile(FILEINF *file)
{
  if (file)
  {
    file->attr = file->attr & FA_CHECK ? file->attr & ~FA_CHECK : file->attr | FA_CHECK;
  }
}

void DoChk()
{
  ChkFile(_CurFile());
  if (CONFIG_CUR_DOWN_ON_CHECK && _CurIndex < _CurCount - 1)
    SetCurTabIndex(++_CurIndex, 1);
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

void CB_Del(int id)
{
  if (id==1) SUBPROC((void *)S_Delit);
}

void DoDel()
{
  if (IsInZip()) return; // Пока не обрабатывается
  
  //if (CONFIG_CONFIRM_DELETE)
   // MsgBoxYesNo(1, (int) muitxt(ind_pmt_del), CB_Del);
  //else
    CB_Del(1);
}

void DoMenu(void) {MM_Main();}

void DoNewDir()
{
  wchar_t *ws;
  if (IsInZip()) return; // Пока не обрабатывается
  
  if (*szLastNewDir)
    ws=szLastNewDir;
  else
    ws=(wchar_t *)str_empty;
  TextInput(muitxt(ind_name), 1, ws, _NewDir);
}

void CB_Cancel(int id)
{
  if (id==IDYES) fn_free(&buffer);
}

void DoCancel()
{
  if (buffer.count)
    MsgBoxYesNo(muitxt(ind_pmt_cancel), CB_Cancel);
}

void _Filter(wchar_t *wsname)
{
  wstrncpy(_CurTab->szFilter, wsname, MAX_PATH);
  if (wstrcmp(_CurTab->szFilter, def_filter) == 0)
    _CurTab->szFilter[0] = 0;
  
  DoRefresh();
}

void DoFilter()
{
  const wchar_t *ws;
  if (_CurTab->szFilter[0])
    ws=_CurTab->szFilter;
  else
    ws=def_filter;
  TextInput(muitxt(ind_name), 0, ws, _Filter);
}

