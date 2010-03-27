#include "..\\include\Lib_Clara.h"
#include "..\\include\Types.h"
#include "vars.h"

#define IDN_ELF_SMALL_ICON _T("CALE_LUNAR_12ANIMALS_1ST_MOUSE_ICN")

#define IDN_FOLDERNAME _T("DB_OTHER_TXT") //old value: _T("DB_APPLICATIONS_TXT")
#define IDN_SAVED_ON_MS _T("DB_SAVED_IN_OTHER_ON_MS_TXT")
#define IDN_SAVED_ON_PH _T("DB_SAVED_IN_OTHER_IN_PH_TXT")
#define IDN_START _T("OAF_START_SK") //old value: _T("MENU_SYNCH_START_REMOTE_TXT")

extern __thumb long elfload(const unsigned short *filename, void *param1, void *param2, void *param3);
extern EP_DATA * getepd(void);


const u16 ext[]={L"elf"};
const char ctype[]={"application/elf"};

const int ex[2]={(int)ext,0};
const int ct[2]={(int)ctype,0};

const char ers[]={"Elf_Run_Subroutine"};

DB_EXT * OtherDbExt()
{
  __get_epd;
  int db_ext_count = 0;
  while  (epd->dbe[db_ext_count++] != LastExtDB());
  return epd->dbe[db_ext_count-2];
}

DB_EXT * CreateDbExt()
{
  DB_EXT * other_db_ext = OtherDbExt();
#if (DB2020 || A2)
  DB_EXT * db_ext = malloc(sizeof(DB_EXT));
  db_ext->type_group = other_db_ext->type_group;
#else
  DB_EXT_2010 * db_ext = malloc(sizeof(DB_EXT_2010));
#endif
  db_ext->content_type = 0;
  db_ext->ext_list = 0;
  db_ext->GetMethods = 0;
  db_ext->sub_execute_size = other_db_ext->sub_execute_size;
  db_ext->dbf = 0;
  db_ext->drm_flags = 0;
  db_ext->unk2 = 0;
  db_ext->unk3 = 0;
  return (DB_EXT*)db_ext;
}

__root void GetMethods(SUB_EXECUTE * sub_execute)
{
	sub_execute->filesub = getepd()->elf_ext_m;
}

__arm int SetSmallIcon(SUB_EXECUTE * sub_execute, wchar_t * iconid)
{
  int tmp;
  iconidname2id(IDN_ELF_SMALL_ICON,-1,&tmp);
  *iconid=tmp;
  return 0;
}

#ifdef DB_CMD_SETTHUMBNAILICON
__arm int SetThumbnailIcon(SUB_EXECUTE * sub_execute, wchar_t * iconid)
{
  return SetSmallIcon(sub_execute, iconid);
}
#endif

__arm int Elf_Run(void * r0, BOOK * book)
{
  char ch[500];
  SUB_EXECUTE * data = BrowserItem_Get_SUB_EXECUTE(book);

  wchar_t * fpath = FILEITEM_GetPath(data->file_item);
  wchar_t * fname = FILEITEM_GetFname(data->file_item);
  u16 * filename = malloc((wstrlen(fpath)+wstrlen(fname)+2)*2);

  wstrcpy(filename,fpath);
  wstrcat(filename,L"/");
  wstrcat(filename,fname);

  wstr2strn(ch,filename,MAXELEMS(ch)-1);

  _printf("Starting %s",ch);

  elfload(filename,0,0,0);

  mfree(filename);
  BookObj_ReturnPage(book,PREVIOUS_EVENT);
  return(1);
};


const PAGE_MSG erp_msg[]={
  PAGE_ENTER_EVENT,  (int*)Elf_Run,
  PREVIOUS_EVENT,    (int*)ELF_RUN_PAGE_PREVIOUS_EVENT,
  ACCEPT_EVENT,      (int*)ELF_RUN_PAGE_ACCEPT_EVENT,
  CANCEL_EVENT,      (int*)ELF_RUN_PAGE_CANCEL_EVENT,
  PAGE_EXIT_EVENT,   (int*)ELF_RUN_PAGE_PAGE_EXIT_EVENT,
  0,0
};

const PAGE_DESC erun_page={"Elf_Run_Page",0,erp_msg};

