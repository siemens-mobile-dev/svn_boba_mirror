/*
  Интерфейс и функции смены своего статуса
*/
#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "../inc/pnglist.h"
#include "NatICQ.h"
#include "main.h"
#include "status_change.h"
#include "language.h"

//===============================================================================================
// ELKA Compatibility
#pragma inline
static void patch_header(const HEADER_DESC* head)
{
  ((HEADER_DESC *)head)->rc.x=0;
  ((HEADER_DESC *)head)->rc.y=YDISP;
  ((HEADER_DESC *)head)->rc.x2=ScreenW()-1;
  ((HEADER_DESC *)head)->rc.y2=HeaderH()-1+YDISP;
}
#pragma inline
void patch_input(const INPUTDIA_DESC* inp)
{
  ((INPUTDIA_DESC *)inp)->rc.x = 0;
  ((INPUTDIA_DESC *)inp)->rc.y = HeaderH() + 1 + YDISP;
  ((INPUTDIA_DESC *)inp)->rc.x2=ScreenW() - 1;
  ((INPUTDIA_DESC *)inp)->rc.y2=ScreenH() - SoftkeyH() - 1;
}
//===============================================================================================


extern int CurrentStatus;
extern int CurrentXStatus;
extern  int S_ICONS[];

#pragma inline
static void Change_Status(int status)
{
  CurrentStatus=status;
  set_my_status();
}

#pragma inline
static void Change_XStatus(int xstatus)
{
  CurrentXStatus=xstatus;
  set_my_xstatus();
}

///////////////////////////////////////////////////////////////////////////////
#pragma inline
static unsigned int GetStatusIndexInMenu(unsigned int status)
{
  switch(status)
  {
  case IS_ONLINE: {return 0;}
  case IS_AWAY: {return 1;}
  case IS_NA: {return 2;}
  case IS_DND: {return 3;}
  case IS_OCCUPIED: {return 4;}
  case IS_FFC: {return 5;}
  case IS_INVISIBLE: {return 6;}
  case IS_DEPRESSION: return 7;
  case IS_EVIL: return 8;
  case IS_HOME: return 9;
  case IS_LUNCH: return 10;
  case IS_WORK: return 11;
  }
  return 0;
}

#pragma inline 
static unsigned int GetStatusInMenuByPos(int pos)
{
  switch(pos)
  {
  case 0:
    return IS_ONLINE;
  case 1:
    return IS_AWAY;
  case 2:
    return IS_NA;
  case 3:
    return IS_DND;
  case 4:
    return IS_OCCUPIED;
  case 5:
    return IS_FFC;
  case 6:
    return IS_INVISIBLE;
  case 7:
    return IS_DEPRESSION;
  case 8:
    return IS_EVIL;
  case 9:
    return IS_HOME;
  case 10:
    return IS_LUNCH;
  case 11:
    return IS_WORK;
  }
  return IS_UNKNOWN;
}

static int st_onkey(void *data, GUI_MSG *msg)
{
  int i;
  if (msg->keys==0x18 || msg->keys==0x3D)
  {
    i=GetCurMenuItem(data);
    Change_Status(GetStatusInMenuByPos(i));
    return (1);
  }
  return (0);
}
    
///////////////////////////////////////////////////////////////////////////////

#define STATUSES_NUM 12

static const char * const menutexts[STATUSES_NUM]=
{
  LG_STONLINE,
  LG_STAWAY,
  LG_STNA,
  LG_STDND,
  LG_STOCCUP,
  LG_STFFC,
  LG_STINVIS,
  LG_STDEPRESSION,
  LG_STEVIL,
  LG_STHOME,
  LG_STLUNCH,
  LG_STWORK
};

void st_icons_set(void *data, int curitem, void *unk)
{
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  extern const char percent_t[];
  ws=AllocMenuWS(data,strlen(menutexts[curitem]));
  wsprintf(ws,percent_t,menutexts[curitem]);
  SetMenuItemIconArray(data,item,S_ICONS+GetStatusInMenuByPos(curitem));
  SetMenuItemText(data, item, ws, curitem);
}

