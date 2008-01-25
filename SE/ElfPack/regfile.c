#include "..\\include\Lib_Clara.h"
#include "vars.h"



extern __thumb long elfload(const unsigned short *filename, void *param1, void *param2, void *param3);
extern EP_DATA * getepd(void);



const u16 ext[]={L"elf"};
const char ctype[]={"application/elf"};

const int ex[2]={(int)ext,0};
const int ct[2]={(int)ctype,0};

const char ers[]={"Elf_Run_Subroutine"};

const DB_EXT_FOLDERS dbfolders={STR_APPLICATION,STR_Saved_on_Memory_Stick,PATH_ELF_ROOT_EXT,
0,STR_APPLICATION,STR_Saved_in_phone_memory,PATH_ELF_ROOT_INT,1,0x6FFFFFFF,0x6FFFFFFF,0,0};




__root void onEv(FILESUBROUTINE ** r0)
{
  EP_DATA *epd=getepd();
  *r0=epd->elf_ext_m;
}


__thumb void SetSmallIcon(void * r0, u16 *r1)
{
  *r1=ELF_SMALL_ICON;
}



__arm int Elf_Run(void * r0, BOOK * book)
{
  char ch[500];
  DATA * data = (DATA * )book->gui;
  FILETYPE *ft = data->unk1;
  u16 * filename = malloc((wstrlen(ft->fpath)+wstrlen(ft->fname)+2)*2);

  wstrcpy(filename,ft->fpath);
  wstrcat(filename,L"/");
  wstrcat(filename,ft->fname);

  wstr2strn(ch,filename,499);

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

const DB_EXT dbe_ELF={(char**)ct,(u16**)ex,onEv,DB_DB_EXT_C1,&dbfolders,0x0,0x00};


__arm int Elf_Run_Subroutine(void * r0)
{
  BookObj_CallSubroutine(((DATA*)(r0))->book,(void*)subrout);
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
    fsnew->StrID=STR_START;
    // забираем к себе старые методы
    memcpy(fsnew+1,fsr,j*sizeof(FILESUBROUTINE));
    fs=fsnew;
    // проходимся по всем, и кого надо, правим..
    while (fs->PROC)
    {
      if (fs->cmd==DB_CMD_SETSMALLICON) fs->PROC=(int(*)(void*))SetSmallIcon;
      fs++;
    }
    epd->elf_ext_m=fsnew;
    {
      DB_EXT ** m;
      i=0;
      while  (epd->dbe[i++]!=LastExtDB());

      m=malloc((i+1)*sizeof(DB_EXT *));
      memcpy(m,epd->dbe,(i)*sizeof(DB_EXT *));

      m[i]=m[i-1];
      m[i-1]=(DB_EXT*)&dbe_ELF;
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
