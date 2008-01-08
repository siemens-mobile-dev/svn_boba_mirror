//#define K750_R1CA021

#include "..\\include\k750.h"
#include "..\\include\Dir.h"


#ifdef DBG
#define _printf(...) {debug_printf("\nOS: ElfPack -> ");debug_printf(__VA_ARGS__);}
#else
#define _printf(...)
#endif



void StartHelper(void);

typedef struct
{
  void (* constr)(void *, void *);
  void * data;
}UDATA;

void * malloc (int size);
void mfree (void * mem);
void HelperEntryPoint(void);

//-------
typedef const unsigned short *FILENAME;
__thumb long elfload(FILENAME filename, void *param1, void *param2, void *param3);
//-------



EP_DATA * getepd(void)
{
  EP_DATA * epd = (EP_DATA *)*((EP_DATA **)RAM_BASE);
  return(epd);
}


__thumb int CmpProc(void * e1, void * e2)
{
  UDATA * ud = (UDATA*)e1;
  if ((int)ud->constr==(int)e2) return (0); else  return(1);
}

__thumb void * GetUserData (int size,void (*constr)(void *, void *), void *param)
{

  UDATA * ud;
  //  EP_DATA * epd = (EP_DATA *)*((EP_DATA **)RAM_BASE);
  __get_epd;
  int index;

  _printf("GetUserData(0x%X,0x%X,0x%X)",size,constr,param);
  index=ListElement_Find(epd->UserDataList,(void*)constr,CmpProc);
  if (index!=0xFFFF)
  {
    ud=ListElement_GetByIndex(epd->UserDataList,index);
    _printf("UserData  @0x%X",ud->data);
  }
  else
  {
    if (constr)
    {
      ud=malloc(sizeof(UDATA));
      ud->constr=constr;
      ud->data=malloc(size);
      ListElement_AddtoTop(epd->UserDataList,ud);
      constr(ud->data,param);

      _printf("New UserData  @0x%X",ud->data);
      _printf("Total Used Bloks  0x%%X",epd->UserDataList->FirstFree);


    }
    else
    {

      _printf("Error. GetUserData() without Constructor!!");
      // нет конструктора
      return(0);
    }
  }
  return(ud->data);

};



__thumb int RemoveUserData(void (*constr)(void *, void *))
{
  __get_epd;
  UDATA * ud;
  int index;

  _printf("RemoveUserData(0x%X)",constr);

  index=ListElement_Find(epd->UserDataList,(void*)constr,CmpProc);
  if (index!=0xFFFF)
  {
    ud=ListElement_Remove(epd->UserDataList,index);
    {
      mfree(ud->data);
      mfree(ud);

      _printf("Removed OK...");

      return(0);

    }
  }
  else
  {

    _printf("0x%X Not in List!!",constr);

    // нет элемента в списке
    return(1);
  }

}

//============================================================================

__thumb void * malloc (int size)
{
#ifdef K750_R1CA021
  return(memalloc(size,1,5,"SwiLib",0));
#endif

#ifdef W700_R1CA021
  return(memalloc(size,1,5,"SwiLib",0));
#endif

#ifdef W810_R4DB005
  return(memalloc(size,1,5));
#endif

#ifdef W810_R4EA031
  return(memalloc(size,1,5));
#endif

#ifdef Z550_R6CA009
  return(memalloc(size,1,5));
#endif

#ifdef W800_R1BC002
  return(memalloc(size,1,5,"SwiLib",0));
#endif

#ifdef W800_R1BD001
  return(memalloc(size,1,5,"SwiLib",0));
#endif

#ifdef K600_R2BB001
  return(memalloc(size,1,5,"SwiLib",0));
#endif

}

__thumb void mfree (void * mem)
{
#ifdef K750_R1CA021
  memfree(mem,"SwiLib",0);
#endif

#ifdef W700_R1CA021
  memfree(mem,"SwiLib",0);
#endif

#ifdef W810_R4DB005
  memfree(mem);
#endif

#ifdef W810_R4EA031
  memfree(mem);
#endif

#ifdef Z550_R6CA009
  memfree(mem);
#endif

#ifdef W800_R1BC002
  memfree(mem,"SwiLib",0);
#endif

#ifdef W800_R1BD001
  memfree(mem,"SwiLib",0);
#endif

#ifdef K600_R2BB001
  memfree(mem,"SwiLib",0);
#endif


}
//============================================================================

