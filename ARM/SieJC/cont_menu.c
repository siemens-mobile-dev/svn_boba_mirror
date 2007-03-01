#include "../inc/swilib.h"
#include "main.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "string_util.h"

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
#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}
//==============================================================================

int cmS_ICONS[1];
int cmdummy_icon[] = {0x50E,0};

#define MAX_ITEMS 8

#define MI_CONF_LEAVE       1
#define MI_CONF_KICK_THIS   2
#define MI_CONF_BAN_THIS    3
#define MI_QUERY_VERSION    4

char Menu_Contents[MAX_ITEMS-1];


int Contact_MenuID;
int Req_Close_Cont_Menu=0;
const char contmenu_header[]="Меню контакта";
int menusoftkeys[]={0,1,2};
HEADER_DESC contact_menuhdr={0,0,0,0,NULL,(int)contmenu_header,LGP_NULL};
SOFTKEY_DESC clmenu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB clmenu_skt=
{
  clmenu_sk,0
};


void contact_menu_ghook(void *data, int cmd);
int contact_menu_onkey(void *data, GUI_MSG *msg);
void contact_menu_iconhndl(void *data, int curitem, int *unk);

MENU_DESC contact_menu=
{
  8,(void *)contact_menu_onkey,(void*)contact_menu_ghook,NULL,
  menusoftkeys,
  &clmenu_skt,
  0x11,
  (void *)contact_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};



void contact_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)  // onFocus
  {
    DisableIDLETMR();
    if(Req_Close_Cont_Menu)
    {
      Req_Close_Cont_Menu=0;
      GeneralFunc_flag1(Contact_MenuID,1);
    }
  }
}

/*
  Обработчик нажатий клавиш меню. Отсюда следует вызывать необходимые действия

*/
int contact_menu_onkey(void *data, GUI_MSG *msg)
{
  int i=GetCurMenuItem(data);

if(msg->keys==0x18 || msg->keys==0x3D)  
{
  switch(Menu_Contents[i])
  {
  case MI_CONF_LEAVE: 
    {
      CLIST* room=CList_FindContactByJID(CList_GetActiveContact()->full_name);
      Leave_Conference(room->JID);
      CList_MakeAllResourcesOFFLINE(room);      
      break;
    }
/*
  case MI_CONF_KICK_THIS: 
    {
      strcpy(test_str,"Кик");
      break;
    }    
  case MI_CONF_BAN_THIS: 
    {
      strcpy(test_str,"Бан");
      break;
    }    
*/
  case MI_QUERY_VERSION: 
    {
      
      Send_Version_Request(CList_GetActiveContact()->full_name);
      break;
    }       

  default:
    {
      ShowDialog_Error(1,(int)"Действие неизвестно или не поддерживается");
    }
  }
  return 1;
}  
//  Req_Close_Cont_Menu = 1;
return 0;
}

void InitMenuArray()
{
  for(int i=0;i<MAX_ITEMS;i++)Menu_Contents[i]=0;
}

/*
  Обработчик появления пунктов динамического меню
  Тут мы сами создаём каждый пункт, указывая для него иконку и текст.
  При создании мы опираемся на данные массива Menu_Contents, в котором описано,
  какие пункты и в каком порядке необходимо создать. 
*/
void contact_menu_iconhndl(void *data, int curitem, int *unk)
{
  cmS_ICONS[0]=(int)cmdummy_icon;
  WSHDR *ws;
  extern const char percent_t[];
  char test_str[48];
  void *item=AllocMenuItem(data);
  strcpy(test_str,"(ошибка)");
  switch(Menu_Contents[curitem])
  {
  case MI_CONF_LEAVE: 
    {
      strcpy(test_str,"Покинуть MUC");
      break;
    }
  case MI_CONF_KICK_THIS: 
    {
      strcpy(test_str,"Кик");
      break;
    }    
  case MI_CONF_BAN_THIS: 
    {
      strcpy(test_str,"Бан");
      break;
    }    
  case MI_QUERY_VERSION: 
    {
      strcpy(test_str,"Версия клиента");
      break;
    }   
  }
  //ShowMSG(1,(int)test_str);
  ws=AllocMenuWS(data,strlen(test_str));
  wsprintf(ws,percent_t,test_str);
  
  SetMenuItemIconArray(data,item,cmS_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,0);  // 0 = индекс иконки
  /*
  CLIST *t;
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  
  t=FindContactByN(curitem);
  if (t)
  {
    ws=AllocMenuWS(data,strlen(t->name));
    wsprintf(ws,percent_t,t->name);
  }
  else
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,"error");
  }
  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(t));
*/  
};


void Disp_Contact_Menu()
{
  int n_items=0;
  InitMenuArray();
  
  TRESOURCE *Act_contact = CList_GetActiveContact();
// Теперь определяем, какие пункты у нас будут, и сколько
  if(!Act_contact)return;
  if(Act_contact->entry_type==T_CONF_ROOT)
  {
    Menu_Contents[n_items++]=MI_CONF_LEAVE;
  }

  if(Act_contact->entry_type==T_CONF_NODE)
  {
    Menu_Contents[n_items++]=MI_CONF_KICK_THIS;
    Menu_Contents[n_items++]=MI_CONF_BAN_THIS;    
  }   
  
  if(Act_contact->entry_type!=T_CONF_ROOT)
  {
    Menu_Contents[n_items++]=MI_QUERY_VERSION;
  }  
  if(n_items+1)
  {
    patch_rect(&contact_menuhdr.rc,0,YDISP,ScreenW()-1,HeaderH()+YDISP);
    Contact_MenuID=CreateMenu(0,0,&contact_menu,&contact_menuhdr,0,n_items,0,0);
  }
  else
  {
    ShowDialog_Error(1,(int)"Нет поддерживаемых действий");
  }
}
