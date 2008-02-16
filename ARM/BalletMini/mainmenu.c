#include "..\inc\swilib.h"
#include "rect_patcher.h"
#include "view.h"
#include "main.h"
#include "string_works.h"
#include "mainmenu.h"
#include "conf_loader.h"
#include "urlstack.h"

extern int view_url_mode; //MODE_FILE, MODE_URL
extern char *view_url;
extern char *goto_url;
extern int maincsm_id;
extern char BOOKMARKS_PATH[];


typedef struct
{
  void *next;
  char urlname[64];
  char fullpath[128];
}URL_STRUCT;

//------------------------------------------------------------------------------

static const SOFTKEY_DESC add_bookmark_sk[]=
{
  {0x0018,0x0000,(int)"Добавить"},
  {0x0001,0x0000,(int)"Отмена"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB add_bookmark_skt=
{
  add_bookmark_sk,0
};


static const HEADER_DESC add_bookmark_hdr={0,0,0,0,NULL,(int)"Закладка...",LGP_NULL};

static void add_bookmark_ghook(GUI *data, int cmd)
{
  static SOFTKEY_DESC sk={0x0FFF,0x0000,(int)"Сохранить"};
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
  }
}

static void add_bookmark_locret(void){}

static int add_bookmark_onkey(GUI *data, GUI_MSG *msg)
{
  EDITCONTROL ec;
  WSHDR *ws;
  char *name, *url, *pathbuf, *tmp;
  unsigned ul;
  int f;

  if (msg->keys==0xFFF)
  {
    ExtractEditControl(data,2,&ec);
    ws = ec.pWS;
    tmp = name = (char *)malloc(ws->wsbody[0]+3);
    for (int i=0; i<ws->wsbody[0]; i++) *name++=char16to8(ws->wsbody[i+1]);
    *name = 0;
    name = tmp;

    ExtractEditControl(data,4,&ec);
    ws = ec.pWS;
    tmp = url = (char *)malloc(ws->wsbody[0]+3);
    for (int i=0; i<ws->wsbody[0]; i++) *url++=char16to8(ws->wsbody[i+1]);
    *url = 0;
    url = tmp;

    pathbuf = malloc(strlen(BOOKMARKS_PATH) + strlen(name) + strlen(".url") + 1);
    strcpy(pathbuf, BOOKMARKS_PATH); strcat(pathbuf, name); strcat(pathbuf, ".url");
    unlink(pathbuf,&ul);
    f=fopen(pathbuf,A_WriteOnly+A_Create+A_BIN,P_READ+P_WRITE,&ul);
    if (f!=-1)
    {
      url = ToWeb(url, 0);
      fwrite(f,url,strlen(url),&ul);
      fclose(f,&ul);
    }
    mfree(url);
    mfree(name);
    mfree(pathbuf);
      
    return (1);
  }
  return (0);
}

static const INPUTDIA_DESC add_bookmark_desc =
{
  1,
  add_bookmark_onkey,
  add_bookmark_ghook,
  (void *)add_bookmark_locret,
  0,
  &add_bookmark_skt,
  {0,0,0,0},
  FONT_SMALL,
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


int CreateAddBookmark(GUI *data)
{
  void *ma=malloc_adr();
  void *eq;
  MAIN_CSM *main_csm;
  VIEW_GUI *p;
  EDITCONTROL ec;
  char *tmp = view_url;
  int tmp2 = view_url_mode, flag = 0;
  
  eq=AllocEQueue(ma,mfree_adr());    // Extension
  WSHDR *ews=AllocWS(1024);

  URL_STRUCT *ustop = MenuGetUserPointer(data);

  if(data)
    if (ReadUrlFile(ustop->fullpath))
      flag = 1;
  
  ascii2ws(ews,"Имя:");
  ConstructEditControl(&ec,ECT_HEADER,ECF_APPEND_EOL,ews,wslen(ews));
  AddEditControlToEditQend(eq,&ec,ma); 
      
  if(data && flag)
    ascii2ws(ews,ustop->urlname);
  else
    if ((main_csm=(MAIN_CSM *)FindCSMbyID(maincsm_id)))
    {
      p=FindGUIbyId(main_csm->view_id,NULL);
      if (p->vd->title)
        ascii2ws(ews,p->vd->title);
      else
        ascii2ws(ews,"New bookmark");
    }
    
    
  PrepareEditControl(&ec);
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,ews,64);
  AddEditControlToEditQend(eq,&ec,ma);   //2

  ascii2ws(ews,"Ссылка:");
  ConstructEditControl(&ec,ECT_HEADER,ECF_APPEND_EOL,ews,wslen(ews));
  AddEditControlToEditQend(eq,&ec,ma); 

  if(data && flag)
    ascii2ws(ews,view_url+2);
  else
    if ((main_csm=(MAIN_CSM *)FindCSMbyID(maincsm_id)))
    {
      p=FindGUIbyId(main_csm->view_id,NULL);
      if (p->vd->pageurl)
        str_2ws(ews,p->vd->pageurl+2,strlen(p->vd->pageurl)-2);
      else
      {
        // url не загружен
        switch(view_url_mode)
        {
        case MODE_FILE:
          str_2ws(ews,view_url,255);
          break;
        case MODE_URL:
          ascii2ws(ews,view_url+2);
          break;
        }
      }
    }

  PrepareEditControl(&ec);
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,ews,1024);
  AddEditControlToEditQend(eq,&ec,ma);   //2

  view_url = tmp;
  view_url_mode = tmp2;
    
  FreeWS(ews);
  patch_header(&add_bookmark_hdr);
  patch_input(&add_bookmark_desc);
  return CreateInputTextDialog(&add_bookmark_desc,&add_bookmark_hdr,eq,1,0);
}

