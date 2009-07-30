#include "..\inc\swilib.h"
#include "main.h"
#include "conf_loader.h"
#include "utils.h"
#include "idle.h"

extern  const int count;
extern const int vybor;

extern const int minus11;
unsigned short maincsm_name_body[140];
extern void kill_data(void *p,void (*func_p)(void *));

int pos;
const RECT Canvas={0,0,0,0};

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;

void OnRedraw(MAIN_GUI *data) // OnRedraw
{
  int i=0;
  do
    {
      Draw_GUI(i, pos);
      i++;
    }
  while (i<=count);

  if (vybor==1)
    {
      Draw_Select(pos);        
    }
}

void onCreate(MAIN_GUI *data, void *(*malloc_adr)(int)) //Create
{
  data->gui.state=1;
  pos=1;
}

void onClose(MAIN_GUI *data, void (*mfree_adr)(void *)) //Close
{
  data->gui.state=0;
}

void onFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))//Focus
{
  data->gui.state=2;
}

void onUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *)) //Unfocus
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

int OnKey(MAIN_GUI *data, GUI_MSG *msg) //OnKey
{
  extern const unsigned int CLOSE_BTN;
  
  DirectRedrawGUI();  
  
  if (msg->gbsmsg->msg==KEY_DOWN)
  { 
    if (msg->gbsmsg->submess == CLOSE_BTN) return (1);
    switch (msg->gbsmsg->submess)
    {
    case RIGHT_BUTTON:
      {
        pos+=1;
        if (pos==(count+2)) pos=1;
      }
        break;
     case LEFT_BUTTON:
       {
          pos-=1;
          if (pos==0) pos=count+1;
       }
        break;        
    case ENTER_BUTTON:
      {
        Draw_DoLabel(pos);   
        return(1);
      }
    case 49: case 50: case 51:  case 52:  case 53:  case 54:  case 55:  case 56: case 57: //1-9
      {
        if (msg->gbsmsg->submess-49>count) return(0);
        
        pos = msg->gbsmsg->submess-48;
        Draw_DoLabel(pos);
        return(1);        
      }
      
    }
  }
  return(0);
}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)OnRedraw,	//Redraw
  (void *)onCreate,	//Create
  (void *)onClose,	//Close
  (void *)onFocus,	//Focus
  (void *)onUnfocus,	//Unfocus
  (void *)OnKey,	//OnKey
  0,
  (void *)kill_data,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};


void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  patch_rect((RECT*)&Canvas,0,YDISP,ScreenW()-1,ScreenH()-1);
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
  InitConfig();
}

void maincsm_onclose(CSM_RAM *csm)
{
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

const struct
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

void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"IdleLinks");
}

void CreateMainCSM()
{
  LockSched();
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,dummy,2);
  UnlockSched();
}