__thumb u16 * GetDir (int DirIndex)
{

  _printf("GetDir(0x%X)",DirIndex);

  switch (DirIndex>>1)
  {
  case  DIR_AUDIO>>1 : return (DirIndex&1)? (u16*)PATH_AUDIO_INT: (u16*)PATH_AUDIO_EXT;
  case  DIR_IMAGE>>1 : return (DirIndex&1)? (u16*)PATH_IMAGE_INT: (u16*)PATH_VIDEO_EXT;
  case  DIR_VIDEO>>1 : return (DirIndex&1)? (u16*)PATH_VIDEO_INT: (u16*)PATH_VIDEO_EXT;
  case  DIR_THEME>>1 : return (DirIndex&1)? (u16*)PATH_THEME_INT: (u16*)PATH_THEME_EXT;
  case  DIR_OTHER>>1 : return (DirIndex&1)? (u16*)PATH_OTHER_INT: (u16*)PATH_OTHER_EXT;


  case	DIR_ELFS>>1 :
    return (DirIndex&1)? PATH_ELF_INT: PATH_ELF_EXT;

    case	DIR_ELFS_DAEMONS>>1 :
      return (DirIndex&1)? PATH_ELF_DAEMONS_INT: PATH_ELF_DAEMONS_EXT;

      case	DIR_ELFS_CONFIG>>1 :
        return (DirIndex&1)? PATH_ELF_CONFIG_INT: PATH_ELF_CONFIG_EXT;

        case	DIR_INI>>1 :
          return PATH_INI;

  default: return(PATH_DEFAULT);
  }
}

__thumb int fopen (const u16 * fname, int mode, int rights)
{
  u16 *path;
  u16 *name;
  int len;
  name=wstrrchr(fname,0x2F);
  if (!name) return -1;
  path=malloc(((len=name-fname)+1)*sizeof(u16));
  memcpy(path,fname,len*sizeof(u16));
  path[len]=0;
  name++;
  len=_fopen(path,name,mode,rights,NULL);
  //  len=_fopen(path,name,1,0x1FF,NULL);
  mfree(path);
  return len;
}

//============================================================================

typedef struct
{
  void * unk;
  void * unk1;
  BOOK * book;
}DATA;

//void main();

void onEv(FILESUBROUTINE ** r0);
void SetSmallIcon(void * r0, u16 *r1);
int Elf_Run_Subroutine(void *r0);
int Elf_Run(void * r0, BOOK * book);

const char ers[]={"Elf_Run_Subroutine"};

const PAGE_MSG erp_msg[]={
  0x05,(int*)Elf_Run,
  0x10,(int*)ELF_RUN_PAGE_EV_0x10 ,
  0x0F,(int*)ELF_RUN_PAGE_EV_0x0F ,
  0x11,(int*)ELF_RUN_PAGE_EV_0x11 ,
  0x06,(int*)ELF_RUN_PAGE_EV_0x06 ,
  0,0
};

const PAGE_DESC erun_page={"Elf_Run_Page",0,erp_msg};

const int subrout[]={(int)&ers,(int)&erun_page,0,0};


const u16 ext[]={L"elf"};
const char ctype[]={"application/elf"};

const int ex[2]={(int)ext,0};
const int ct[2]={(int)ctype,0};


typedef struct
{
  u16* fname;
  u16* fpath;
}FILETYPE;



const DB_EXT_FOLDERS dbfolders={STR_APPLICATION,STR_Saved_on_Memory_Stick,PATH_ELF_ROOT_EXT,
0,STR_APPLICATION,STR_Saved_in_phone_memory,PATH_ELF_ROOT_INT,1,NULL_STR,NULL_STR,0,0};

const DB_EXT dbe_ELF={(char**)ct,(u16**)ex,onEv,DB_DB_EXT_C1,&dbfolders,0x0,0x00};


__root void onEv(FILESUBROUTINE ** r0)
{
  __get_epd;
  *r0=epd->elf_ext_m;
}