const int subrout[]={(int)&ers,(int)&erun_page,0,0};

__arm int Elf_Run_Subroutine(SUB_EXECUTE * sub_execute)
{
  BookObj_CallSubroutine(sub_execute->BrowserItemBook,(void*)subrout);
  return(1);
};

__arm void ELFExtrRegister(EP_DATA * epd)
{
  int ofs_count = 0;
  FILESUBROUTINE * OtherFileSub;
  FILESUBROUTINE * NewFileSub;
  
  GetOtherExtMethods(&OtherFileSub);
  while (OtherFileSub[ofs_count++].ON_CMD);
  epd->elf_ext_m = NewFileSub = (FILESUBROUTINE*)malloc((ofs_count+1)*sizeof(FILESUBROUTINE));
  memcpy(NewFileSub+1, OtherFileSub, ofs_count*sizeof(FILESUBROUTINE));
  
  NewFileSub->cmd = 1;
  NewFileSub->ON_CMD_RUN = Elf_Run_Subroutine;
  NewFileSub->ON_CMD_RUN_CHECK = (int(*)(SUB_EXECUTE*))RUN_CHECK;
  textidname2id(IDN_START,-1,&NewFileSub->StrID);
  
  while (NewFileSub->ON_CMD)
  {
    int cmd = NewFileSub->cmd;
    if (cmd == DB_CMD_SETSMALLICON)
    {
      NewFileSub->ON_CMD_ICON = SetSmallIcon;
    }
#ifdef DB_CMD_SETTHUMBNAILICON
    else if(cmd == DB_CMD_SETTHUMBNAILICON)
    {
      NewFileSub->ON_CMD_ICON = SetThumbnailIcon;
    }
#endif
    NewFileSub++;
  }
  NewFileSub = epd->elf_ext_m;
  
#ifndef DAEMONS_INTERNAL
  int efnum=3;
#else
  int efnum=2;
#endif
  DB_EXT_FOLDERS * dbfolders = malloc(sizeof(DB_EXT_FOLDERS)*efnum--);
  //last empty record
  dbfolders[efnum].StrID_FolderName = EMPTY_SID;
  dbfolders[efnum].StrID_SavedTo = EMPTY_SID;
  dbfolders[efnum].Path = NULL;
  dbfolders[efnum--].isInternal = NULL;
  //internal folder
  textidname2id(IDN_FOLDERNAME, -1, &dbfolders[efnum].StrID_FolderName);
  textidname2id(IDN_SAVED_ON_PH, -1, &dbfolders[efnum].StrID_SavedTo);
  dbfolders[efnum].Path = PATH_ELF_ROOT_INT;
  dbfolders[efnum--].isInternal = 1;
#ifndef DAEMONS_INTERNAL
  //external folder
  textidname2id( IDN_FOLDERNAME, -1, &dbfolders[efnum].StrID_FolderName);
  textidname2id( IDN_SAVED_ON_MS, -1, &dbfolders[efnum].StrID_SavedTo);
  dbfolders[efnum].Path = PATH_ELF_ROOT_EXT;
  dbfolders[efnum].isInternal = 0;
#endif
  
  DB_EXT * db_ext = CreateDbExt();
  db_ext->content_type = (char**)ct;
  db_ext->ext_list = (wchar_t**)ex;
  db_ext->GetMethods = GetMethods;
  db_ext->dbf = dbfolders;
  
  int db_ext_count = 0;
  while  (epd->dbe[db_ext_count++] != LastExtDB());
  DB_EXT ** db_ext_list;
  db_ext_list = malloc((db_ext_count+1)*sizeof(DB_EXT*));
  memcpy(db_ext_list+1, epd->dbe, db_ext_count*4);
  db_ext_list[0] = db_ext ;
  mfree(epd->dbe);
  epd->dbe=db_ext_list;
}

__arm DB_EXT **MoveExtTable(DB_EXT **old)
{
  int db_ext_count = 0;
  DB_EXT ** db_ext_list;
  while(old[db_ext_count++] != LastExtDB());
  db_ext_list = malloc(db_ext_count*sizeof(DB_EXT *));
  memcpy(db_ext_list, old, db_ext_count*sizeof(DB_EXT *));
  return db_ext_list;
}