static const HEADER_DESC st_menuhdr={0,0,0,0,NULL,(int)LG_CHGSTATUS,LGP_NULL};

static const int st_menusoftkeys[]={0,1,2};

static const SOFTKEY_DESC st_menu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB st_menu_skt=
{
  st_menu_sk,0
};

static void stmenu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

static const MENU_DESC st_tmenu=
{
  8,st_onkey,stmenu_ghook,NULL,
  st_menusoftkeys,
  &st_menu_skt,
  0x11,//MENU_FLAG,
  st_icons_set,
  NULL,//st_menuitems,
  NULL,
  STATUSES_NUM
};


void DispStatusChangeMenu()
{
  *((int **)(&st_menuhdr.icon))=S_ICONS+CurrentStatus;
  patch_header(&st_menuhdr);
  CreateMenu(0,0,&st_tmenu,&st_menuhdr,GetStatusIndexInMenu(CurrentStatus),STATUSES_NUM,0,0);
}

//--------------------------------
// XStatuses
//--------------------------------
extern DYNPNGICONLIST *XStatusesImgList;
extern int *XStatusesIconArray;
extern volatile int total_xstatuses;


static void EditXStatus(int xstatus)
{
}

static int xst_onkey(void *data, GUI_MSG *msg)
{
  int i;
  i=GetCurMenuItem(data);
  if (msg->keys==0x18)
  {
    EditXStatus(i);
    return(1);
  }
  if (msg->keys==0x3D)
  {
    Change_XStatus(i);
    return(1);
  }
  return (0);
}

static void xst_ghook(void *data, int cmd)
{
  PNGTOP_DESC *pltop=PNG_TOP();
  if (cmd==9)
  {
    pltop->dyn_pltop=NULL;
  }
  if (cmd==0x0A)
  {
    pltop->dyn_pltop=XStatusesImgList;
    DisableIDLETMR();
  }
}

static void xst_itemh(void *data, int curitem, void *unk)
{
  void *item=AllocMLMenuItem(data);
  WSHDR ws1loc, *ws1, *ws2;
  unsigned short num[128];
  WSHDR ws3loc, *ws3, *ws4;
  unsigned short num3[128];
  ws1=CreateLocalWS(&ws1loc,num,128);
  ws3=CreateLocalWS(&ws3loc,num3,128);

  wsprintf(ws1,"Short XS %d",curitem);
  wsprintf(ws3,"Large XS %d",curitem);
  ws2=AllocMenuWS(data,ws1->wsbody[0]);

  wstrcpy(ws2,ws1);
  ws4=AllocMenuWS(data,ws3->wsbody[0]);
  wstrcpy(ws4,ws3);
  SetMenuItemIconArray(data, item, XStatusesIconArray+curitem);
  SetMLMenuItemText(data, item, ws2, ws4, curitem);
}

static const SOFTKEY_DESC xst_menu_sk[]=
{
  {0x0018,0x0000,(int)LG_TEXT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB xst_menu_skt=
{
  xst_menu_sk,0
};

static const HEADER_DESC xst_menuhdr={0,0,0,0,NULL,(int)LG_CHGXSTATUS,LGP_NULL};

static const ML_MENU_DESC xst_menu=
{
  8,xst_onkey,xst_ghook,NULL,
  st_menusoftkeys,
  &xst_menu_skt,
  0x11,
  xst_itemh,
  NULL,
  NULL,
  0,
  1
};

void DispXStatusChangeMenu(void)
{
  patch_header(&xst_menuhdr);
  *((int **)(&xst_menuhdr.icon))=XStatusesIconArray+CurrentXStatus;
  CreateMultiLinesMenu(0,0,&xst_menu,&xst_menuhdr,CurrentXStatus,total_xstatuses);
}