__thumb void SetSmallIcon(void * r0, u16 *r1)
{
  *r1=ELF_SMALL_ICON;
}


__thumb int Elf_Run_Subroutine(void * r0)
{
  BookObj_CallSubroutine(((DATA*)(r0))->book,(void*)subrout);
  return(1);
};


__thumb int Elf_Run(void * r0, BOOK * book)
{
  char ch[500];
  DATA * data = (DATA * )book->gui;
  FILETYPE *ft = data->unk1;
  u16 * filename = malloc((wstrlen(ft->fpath)+wstrlen(ft->fname)+2)*2);

  wstrcpy(filename,ft->fpath);
  wstrcat(filename,L"/");
  wstrcat(filename,ft->fname);

  wstr2str(ch,filename);

  _printf("Starting %s",ch)  ;

  {
    //    int err;
    //    err=
    elfload(filename,0,0,0);

    //    StatusIndication_ShowNotes(int2strID(elfload(filename,0,0,0)));
  }


  mfree(filename);
  BookObj_ReturnPage(book,0x10);
  return(1);
};


//===============  KBD_HOOK  ================


__thumb int ModifyKeyHook(int(*proc)(int,int,int),int mode)
{
  __get_epd;
  int i=0;

  _printf("ModifyKeyHook PROC@0x%x, mode=0x%x",proc,mode)  ;

  i=ListElement_Prt2NumElement(epd->gKbdHookList,(void*)proc);
  switch (mode)
  {
  case 0: //remove
    if (i!=0xFFFF)
    {
      ListElement_Remove(epd->gKbdHookList,i);

      _printf("PROC(0x%x) Removed..",proc)  ;

      return(0);
    }
    else
    {

      _printf("PROC(0x%x) Not in List !!!",proc)  ;

      return(-2);
    }
  case 1:
    if (i==0xFFFF)
    {
      ListElement_AddtoTop(epd->gKbdHookList,(void*)proc);

      _printf("PROC(0x%x) Added in List",proc)  ;

      return(0);
    }
    else
    {

      _printf("Warning! PROC(0x%x) was in List",proc)  ;

      return(-3);
    }
  default:
    {

      _printf("Error. Wrong mode(0x%x). Use 0/1 for remove/add",proc)  ;


      return(-4);
    }
  }

}

__thumb int KbdHook(int r0,int r1,int r2)
{
  __get_epd;
  int i=0;
  int result;
  int(*proc)(int,int,int);
  while(i<epd->gKbdHookList->FirstFree)
  {
    proc=(int(*)(int,int,int))(epd->gKbdHookList->listdata[i++]);
    result=proc(r0,r1,r2);
    if (result==-1) return(0);
  }
  return(0);
}
//===============  OSE_HOOK  ================

__thumb int OSEHookCmpProc(void * e1, void * e2)
{
  OSE_HOOK_ITEM * ud = (OSE_HOOK_ITEM*)e1;
  if ((int)ud->HOOK==(int)e2) return (0); else  return(1);
}

__thumb int ModifyOSEHook(int event , void (*PROC)(void*),int mode)
{
  __get_epd;
  LIST * OSE_Hook_List=epd->OseHookList;

  int n=ListElement_Find(OSE_Hook_List,(void*)PROC,OSEHookCmpProc);

  switch (mode)
  {
  case 0: //remove
    if (n!=0xFFFF)
    {
      mfree(ListElement_Remove(OSE_Hook_List,n));
      return(0);
    }
    else
    {
      return(-2);
    }

  case 1:
    if (n==0xFFFF)
    {
      OSE_HOOK_ITEM *item=malloc(sizeof(OSE_HOOK_ITEM));
      item->signo=event;
      item->HOOK=PROC;
      ListElement_AddtoTop(OSE_Hook_List,(void*)item);
      return(0);
    }
    else
    {
      return(-3);
    }
  default: return(-4);
  }
}

//===============  UI_HOOK  ================

