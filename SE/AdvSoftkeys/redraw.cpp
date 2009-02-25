#include "..\include\Lib_Clara.h"
#include "..\include\Dir.h"
#include "header\structs.h"
#include "header\books.h"
#include "header\images.h"
#include "header\presets.h"

typedef struct
{
  char unk[0x18];
  char type;
  char unk1[3];
  void *defmenu;
}MBOOK;

DISP_OBJ *soft=0;
DISP_OBJ_METHOD DREFRESH;
DISP_OBJ_ONREDRAW_METHOD DREDRAW;

#define INT_NUM 6
extern int colors[INT_NUM];
extern bool works;
extern bool pressed[3];
extern ITEM*lastitem;
extern UI_SOFTKEY sk[3];

int StatusSize=0;
int SoftSize=0;
RECT rect;
int height=0,width=0,uic=0,DB=2020;

ITEM *getbyname(wchar_t *name);
void getitem(BOOK *bk);

LABELS *get_labels(DISP_OBJ *sk, DISP_OBJ *DO, BOOK *bk, int vis);

int GetOrientation(BOOK *bk);
void __getitem(BOOK *bk);
void DrawHighlightID2(int font,int text,int ct, int XPos, int YPos, int borderColor, int NormalColor, bool);
void DrawHighlightID(int font,int text,int CEN, int XPos, int YPos, int MaxXPos, int MaxYPos, int borderColor, int NormalColor, bool);

void GetSize()
{
  int a=Display_GetHeight(0);
  switch (a)
  {
  case 320:
    StatusSize=24;
    SoftSize=30;
    break;
  case 220:
    StatusSize=18;
    SoftSize=26;
    break;
  case 160:
    StatusSize=14;
    SoftSize=18;
    break;
  default:
    StatusSize=18;
    SoftSize=26;
    break;
  }
};

char *books[]={"MM_Browser",
               "ImageViewerBook",
               "AudioPlayerBook"};

int NotSupported(BOOK *bk)
{
  int x;
  for (x=0;x<3;x++)
  {
    if (strstr(bk->xbook->name,books[x]))
    {
      return 1;
    }
  }
  if (bk==FindBook(isMediaPlayerVideoBook()))
  {
    return 1;
  }
  return 0;
};

int ourredraw(DISP_OBJ *DO, int a, int b, int c)
{
  if (!works) return 0;
  DISP_OBJ *foc=DISPBASE_GetFocused(0);
  BOOK *bk=DISPLAY_GetTopBook(0);
  __getitem(bk);
  if (lastitem){ if (lastitem->style==2) return 0; }
  int stat=GetOrientation(bk);
  SOFTKEY_PARAMS *xls=(SOFTKEY_PARAMS*)DispObject_Softkeys_GetParams(foc);
  if (xls->visible_softs_count==3 && bk!=Find_StandbyBook())return 0;
  void *GC=get_DisplayGC();
  if (!GC)return 1;
  if (bk==FindBook(isMenuBook))
  {
    MBOOK *mb=(MBOOK*)bk;
    if (mb->type==0)
    {
      if (mb->defmenu)
      {
        if (images[DESKTOP]!=0xFFFF)
        {
          putchar(GC,0,SoftSize-height,0,0,images[DESKTOP]);
        }
      }
    }
    else
    {
      if (images[NAVIGATION]!=0xFFFF)
      {
        putchar(GC,0,SoftSize-height,0,0,images[NAVIGATION]);
      }
    }
  }
  else if (bk==Find_StandbyBook())
  {
    if (lastitem && lastitem->type>T_STANDBY)
    {
      if (images[STANDBY]!=0xFFFF)
      {
        putchar(GC,0,SoftSize-height,0,0,images[STANDBY]);
      }
      if (images[STANDBY_SOFTKEY]!=0xFFFF)
      {
        putchar(GC,0,0,0,0,images[STANDBY_SOFTKEY]);
      }
    }
  }
  else
  { 
    if (DB==2010)goto L_draw;
    if (xls->background==0 && stat==0 && NotSupported(bk)==false)
    {
    L_draw:
      if (images[NAVIGATION]!=0xFFFF)
      {
        putchar(GC,0,SoftSize-height,0,0,images[NAVIGATION]);
      }
    }
  }
  return 1;
};

int GetOrientation(BOOK *bk)
{
  if (DB==2010)
  {
    return 0;
  }
  return BookObj_GetDisplayOrientation(bk);
};

