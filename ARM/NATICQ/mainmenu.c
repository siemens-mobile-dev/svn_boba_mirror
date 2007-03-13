#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "NatICQ.h"
#include "history.h"
#include "mainmenu.h"
#include "main.h"
#include "conf_loader.h"
#include "status_change.h"
#include "language.h"

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
extern SOFTKEYSTAB menu_skt;

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

HEADER_DESC ac_hdr={0,0,NULL,NULL,NULL,(int)LG_ADDCNT,LGP_NULL};


INPUTDIA_DESC ac_desc=
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


//void AboutDlg(){};
void AboutDlg()
{
  ShowMSG(2, (int)"NATICQ v1.0\n(C) by Rst7/CBSIE\nKren\nKibab\nShadow");
}

extern void ChangeVibra(void);
extern void ChangeSound(void);

int icon[]={0,0};
int about_icon[]={0,0};
int dummy_icon[] = {0,0};

HEADER_DESC menuhdr={0,0,0,0,NULL,(int)LG_MENU,LGP_NULL};

int mmenusoftkeys[]={0,1,2};

MENUITEM_DESC menuitems[6]=
{
  {NULL,(int)LG_MNUSTATUS,  LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUADDCONT, LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUVIBRA,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUSOUND,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUEDCFG,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)LG_MNUABOUT,   LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void *menuprocs[6]=
{
  (void *)DispStatusChangeMenu,
  (void *)AddContactMenu,
  (void *)ChangeVibra,
  (void *)ChangeSound,
  (void *)EditConfig,
  (void *)AboutDlg
};

SOFTKEY_DESC mmenu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
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
    menuitems[0].icon=S_ICONS+CurrentStatus;
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
  6
};

void ShowMainMenu()
{
  menuitems[0].icon=S_ICONS+CurrentStatus;
  menuitems[5].icon=S_ICONS+IS_UNKNOWN;
  menuhdr.icon=S_ICONS+IS_ONLINE;
  patch_header(&menuhdr);
  MainMenu_ID=CreateMenu(0,0,&tmenu,&menuhdr,0,6,0,0);
}
