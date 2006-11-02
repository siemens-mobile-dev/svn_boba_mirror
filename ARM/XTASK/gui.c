#include "..\inc\swilib.h"
#include "conf_loader.h"

extern int mode;
extern CSM_RAM *under_idle;

extern const char BM1NAME[32];
extern const char BM1FILE[128];
extern const char BM2NAME[32];
extern const char BM2FILE[128];
extern const char BM3NAME[32];
extern const char BM3FILE[128];
extern const char BM4NAME[32];
extern const char BM4FILE[128];
extern const char BM5NAME[32];
extern const char BM5FILE[128];
extern const char BM6NAME[32];
extern const char BM6FILE[128];
extern const char BM7NAME[32];
extern const char BM7FILE[128];
extern const char BM8NAME[32];
extern const char BM8FILE[128];
extern const char BM9NAME[32];
extern const char BM9FILE[128];

volatile int do_idle;

const CSM_DESC maincsm;

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  //  int i1;
}MAIN_GUI;

int my_csm_id;

int cursor;
void * volatile selcsm;

typedef struct
{
  void *next;
  WSHDR *name;
  void *p;
} NAMELIST;

NAMELIST * volatile nltop;
NAMELIST * volatile nlbot;

char csm_text[32768];

const char percent_t[]="%t";

void ClearNL(void)
{
  LockSched();
  NAMELIST *nl=nltop;
  nltop=0;
  nlbot=0;
  UnlockSched();
  while(nl)
  {
    NAMELIST *p=nl;
    FreeWS(nl->name);
    nl=p->next;
    mfree(p);
  }
}

void AddNL(WSHDR *ws)
{
  NAMELIST *nnl=malloc(sizeof(NAMELIST));
  nnl->name=ws;
  nnl->next=0;
  LockSched();
  if (!nltop)
  {
    nlbot=nltop=nnl;
  }
  else
  {
    nnl->next=nltop;
    nltop=nnl;
  }
  UnlockSched();
}

char *find_name(CSM_RAM *csm)
{
  char s[40];
  char *p;
  static char u[40];

  CSM_DESC *desc=csm->constr;

  sprintf(s,"%08X ",desc);
  p=strstr(csm_text,s);
  if (p)
  {
    return(p+9);
  }
  else
  {
    sprintf(u,"Unknown %08X!",desc);
    return(u);
  }
}

int GetNumberOfDialogs(void)
{
  int count=0;
  CSM_RAM *icsm=under_idle->next; //Начало карусели
  ClearNL();
  WSHDR *ws;

  void *ircsm=FindCSMbyID(CSM_root()->idle_id);

  do
  {
    if (icsm==ircsm)
    {
      ws=AllocWS(40);
      wsprintf(ws,"IDLE Screen");
      AddNL(ws);
      nltop->p=icsm;
      count++;
    }
    else
    {
      if (icsm->constr!=&maincsm)
      {
	WSHDR *tws=(WSHDR *)(((char *)icsm->constr)+sizeof(CSM_DESC));
	char *s;
	if((tws->ws_malloc==NAMECSM_MAGIC1)&&(tws->ws_mfree==NAMECSM_MAGIC2))
	{
	  ws=AllocWS(40);
	  wstrcpy(ws,tws);
	  AddNL(ws);
	  nltop->p=icsm;
	  count++;
	}
	else
	{
	  s=find_name(icsm);
	  if (strncmp(s,"!SKIP!",6))
	  {
	    ws=AllocWS(40);
	    wsprintf(ws,percent_t,s);
	    AddNL(ws);
	    nltop->p=icsm;
	    count++;
	  }
	}
      }
    }
  }
  while((icsm=icsm->next));
  return(count);
}

