#include "../inc/swilib.h"
#include "view.h"
#include "parse_oms.h"
#include "main.h"
#include "rect_patcher.h"
#include "local_ipc.h"
#include "display_utils.h"


extern void kill_data(void *p, void (*func_p)(void *));

const int minus11=-11;

int strcmp_nocase(const char *s, const char *d)
{
  int cs;
  int ds;
  do
  {
    cs=*s++;
    if (cs&0x40) cs&=0xDF;
    ds=*d++;
    if (ds&0x40) ds&=0xDF;
    cs-=ds;
    if (cs) break;
  }
  while(ds);
  return(cs);
}

const char ipc_my_name[32]=IPC_BALLETMINI_NAME;
IPC_REQ gipc;

int view_url_mode; //MODE_FILE, MODE_URL
char *view_url;

//static const char percent_t[]="%t";


typedef struct
{
  CSM_RAM csm;
  int view_id;
}MAIN_CSM;

void FreeRawText(VIEWDATA *vd)
{
  mfree(vd->rawtext);
  vd->rawtext=NULL;
  vd->rawtext_size=0;
}

void FreeDynImgList(VIEWDATA *vd)
{
  OMS_DYNPNGLIST *dpl=vd->dynpng_list;
  vd->dynpng_list=NULL;
  while(dpl)
  {
    OMS_DYNPNGLIST *p=dpl;
    dpl=dpl->dp.next;
    if (p->dp.img)
    {
      mfree(p->dp.img->bitmap);
      mfree(p->dp.img);
    }
    mfree(p);
  }
}

void FreeViewData(VIEWDATA *vd)
{
  if (vd->ws) FreeWS(vd->ws);
  mfree(vd->lines_cache);
  mfree(vd->ref_cache);
  mfree(vd->rawtext);
  mfree(vd->oms);
  mfree(vd->I_cache);
  mfree(vd->S_cache);
  FreeDynImgList(vd); 
  mfree(vd);
}

static void StartGetFile(void)
{
  if (view_url_mode==MODE_FILE)
  {
    unsigned int err;
    char buf[1024];
    int i;
    int f=fopen(view_url,A_ReadOnly+A_BIN,P_READ,&err);
    if (f!=-1)
    {
      while((i=fread(f,buf,sizeof(buf),&err))>0)
      {
	gipc.name_to=ipc_my_name;
	gipc.name_from=ipc_my_name;
	gipc.data=malloc(i+4);
	*((int *)gipc.data)=i;
	memcpy(((char *)gipc.data)+4,buf,i);
	GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_DATA_ARRIVED,&gipc);
      }
      fclose(f,&err);
    }
    else
    {
      LockSched();
      ShowMSG(1,(int)"Can't open file!");
      UnlockSched();
    }
  }
}

//===============================================================================================
typedef struct
{
  GUI gui;
  VIEWDATA *vd;
}VIEW_GUI;

