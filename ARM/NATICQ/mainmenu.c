#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "NatICQ.h"
#include "history.h"
#include "mainmenu.h"
#include "main.h"
#include "conf_loader.h"
#include "status_change.h"

//==============================================================================
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
  inp->rc.x=0;
  inp->rc.y=HeaderH()+1+YDISP;
  inp->rc.x2=ScreenW()-1;
  inp->rc.y2=ScreenH()-SoftkeyH()-1;
}
//==============================================================================

int MainMenu_ID;

extern int CurrentStatus;
extern  int S_ICONS[11];

//void AboutDlg(){};
void AboutDlg()
{
  ShowMSG(2, (int)"NATICQ v1.0\n(C) by Rst7/CBSIE\nKren\nKibab\nShadow");
}

int icon[]={0,0};
int about_icon[]={0,0};
int dummy_icon[] = {0,0};

HEADER_DESC menuhdr={0,0,131,21,NULL,(int)"Меню",LGP_NULL};

int mmenusoftkeys[]={0,1,2};

MENUITEM_DESC menuitems[2]=
{
  {NULL,(int)"Статус...",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Об эльфе...",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void *menuprocs[3]={(void *)DispStatusChangeMenu, (void *) AboutDlg};

SOFTKEY_DESC mmenu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB mmenu_skt=
{
  mmenu_sk,0
};

void tmenu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    menuitems[0].icon = S_ICONS+CurrentStatus;
    DisableIDLETMR();
  }
}


MENU_DESC tmenu=
{
  8,NULL,(void *)tmenu_ghook,NULL,
  mmenusoftkeys,
  &mmenu_skt,
  1,//MENU_FLAG,
  NULL,
  menuitems,
  menuprocs,
  2
};

void ShowMainMenu()
{
  menuitems[0].icon = S_ICONS+CurrentStatus;
  menuitems[1].icon = S_ICONS+IS_UNKNOWN;
  menuhdr.icon= S_ICONS+IS_ONLINE;
  patch_header(&menuhdr);
  MainMenu_ID = CreateMenu(0,0,&tmenu,&menuhdr,0,2,0,0);
}
