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

void AboutDlg(){};

int icon[]={0x3DB,0};
int about_icon[]={0x4DB,0};
int dummy_icon[] = {0x50E,0};

HEADER_DESC menuhdr={0,0,131,21,icon,(int)"Тест меню",0x7FFFFFFF};

int mmenusoftkeys[]={0,1,2};

MENUITEM_DESC menuitems[2]=
{
  {dummy_icon,(int)"Статус...",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {about_icon,(int)"Об эльфе...",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
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

MENU_DESC tmenu=
{
  8,NULL,NULL,NULL,
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
  menuitems[0].icon = &S_ICONS[CurrentStatus];
  patch_header(&menuhdr);
  MainMenu_ID = CreateMenu(0,0,&tmenu,&menuhdr,0,2,0,0);
}
