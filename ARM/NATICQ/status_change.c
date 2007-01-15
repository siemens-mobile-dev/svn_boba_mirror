/*
  Интерфейс и функции смены своего статуса
*/
#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "NatICQ.h"
#include "main.h"
#include "status_change.h"

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
  inp->rc.x=0;
  inp->rc.y=HeaderH()+1+YDISP;
  inp->rc.x2=ScreenW()-1;
  inp->rc.y2=ScreenH()-SoftkeyH()-1;
}
//===============================================================================================


extern int CurrentStatus;
extern  int S_ICONS[11];

int StatChange_Menu_ID;

void Change_Status(char status)
{
    CurrentStatus = status;
    TPKT *p;
    p=malloc(sizeof(PKT)+1);
    p->pkt.uin=0;               // Никому; поле нужно проигнорировать на сервере
    p->pkt.type=T_MY_STATUS_CH; // Тип пакета: изменение статуса
    p->pkt.data_len=1;          // Длина пакета: 1 байт
    memcpy(p->data, &status, 1);

    SUBPROC((void *)SendAnswer,0,p);

    GeneralFunc_flag1(StatChange_Menu_ID,1);    
}
///////////////////////////////////////////////////////////////////////////////
// Пока так, ждём динамическую менюшку с радиобаттонами... :)
void Ch_Online()
{
  Change_Status(IS_ONLINE);
}

void Ch_Away()
{
  Change_Status(IS_AWAY);
}

void Ch_NA()
{
  Change_Status(IS_NA);
}

void Ch_Ocuppied()
{
  Change_Status(IS_OCCUPIED);
}

void Ch_DND()
{
  Change_Status(IS_DND);
}

void Ch_FFC()
{
  Change_Status(IS_FFC);
}

void Ch_Invisible()
{
  Change_Status(IS_INVISIBLE);
}

unsigned short GetStatusIndexInMenu(unsigned short status)
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
///////////////////////////////////////////////////////////////////////////////

#define STATUSES_NUM 7

HEADER_DESC st_menuhdr={0,0,131,21,NULL,(int)"Выбор статуса",0x7FFFFFFF};

int st_menusoftkeys[]={0,1,2};

MENUITEM_DESC st_menuitems[STATUSES_NUM]=
{
  {NULL,(int)"Он-лайн",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Отсутствую",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Недоступен",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},  
  {NULL,(int)"DND",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Занят",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Готов болтать",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Инвиз",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void *st_menuprocs[STATUSES_NUM]={
                                  (void *)Ch_Online,
                                  (void *)Ch_Away,
                                  (void *)Ch_NA,
                                  (void *)Ch_DND,                                  
                                  (void *)Ch_Ocuppied,
                                  (void *)Ch_FFC ,
                                  (void *)Ch_Invisible
                                 };

SOFTKEY_DESC st_menu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
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
  1,//MENU_FLAG,
  NULL,
  st_menuitems,
  st_menuprocs,
  STATUSES_NUM
};


void DispStatusChangeMenu()
{
  st_menuitems[0].icon = S_ICONS+IS_ONLINE;
  st_menuitems[1].icon = S_ICONS+IS_AWAY;  
  st_menuitems[2].icon = S_ICONS+IS_NA;  
  st_menuitems[3].icon = S_ICONS+IS_DND;  
  st_menuitems[4].icon = S_ICONS+IS_OCCUPIED;  
  st_menuitems[5].icon = S_ICONS+IS_FFC;  
  st_menuitems[6].icon = S_ICONS+IS_INVISIBLE;    
  st_menuhdr.icon=S_ICONS+CurrentStatus;
  patch_header(&st_menuhdr);
  StatChange_Menu_ID = CreateMenu(0,0,&st_tmenu,&st_menuhdr,GetStatusIndexInMenu(CurrentStatus),STATUSES_NUM,0,0);
}
