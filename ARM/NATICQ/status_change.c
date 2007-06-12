/*
  Интерфейс и функции смены своего статуса
*/
#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "NatICQ.h"
#include "main.h"
#include "status_change.h"
#include "language.h"

//===============================================================================================
// ELKA Compatibility
#pragma inline
void patch_header(const HEADER_DESC* head)
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
extern void set_my_status(void);
extern  int S_ICONS[];

#pragma inline
void Change_Status(char status)
{
  CurrentStatus=status;
  set_my_status();
}
///////////////////////////////////////////////////////////////////////////////
#pragma inline
unsigned int GetStatusIndexInMenu(unsigned int status)
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
unsigned int GetStatusInMenuByPos(int pos)
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

int st_onkey(void *data, GUI_MSG *msg)
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

void st_icons_set(void *data, int curitem, int *unk)
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

/*MENUITEM_DESC st_menuitems[STATUSES_NUM]=
{
  {S_ICONS,(int)LG_STONLINE, LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STAWAY,   LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STNA,     LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},  
  {S_ICONS,(int)LG_STDND,    LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STOCCUP,  LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STFFC,    LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STINVIS,  LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
};*/

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

void stmenu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

static const MENU_DESC st_tmenu=
{
  8,(void *)st_onkey,(void *)stmenu_ghook,NULL,
  st_menusoftkeys,
  &st_menu_skt,
  0x11,//MENU_FLAG,
  (void*)st_icons_set,
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
