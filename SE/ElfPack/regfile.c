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


__root void onEv(FILESUBROUTINE ** r0)
{
  EP_DATA *epd=getepd();
  *r0=epd->elf_ext_m;
}

__arm int SetSmallIcon(void * r0, u16 *r1)
{
  int tmp;
  iconidname2id(IDN_ELF_SMALL_ICON,-1,&tmp);
  *r1=tmp;
  return 0;
}

#ifdef DB_CMD_SETTHUMBNAILICON
__arm int SetThumbnailIcon(void * r0, u16 *r1)
{
  //2do: read external images
  return SetSmallIcon(r0,r1);
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

  _printf("Starting %s",ch)  ;

  {
    //    int err;
    //    err=
    elfload(filename,0,0,0);
  }


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

const DB_EXT dbe_ELF={(char**)ct,(u16**)ex,onEv,DB_DB_EXT_C1,NULL,0x0,0x00};


__arm int Elf_Run_Subroutine(void * r0)
{
  BookObj_CallSubroutine(((SUB_EXECUTE*)r0)->BrowserItemBook,(void*)subrout);
  return(1);
};


__arm void ELFExtrRegister(EP_DATA * epd)
{
  FILESUBROUTINE * fsr;
  FILESUBROUTINE * fs;
  FILESUBROUTINE * fsnew;
  int i=0;
  int j=0;

  if (epd->dbe)
  {
    // берем его методы
    GetOtherExtMethods(&fsr);
    fs=fsr;
    // считаем их
    while (fs[j++].PROC);
    // выделяем память под n+1
    fsnew=malloc((j+1)*sizeof(FILESUBROUTINE));
    // первым вставляем своё
    fsnew->cmd=1;
    fsnew->PROC=Elf_Run_Subroutine;
    fsnew->PROC1=(int(*)(void*,void*))RUN_CHECK;
    textidname2id(IDN_START,-1,&fsnew->StrID);
    // забираем к себе старые методы
    memcpy(fsnew+1,fsr,j*sizeof(FILESUBROUTINE));
    fs=fsnew;
    // проходимся по всем, и кого надо, правим..
    while (fs->PROC)
    {
      if (fs->cmd==DB_CMD_SETSMALLICON) fs->PROC=(int(*)(void*))SetSmallIcon;
#ifdef DB_CMD_SETTHUMBNAILICON
      if (fs->cmd==DB_CMD_SETTHUMBNAILICON) fs->PROC=(int(*)(void*))SetThumbnailIcon;
#endif
      fs++;
    }
    epd->elf_ext_m=fsnew;
    {
      //build dbfolders + dbe_ELF
      DB_EXT_FOLDERS* dbfolders;
      DB_EXT* new_dbe_ELF;
#ifndef DAEMONS_INTERNAL
      int efnum=3;
#else
      int efnum=2;
#endif
      dbfolders = malloc( sizeof(DB_EXT_FOLDERS)* efnum-- );
      //last empty record
      dbfolders[efnum].StrID_FolderName = dbfolders[efnum].StrID_SavedTo = 0x6FFFFFFF;
      dbfolders[efnum].Path=NULL;
      dbfolders[efnum--].isInternal=0;
      //internal folder
      textidname2id( IDN_FOLDERNAME, -1, &dbfolders[efnum].StrID_FolderName );
      textidname2id( IDN_SAVED_ON_PH, -1, &dbfolders[efnum].StrID_SavedTo );
      dbfolders[efnum].Path=PATH_ELF_ROOT_INT;
      dbfolders[efnum--].isInternal=1;
#ifndef DAEMONS_INTERNAL
      //external folder
      textidname2id( IDN_FOLDERNAME, -1, &dbfolders[efnum].StrID_FolderName );
      textidname2id( IDN_SAVED_ON_MS, -1, &dbfolders[efnum].StrID_SavedTo );
      dbfolders[efnum].Path=PATH_ELF_ROOT_EXT;
      dbfolders[efnum].isInternal=0;
#endif
      memcpy( new_dbe_ELF = malloc( sizeof(dbe_ELF) ), &dbe_ELF, sizeof(dbe_ELF) );
      new_dbe_ELF->dbf = dbfolders;

      DB_EXT ** m;
      i=0;
      while  (epd->dbe[i++]!=LastExtDB());
      m=malloc((i+1)*sizeof(DB_EXT *));
      memcpy(&m[1],epd->dbe,(i)*sizeof(DB_EXT *));
      m[0] = new_dbe_ELF ;

      mfree(epd->dbe);
      epd->dbe=m;
    }
  }
}



__arm DB_EXT **MoveExtTable(DB_EXT **old)
{
  int i=0;
  DB_EXT **m;
  while(old[i++]!=LastExtDB());
  m=malloc(i*sizeof(DB_EXT *));
  memcpy(m,old,i*sizeof(DB_EXT *));
  return (m);
}
