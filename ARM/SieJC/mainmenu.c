#include "../inc/swilib.h"
#include "main.h"
#include "roster_icons.h"
#include "mainmenu.h"
#include "status_change.h"
#include "bookmarks.h"
#include "jabber_util.h"
#include "MUC_Enter_UI.h"
#include "cont_menu.h"
#include "revision.h"
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

#define N_ITEMS 8

extern int Is_Sounds_Enabled;
extern int Is_Vibra_Enabled;

int MainMenu_ID;

extern char My_Presence;

extern const char VERSION_VERS[];


void AboutDlg(GUI *data)
{
  char msg_tpl[]="Siemens natJabber Client\n(c)Kibab, Ad, Borman99\n%s r%d\nCompiled %s";
  int l;
  char *msg = malloc(l=strlen(msg_tpl)+20+1);
  snprintf(msg,l,msg_tpl, VERSION_VERS, __SVN_REVISION__, __DATE__);
  ShowMSG(0,(int)msg);
  mfree(msg);
};


void Dummy()
{
  ShowMSG(1,(int)"������ � ���������� :)");
};


// Ad, ���� ������ ������, ������� ������ ������!
int icon[]={0x3DB,0};
int about_icon[]={0x4DB,0};
int dummy_icon[] = {0x50E,0};

HEADER_DESC menuhdr={0,0,131,21,NULL,(int)"����",LGP_NULL};

int mmenusoftkeys[]={0,1,2};

int icon_array[2];

void ChangeVibraMode(GUI *data)
{
  Is_Vibra_Enabled=!(Is_Vibra_Enabled);
  RefreshGUI();
}

void ChangeSoundMode(GUI *data)
{
  Is_Sounds_Enabled=!(Is_Sounds_Enabled);
  RefreshGUI();
}

void Exit_SieJC(GUI *data)
{
  QuitCallbackProc(0);
}

MENUITEM_DESC menuitems[N_ITEMS]=
{
  {dummy_icon,(int)"�������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"�����������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"��������",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {icon_array,(int)"����� �����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {icon_array,(int)"����� �����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"�� �����...",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {dummy_icon,(int)"�����",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

MENUPROCS_DESC menuprocs[N_ITEMS]={
                          Disp_Contact_Menu,
                          DispStatusChangeMenu,
                          Disp_MUC_Enter_Dialog,
                          Get_Bookmarks_List,
                          ChangeVibraMode,
                          ChangeSoundMode,
                          AboutDlg,
                          Exit_SieJC
                         };

SOFTKEY_DESC mmenu_sk[]=
{
  {0x0018,0x0000,(int)"�����"},
  {0x0001,0x0000,(int)"�����"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB mmenu_skt=
{
  mmenu_sk,0
};

void menuitemhandler(void *data, int curitem, void *unk)
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
  menuitemhandler,
  menuitems,
  menuprocs,
  N_ITEMS
};
extern const char PATH_TO_PIC[128], png_t[];
int S_ICONS[N_ITEMS];
char mypic[128];
char confpic[128];
char exitpic[128];
char infopic[128];
char aboutpic[128];
extern const char conference_t[];
const char exit_t[] = "exit";
const char info_t[] = "info";
const char about_t[] = "about";


void MM_Show()
{
#ifdef USE_PNG_EXT
  // �������
  strcpy(infopic, PATH_TO_PIC);
  strcat(infopic,info_t);
  strcat(infopic,png_t);
  S_ICONS[0] = (int)infopic;

  // ������
  Roster_getIconByStatus(mypic, My_Presence);
  S_ICONS[1] = (int)mypic;

  //�����������...
  strcpy(confpic, PATH_TO_PIC);
  strcat(confpic,conference_t);
  strcat(confpic,png_t);
  S_ICONS[2] = (int)confpic;

  // ��������
  S_ICONS[3] = (int)confpic;

  // ����� �����
  // S_ICONS[4]

  // ����� �����
  // S_ICONS[5]


  // �� �����...
  strcpy(aboutpic, PATH_TO_PIC);
  strcat(aboutpic,about_t);
  strcat(aboutpic,png_t);
  S_ICONS[6] = (int)aboutpic;

  // �����
  strcpy(exitpic, PATH_TO_PIC);
  strcat(exitpic,exit_t);
  strcat(exitpic,png_t);
  S_ICONS[7] = (int)exitpic;

  menuitems[0].icon = S_ICONS+0;
  menuitems[1].icon = S_ICONS+1;
  menuitems[2].icon = S_ICONS+2;
  menuitems[3].icon = S_ICONS+3;
  menuitems[6].icon = S_ICONS+6;
  menuitems[7].icon = S_ICONS+7;


#else
  S_ICONS[0] = Roster_getIconByStatus(My_Presence);
  menuitems[1].icon = S_ICONS;
#endif

  icon_array[0]=GetPicNByUnicodeSymbol(CBOX_CHECKED);
  icon_array[1]=GetPicNByUnicodeSymbol(CBOX_UNCHECKED);

  patch_header(&menuhdr);
  MainMenu_ID = CreateMenu(0,0,&tmenu,&menuhdr,0,N_ITEMS,0,0);
}
