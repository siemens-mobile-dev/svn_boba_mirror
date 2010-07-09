//#define K750_R1CA021

#include "..\\include\Dir.h"
#include "..\\include\Types.h"
#include "..\\include\dll.h"
#include "calls.h"
#include "vars.h"


#define EPDVARNAME "elfpackdata"


__no_init int (*pKBD) (int,int,int) @ "OLD_KEY_HANDLER";

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
void * LoadDLL(wchar_t * DllName);
//-------



EP_DATA * getepd(void)
{
	return (EP_DATA *)get_envp(get_bid(current_process()),EPDVARNAME);
}


__thumb int CmpProc(void * e1, void * e2)
{
	UDATA * ud = (UDATA*)e1;
	if ((int)ud->constr==(int)e2) return (0); else  return(1);
}

__thumb void * GetUserData (int size,void (*constr)(void *, void *), void *param)
{
	
	UDATA * ud;
	__get_epd;
	int index;
	
	_printf("GetUserData(0x%X,0x%X,0x%X)",size,constr,param);
	index=List_Find(epd->UserDataList,(void*)constr,CmpProc);
	if (index!=0xFFFF)
	{
		ud=List_Get(epd->UserDataList,index);
		_printf("UserData  @0x%X",ud->data);
	}
	else
	{
		if (constr)
		{
			ud=malloc(sizeof(UDATA));
			ud->constr=constr;
			ud->data=malloc(size);
			List_InsertFirst(epd->UserDataList,ud);
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
	
	index=List_Find(epd->UserDataList,(void*)constr,CmpProc);
	if (index!=0xFFFF)
	{
		ud=List_RemoveAt(epd->UserDataList,index);
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
#ifdef DB2020
	return(memalloc(0,size,1,5,"SwiLib",0));
#elif A2
	return(memalloc(0xFFFFFFFF,size,1,5,"SwiLib",0));
#else
	return(memalloc(size,1,5,"SwiLib",0));
#endif
}

__thumb void mfree (void * mem)
{
#ifdef DB2020
	if (mem) memfree(0, mem,"SwiLib",0);
#elif A2
	if (mem) memfree(0, mem,"SwiLib",0);
#else
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
	case  DIR_IMAGE>>1 : return (DirIndex&1)? (u16*)PATH_IMAGE_INT: (u16*)PATH_IMAGE_EXT;
	case  DIR_VIDEO>>1 : return (DirIndex&1)? (u16*)PATH_VIDEO_INT: (u16*)PATH_VIDEO_EXT;
	case  DIR_THEME>>1 : return (DirIndex&1)? (u16*)PATH_THEME_INT: (u16*)PATH_THEME_EXT;
	case  DIR_OTHER>>1 : return (DirIndex&1)? (u16*)PATH_OTHER_INT: (u16*)PATH_OTHER_EXT;
	
	case  DIR_ELFS>>1 : return (DirIndex&1)? PATH_ELF_INT: PATH_ELF_EXT;
	case  DIR_ELFS_DAEMONS>>1 : return (DirIndex&1)? PATH_ELF_DAEMONS_INT: PATH_ELF_DAEMONS_EXT;
	case  DIR_ELFS_CONFIG>>1 : return (DirIndex&1)? PATH_ELF_CONFIG_INT: PATH_ELF_CONFIG_EXT;
	case  DIR_INI>>1 : return PATH_INI;
	case  DIR_DLL>>1: return PATH_DLL;
	
	case  DIR_USER>>1 : return (DirIndex&1)? (u16*)PATH_USER_INT: (u16*)PATH_USER_EXT;
	
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
	mfree(path);
	return len;
}


//===============  KBD_HOOK  ================


__thumb int ModifyKeyHook(int(*proc)(int,int,int),int mode)
{
	__get_epd;
	int i=0;
	
	_printf("ModifyKeyHook PROC@0x%x, mode=0x%x",proc,mode)  ;
	
	i=List_IndexOf(epd->gKbdHookList,(void*)proc);
	switch (mode)
	{
	case 0: //remove
		if (i!=0xFFFF)
		{
			List_RemoveAt(epd->gKbdHookList,i);
			
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
			List_InsertFirst(epd->gKbdHookList,(void*)proc);
			
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
	
	int n=List_Find(OSE_Hook_List,(void*)PROC,OSEHookCmpProc);
	
	switch (mode)
	{
	case 0: //remove
		if (n!=0xFFFF)
		{
			mfree(List_RemoveAt(OSE_Hook_List,n));
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
			List_InsertFirst(OSE_Hook_List,(void*)item);
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

__thumb UI_MESSAGE * PageAction_Hook(UI_MESSAGE * ui)
//__thumb void UI_Message_Hook(LIST * lst, UI_MESSAGE *ui)
{
	int f=0;
	int n;
	
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

__thumb int ModifyUIHook(int event , int (*PROC)(UI_MESSAGE*),int mode)
{
	__get_epd;
	LIST * UI_Hook_List=epd->UIHookList;
	
	int n=List_Find(UI_Hook_List,(void*)PROC,UIHookCmpProc);
	
	_printf("ModifyUIHook PROC@0x%x, mode=0x%x",PROC,mode)  ;
	
	switch (mode)
	{
	case 0: //remove
		if (n!=0xFFFF)
		{
			mfree(List_RemoveAt(UI_Hook_List,n));
			
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
			List_InsertFirst(UI_Hook_List,(void*)item);
			
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
// ===============================================================

typedef struct {
	int (*PROC)(void *msg, BOOK *bk);
	int event;
	int book_id;
} PAGE_HOOK_ELEM;

__thumb void PageAction_Hook1(LIST *lst)
{
	__get_epd;
	if (epd)
	{
		LIST *UIPageHook=epd->UIPageHook;
		int n_pages=UIPageHook->FirstFree;
		while(n_pages--)
		{
			PAGE_HOOK_ELEM *my_act=(PAGE_HOOK_ELEM *)List_Get(UIPageHook,n_pages);
			int f=0;
			int n=lst->FirstFree;
			while(n--)
			{
				ACTION *act=(ACTION *)List_Get(lst,n);
				if (act->event==my_act->event)
				{
					f=1;
					break;
				}
			}
			if (f)
			{
				ACTION *new_act = ActionCreate(my_act->PROC, my_act->book_id, my_act->event, NULL, NULL);
				List_InsertFirst(lst, new_act);
			}
		}
	}
}


__thumb int UIHook1CmpProc(void * e1, void * e2)
{
	ACTION * ud = (ACTION*)e1;
	PAGE_HOOK_ELEM *page_elem=(PAGE_HOOK_ELEM *)e2;
	if (ud->PROC==page_elem->PROC && ud->event==page_elem->event) return (0); else  return(1);
}

__thumb int ModifyUIHook1(int event , int (*PROC)(void *msg, BOOK *bk), int book_id ,int mode)
{
	__get_epd;
	LIST * UIPageHook=epd->UIPageHook;
	PAGE_HOOK_ELEM page_elem;
	page_elem.PROC=PROC;
	page_elem.event=event;
	page_elem.book_id=book_id;
	int n=List_Find(UIPageHook,&page_elem,UIHook1CmpProc);
	_printf("ModifyUIHook PROC@0x%x, mode=0x%x",PROC,mode)  ;
	
	switch (mode)
	{
	case 0: //remove
		if (n!=0xFFFF)
		{
			mfree(List_RemoveAt(UIPageHook,n));
			return(0);
		}
		else
		{
			return(-2);
		}
		
	case 1:
		if (n==0xFFFF)
		{
			ACTION *item=malloc(sizeof(PAGE_HOOK_ELEM));
			memcpy(item,&page_elem,sizeof(PAGE_HOOK_ELEM));
			List_InsertLast(UIPageHook,item);
			return(0);
		}
		else
		{
			return(-3);
		}
	default: return(-4);
	}
}


extern LIST *CreateDBExtList();
extern DB_EXT * CreateDbExt();

void CreateLists(void)
{
	
	EP_DATA * epd = malloc(sizeof(EP_DATA));
	memset(epd,0,sizeof(EP_DATA));
	set_envp(get_bid(current_process()),EPDVARNAME,(OSADDRESS)epd);
	
	_printf("EP_DATA @%x",epd)  ;
	
	epd->UserDataList=List_Create();
	epd->gKbdHookList=List_Create();
	epd->UIHookList=List_Create();
	epd->OseHookList=List_Create();
	epd->DLLList=List_Create();
	epd->UIPageHook=List_Create();
        epd->LibraryCache=malloc(0x4000);
	memcpy(epd->LibraryCache,(void*)Library_Start,0x4000);
	
	_printf("   epd->UserDataList @%x",epd->UserDataList)  ;
	_printf("   epd->gKbdHookList @%x",epd->gKbdHookList)  ;
	_printf("   epd->UIHookList @%x",epd->UIHookList)  ;
	_printf("   epd->OseHookList @%x",epd->OseHookList)  ;
	//  _printf("   epd->elflist @%x",epd->elflist)  ;
	
	epd->DBExtList=CreateDBExtList();
	epd->CreateDbExt = CreateDbExt;
	epd->IconSmall = NOIMAGE;
	epd->IconBig = NOIMAGE;
	epd->LibraryDLL = 0x0;
	
	ELFExtrRegister(epd);
}




__thumb void Init()
{
	
	//CreateLists();
	
	char * mem = malloc(0x300);
	
	FILELISTITEM * fli;
	__get_epd;
	
	LIST * gkhook;
	
	_printf("     Entered Init...")  ;
	
	// отняли обработчик клавы
	gkhook=epd->gKbdHookList;
	List_InsertFirst(gkhook,(void*)pKBD);
	pKBD=KbdHook;
	
	_printf("     Changing KBD Hook (LIST@0x%X OldProc=0x%x)",gkhook,(void*)pKBD)  ;
	
	// запустили хелпера
	
	_printf("     StartHelper....")  ;
	
	StartHelper();
	
	_printf("     StartHelper OK. PID=%x",epd->HPID)  ;

	// инитим DLL библиотеку

	_printf("     Load LibraryDLL....")  ;

	epd->LibraryDLL = LoadDLL(L"LibraryDLL.dll");
        if ((INVALID(epd->LibraryDLL)))
        {
          _printf("     Load LibraryDLL Error")  ;
          epd->LibraryDLL = 0;
        }
	else _printf("     Load LibraryDLL OK")  ;
	
        // правим кэш либы
        
        _printf("     Patching LibraryCache....")  ;
        
typedef struct
{
  int num;
  void * func;
}LIBPATCH_ELEM;

        if (epd->LibraryDLL)
        {
          LIBPATCH_ELEM * lp = (LIBPATCH_ELEM*)((char*)epd->LibraryDLL + 0x4);
          while (lp->num!=-1)
          {
            if (lp->func) epd->LibraryCache[lp->num]=lp->func;
            lp++;
          }
        }
        
        _printf("     Patching LibraryCache OK")  ;
        
	// запустили демонов
	
	_printf("     StartDaemons....")  ;
	_printf("     ------Begin List-------")  ;

#ifdef DAEMONS_INTERNAL
	int * handle=AllocDirHandle(GetDir(DIR_ELFS_DAEMONS | MEM_INTERNAL));
#else
	int * handle=AllocDirHandle(GetDir(DIR_ELFS_DAEMONS | MEM_EXTERNAL));
#endif
	
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
	
	
	if (handle) DestroyDirHandle(handle);
	mfree(mem);

	_printf("     Exit Init")  ;
}

__root  const int PATCH_AUTO_RUN @ "PATCH_AUTO_RUN" =(int)PAGE_ENTER_EVENT;
__root  const int PATCH_AUTO_RUN1 @ "PATCH_AUTO_RUN1" =(int)Init;

int GetExtTable()
{
	__get_epd;
	return((int)epd->DBExtList->listdata);
}
