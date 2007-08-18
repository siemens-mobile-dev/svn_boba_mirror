#include "../inc/swilib.h"
#include "main.h"
#include "string_util.h"

//=================================== Команды ===========================================
int Templates_Menu_ID;
int Select_Menu_ID;

int Mode;
//0 - Команды
//1 - Шаблоны сообщений
//2 - Смайлы

char **commands_lines; //Массив указателей на строки
int tmpl_num=0;

void FreeTemplates(void)
{
  if (commands_lines) mfree(commands_lines);
  commands_lines=NULL;
}

extern const char COMMANDS_PATH[];
extern const char MESSAGES_PATH[];
extern const char SMILES_PATH[];

int LoadTemplates(void)
{
  FSTATS stat;
  char fn[256];
  int f;
  unsigned int ul;
  int i;
  int fsize;
  char *p;
  char *pp;
  int c;
  FreeTemplates();
  
  switch (Mode)
  {
    case 0:
      strcpy(fn,COMMANDS_PATH);
    break;
    case 1:
      strcpy(fn,MESSAGES_PATH);
    break;
    case 2:
      strcpy(fn,SMILES_PATH);
    break;    
  }

  if (GetFileStats(fn,&stat,&ul)==-1) return 0;
  if ((fsize=stat.size)<=0) return 0;
  if ((f=fopen(fn,A_ReadOnly+A_BIN,P_READ,&ul))==-1) return 0;

  p=malloc(fsize+1);
  p[fread(f,p,fsize,&ul)]=0;
  fclose(f,&ul);
  i=0;
  pp=p;
  for(;;)
  {
    c=*p;
    if (c<32)
    {
      if (pp&&(pp!=p))
      {
	commands_lines=realloc(commands_lines,(i+1)*sizeof(char *));
	commands_lines[i++]=pp;
      }
      pp=NULL;
      if (!c) break;
      *p=0;
    }
    else
    {
      if (pp==NULL) pp=p;
    }
    p++;
  }
  return i;
}


char clm_hdr_text[48];
void UpdateTemplatesMenu_header(void)
{
  switch (Mode)
  {
    case 0:
      strcpy(clm_hdr_text,"Выбор команды...");
    break;
    case 1:
      strcpy(clm_hdr_text,"Выбор шаблона...");
    break;
    case 2:
      strcpy(clm_hdr_text,"Выбор смайла...");
    break;    
  }
}

HEADER_DESC tmpl_menuhdr={0,0,131,21,NULL,(int)clm_hdr_text,LGP_NULL};

int tmpl_menusoftkeys[]={0,1,2};
extern int edmessage_id;
extern WSHDR* ws_eddata;

void SetCmdToEditMessage(char *command)
{
  void *data=FindGUIbyId(edmessage_id,NULL);
  
  EDITCONTROL ec;
  ExtractEditControl(data,1,&ec);
  int pos=EDIT_GetCursorPos(data);
  WSHDR *ws_me = AllocWS(ec.pWS->wsbody[0]+strlen(command));
  //utf8_2ws(ws_me, command, strlen(command));

  switch (Mode)
  {
    case 0:
      ascii2ws(ws_me,command);
      wstrcpy(ws_eddata, ws_me);
      wstrcat(ws_eddata,ec.pWS);
      EDIT_SetTextToEditControl(data, 1, ws_eddata);
      EDIT_SetCursorPos(data,pos + strlen(command));
    break;
   
    case 1:
    case 2:
      {
	int c;
	char *p=command;
	wstrcpy(ws_me,ec.pWS);

        while(c=*p++)
          {
            wsInsertChar(ws_me,char8to16(c),pos++);
          }
        EDIT_SetTextToEditControl(data,1,ws_me);
        EDIT_SetCursorPos(data,pos);
      }
    break;
  } 
  FreeWS(ws_me);
  FreeTemplates();
  GeneralFunc_flag1(Templates_Menu_ID,1);
}

