#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "NatICQ.h"
#include "history.h"
#include "mainmenu.h"
#include "main.h"
#include "conf_loader.h"
#include "status_change.h"
#include "language.h"
#include "revision.h"

//==============================================================================
// ELKA Compatibility
#pragma inline
void patch_header(const HEADER_DESC* head)
{
  ((HEADER_DESC*)head)->rc.x=0;
  ((HEADER_DESC*)head)->rc.y=YDISP;
  ((HEADER_DESC*)head)->rc.x2=ScreenW()-1;
  ((HEADER_DESC*)head)->rc.y2=HeaderH()-1+YDISP;
}
#pragma inline
void patch_input(const INPUTDIA_DESC* inp)
{
  ((INPUTDIA_DESC*)inp)->rc.x=0;
  ((INPUTDIA_DESC*)inp)->rc.y=HeaderH()+1+YDISP;
  ((INPUTDIA_DESC*)inp)->rc.x2=ScreenW()-1;
  ((INPUTDIA_DESC*)inp)->rc.y2=ScreenH()-SoftkeyH()-1;
}
//==============================================================================
int MainMenu_ID;

extern int CurrentStatus;
extern  int S_ICONS[];
extern const SOFTKEYSTAB menu_skt;

void ac_locret(void){}

int ac_onkey(GUI *data, GUI_MSG *msg)
{
  TPKT *p;
  char num[10];
  int key=msg->gbsmsg->submess;
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    if(key=='*'||key=='#') return (-1);
  }
  if (msg->keys==0xFFF)
  {
    if (connect_state==3)
    {
      int len;
      unsigned int uin;
      EDITCONTROL ec;
      ExtractEditControl(data,2,&ec);
      if ((len=ec.pWS->wsbody[0]))
      {
        ws_2str(ec.pWS,num,9);
        num[len]=0;
        uin=strtoul(num,0,10);
        if(uin)
        {
          p=malloc(sizeof(PKT)+len);
          p->pkt.uin=uin;
          p->pkt.type=T_ADDCONTACT;
          p->pkt.data_len=len;
          strncpy(p->data,num,len);
          SUBPROC((void *)SendAnswer,0,p);
          return (1);          
        }
      }      
    }
  }
  return(0);
}
       
void ac_ghook(GUI *data, int cmd)
{
  static SOFTKEY_DESC sk={0x0FFF,0x0000,(int)LG_ADD};
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
  }
}

static const HEADER_DESC ac_hdr={0,0,NULL,NULL,NULL,(int)LG_ADDCNT,LGP_NULL};


static const INPUTDIA_DESC ac_desc=
{
  1,
  ac_onkey,
  ac_ghook,
  (void *)ac_locret,
  0,
  &menu_skt,
  {0,NULL,NULL,NULL},
  4,
  100,
  101,
  2,
  //  0x00000001 - Выровнять по правому краю
  //  0x00000002 - Выровнять по центру
  //  0x00000004 - Инверсия знакомест
  //  0x00000008 - UnderLine
  //  0x00000020 - Не переносить слова
  //  0x00000200 - bold
  0,
  //  0x00000002 - ReadOnly
  //  0x00000004 - Не двигается курсор
  //  0x40000000 - Поменять местами софт-кнопки
  0x40000000
};


void AddContactMenu(void)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  WSHDR *ews=AllocWS(256);
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  ascii2ws(ews, LG_ENTERUIN);
  ConstructEditControl(&ec,1,0x40,ews,ews->wsbody[0]);
  AddEditControlToEditQend(eq,&ec,ma);
  ConstructEditControl(&ec,ECT_NORMAL_NUM,0x40,0,9);
  AddEditControlToEditQend(eq,&ec,ma);
  patch_header(&ac_hdr);
  patch_input(&ac_desc);
  CreateInputTextDialog(&ac_desc,&ac_hdr,eq,1,0);
  FreeWS(ews);
  GeneralFuncF1(1);
}

extern int Is_Vibra_Enabled;
extern unsigned int Is_Sounds_Enabled; 
extern int Is_Show_Offline;


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

void ChangeShowOfflineMode(void)
{
  Is_Show_Offline=!(Is_Show_Offline);
  RefreshGUI();
}

