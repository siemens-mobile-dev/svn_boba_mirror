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
void patch_header(HEADER_DESC* head)
{
  head->rc.x=0;
  head->rc.y=YDISP;
  head->rc.x2=ScreenW()-1;
  head->rc.y2=HeaderH()+YDISP;
}
#pragma inline
void patch_input(INPUTDIA_DESC* inp)
{
  inp->rc.x = 0;
  inp->rc.y = HeaderH() + 1 + YDISP;
  inp->rc.x2=ScreenW() - 1;
  inp->rc.y2=ScreenH() - SoftkeyH() - 1;
}
//===============================================================================================


extern int CurrentStatus;
extern void set_my_status(void);
extern  int S_ICONS[13];

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
  }
  return IS_UNKNOWN;
}
  
  

void st_icons_set(void *data, int curitem, int *unk)
{
  SetMenuItemIcon(data,curitem,GetStatusInMenuByPos(curitem));
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

#define STATUSES_NUM 7

HEADER_DESC st_menuhdr={0,0,0,0,NULL,(int)LG_CHGSTATUS,LGP_NULL};

int st_menusoftkeys[]={0,1,2};

MENUITEM_DESC st_menuitems[STATUSES_NUM]=
{
  {S_ICONS,(int)LG_STONLINE, LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STAWAY,   LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STNA,     LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},  
  {S_ICONS,(int)LG_STDND,    LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STOCCUP,  LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STFFC,    LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,(int)LG_STINVIS,  LGP_NULL, 0, NULL, MENU_FLAG3,MENU_FLAG2},
};

SOFTKEY_DESC st_menu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB st_menu_skt=
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

MENU_DESC st_tmenu=
{
  8,(void *)st_onkey,(void *)stmenu_ghook,NULL,
  st_menusoftkeys,
  &st_menu_skt,
  1,//MENU_FLAG,
  (void*)st_icons_set,
  st_menuitems,
  NULL,
  STATUSES_NUM
};


void DispStatusChangeMenu()
{
  st_menuhdr.icon=S_ICONS+CurrentStatus;
  patch_header(&st_menuhdr);
  CreateMenu(0,0,&st_tmenu,&st_menuhdr,GetStatusIndexInMenu(CurrentStatus),STATUSES_NUM,0,0);
}