void DrawSofts(DISP_OBJ *DO,BOOK *bk, LABELS *lbl)
{
  int stat=GetOrientation(bk);
  void *GC=get_DisplayGC();
  SOFTKEY_PARAMS *xls=(SOFTKEY_PARAMS*)DispObject_Softkeys_GetParams(DISPBASE_GetFocused(0));
  if (!xls)return;
  if (!GC)return;
  int color=xls->color;
  if (DB==2010)goto L_draw2;
  if (lastitem)
  {
    if (lastitem->style==0 && pressed[1]==true && lbl->strids[1]==0x6FFFFFFF)
    {
      pressed[1]=false;
      pressed[0]=true;
    }
    if (xls->background==0 && !FindBook(isMMBrowserBook) && bk!=FindBook(isAudioPlayerBook()) && bk!=FindBook(isMediaPlayerVideoBook()))//bk!=FindBook(isImageViewerBook) && ) ) && NotSupported(bk)==false && xls->background==0 && stat==0)
    {
    L_draw2:
      if (pressed[0] && images[LEFT_PRESSED]!=0xFFFF && lbl->strids[0]!=0x6FFFFFFF && lastitem->type>T_STANDBY)
      {
        putchar(GC,0,0,0,0,images[LEFT_PRESSED]);
      }
      else if (pressed[1] && images[MIDDLE_PRESSED]!=0xFFFF && lbl->strids[1]!=0x6FFFFFFF)
      {
        putchar(GC,0,0,0,0,images[MIDDLE_PRESSED]);
      }
      else if (pressed[2] && images[RIGHT_PRESSED]!=0xFFFF && lbl->strids[2]!=0x6FFFFFFF && lastitem->type>T_STANDBY)
      {
        putchar(GC,0,0,0,0,images[RIGHT_PRESSED]);
      }
    }
  }
  int x;
  for (x=0;x<3;x++)
  {
    int c=0;
    int b=0;
    if (lbl->enable[x])
    {
      c=colors[x*2];
    }
    else
    {
      c=0xAAAAAAAA;
    }
    if (pressed[x])
    {
      c=colors[x*2+1];
    }
    b=clBlack;
    if (uic && color)c=color;
    if (xls->background==1)
    {
      if (pressed[x])
      {
        b=clWhite;
        c=clBlack;
      }
      else
      {
        b=clBlack;
        c=clWhite;
      }
    }
    else
    {
      b=clBlack;
    }
    if (stat==0)
    {
      int pos=sk[x].y-(height-SoftSize);
      if (pressed[x])pos++;
      DrawHighlightID2(sk[x].font, lbl->strids[x], sk[x].ct, sk[x].x, pos, b, c, xls->background);
    }
    else
    {
      if (pressed[x])
      {
        b=clWhite;
        c=clBlack;
      }
      else
      {
        b=clBlack;
        c=clWhite;
      }
      DrawHighlightID(sk[x].hfont,lbl->strids[x],sk[x].hct,sk[x].hx,sk[x].hy,height-DISP_OBJ_GetWindowWidth(DO),width,b, c, xls->background);
    }
  }
};

void __getitem(BOOK *bk)
{
  bool t=false;
  if (Find_StandbyBook()==bk)
  {
    GUI *g=SBY_GetMainInput(bk);
    if (!g)return;
    int vis=MainInput_getVisible(g);
    if (vis==1)
    {
      lastitem=getbyname(L"Keylock");
      if (!lastitem)lastitem=getbyname(L"DEFAULT");
      t=true;
    }
  }
  if (t==false)
  {
    getitem(bk);
  }
};

void DispDraw(DISP_OBJ* DO,int a,int b,int c)
{ 
  BOOK *bk=DISPLAY_GetTopBook(0);
  DISP_OBJ *focused = DISPBASE_GetFocused(0);
  if (!bk)return;
  __getitem(bk);
  DREDRAW(DO, a, b, c);
  if (!works)return;
  Softkeys_Update(DO);
  if (!focused)return;
  SOFTKEY_PARAMS *xls=(SOFTKEY_PARAMS*)DispObject_Softkeys_GetParams(focused);
  int vis=0;
  if (xls)
  {
    vis = xls->visible_softs_count;
    if (vis==3)
    {
      if (bk!=Find_StandbyBook())
      {
        if (lastitem)
        {
          lastitem->type=T_3SK_LAYOUT;
        }
        return;
      }
      else
      {
        if (lastitem)lastitem->type=T_STANDBY;
      }
    }
    else if (vis==0xFFFF || vis==0)
    {
      return;
    }
  }
  else return;
  LABELS* lbl= get_labels(DO, focused, bk, vis);
  if (lastitem && lbl)
  {
    if (lastitem->style==2)return;
    if (lastitem->type!=T_DDMENU && lastitem->type!=T_MESSAGEBOX)
    {
      int x;
      for (x=0;x<3;x++)
      {
        if (lastitem->strids[x]!=0x6FFFFFFF)
        {
          lbl->strids[x]=lastitem->strids[x];
          lbl->enable[x]=true;
        }
      }
    }
    DrawSofts(DO, bk, lbl);
    pressed[0] = false;
    pressed[1] = false;
    pressed[2] = false;
  }
  delete(lbl);
};

void RefreshTimer(DISP_OBJ *DO)
{
  pressed[0]=false;
  pressed[1]=false;
  pressed[2]=false;
  InvalidateRect(DO,&rect);
  DispObject_KillRefreshTimer(soft);
};

int redraw_init()
{
  height=Display_GetHeight(0);
  width=Display_GetWidth(0);
  switch(GetChipID())
  {
  case 0x7100:
  case 0x8000:
  case 0x8040:
    DB=2010;
    uic=false;
    break;
  case 0x9900:
    DB=2020;
    uic=true;
    break;
  }
  GetSize();
  rect.x1=0;
  rect.y1=0;
  rect.x2=Display_GetWidth(0);
  rect.y2=SoftSize;
  soft=DispObject_SoftKeys_Get();
  if (soft)
  {
    DREDRAW=DISP_OBJ_GetOnRedraw(soft);
    DISP_DESC_SetOnRedraw(DISP_OBJ_GetDESC(soft), DispDraw);
    DREFRESH=DISP_OBJ_GetonRefresh(soft);
    DISP_DESC_SetonRefresh(DISP_OBJ_GetDESC(soft), RefreshTimer);
    //---------------------------------------------------------
  }
  return 1;
};