static void method0(VIEW_GUI *data)
{
  int scr_w=ScreenW()-1;
  int scr_h=ScreenH()-1;
  int sc;
  int dc;
  VIEWDATA *vd=data->vd;
  WSHDR *ws=vd->ws;
  LINECACHE *lc;
  unsigned int vl;
  int ypos=YDISP;
  unsigned int store_pos=vd->view_pos;
  unsigned int store_line=vl=vd->view_line;
  unsigned int len;
  unsigned int y2;
  
  char def_ink[4];
  char def_paper[4];
  
  if (data->gui.state==2)
  {
    DrawRectangle(0,YDISP,scr_w,scr_h,0,
		  GetPaletteAdrByColorIndex(0),
		  GetPaletteAdrByColorIndex(0));
    while(ypos<scr_h)
    {
      if (LineDown(vd))
      {
	lc=vd->lines_cache+vl;
	dc=0;
//	ws->wsbody[++dc]=0xE006;
//	ws->wsbody[++dc]=lc->ink1;
//	ws->wsbody[++dc]=lc->ink2;
	def_ink[0]=lc->ink1>>8;
	def_ink[1]=lc->ink1;
	def_ink[2]=lc->ink2>>8;
	def_ink[3]=lc->ink2;
//	ws->wsbody[++dc]=0xE007;
//	ws->wsbody[++dc]=lc->paper1;
//	ws->wsbody[++dc]=lc->paper2;
	def_paper[0]=lc->paper1>>8;
	def_paper[1]=lc->paper1;
	def_paper[2]=lc->paper2>>8;
	def_paper[3]=lc->paper2;
//	ws->wsbody[++dc]=lc->bold?0xE013:0xE012;
//	ws->wsbody[++dc]=lc->underline?0xE001:0xE002;
	if ((vl+1)<vd->lines_cache_size)
	{
	  len=(lc[1]).pos-(lc[0]).pos;
	}
	else
	  len=vd->rawtext_size-lc->pos;
	sc=lc->pos;
	while(len&&(dc<32000))
	{
	  ws->wsbody[++dc]=vd->rawtext[sc++];
	  len--;
	}
	ws->wsbody[0]=dc;
	y2=lc->pixheight+ypos-1;
	DrawRectangle(0,ypos,scr_w,y2,
	     RECT_FILL_WITH_PEN,def_paper,def_paper);
	DrawString(ws,0,ypos,scr_w,lc->pixheight+ypos-1,
		   lc->bold?FONT_SMALL_BOLD:FONT_SMALL,TEXT_NOFORMAT+(lc->underline?TEXT_UNDERLINE:0),def_ink,def_paper);
	ypos+=lc->pixheight;
	vl++;
      }
      else
	break;
    }
  }
  vd->view_pos=store_pos;
  vd->view_line=store_line;
}

static void method1(VIEW_GUI *data,void *(*malloc_adr)(int))
{
  VIEWDATA *vd=malloc(sizeof(VIEWDATA));
  zeromem(vd,sizeof(VIEWDATA));
  vd->ws=AllocWS(32767);
  *((unsigned short *)(&vd->current_tag_d))=0xFFFF;
  data->vd=vd;
  data->gui.state=1;
  SUBPROC((void *)StartGetFile);
}

static void method2(VIEW_GUI *data,void (*mfree_adr)(void *))
{
  FreeViewData(data->vd);
  data->gui.state=0;
}

static void method3(VIEW_GUI *data,void *(*malloc_adr)(int),void (*mfree_adr)(void *))
{
  PNGTOP_DESC *pltop=PNG_TOP();
  pltop->dyn_pltop=(DYNPNGICONLIST *)(data->vd->dynpng_list);
  DisableIDLETMR();
  data->gui.state=2;
}

static void method4(VIEW_GUI *data,void (*mfree_adr)(void *))
{
  PNGTOP_DESC *pltop=PNG_TOP();
  pltop->dyn_pltop=NULL;
  if (data->gui.state!=2)
    return;
  data->gui.state=1;
}

static int method5(VIEW_GUI *data,GUI_MSG *msg)
{
  VIEWDATA *vd=data->vd;
  int m=msg->gbsmsg->msg;
  if ((m==KEY_DOWN)||(m==LONG_PRESS))
  {
    switch(msg->gbsmsg->submess)
    {
    case UP_BUTTON:
      LineUp(vd);
      break;
    case DOWN_BUTTON:
      LineDown(vd);
      break;
    case LEFT_BUTTON:
      m=6;
      do
      {
	if (!LineUp(vd)) break;
      }
      while(--m);
      break;
    case RIGHT_BUTTON:
      m=6;
      do
      {
	if (!LineDown(vd)) break;
      }
      while(--m);
      break;
    case LEFT_SOFT:
      break;
    case RIGHT_SOFT:
      return(1); //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI
    case GREEN_BUTTON:
      break;
    }
  }
  DirectRedrawGUI();
  return(0);
}

static int method8(void){return(0);}

static int method9(void){return(0);}

static const void * const gui_methods[11]={
  (void *)method0,  //Redraw
  (void *)method1,  //Create
  (void *)method2,  //Close
  (void *)method3,  //Focus
  (void *)method4,  //Unfocus
  (void *)method5,  //OnKey
  0,
  (void *)kill_data, //method7, //Destroy
  (void *)method8,
  (void *)method9,
  0
};

