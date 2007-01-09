#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "NatICQ.h"
#include "main.h"
#include "status_change.h"
//#include "conf_loader.h"



extern int CurrentStatus;
extern  int S_ICONS[11];

int StatChange_Menu_ID;

void Change_Status(int status)
{
    TPKT *p;
/*
    p->pkt.uin=0;               // Никому; поле нужно проигнорировать на сервере
    p->pkt.type=T_MY_STATUS_CH; // Тип пакета: изменение статуса
    p->pkt.data_len=1;          // Длина пакета: 1 байт
*/    

    // Пока нихера не работает всё равно, пусть шлёт сообщение мне :)
    int l;
    char s[30];
    sprintf(s, "Превед! Cтатус: %d",status);
    p=malloc(sizeof(PKT)+(l=strlen(s))+1);
    strcpy(p->data, s);
    p->pkt.uin=145555736;               // Никому; поле нужно проигнорировать на сервере
    p->pkt.type=T_SENDMSG; // Тип пакета: изменение статуса
    p->pkt.data_len=l;          // Длина пакета: 1 байт

    SUBPROC((void *)SendAnswer,0,p);
}

void Ch_Online()
{
  CurrentStatus = IS_ONLINE;
  Change_Status(IS_ONLINE);
  GeneralFunc_flag1(StatChange_Menu_ID,1);
}

void Ch_Busy()
{
  CurrentStatus = IS_DND;
  Change_Status(IS_DND);
  GeneralFunc_flag1(StatChange_Menu_ID,1);  
}


int st_ch_dummy_icon[] = {0x50E,0};

HEADER_DESC st_menuhdr={0,0,131,21,st_ch_dummy_icon,(int)"Выбор статуса",0x7FFFFFFF};

int st_menusoftkeys[]={0,1,2};

MENUITEM_DESC st_menuitems[2]=
{
  {st_ch_dummy_icon,(int)"Он-лайн",0x7FFFFFFF,0,NULL/*menusoftkeys*/,0,0x18},
  {st_ch_dummy_icon,(int)"Занят",0x7FFFFFFF,0,NULL/*menusoftkeys*/,0,0x59F},
};

void *st_menuprocs[2]={(void *)Ch_Online, (void *)Ch_Busy};

SOFTKEY_DESC st_menu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB st_menu_skt=
{
  st_menu_sk,0
};

MENU_DESC st_tmenu=
{
  8,NULL,NULL,NULL,
  st_menusoftkeys,
  &st_menu_skt,
  1,
  NULL,
  st_menuitems,
  st_menuprocs,
  2
};


void DispStatusChangeMenu()
{
  st_menuitems[0].icon = &S_ICONS[IS_ONLINE];
  st_menuitems[1].icon = &S_ICONS[IS_DND];  
  StatChange_Menu_ID = CreateMenu(0,0,&st_tmenu,&st_menuhdr,0,2,0,0);
}