static void do_add_bookmark(GUI *data)
{
  CreateAddBookmark(0);
  GeneralFuncF1(2);  
}
//------------------------------------------------------------------------------

static void do_edit_bookmark(GUI *data)
{
  CreateAddBookmark(data);
  GeneralFuncF1(2);  
}

//------------------------------------------------------------------------------

static void do_delete_bookmark(GUI *data)
{
  URL_STRUCT *ustop = MenuGetUserPointer(data);
  unsigned ul;
  if(ustop)
    if(ustop->fullpath)
      unlink(ustop->fullpath,&ul);
  GeneralFuncF1(2);  
}
  
//------------------------------------------------------------------------------

void back(GUI *data)
{
  GeneralFuncF1(1);
}


#define OPTIONS_ITEMS_N 4
HEADER_DESC options_menuhdr={0,0,0,0,NULL,(int)"Опции",LGP_NULL};

MENUITEM_DESC options_menu_ITEMS[OPTIONS_ITEMS_N]=
{
  {NULL,(int)"Добавить",            LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}, //0
  {NULL,(int)"Изменить",            LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}, //1
  {NULL,(int)"Удалить",             LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}, //2
  {NULL,(int)"Назад",               LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}, //3
};

const MENUPROCS_DESC options_menu_HNDLS[OPTIONS_ITEMS_N]=
{
  do_add_bookmark,  
  do_edit_bookmark,  
  do_delete_bookmark,  
  back,  
};

int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Options"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

MENU_DESC options_menu_STRUCT=
{
  0,(void *)NULL,(void*)NULL,NULL,
  menusoftkeys,
  &menu_skt,
  8,
  NULL,
  options_menu_ITEMS,   //Items
  options_menu_HNDLS,   //Procs
  OPTIONS_ITEMS_N //n
};

void create_options_menu(URL_STRUCT *ustop)
{
  int to_remove[OPTIONS_ITEMS_N+1];
  int n = 0;
  if(!ustop)
  {
    to_remove[++n]=1;
    to_remove[++n]=2;
  }
  to_remove[0]=n;
  patch_header_small((HEADER_DESC*)(&options_menuhdr));
  CreateMenu(1,0,&options_menu_STRUCT,&options_menuhdr,0,OPTIONS_ITEMS_N,ustop,to_remove);
}

// ----------------------------------------------------------------------------------


int selurl_menu_onkey(void *gui, GUI_MSG *msg) //bookmarks
{
  int l;
  URL_STRUCT *ustop=MenuGetUserPointer(gui);

  int i=GetCurMenuItem(gui);
  for (int n=0; n!=i; n++) ustop=ustop->next;
  if (ustop)
  {
    if (msg->keys==0x3D)
    {
      if (ReadUrlFile(ustop->fullpath))
      {
        goto_url=malloc((l=strlen(view_url))+1);
        memcpy(goto_url,view_url,l+1);
        return (0xFF);
      }
      return(1);
    }
    if(msg->keys==0x18)
    {
      create_options_menu(ustop);
      return(1);
    }
  }
  else
    if(msg->keys==0x18)
    {
      create_options_menu(0);
      return(1);
    }

/*
  if (msg->keys==0x3D)
  {
    int i=GetCurMenuItem(gui);
    for (int n=0; n!=i; n++) ustop=ustop->next;
    if (ustop)
    {
      if (ReadUrlFile(ustop->fullpath))
      {
        goto_url=malloc((l=strlen(view_url))+1);
        memcpy(goto_url,view_url,l+1);
        return (0xFF);
      }
    }
  }
  if(msg->keys==0x18)
  {
    create_options_menu();
    return(1);
  }
*/
  return (0);
}