/*
__thumb int PageAction_Hook(UI_MESSAGE * ui, BOOK * book, ACTION * action)
//__thumb void UI_Message_Hook(LIST * lst, UI_MESSAGE *ui)
{
int f=0;
int n;
int uiev=action->event;

__get_epd;
if (epd)
{
LIST * UI_Hook_List=epd->UIHookList;
_printf("PageAction_Hook msg:0x%X",action->event)  ;
n=UI_Hook_List->FirstFree;

if (n)
{
n--;
UI_HOOK_ITEM ** uihook = (UI_HOOK_ITEM**)(UI_Hook_List->listdata);
do
{
{
UI_HOOK_ITEM * item=uihook[n];
_printf("PageAction 0x%x 0x%x",item->event,uiev)  ;
{
if (item->event==uiev) f|=item->HOOK(ui,book);
          }
        }
      }
while(n--);
    }
  }
if (!f) return(action->PROC(ui,book)); else return(f);
}
*/
__thumb UI_MESSAGE * PageAction_Hook(UI_MESSAGE * ui)
//__thumb void UI_Message_Hook(LIST * lst, UI_MESSAGE *ui)
{
  int f=0;
  int n;
  //  int uiev=action->event;

  __get_epd;
  if (epd)
  {
    LIST * UI_Hook_List=epd->UIHookList;
    _printf("PageAction_Hook msg:0x%X",ui->event)  ;
    n=UI_Hook_List->FirstFree;

    if (n)
    {
      n--;
      UI_HOOK_ITEM ** uihook = (UI_HOOK_ITEM**)(UI_Hook_List->listdata);
      do
      {
        {
          UI_HOOK_ITEM * item=uihook[n];
          //          _printf("PageAction 0x%x 0x%x",item->event,uiev)  ;
          {
            if (item->event==ui->event) f|=item->HOOK(ui);
          }
        }
      }
      while(n--);
    }
  }
  if (f) ui->mode=0xFF;
  return(ui);
}


__thumb int UIHookCmpProc(void * e1, void * e2)
{
  UI_HOOK_ITEM * ud = (UI_HOOK_ITEM*)e1;
  if ((int)ud->HOOK==(int)e2) return (0); else  return(1);
}
/*
__thumb int ModifyUIHook(int event , int (*PROC)(UI_MESSAGE*,BOOK * book),int mode)
{
__get_epd;
LIST * UI_Hook_List=epd->UIHookList;

int n=ListElement_Find(UI_Hook_List,(void*)PROC,UIHookCmpProc);

_printf("ModifyUIHook PROC@0x%x, mode=0x%x",PROC,mode)  ;

switch (mode)
{
  case 0: //remove
if (n!=0xFFFF)
{
mfree(ListElement_Remove(UI_Hook_List,n));

_printf("PROC@0x%X, Removed OK..",PROC)  ;

return(0);
    }
    else
{
return(-2);
    }

  case 1:
if (n==0xFFFF)
{
UI_HOOK_ITEM *item=malloc(sizeof(UI_HOOK_ITEM));
item->event=event;
item->HOOK=PROC;
ListElement_AddtoTop(UI_Hook_List,(void*)item);

_printf("PROC@0x%X, Added OK..",PROC)  ;
_printf("Total HookItems : 0x%X",UI_Hook_List->FirstFree)  ;

return(0);
    }
    else
{
return(-3);
    }
default: return(-4);
  }
}
*/


__thumb int ModifyUIHook(int event , int (*PROC)(UI_MESSAGE*),int mode)
{
  __get_epd;
  LIST * UI_Hook_List=epd->UIHookList;

  int n=ListElement_Find(UI_Hook_List,(void*)PROC,UIHookCmpProc);

  _printf("ModifyUIHook PROC@0x%x, mode=0x%x",PROC,mode)  ;

  switch (mode)
  {
  case 0: //remove
    if (n!=0xFFFF)
    {
      mfree(ListElement_Remove(UI_Hook_List,n));

      _printf("PROC@0x%X, Removed OK..",PROC)  ;

      return(0);
    }
    else
    {
      return(-2);
    }

  case 1:
    if (n==0xFFFF)
    {
      UI_HOOK_ITEM *item=malloc(sizeof(UI_HOOK_ITEM));
      item->event=event;
      item->HOOK=PROC;
      ListElement_AddtoTop(UI_Hook_List,(void*)item);

      _printf("PROC@0x%X, Added OK..",PROC)  ;
      _printf("Total HookItems : 0x%X",UI_Hook_List->FirstFree)  ;

      return(0);
    }
    else
    {
      return(-3);
    }
  default: return(-4);
  }
}

