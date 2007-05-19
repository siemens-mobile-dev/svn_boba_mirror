#include "../inc/swilib.h"
#include "main.h"
#include "roster_icons.h"
#include "mainmenu.h"
#include "status_change.h"
#include "jabber_util.h"
#include "MUC_Enter_UI.h"
#include "cont_menu.h"
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

#define N_ITEMS 7

extern int Is_Sounds_Enabled;
extern int Is_Vibra_Enabled;

int MainMenu_ID;

extern char My_Presence;

void AboutDlg()
{
  ShowMSG(0,(int)"Siemens natJabber Client\n(c)Kibab, Ad, Borman99");  
};
 

void Dummy()
{
  ShowMSG(1,(int)"Раздел в разработке :)");  
};


// Ad, сюда напиши номера, которые сочтёшь нужным!
int icon[]={0x3DB,0};
int about_icon[]={0x4DB,0};
int dummy_icon[] = {0x50E,0};

HEADER_DESC menuhdr={0,0,131,21,NULL,(int)"Меню",LGP_NULL};

int mmenusoftkeys[]={0,1,2};

int icon_array[2];

void ChangeVibraMode(void)
{
  Is_Vibra_Enabled=!(Is_Vibra_Enabled);
  RefreshGUI();
}
  
void ChangeSoundMode(void)
{
  Is_Sounds_Enabled=!(Is_Sounds_Enabled);
  RefreshGUI();
}

MENUITEM_DESC menuitems[N_ITEMS]=
{
  {dummy_icon,(int)"Контакт",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"Статус",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"Конференция",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"Закладки",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},  
  {icon_array,(int)"Режим вибры",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {icon_array,(int)"Режим звука",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {about_icon,(int)"Об эльфе...",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void *menuprocs[N_ITEMS]={
                          (void *)Disp_Contact_Menu,
                          (void *)DispStatusChangeMenu,
                          (void *)Disp_MUC_Enter_Dialog,
                          (void *)Get_Bookmarks_List,
                          (void *)ChangeVibraMode,
                          (void *)ChangeSoundMode,
                          (void *)AboutDlg
                         };

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

void menuitemhandler(void *data, int curitem, int *unk)
{
  switch(curitem)
  {
  case 4:
    SetMenuItemIcon(data,curitem,Is_Vibra_Enabled?0:1);
    break;
    
  case 5:
    SetMenuItemIcon(data,curitem,Is_Sounds_Enabled?0:1);
    break;
  }
}

MENU_DESC tmenu=
{
  8,NULL,NULL,NULL,
  mmenusoftkeys,
  &mmenu_skt,
  1,
  (void*)menuitemhandler,
  menuitems,
  menuprocs,
  N_ITEMS
};
extern const char PATH_TO_PIC[128], png_t[];
int S_ICONS[2];
char mypic[128];
char confpic[128];
extern const char conference_t[];
void MM_Show()
{
#ifdef USE_PNG_EXT
  Roster_getIconByStatus(mypic, My_Presence);
  S_ICONS[0]=(int)mypic;
  menuitems[1].icon = S_ICONS;
  
  strcpy(confpic, PATH_TO_PIC);
  strcat(confpic,conference_t);
  strcat(confpic,png_t);  
  S_ICONS[1]=(int)confpic;
  menuitems[2].icon = S_ICONS+1;
  menuitems[3].icon = S_ICONS+1;  
#else
  S_ICONS[0]=Roster_getIconByStatus(My_Presence);
  menuitems[1].icon = S_ICONS;
#endif  
  
  icon_array[0]=GetPicNByUnicodeSymbol(CBOX_CHECKED);
  icon_array[1]=GetPicNByUnicodeSymbol(CBOX_UNCHECKED);
  
  patch_header(&menuhdr);
  MainMenu_ID = CreateMenu(0,0,&tmenu,&menuhdr,0,N_ITEMS,0,0);
}