SOFTKEY_DESC tmpl_menu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Отмена"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB tmpl_menu_skt=
{
  tmpl_menu_sk,3
};

void tmpl_menuitemhandler(void *data, int curitem, void *unk)
{
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  extern const char percent_t[];
  ws=AllocMenuWS(data,strlen(commands_lines[curitem]));
  //wsprintf(ws,percent_t,commands_lines[curitem]);
  ascii2ws(ws,commands_lines[curitem]);
  SetMenuItemText(data, item, ws, curitem);
}

void tmpl_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

static int tmpl_menu_keyhook(void *data, GUI_MSG *msg)
{
  if ((msg->keys==0x18)||(msg->keys==0x3D))
  {
    if (tmpl_num) SetCmdToEditMessage(commands_lines[GetCurMenuItem(data)]);
       else
         {
           FreeTemplates();
           GeneralFunc_flag1(Templates_Menu_ID,1);           
         }
  }
  
  if (msg->keys==0x01)
  {
    FreeTemplates();
    GeneralFunc_flag1(Select_Menu_ID,1);
  }  
  return(0);
}

static const MENU_DESC tmpl_menu=
{
  0,tmpl_menu_keyhook,tmpl_menu_ghook,NULL,
  tmpl_menusoftkeys,
  &tmpl_menu_skt,
  8,
  tmpl_menuitemhandler,
  NULL, //menuitems,
  NULL, //menuprocs,
  0
};

HEADER_DESC tmpl_menuhdr;

void DispTemplatesMenu()
{
  UpdateTemplatesMenu_header();
  tmpl_num=LoadTemplates();
  patch_header(&tmpl_menuhdr);
  Templates_Menu_ID = CreateMenu(0,0,&tmpl_menu,&tmpl_menuhdr,0,tmpl_num,0,0);
}

//================================== Меню выбора ========================================
#ifdef NEWSGOLD
#define SEL_MANU_ITEMS_NUM 4
#else
#define SEL_MANU_ITEMS_NUM 3
#endif

HEADER_DESC sel_menuhdr={0,0,131,21,NULL,(int)"Выбор...",LGP_NULL};

int sel_menusoftkeys[]={0,1,2};

static const char * const sel_menutexts[SEL_MANU_ITEMS_NUM]=
{
  "Команды",
  "Шаблоны сообщений",
  "Смайлы"
#ifdef NEWSGOLD
  , "Закрыть диалог"  
#endif
};

SOFTKEY_DESC sel_menu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB sel_menu_skt=
{
  tmpl_menu_sk,3
};

void sel_menuitemhandler(void *data, int curitem, void *unk)
{
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  extern const char percent_t[];
  ws=AllocMenuWS(data,strlen(sel_menutexts[curitem]));
  wsprintf(ws,percent_t,sel_menutexts[curitem]);
  SetMenuItemText(data, item, ws, curitem);
}

void sel_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

static int sel_menu_keyhook(void *data, GUI_MSG *msg)
{
  if ((msg->keys==0x18)||(msg->keys==0x3D))
  {
    Mode = GetCurMenuItem(data);
    #ifdef NEWSGOLD
      if (Mode==3)
      {
        GeneralFunc_flag1(edmessage_id, 1);
        return(1);
      }
    #endif    
    
    DispTemplatesMenu();    
    GeneralFunc_flag1(Select_Menu_ID,1);
  }
  return(0);
}

static const MENU_DESC sel_menu=
{
  0,sel_menu_keyhook,sel_menu_ghook,NULL,
  sel_menusoftkeys,
  &sel_menu_skt,
  8,
  sel_menuitemhandler,
  NULL, //menuitems,
  NULL, //menuprocs,
  SEL_MANU_ITEMS_NUM
};

void DispSelectMenu()
{
  patch_header(&sel_menuhdr);
  Select_Menu_ID = CreateMenu(0,0,&sel_menu,&sel_menuhdr,0,SEL_MANU_ITEMS_NUM,0,0);
}
