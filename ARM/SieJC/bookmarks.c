#include "../inc/swilib.h"
#include "main.h"
#include "jabber_util.h"
//#include "serial_dbg.h"


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

#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}
//===============================================================================================

int BM_MenuID;
int Req_Close_BM_Menu=0;
extern BM_ITEM *BM_ROOT;

const char bmmenu_header[]="Меню закладок";
int bmmenusoftkeys[]={0,1,2};
HEADER_DESC bm_menuhdr={0,0,0,0,NULL,(int)bmmenu_header,LGP_NULL};
SOFTKEY_DESC bmmenu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB bmmenu_skt=
{
  bmmenu_sk,0
};


void bm_menu_ghook(void *data, int cmd);
int bm_menu_onkey(void *data, GUI_MSG *msg);
void bm_menu_iconhndl(void *data, int curitem, int *unk);

MENU_DESC bm_menu=
{
  8,(void *)bm_menu_onkey,(void*)bm_menu_ghook,NULL,
  bmmenusoftkeys,
  &bmmenu_skt,
  0x11,
  (void *)bm_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};



void bm_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)  // onFocus
  {
    DisableIDLETMR();
    if(Req_Close_BM_Menu)
    {
      Req_Close_BM_Menu=0;
      GeneralFunc_flag1(BM_MenuID,1);
    }
  }
}

/*
  Обработчик нажатий клавиш меню. Отсюда следует вызывать необходимые действия

*/
int bm_menu_onkey(void *data, GUI_MSG *msg)
{
  int i=GetCurMenuItem(data);
  if(msg->keys==0x18 || msg->keys==0x3D)  
  {
    char s=0;
    BM_ITEM *it = BM_ROOT;
    while(s!=i && it)
    {
      s++;
      it=it->next;    
      
    };
    Enter_Conference(it->mucname, it->nick, 20);
  }
  Req_Close_BM_Menu = 1;
return 0;
}


int BM_ICON[2];
char ICON_MUC[128];

/*
  Обработчик появления пунктов динамического меню
  Тут мы сами создаём каждый пункт, указывая для него иконку и текст.
*/
void bm_menu_iconhndl(void *data, int curitem, int *unk)
{
  WSHDR *ws;
  extern const char percent_t[], empty_t[];
  char test_str[48];
  void *item=AllocMenuItem(data);
  strcpy(test_str,empty_t);
  char s=0;
  BM_ITEM *it = BM_ROOT;
  while(s!=curitem && it)
  {
    s++;
    it=it->next;    
  };
  
  ws=AllocMenuWS(data,strlen(it->mucname));
  wsprintf(ws,percent_t,it->mucname);
  
  SetMenuItemIconArray(data,item,BM_ICON);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,1);  // 0 = индекс иконки
};

extern const char PATH_TO_PIC[128];




void Init_bm_Icon_array()
{
  strcpy(ICON_MUC, PATH_TO_PIC);strcat(ICON_MUC, "conference.png");
  BM_ICON[1]=(int)ICON_MUC;    
}

void Disp_BM_Menu()
{
  Init_bm_Icon_array();
  int n_items=0;
  BM_ITEM *i;
  if(BM_ROOT)
  {
    i=BM_ROOT;
    n_items=1;
    while(i->next)
    {
      n_items++;
      i = i->next;
    }
    patch_rect(&bm_menuhdr.rc,0,YDISP,ScreenW()-1,HeaderH()+YDISP);
    BM_MenuID=CreateMenu(0,0,&bm_menu,&bm_menuhdr,0,n_items,0,0);
  }
}
//EOL,EOF
