#include "../inc/swilib.h"
#include "main.h"
#include "jabber_util.h"
#include "bookmarks.h"
#include "lang.h"

BM_ITEM *BM_ROOT  = NULL;

void KillBMList()
{
  LockSched();
  BM_ITEM* cl=BM_ROOT;
  BM_ROOT=NULL;
  while(cl)
  {
    BM_ITEM *p;
    mfree(cl->mucname);
    mfree(cl->nick);
    if(cl->pass)mfree(cl->pass);
    p=cl;
    cl=(BM_ITEM*)(cl->next);
    mfree(p);
    p=NULL;
  }
  UnlockSched();
}

void Process_Bookmarks_Storage(XMLNode* nodeEx)
{
  XMLNode *elem = nodeEx->subnode;
  XMLNode *tmpnode;
  extern const char conference_t[];
  char jid[]="jid";
  char *n_name = NULL;
  char *c_name=NULL;
  char *c_nick=NULL;
  char *c_pass=NULL;

  while(elem)
  {
    n_name = elem->name;
    if(!n_name)return;
    if(!strcmp(n_name, conference_t))  // Элемент конференции
    {
      c_name = XML_Get_Attr_Value(jid,elem->attr);
      tmpnode = XML_Get_Child_Node_By_Name(elem, "nick");
      if(!tmpnode)return;
      c_nick = tmpnode->value;
      if(!c_nick)return;
      tmpnode = XML_Get_Child_Node_By_Name(elem, "password");
      if(tmpnode)
      {
        c_pass = tmpnode->value;
      }

      // Создаём очередной элемент списка
      BM_ITEM *bmitem = malloc(sizeof(BM_ITEM));
      bmitem->mucname = malloc(strlen(c_name)+1);
      strcpy(bmitem->mucname, c_name);

      bmitem->nick = malloc(strlen(c_nick)+1);
      strcpy(bmitem->nick, c_nick);

      if(c_pass)
      {
        bmitem->pass = malloc(strlen(c_pass)+1);
        strcpy(bmitem->pass, c_pass);
      }
      else bmitem->pass = NULL;
      bmitem->next = NULL;

      BM_ITEM *tmp=BM_ROOT;
      if(tmp)
        while(tmp->next)tmp = tmp->next;
      if(tmp)
      {
        tmp->next = bmitem;
      }
      else BM_ROOT = bmitem;
    }
    elem = elem->next;
  }

  Disp_BM_Menu();
}

//Context:HELPER
void _getbookmarkslist()
{
  static char priv_id[]="SieJC_priv_req";
  static char bm[]="<storage xmlns='storage:bookmarks'/>";
  char gget[]=IQTYPE_GET;
  char iqv[]=IQ_PRIVATE;
  SendIq(NULL, gget, priv_id, iqv, bm);
}

void Get_Bookmarks_List()
{
  if(!BM_ROOT)
  {
    SUBPROC((void*)_getbookmarkslist);
  }
  else Disp_BM_Menu();
}


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

const char bmmenu_header[]=LG_MENUBOOKMARK;
int bmmenusoftkeys[]={0,1,2};
HEADER_DESC bm_menuhdr={0,0,0,0,NULL,(int)bmmenu_header,LGP_NULL};
SOFTKEY_DESC bmmenu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB bmmenu_skt=
{
  bmmenu_sk,0
};


void bm_menu_ghook(void *data, int cmd);
int bm_menu_onkey(void *data, GUI_MSG *msg);
void bm_menu_iconhndl(void *data, int curitem, void *unk);

MENU_DESC bm_menu=
{
  8,bm_menu_onkey,bm_menu_ghook,NULL,
  bmmenusoftkeys,
  &bmmenu_skt,
  0x11,
  bm_menu_iconhndl,
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
void bm_menu_iconhndl(void *data, int curitem, void *unk)
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
  
  SetMenuItemIconArray(data,item,BM_ICON+1);
  SetMenuItemText(data,item,ws,curitem);
  //SetMenuItemIcon(data,curitem,1);  // 0 = индекс иконки
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
