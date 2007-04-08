#include "..\inc\swilib.h"

typedef struct
{
  void *next;
  char *pngname;
  IMGHDR * img;
}PNGLIST;

typedef struct
{
  PNGLIST *top;
  char *bitmap;
}A_pltop;

const int minus11=-11;
unsigned short maincsm_name_body[140];
extern void kill_data(void *p, void (*func_p)(void *));
int show_pic=0;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

int get_number_of_png()
{
  A_pltop *pltop=PNG_TOP();
  int i=0;
  if (pltop)
  {
    PNGLIST *pl=pltop->top;
    while (pl)
    {
      pl=pl->next;
      i++;
    }
  }
  return (i);
}

PNGLIST* find_pic_by_n(int n)
{
  A_pltop *pltop=PNG_TOP();
  if (pltop)
  {
    int i=0;
    PNGLIST *pl=pltop->top;
    while(i!=n && pl)
    {
      pl=pl->next;
      i++;
    }
    return (pl);
  }
  else return (0);
}

void DrwImg(IMGHDR *img, int x, int y, char *pen, char *brush)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc,x,y,img->w,img->h);
  SetPropTo_Obj5(&drwobj,&rc,0,img);
  SetColor(&drwobj,pen,brush);
  DrawObject(&drwobj);
}

void clear_cache()
{
  A_pltop *pltop=PNG_TOP();
  LockSched();
  PNGLIST *pl=pltop->top;
  PNGLIST *pl_prev;
  pltop->top=0;
  while(pl)
  {
    pl_prev=pl;
    pl=pl->next;
    mfree(pl_prev->pngname);
    if(pl_prev->img)
    {
      mfree(pl_prev->img->bitmap);
      mfree(pl_prev->img);
    }
    mfree(pl_prev);
  }
  show_pic=0;
  UnlockSched();
  REDRAW();
}

void clear_bitmap()
{
  A_pltop *pltop=PNG_TOP();  
  if (pltop)
  {
    char *bitmap=pltop->bitmap;
    if (bitmap)
    {
      zeromem(bitmap,0x10000/8*2);
    }
  }
}
  
unsigned int total_size()
{
  unsigned int n=0;
  A_pltop *pltop=PNG_TOP();
  PNGLIST *pl=pltop->top;
  while (pl)
  {
    n+=sizeof(PNGLIST);
    n+=(strlen(pl->pngname)+1);
    if(pl->img)
    {
      n+=sizeof(IMGHDR);
      switch (pl->img->bpnum)
      {
      case 1:
        n+=((pl->img->w>>3)*pl->img->h);
        break;
        
      case 5:
        n+=(pl->img->w*pl->img->h);
        break;
        
      case 8:
        n+=(2*pl->img->w*pl->img->h);
        break;
      }
    }
    pl=pl->next;
  }
  return (n);
}


void OnRedraw(MAIN_GUI *data)
{
  int x=ScreenW();
  int y=ScreenH();
  PNGLIST* current;
  DrawRoundedFrame(0,0,x-1,y-1,0,0,0,
                   GetPaletteAdrByColorIndex(0),
                   GetPaletteAdrByColorIndex(20));
  int num=get_number_of_png(); 
  wsprintf(data->ws1,"Opened:%u",num);
  DrawString(data->ws1,3,3,74,15,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
    
  if (!show_pic)
    wsprintf(data->ws1,"Top Pic");
  else
  {
    if (show_pic<0) show_pic=num-1;
    if (show_pic>num-1) show_pic=0;
    wsprintf(data->ws1,"Cache %u",show_pic);
  }
  DrawString(data->ws1,75,3,x-4,15,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
  current=find_pic_by_n(show_pic);

  int width=x-2-1;
  int height=y-2-29;
  if (current)
  {
    wsprintf(data->ws1,"%s",current->pngname);
    DrawString(data->ws1,3,16,x-4,28,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));  
    DrawRoundedFrame(1,29,x-2,y-2,0,0,0,
                     GetPaletteAdrByColorIndex(0),
                     GetPaletteAdrByColorIndex(20));
    
    if (current->img)
    {
      
      if (current->img->w>=width)
        width=0;
      
      else width=(width-current->img->w)/2;
      
      if (current->img->h>=height)
        height=0;
      
      else height=29+(height-current->img->h)/2; 
      DrwImg(current->img,width,height,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
    }      
  }
  wsprintf(data->ws1,"%u",total_size());
  DrawString(data->ws1,0,y-GetFontYSIZE(SMALL_FONT)-1,x-1,y-1,SMALL_FONT,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));  
}


void OnCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->gui.state=1;
}

void OnClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  data->gui.state=0;
}

void OnFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  data->gui.state=2;
}

void OnUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}


int OnKey(MAIN_GUI *data, GUI_MSG *msg)
{
  DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case RIGHT_SOFT:
      return(1); //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI
    case '0':
      show_pic=0;
      break;
    case '1':
      clear_cache();
      break;
    case '2':
      clear_bitmap();
      break;
    case LEFT_BUTTON:
    case '4':
      show_pic--;
      break;
    case RIGHT_BUTTON:
    case '6':
      show_pic++;
      break;
      
      
    }
  }
  return (0);
  
}


int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)OnRedraw,	//Redraw
  (void *)OnCreate,	//Create
  (void *)OnClose,	//Close
  (void *)OnFocus,	//Focus
  (void *)OnUnfocus,	//Unfocus
  (void *)OnKey,	//OnKey
  0,
  (void *)kill_data, //method7,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

RECT Canvas={0,0,0,0};

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  Canvas.x2=ScreenW()-1;
  Canvas.y2=ScreenH()-1;
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
  SUBPROC((void *)ElfKiller);
}


int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"Png_ext_test");
}

int main()
{
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  LockSched();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
}