void selurl_menu_ghook(void *gui, int cmd)
{
  URL_STRUCT *ustop=MenuGetUserPointer(gui);
  if (cmd==3)
  {
    while(ustop)
    {
      URL_STRUCT *us=ustop;
      ustop=ustop->next;
      mfree(us);
    }    
  }
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

void selurl_menu_iconhndl(void *gui, int cur_item, void *user_pointer)
{
  URL_STRUCT *ustop=user_pointer;
  WSHDR *ws;
  int len;
  for (int n=0; n!=cur_item; n++) ustop=ustop->next;
  void *item=AllocMenuItem(gui);
  if (ustop)
  {
    len=strlen(ustop->urlname);
    ws=AllocMenuWS(gui,len+4);
    ascii2ws(ws,ustop->urlname);
    //str_2ws(ws,ustop->urlname,64);
  }
  else
  {
    ws=AllocMenuWS(gui,10);
    ascii2ws(ws,"Ошибка");
  }
  SetMenuItemText(gui, item, ws, cur_item);
}

int selurl_softkeys[]={0,1,2};
SOFTKEY_DESC selurl_sk[]=
{
  {0x0018,0x0000,(int)"Опции"},
  {0x0001,0x0000,(int)"Отмена"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB selurl_skt=
{
  selurl_sk,0
};

HEADER_DESC selurl_HDR={0,0,0,0,NULL,(int)"Select Bookmark",LGP_NULL};


MENU_DESC selurl_STRUCT=
{
  8,selurl_menu_onkey,selurl_menu_ghook,NULL,
  selurl_softkeys,
  &selurl_skt,
  0x10,
  selurl_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

int CreateBookmarksMenu()
{
  unsigned int err;
  DIR_ENTRY de;
  const char *s;
  URL_STRUCT *ustop=0, *usbot=0;
  URL_STRUCT *us;
  int n_url=0;
  char str[128];
  s=BOOKMARKS_PATH;
  strcpy(str,s);
  strcat(str,"*.url");
  if (FindFirstFile(&de,str,&err))
  {
    do
    {
      if (!(de.file_attr&FA_DIRECTORY))
      {
        us=malloc(sizeof(URL_STRUCT));
        strcpy(us->fullpath,s);
        strcat(us->fullpath,de.file_name);
        strcpy(us->urlname,de.file_name);
        us->urlname[strlen(de.file_name)-4]=0;
        us->next=0;
        if (usbot)
        {
          usbot->next=us;
          usbot=us;
        }
        else
        {
          ustop=us;
          usbot=us;
        }
        n_url++;
      }
    }
    while(FindNextFile(&de,&err));
  }
  FindClose(&de,&err);
  patch_header(&selurl_HDR);
  return CreateMenu(0,0,&selurl_STRUCT,&selurl_HDR,0,n_url,ustop,0);
}

//------------------------------------------------------------------------------

static const SOFTKEY_DESC input_menu_sk[]=
{
  {0x0018,0x0000,(int)"Перейти"},
  {0x0001,0x0000,(int)"Отмена"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB input_menu_skt=
{
  input_menu_sk,0
};


static const HEADER_DESC input_url_hdr={0,0,0,0,NULL,(int)"Адрес",LGP_NULL};

static void input_url_ghook(GUI *data, int cmd)
{
  static SOFTKEY_DESC sk={0x0FFF,0x0000,(int)"Перейти"};
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
  }
}

static void input_url_locret(void){}

static int input_url_onkey(GUI *data, GUI_MSG *msg)
{
  EDITCONTROL ec;
  WSHDR *ws;
  char *s;
  if (msg->keys==0xFFF)
  {
    ExtractEditControl(data,1,&ec);
    ws = ec.pWS;
    s = goto_url = (char *)malloc(ws->wsbody[0]+3);
    *s++='0';
    *s++='/';
    for (int i=0; i<ws->wsbody[0]; i++) *s++=char16to8(ws->wsbody[i+1]);
    *s = 0;
    goto_url = ToWeb(goto_url,0);
    return (0xFF);
  }
  return (0);
}

static const INPUTDIA_DESC input_url_desc =
{
  1,
  input_url_onkey,
  input_url_ghook,
  (void *)input_url_locret,
  0,
  &input_menu_skt,
  {0,0,0,0},
  FONT_SMALL,
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


int CreateInputUrl()
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  
  eq=AllocEQueue(ma,mfree_adr());    // Extension
  WSHDR *ews=AllocWS(1024);

  MAIN_CSM *main_csm;
  if ((main_csm=(MAIN_CSM *)FindCSMbyID(maincsm_id)))
  {
    VIEW_GUI *p=FindGUIbyId(main_csm->view_id,NULL);
    if (p->vd->pageurl)
      str_2ws(ews,p->vd->pageurl+2,strlen(p->vd->pageurl)-2);
    else
    {
      // url не загружен
      switch(view_url_mode)
      {
      case MODE_FILE:
        str_2ws(ews,view_url,255);
        break;
      case MODE_URL:
        ascii2ws(ews,view_url+2);
        break;
      }
    }
  }

  PrepareEditControl(&ec);
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,ews,1024);
  AddEditControlToEditQend(eq,&ec,ma);   //2

  FreeWS(ews);
  patch_header(&input_url_hdr);
  patch_input(&input_url_desc);
  return CreateInputTextDialog(&input_url_desc,&input_url_hdr,eq,1,0);
}

//------------------------------------------------------------------------------

static void mm_goto_url(GUI *gui)
{
  MAIN_CSM *main_csm;
  int goto_url_id;
  if ((main_csm=(MAIN_CSM *)FindCSMbyID(maincsm_id)))
  {
    goto_url_id=CreateInputUrl();
    main_csm->goto_url=goto_url_id;
  } 
  GeneralFuncF1(1);  
}

static void mm_goto_bookmarks(GUI *gui)
{
  MAIN_CSM *main_csm;
  int bookmark_menu_id;
  if ((main_csm=(MAIN_CSM *)FindCSMbyID(maincsm_id)))
  {
    bookmark_menu_id=CreateBookmarksMenu();
    main_csm->sel_bmk=bookmark_menu_id;
  } 
  GeneralFuncF1(1);
}

static void mm_options(GUI *gui)
{
  WSHDR *ws;
  ws=AllocWS(150);
  str_2ws(ws,successed_config_filename,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
  GeneralFuncF1(1);
}

static void mm_quit(GUI *gui)
{
  MAIN_CSM *main_csm;
  if ((main_csm=(MAIN_CSM *)FindCSMbyID(maincsm_id)))
  {
    FreePageStack();
    goto_url = 0;
    GeneralFunc_flag1(main_csm->view_id,0xFF);
//    GeneralFunc_flag1(1,main_csm->view_id);
    GeneralFuncF1(1);
  }
  else
  {
    GeneralFuncF1(1);
  }
}

static const int mmenusoftkeys[]={0,1,2};

static const SOFTKEY_DESC mmenu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB mmenu_skt=
{
  mmenu_sk,0
};

#define MAIN_MENU_ITEMS_N 4
static HEADER_DESC main_menuhdr={0,0,0,0,NULL,(int)"Меню",LGP_NULL};

static MENUITEM_DESC main_menu_ITEMS[MAIN_MENU_ITEMS_N]=
{
  {NULL,(int)"Перейти к",    LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}, //0
  {NULL,(int)"Закладки",     LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}, //1
  {NULL,(int)"Настройки",    LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}, //2
  {NULL,(int)"Выход",        LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2} //3
};

static const MENUPROCS_DESC main_menu_HNDLS[MAIN_MENU_ITEMS_N]=
{
  mm_goto_url,
  mm_goto_bookmarks,
  mm_options,
  mm_quit
};

static int mainmenu_keyhook(void *data, GUI_MSG *msg)
{
  return(0);
}

static void mainmenu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

static const MENU_DESC mainmenu=
{
  8,mainmenu_keyhook,mainmenu_ghook,NULL,
  mmenusoftkeys,
  &mmenu_skt,
  0x10,//MENU_FLAG,
  NULL,
  main_menu_ITEMS,//menuitems,
  main_menu_HNDLS,//menuprocs,
  MAIN_MENU_ITEMS_N
};

int CreateMainMenu(VIEWDATA *vd)
{
  MAIN_CSM *main_csm;
  int main_menu_id;
  if ((main_csm=(MAIN_CSM *)FindCSMbyID(maincsm_id)))
  {
    patch_header(&main_menuhdr);
    main_menu_id=CreateMenu(0,0,&mainmenu,&main_menuhdr,0,MAIN_MENU_ITEMS_N,vd,0);
    main_csm->main_menu_id=main_menu_id;
  }
  return main_menu_id;
}
