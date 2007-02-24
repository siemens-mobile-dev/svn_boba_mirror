/*
     SieNatJabber Project
    UI для смены своего статуса
*/
#include "../inc/swilib.h"
#include "main.h"
#include "jabber_util.h"
#include "status_change.h"
#include "string_util.h"

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


extern int My_Presence;
extern const char* PRESENCES[PRES_COUNT];
int StatChange_Menu_ID;

void Change_Status(char status)
{
    //CurrentStatus = status;
    GeneralFunc_flag1(StatChange_Menu_ID,1);    
}

#define STATUSES_NUM 6

HEADER_DESC st_menuhdr={0,0,131,21,NULL,(int)"Выбор статуса",LGP_NULL};

int st_menusoftkeys[]={0,1,2};

// "online", "chat", "away", "xa", "dnd", "invisible"
MENUITEM_DESC st_menuitems[STATUSES_NUM]=
{
  {NULL,(int)"Онлайн",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Готов болтать",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Отсутствую",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},  
  {NULL,(int)"Недоступен",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Занят",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Инвиз",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

void dummy(void){};
void *st_menuprocs[STATUSES_NUM]={
                                  (void *)dummy,
                                  (void *)dummy,
                                  (void *)dummy,
                                  (void *)dummy,                                  
                                  (void *)dummy,
                                  (void *)dummy
                                 };

SOFTKEY_DESC st_menu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB st_menu_skt=
{
  st_menu_sk,3
};

WSHDR* ews;
char sTerminate=0;
char Selected_Status=0;

int ed1_onkey(GUI *data, GUI_MSG *msg)
{
  //-1 - do redraw
  if(msg->gbsmsg->submess==GREEN_BUTTON)
  {
    sTerminate = 1;
    return 1;
  }
  return 0;
  //1: close
}

void ed1_ghook(GUI *data, int cmd)
{
  EDITCONTROL ec;
/*  
  if (cmd==7)
  {
    //OnRun
    ExtractEditControl(data,EDIT_GetFocus(data)-1,&ec);
    wstrcpy(ews,ec.pWS);
  }
*/  
  if(cmd==0x0A)   // Фокусирование
  {
     DisableIDLETMR();   // Отключаем таймер выхода по таймауту
  }


  if(sTerminate || cmd==9)  // cmd==9 - нажатие на левую софт-кнопку "ОК"
 {
//     char q[10];
//     sprintf(q,"N=%d",cmd);
//     ShowMSG(1,(int)q); 
   sTerminate=0;
   ExtractEditControl(data,2,&ec);    
   wstrcpy(ews,ec.pWS);
   size_t xz = wstrlen(ews)*2;
   char* body;
   if(xz)
   {
      body =  utf16_to_utf8((char**)ews,&xz);
      body[xz]='\0';
   }
   else body = NULL;
    PRESENCE_INFO *pr_info = malloc(sizeof(PRESENCE_INFO));
    extern long  strtol (const char *nptr,char **endptr,int base);
    ExtractEditControl(data,4,&ec);    // = priority
    wstrcpy(ews,ec.pWS);
    char ss[10];
    ws_2str(ews,ss,15);
    pr_info->priority = strtol (ss,0,10);
    pr_info->status=Selected_Status;
    pr_info->message=body;  
    SUBPROC((void*)Send_Presence,pr_info);
    FreeWS(ews);
   }
}

HEADER_DESC ed1_hdr={0,0,131,21,NULL,(int)"Статус",0x7FFFFFFF};

INPUTDIA_DESC ed1_desc=
{
  1,
  ed1_onkey,
  ed1_ghook,
  (void *)dummy,
  0,
  &st_menu_skt,
  {0,22,131,153},
  4,
  100,
  101,
  0,

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

void Disp_AddSettings_Dialog()
{
  void *ma=malloc_adr();
  extern const char percent_t[];
  void *eq;
  EDITCONTROL ec;
  ews=AllocWS(256);
  
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  
  wsprintf(ews,percent_t,"Введите текст статуса:");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"");
  ConstructEditControl(&ec,3,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);  

  wsprintf(ews,percent_t,"Приоритет:");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);  

  wsprintf(ews,"0");
  ConstructEditControl(&ec,5,0x40,ews,2);
  AddEditControlToEditQend(eq,&ec,ma);
  
  patch_input(&ed1_desc);
  patch_header(&ed1_hdr);
  CreateInputTextDialog(&ed1_desc,&ed1_hdr,eq,1,0);
}

int menu_onKey(void *data, GUI_MSG *msg)
{
  if (msg->keys==0x3D)
  {    
    int i=GetCurMenuItem(data);
    Selected_Status=i;
    Disp_AddSettings_Dialog();
    return(-1);
  }  return 0;
}

MENU_DESC st_tmenu=
{
  8,(void*)menu_onKey,NULL,NULL,
  st_menusoftkeys,
  &st_menu_skt,
  1,//MENU_FLAG,
  NULL,
  st_menuitems,
  st_menuprocs,
  STATUSES_NUM
};

//int SS_ICONS[STATUSES_NUM];

void DispStatusChangeMenu()
{
  /*
  st_menuitems[0].icon = S_ICONS+IS_ONLINE;
  st_menuitems[1].icon = S_ICONS+IS_AWAY;  
  st_menuitems[2].icon = S_ICONS+IS_NA;  
  st_menuitems[3].icon = S_ICONS+IS_DND;  
  st_menuitems[4].icon = S_ICONS+IS_OCCUPIED;  
  st_menuitems[5].icon = S_ICONS+IS_FFC;  
  st_menuitems[6].icon = S_ICONS+IS_INVISIBLE;    
  st_menuhdr.icon=S_ICONS+CurrentStatus;
*/
  patch_header(&st_menuhdr);
  sTerminate =0;
  Selected_Status = 0;
  StatChange_Menu_ID = CreateMenu(0,0,&st_tmenu,&st_menuhdr,0,STATUSES_NUM,0,0);
}