void SwapCSMS(int no_no_gui)
{
  CSM_RAM *icsm; //Нижний CSM
  CSM_RAM *ucsm; //Верхний CSM
  CSM_RAM *wcsm; //Перемещаемый CSM
  extern WSHDR *ws_nogui;

  if (GetNumberOfDialogs()<2) return; //Нечего сворачивать
  if (!selcsm) return;
  do
  {
    icsm=under_idle;
    ucsm=FindCSMbyID(my_csm_id);
    wcsm=(CSM_RAM *)ucsm->prev; //Получаем перемещаемый CSM
    ((CSM_RAM *)(wcsm->prev))->next=ucsm; //CSM перед перемещаемым теперь указывает на верхний CSM
    ucsm->prev=wcsm->prev; //prev верхнего CSM указывает на CSM перед перемещаемым
    //Теперь вставляем в нужное место перемещаемый CSM
    ((CSM_RAM *)(wcsm->next=icsm->next))->prev=wcsm;
    icsm->next=wcsm;
    wcsm->prev=icsm;
  }
  while(ucsm->prev!=selcsm);
  if (no_no_gui) return; //
  //Теперь рисуем "Нет GUI" на всякий случай
  DrawRoundedFrame(0,0,131,175,0,0,0,
			GetPaletteAdrByColorIndex(0),
			GetPaletteAdrByColorIndex(20));
  DrawString(ws_nogui,3,70,128,172,1,2,GetPaletteAdrByColorIndex(2),GetPaletteAdrByColorIndex(23));
}