void AddExt(DB_EXT * dbe)
{
  __get_epd;
  //  DB_EXT * last_ext;
  DB_EXT ** m;
  int i=0;
  if (epd->dbe)
  {
    while  (epd->dbe[i]->content_type) i++;
    //  last_ext=epd->dbe[i];
    i++;
    m=malloc((i+1)*4);

    memcpy(m,epd->dbe,i*4);
    m[i]=m[i-1];
    m[i-1]=dbe;

    mfree(epd->dbe);
    epd->dbe=m;
    _printf("   %X %X %X %X %X %X",i,(i+1)*4,m,m[i],m[i-1],epd->dbe)  ;
  }
}


void ELFExtrRegister(void)
{
  FILESUBROUTINE * fsr;
  FILESUBROUTINE * fs;
  FILESUBROUTINE * fsnew;
  __get_epd;
  int i=0;
  int j=0;

  if (epd->dbe)
  {
    _printf("     epd->dbe @0x%X",epd->dbe)  ;

    // находим other
    while  (*(epd->dbe[i]->content_type[0])) i++;

    // берем его методы
    epd->dbe[i]->GetMethods(&fsr);
    fs=fsr;
    // считаем их
    while (fs->PROC) {fs++;j++;}
    // выделяем память под n+1
    _printf("     meth @0x%X items 0x%X",fsr,j+1)  ;

    fsnew=malloc((j+2)*sizeof(FILESUBROUTINE));
    _printf("     FILESUBROUTINE @0x%X(sz 0x%X)",fsnew,(j+2)*sizeof(FILESUBROUTINE))  ;
    // первым вставляем своё
    fsnew->cmd=1;
    fsnew->PROC=Elf_Run_Subroutine;
    fsnew->PROC1=(int(*)(void*,void*))RUN_CHECK;
    fsnew->StrID=STR_START;
    // забираем к себе старые методы
    fs=fsnew+1;
    memcpy(fs,fsr,(j+1)*sizeof(FILESUBROUTINE));
    fs=fsnew;
    // проходимся по всем, и кого надо, правим..
    while (fs->PROC)
    {
      if (fs->cmd==DB_CMD_SETSMALLICON) fs->PROC=(int(*)(void*))SetSmallIcon;
      fs++;
    }
    epd->elf_ext_m=fsnew;
    {
      DB_EXT **ext_desc=epd->dbe;
      DB_EXT ** m;
      i=0;
      while (ext_desc[i]->content_type) i++;

      m=malloc((i+2)*4);
      memcpy(m,epd->dbe,(i+1)*4);

      m[i+1]=m[i];
      m[i]=(DB_EXT*)&dbe_ELF;
      mfree(epd->dbe);
      epd->dbe=m;
    }
  }
}

/*

void AssociateELF(void)
{
FILESUBROUTINE * fsr;
__get_epd;
int i=0;
int j;
if (epd->dbe)
{
while  (epd->dbe[i]->content_type[0]) i++;
  }
epd->dbe[i]->PROC(&j);



}

*/
void CreateLists(void)
{
  EP_DATA ** p=(EP_DATA**)RAM_BASE;
  EP_DATA * epd = malloc(sizeof(EP_DATA));
  *p=epd;

  _printf("EP_DATA @%x",epd)  ;

  epd->UserDataList=List_New();
  epd->gKbdHookList=List_New();
  epd->UIHookList=List_New();
  epd->OseHookList=List_New();
  //  epd->elflist=List_New();

  _printf("   epd->UserDataList @%x",epd->UserDataList)  ;
  _printf("   epd->gKbdHookList @%x",epd->gKbdHookList)  ;
  _printf("   epd->UIHookList @%x",epd->UIHookList)  ;
  _printf("   epd->OseHookList @%x",epd->OseHookList)  ;
  //  _printf("   epd->elflist @%x",epd->elflist)  ;
  {

    int i=0;
    DB_EXT ** m;
    DB_EXT **tab= (DB_EXT **)EXT_TABLE;

    while (tab[i]->content_type) i++;
    i++;
    m=malloc(i*4);
    memcpy(m,tab,i*4);
    epd->dbe=m;
    _printf("     New ExtTable @0x%X, total ext 0x%X",m,i)  ;


    ELFExtrRegister();

  }
}