void EditConfig(void)
{
  extern const char *successed_config_filename;
  WSHDR *ws;
  ws=AllocWS(150);
  str_2ws(ws,successed_config_filename,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
  GeneralFuncF1(1);
}

void PingToServer(void)
{
  TPKT *p;
  TDate d;
  TTime t;
  GetDateTime(&d,&t);
  p=malloc(sizeof(PKT)+sizeof(TTime));
  memcpy(p->data,&t,sizeof(TTime));
  p->pkt.uin=0;
  p->pkt.type=T_ECHO;
  p->pkt.data_len=sizeof(TDate)+sizeof(TTime);
  SUBPROC((void *)SendAnswer,0,p);
}

//void AboutDlg(){};
void AboutDlg()
{
  char s[256];
  snprintf(s,255,LG_COPYRIGHT,__SVN_REVISION__);
  ShowMSG(2, (int)s);
}

static const HEADER_DESC menuhdr={0,0,0,0,NULL,(int)LG_MENU,LGP_NULL};

static const int mmenusoftkeys[]={0,1,2};

static const char * const menutexts[8]=
{
  LG_MNUSTATUS,
  LG_MNUADDCONT,
  LG_MNUVIBRA,
  LG_MNUSOUND,
  LG_MNUSHOWOFF,
  LG_MNUEDCFG,
  LG_MNUPING,
  LG_MNUABOUT
};

/*MENUITEM_DESC menuitems[7]=
{
  {S_ICONS,    (int)LG_MNUSTATUS,  LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,       (int)LG_MNUADDCONT, LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {icon_array, (int)LG_MNUVIBRA,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {icon_array, (int)LG_MNUSOUND,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,       (int)LG_MNUEDCFG,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,       (int)LG_MNUPING ,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {S_ICONS,    (int)LG_MNUABOUT,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};*/

static const void *menuprocs[8]=
{
  (void *)DispStatusChangeMenu,
  (void *)AddContactMenu,
  (void *)ChangeVibraMode,
  (void *)ChangeSoundMode,
  (void *)ChangeShowOfflineMode,
  (void *)EditConfig,
  (void *)PingToServer,
  (void *)AboutDlg,
};

static const SOFTKEY_DESC mmenu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB mmenu_skt=
{
  mmenu_sk,0
};

void tmenu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

void menuitemhandler(void *data, int curitem, int *unk)
{
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  extern const char percent_t[];
  ws=AllocMenuWS(data,strlen(menutexts[curitem]));
  wsprintf(ws,percent_t,menutexts[curitem]);
  switch(curitem)
  {
  case 0:
    SetMenuItemIconArray(data,item,S_ICONS+CurrentStatus);
    break;
  case 2:
    SetMenuItemIconArray(data,item,S_ICONS+(Is_Vibra_Enabled?IS_GROUP+1:IS_GROUP));
    break;
  case 3:
    SetMenuItemIconArray(data,item,S_ICONS+(Is_Sounds_Enabled?IS_GROUP+1:IS_GROUP));
    break;
  case 4:
    SetMenuItemIconArray(data,item,S_ICONS+(Is_Show_Offline?IS_GROUP+1:IS_GROUP));
    break;
  case 7:
    SetMenuItemIconArray(data,item,S_ICONS+IS_UNKNOWN);
    break;
  }
  SetMenuItemText(data, item, ws, curitem);
}

int tmenu_keyhook(void *data, GUI_MSG *msg)
{
  if ((msg->keys==0x18)||(msg->keys==0x3D))
  {
    ((void (*)(void))(menuprocs[GetCurMenuItem(data)]))();
  }
  return(0);
}


static const MENU_DESC tmenu=
{
  8,(void*)tmenu_keyhook,(void *)tmenu_ghook,NULL,
  mmenusoftkeys,
  &mmenu_skt,
  0x11,//MENU_FLAG,
  (void*)menuitemhandler,
  NULL,//menuitems,
  NULL,//menuprocs,
  8
};

void ShowMainMenu()
{
//  icon_array[0]=GetPicNByUnicodeSymbol(CBOX_CHECKED);
//  icon_array[1]=GetPicNByUnicodeSymbol(CBOX_UNCHECKED);
  *((int **)(&menuhdr.icon))=S_ICONS+IS_ONLINE;
  patch_header(&menuhdr);
  MainMenu_ID=CreateMenu(0,0,&tmenu,&menuhdr,0,7,0,0);
}