static void maincsm_oncreate(CSM_RAM *data)
{
  static const RECT Canvas={0,0,0,0};
  MAIN_CSM *csm=(MAIN_CSM*)data;
  VIEW_GUI *view_gui=malloc(sizeof(VIEW_GUI));
  zeromem(view_gui,sizeof(VIEW_GUI));
  patch_rect((RECT*)&Canvas,0,0,ScreenW()-1,ScreenH()-1);
  view_gui->gui.canvas=(void *)(&Canvas);
//  view_gui->gui.flag30=2;
  view_gui->gui.methods=(void *)gui_methods;
  view_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->view_id=CreateGUI(view_gui);
}

void FreeViewUrl(void)
{
  mfree(view_url);
  view_url=NULL;
}

static void KillAll(void)
{
  FreeViewUrl();
}

static void Killer(void)
{
  extern void *ELF_BEGIN;
  KillAll();
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *csm)
{
  SUBPROC((void *)Killer);
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  //IPC
  if (msg->msg==MSG_IPC)
  {
    IPC_REQ *ipc;
    if ((ipc=(IPC_REQ*)msg->data0))
    {
      if (strcmp_nocase(ipc->name_to,ipc_my_name)==0)
      {
	switch (msg->submess)
	{
	case IPC_DATA_ARRIVED:
	  //Если приняли свое собственное сообщение, значит запускаем чекер
	  if (ipc->name_from==ipc_my_name)
	  {
	    VIEW_GUI *p=FindGUIbyId(csm->view_id,NULL);
	    VIEWDATA *vd;
	    int len=*((int*)(ipc->data));
	    char *buf=((char*)(ipc->data))+4;
	    if (p)
	    {
	      vd=p->vd;
	      if (vd)
	      {
		OMS_DataArrived(vd,buf,len);
		if (IsGuiOnTop(csm->view_id)) DirectRedrawGUI();
	      }
	    }
	    mfree(ipc->data);
	  }
	  break;
	}
      }
    }
  }
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->view_id)
    {
      {
	csm->csm.state=-3;
      }
    }
  }
  return(1);
}

static unsigned short maincsm_name_body[140];

static const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM =
{
  {
  maincsm_onmessage,
  maincsm_oncreate,
#ifdef NEWSGOLD
  0,
  0,
  0,
  0,
#endif
  maincsm_onclose,
  sizeof(MAIN_CSM),
  1,
  &minus11
  },
  {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};

static void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"BM: %s",view_url);
}

char *globalstr(const char *s)
{
  int l=strlen(s)+1;
  char *d=malloc(l);
  memcpy(d,s,l);
  return d;
}

int ParseInputFilename(const char *fn)
{
  char *s=strrchr(fn,'.');
  FreeViewUrl();
  if (s)
  {
    s++;
    if (!strcmp_nocase(s,"oms"))
    {
      view_url=globalstr(fn);
      view_url_mode=MODE_FILE;
    }
    else
      if (!strcmp_nocase(s,"url"))
      {
	int f;
	unsigned int err;
	int fsize;
	char *buf;
	FSTATS stat;
	if (GetFileStats(fn,&stat,&err)==-1) return 0;
	if ((fsize=stat.size)<=0) return 0;
	if ((f=fopen(fn,A_ReadOnly+A_BIN,P_READ,&err))==-1) return 0;
	buf=malloc(fsize+1);
	buf[fread(f,buf,fsize,&err)]=0;
	fclose(f,&err);
	s=buf;
	while(*s>32) s++;
	*s++=0;
	view_url=realloc(buf,s-buf);
	view_url_mode=MODE_URL;
      }
      else return 0;
    return 1;
  }
  return 0;
}

int main(const char *exename, const char *filename)
{
  char dummy[sizeof(MAIN_CSM)];
  if (ParseInputFilename(filename))
  {
    UpdateCSMname();
    LockSched();
    CreateCSM(&MAINCSM.maincsm,dummy,0);
    UnlockSched();
  }
  else
  {
    LockSched();
    ShowMSG(1,(int)"Nothing to do!");
    UnlockSched();
    SUBPROC((void *)Killer);
  }
  return 0;
}