__thumb void Init()
{

  CreateLists();

  char * mem = malloc(0x300);

#ifdef DAEMONS_INTERNAL
  int * handle=AllocDirHandle(GetDir(DIR_ELFS_DAEMONS | MEM_INTERNAL));
#else
  int * handle=AllocDirHandle(GetDir(DIR_ELFS_DAEMONS | MEM_EXTERNAL));
#endif

  FILELISTITEM * fli;
  __get_epd;

  LIST * gkhook;

  _printf("     Entered Init...")  ;

  // отняли обработчик клавы
  gkhook=epd->gKbdHookList;
  ListElement_AddtoTop(gkhook,(void*)*((int(**)(int,int,int))pKBD));
  *((int(**)(int,int,int))pKBD)=KbdHook;

  _printf("     Changing KBD Hook (LIST@0x%X OldProc=0x%x)",gkhook,(void*)*((int(**)(int,int,int))pKBD))  ;

  // запустили хелпера

  _printf("     StartHelper....")  ;

  StartHelper();

  _printf("     StartHelper OK. PID=%x",epd->HPID)  ;
  //  StatusIndication_ShowNotes(int2strID(epd->HPID));


  // запустили демонов

  _printf("     StartDaemons....")  ;
  _printf("     ------Begin List-------")  ;

  do
  {
    if(fli=GetFname(handle,mem))
    {	
      u16 * filename = malloc((wstrlen(fli->path)+wstrlen(fli->fname)+2)*2);
      wstrcpy(filename,fli->path);
      wstrcat(filename,L"/");
      wstrcat(filename,fli->fname);

      _printf("     ->...")  ;
#ifdef DBG
      _printf("     elfload Result=0x%X",elfload(filename,0,0,0));
#else
      elfload(filename,0,0,0);
#endif
      mfree(filename);
    }
  }
  while(fli);

  _printf("     ------End List-------")  ;


  mfree(handle);
  mfree(mem);

  _printf("     Exit Init")  ;
}

/*
static void _ELF_RemoveFromList(int dummy, void *elf_begin)
{
__get_epd;
LIST * List=epd->elflist;
ELF_DESC * ed = ListElement_Remove(List,ListElement_Prt2NumElement(List,elf_begin));
//  mfree(ed->fname);
mfree(ed);
_printf("     _ELF_RemoveFromList OK..");

}
*/
/*
void ELF_RemoveFromList(void *elf_begin)
{

void MMIPROCExec (void(*PROC)(int,void*),int p1 , void * p2);

_printf("     MMIPROCExec(_ELF_RemoveFromList,0,%x)",elf_begin)  ;

MMIPROCExec(_ELF_RemoveFromList,0,elf_begin);
}
*/
__root  const int PATCH_AUTO_RUN @ "PATCH_AUTO_RUN" =(int)5;
__root  const int PATCH_AUTO_RUN1 @ "PATCH_AUTO_RUN1" =(int)Init;

int GetExtTable()
{
  __get_epd;
  return((int)epd->dbe);
}
/*
__root  const u16 PATCH_DB_INT[] @ "PATCH_DB_INT" ={L"/tpa/user/exec/ZBin"};
__root  const u16 PATCH_DB_EXT[] @ "PATCH_DB_EXT" ={L"/card/MSSEMC/Media files/exec/ZBin"};
*/
/*
const u16 eint[]={L"/tpa/user/ZBin"};
const u16 eext[]={L"/card/MSSEMC/Media files/ZBin"};
//static const u16* eext={L"/card/MSSEMC/Media files/ZBin"};
const int efolders[3] ={(int)eint,(int)eext,0};

__root  const int PATCH_DB_INT @ "PATCH_DB_INT" =(int)eint;
__root  const int PATCH_DB_EXT @ "PATCH_DB_EXT" =(int)eext;
__root  const int PATCH_DB_THEME_ITEM @ "PATCH_DB_THEME_ITEM" =(int)0x14E2;
__root  const int PATCH_DB_THEME_HEADER @ "PATCH_DB_THEME_HEADER" =(int)0x14DE;
*/