void method0(MAIN_GUI *data)
{
  int i;
  int pos;
  int vcur;
  NAMELIST *nl;

  if (do_idle)
  {
    selcsm=FindCSMbyID(CSM_root()->idle_id);
    SwapCSMS(1);
    GeneralFuncF1(1);
    do_idle=0;
    return;
  }

  DrawRoundedFrame(0,0,131,175,0,0,0,
			GetPaletteAdrByColorIndex(0),
			GetPaletteAdrByColorIndex(20));
  wsprintf(data->ws1,"XTask v1.4\n(C)2006 by Rst7/CBSIE\n\n%t%d",
	   "Сейчас диалогов: ",
	   GetNumberOfDialogs());
  DrawString(data->ws1,3,3,128,51,11,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));

  nl=nltop;
  i=0;
  while(nl)
  {
    i++;
    nl=nl->next;
  }
  if (i)
  {
    if (cursor>=i) cursor=i-1;
  }
  else cursor=0;
  nl=nltop;
  if (cursor<2)
  {
    pos=0;
    vcur=cursor;
  }
  else
  {
    pos=cursor-2;
    vcur=2;
  }
  while(pos)
  {
    if (nl)
    {
      nl=nl->next;
    }
    pos--;
  }
  i=0;
  do
  {
    if (nl)
    {
      if (nl->name)
      {
	if (i==vcur)
	{
	  selcsm=nl->p;
	  DrawRoundedFrame(3,55+14*i,128,58+11+14*i,0,0,0,
			GetPaletteAdrByColorIndex(0),
			GetPaletteAdrByColorIndex(3));
	}
	DrawString(nl->name,5,57+14*i,126,57+11+14*i,11,0x80,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      }
      nl=nl->next;
    }
    i++;
  }
  while(i<5);

  wsprintf(data->ws2,percent_t," Idle           Назад");
  DrawString(data->ws2,3,157,128,172,5,2,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
}

void method1(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  int f;
  int sz=0;
  unsigned int ul;
  extern const char csmlist_fname[128];

  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  if ((f=fopen(csmlist_fname,A_ReadOnly+A_BIN,0,&ul))!=-1)
  {
    sz=fread(f,csm_text,32767,&ul);
    fclose(f,&ul);
  }
  if (sz>0) csm_text[sz]=0;
  cursor=1; //На следующий
  selcsm=0;
  data->gui.state=1;
}

void method2(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  ClearNL();
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void method3(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  data->gui.state=2;
}

void method4(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

const char *bm_name(int bm)
{
  switch(bm)
  {
  case 0: return BM1NAME;
  case 1: return BM2NAME;
  case 2: return BM3NAME;
  case 3: return BM4NAME;
  case 4: return BM5NAME;
  case 5: return BM6NAME;
  case 6: return BM7NAME;
  case 7: return BM8NAME;
  case 8: return BM9NAME;
  }
  return(0);
}

const char *bm_file(int bm)
{
  switch(bm)
  {
  case 0: return BM1FILE;
  case 1: return BM2FILE;
  case 2: return BM3FILE;
  case 3: return BM4FILE;
  case 4: return BM5FILE;
  case 5: return BM6FILE;
  case 6: return BM7FILE;
  case 7: return BM8FILE;
  case 8: return BM9FILE;
  }
  return(0);
}

int RunBM(int bm)
{
  const char *s=bm_file(bm);
  if (s)
  {
    if (strlen(s))
    {
      WSHDR *ws;
      ws=AllocWS(150);
      str_2ws(ws,s,128);
      ExecuteFile(ws,0);
      FreeWS(ws);
      return(1);
    }
  }
  return(0);
}

HEADER_DESC bm_menuhdr={0,0,131,21,NULL,(int)"Bookmarks...",0x7FFFFFFF};
int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Options"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

void bm_menu_ghook(void *data, int cmd){}
int bm_menu_onkey(void *data, GUI_MSG *msg);
void bm_menu_iconhndl(void *data, int curitem, int *unk);

MENU_DESC bm_menu=
{
  8,(void *)bm_menu_onkey,(void*)bm_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0,//0x11,
  (void *)bm_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

int bm_menu_onkey(void *data, GUI_MSG *msg)
{
  int i;
  i=GetCurMenuItem(data);
  if (msg->keys==0x18)
  {
    //    GeneralFunc_F1(1);
    ShowMSG(1,(int)"Under construction!");
    return(-1);
  }
  if (msg->keys==0x3D)
  {
    if (RunBM(i))
    {
      GeneralFunc_flag1(((MAIN_CSM*)FindCSMbyID(my_csm_id))->gui_id,1);
      return(1);
    }
    return(-1);
  }
  return(0);
}


void bm_menu_iconhndl(void *data, int curitem, int *unk)
{
  const char *s;
  WSHDR *ws;
  void *item=AllocMenuItem(data);

  s=bm_name(curitem);
  if (s)
  {
    if (strlen(s))
    {
      ws=AllocMenuWS(data,strlen(s));
      wsprintf(ws,percent_t,s);
    }
    else
    {
      ws=AllocMenuWS(data,10);
      wsprintf(ws,"Empty");
    }
  }
  else
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,"error");
  }
//  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
//  SetMenuItemIcon(data,curitem,GetIconIndex(t));
}

void ShowBMmenu(void)
{
  CreateMenu(0,0,&bm_menu,&bm_menuhdr,0,9,0,0);
}

int method5(MAIN_GUI *data, GUI_MSG *msg)
{
  int i;
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    i=msg->gbsmsg->submess;
    {
      if (i=='0')
      {
        ShowBMmenu();
        return(0);
      }
      if ((i>='1')&&(i<='9'))
      {
        return RunBM(i-'1');
      }
    }
    switch(i)
    {
    case LEFT_SOFT:
      selcsm=FindCSMbyID(CSM_root()->idle_id);
    case ENTER_BUTTON:
      SwapCSMS(0);
    case RIGHT_SOFT:
      return(1); //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI
    case UP_BUTTON:
      if (cursor) cursor--;
      break;
    case DOWN_BUTTON:
      cursor++;
      break;
    }
  }
  DirectRedrawGUI();
  return(0);
}

void method7(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(data,mfree_adr);
}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)method0,	//Redraw
  (void *)method1,	//Create
  (void *)method2,	//Close
  (void *)method3,	//Focus
  (void *)method4,	//Unfocus
  (void *)method5,	//OnKey
  0,
  (void *)method7,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,131,175};

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
}

void maincsm_onclose(CSM_RAM *csm)
{
  //  extern void *ELF_BEGIN;
  //  ((void (*)(void *))(mfree_adr()))(&ELF_BEGIN);
  mode=0;
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if ((msg->msg==MSG_GUI_DESTROYED)&&((int)msg->data0==csm->gui_id))
  {
    csm->csm.state=-3;
  }
  return(1);
}

const int minus11=-11;

const CSM_DESC maincsm=
{
  maincsm_onmessage,
  maincsm_oncreate,
  0,
  0,
  0,
  0,
  maincsm_onclose,
  sizeof(MAIN_CSM),
  1,
  &minus11
};

void do_gui(int _do_idle)
{
  char dummy[sizeof(MAIN_CSM)];
  do_idle=_do_idle;
  InitConfig();
  my_csm_id=CreateCSM(&maincsm,dummy,0);
  mode=-1;
}
