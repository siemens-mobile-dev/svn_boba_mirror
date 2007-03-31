#include "..\inc\swilib.h"

extern long  strtol (const char *nptr,char **endptr,int base);
extern unsigned long  strtoul (const char *nptr,char **endptr,int base);
extern void EditCoordinates(unsigned int*xy);
extern void EditColors(char*color);
#pragma inline
void patch_input(INPUTDIA_DESC* inp,int x,int y,int x2,int y2)
{
  inp->rc.x=x;
  inp->rc.y=y+YDISP;
  inp->rc.x2=x2;
  inp->rc.y2=y2;
}

#pragma inline
void patch_header(HEADER_DESC* head,int x,int y,int x2,int y2)
{
  head->rc.x=x;
  head->rc.y=y+YDISP;
  head->rc.x2=x2;
  head->rc.y2=y2+YDISP;
}
#include "..\inc\cfg_items.h"

unsigned int level=0;
CFG_HDR *levelstack[16];

int icon[]={0x58,0};
const int minus11=-11;

char cfg_name[256];

//Указатель на буфер конфигурации
char *cfg;
//Длинна файла конфигурации
int size_cfg;

CFG_HDR *cfg_h[512];
int total_items;

SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Лев"},
  {0x0001,0x0000,(int)"Прав"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

WSHDR *ews;

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

const char _percent_u[]="%u";
const char _percent_d[]="%d";
const char _percent_t[]="%t";

int create_ed(void);
unsigned int char16to8(unsigned int c);

void ed1_locret(void){}

int ed1_onkey(GUI *data, GUI_MSG *msg)
{
  EDITCONTROL ec;
  CFG_HDR *hp;
  int l;
  int i;
  int n;
  char ss[16];
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    l=msg->gbsmsg->submess;
    i=EDIT_GetFocus(data);
    ExtractEditControl(data,i,&ec);
    if ((i>1)&&(i&1))
    {
      n=(i-3)>>1; //Индекс элемента в массиве cfg_h
      hp=cfg_h[n];
      wstrcpy(ews,ec.pWS);
      ws_2str(ews,ss,15);
      if (l==LEFT_SOFT||l==ENTER_BUTTON)
      {
        switch(hp->type)
        {
        case CFG_COORDINATES:
          EditCoordinates((unsigned int *)(hp+1));
          break;
        case CFG_COLOR:
          EditColors((char *)(hp+1));
          break;
	case CFG_LEVEL:
	  level++;
	  levelstack[level]=hp;
	  level++;
	  return 1;
        default:
          return(0);
        }
        return (-1);
      }
    }
  }
  return(0); //Do standart keys
  //1: close
}

void ed1_ghook(GUI *data, int cmd)
{
//  static SOFTKEY_DESC sk={0x0018,0x0000,(int)"Menu"};
  EDITCONTROL ec;
  int i;
  int n;
  int j;
  int vi;
  unsigned int vui;
  int pos;
  char ss[16];
  char *p;

  CFG_HDR *hp;
  if (cmd==2)
  {
    //Create
  }
  if (cmd==7)
  {
    i=EDIT_GetFocus(data);
    ExtractEditControl(data,i,&ec);
    if ((i>1)&&(i&1))
    {
      n=(i-3)>>1; //Индекс элемента в массиве cfg_h
      hp=cfg_h[n];
      wstrcpy(ews,ec.pWS);
      ws_2str(ews,ss,15);
      switch(hp->type)
      {
      case CFG_UINT:
        pos=EDIT_GetCursorPos(data);
        vui=strtoul(ss,0,10);
        if (vui<hp->min) vui=hp->min;
        if (vui>hp->max) vui=hp->max;
        *((unsigned int *)(hp+1))=vui;
        wsprintf(ews,_percent_u,vui);
        EDIT_SetTextToFocused(data,ews);
        EDIT_SetCursorPos(data,pos);
        break;
      case CFG_INT:
        pos=EDIT_GetCursorPos(data);
        vi=strtol(ss,0,10);
        if (vi<(int)hp->min) vi=(int)hp->min;
        if (vi>(int)hp->max) vi=(int)hp->max;
        *((int *)(hp+1))=vi;
        wsprintf(ews,_percent_d,vi);
        EDIT_SetTextToFocused(data,ews);
        EDIT_SetCursorPos(data,pos);
        break;
      case CFG_STR_UTF8:
        ws_2str(ews,(char *)(hp+1),hp->max);
        break;
      case CFG_STR_PASS:
      case CFG_STR_WIN1251:
        j=0;
        p=(char *)(hp+1);
        while(j<hp->max)
        {
          if (j>=wstrlen(ews)) break;
          *p++=char16to8(ews->wsbody[j+1]);
          j++;
        }
        *p=0;
        break;
      case CFG_CBOX:
         *((int *)(hp+1))=EDIT_GetItemNumInFocusedComboBox(data)-1;
         break;
      case CFG_COORDINATES:
        wsprintf(ews,"%d,%d",*((int *)(hp+1)),*((int *)(hp+1)+1));
        EDIT_SetTextToFocused(data,ews);
        break;
      case CFG_COLOR:
        wsprintf(ews,"%02X,%02X,%02X,%02X",*((char *)(hp+1)),*((char *)(hp+1)+1),*((char *)(hp+1)+2),*((char *)(hp+1)+3));
        EDIT_SetTextToFocused(data,ews);        
        break;
      default:
        break;
      }
    }
  }
  if (cmd==0x0D)
  {
    //onCombo
    i=EDIT_GetFocus(data);
    ExtractEditControl(data,i,&ec);
    if ((i>1)&&(i&1))
    {
      n=(i-3)>>1; //Индекс элемента в массиве cfg_h
      hp=cfg_h[n];
      if (hp->type==CFG_CBOX)
      {
        if ((j=EDIT_GetItemNumInFocusedComboBox(data)))
        {
          wsprintf(ews,_percent_t,((CFG_CBOX_ITEM*)((char *)hp+sizeof(CFG_HDR)+4))+(j-1));
        }
        else
        {
          ExtractEditControl(data,EDIT_GetFocus(data)-1,&ec);
          wstrcpy(ews,ec.pWS);
        }
        EDIT_SetTextToFocused(data,ews);
      }
    }
  }
}

HEADER_DESC ed1_hdr={0,0,0,0,NULL,(int)"Edit Config",LGP_NULL};

INPUTDIA_DESC ed1_desc=
{
  1,
  ed1_onkey,
  ed1_ghook,
  (void *)ed1_locret,
  0,
  &menu_skt,
  {0,0,0,0},
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

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  ews=AllocWS(256);
  csm->gui_id=create_ed();
  csm->csm.state=0;
  csm->csm.unk1=0;
}

void Killer(void)
{
  extern void *ELF_BEGIN;
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

void SaveConfig(void)
{
  unsigned int ul;
  int f=fopen(cfg_name,A_ReadWrite+A_Create+A_Truncate+A_BIN,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    fwrite(f,cfg,size_cfg,&ul);
    fclose(f,&ul);
  }
  GBS_SendMessage(MMI_CEPID, MSG_RECONFIGURE_REQ,0,cfg_name);
}

void maincsm_onclose(CSM_RAM *csm)
{
  FreeWS(ews);
  if (cfg) mfree(cfg);
  SUBPROC((void *)Killer);
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      if (level)
      {
	levelstack[level]=NULL;
	level--;
	csm->gui_id=create_ed();
	return 1;
      }
      if ((int)msg->data1==1)
	SaveConfig();
      else 
	csm->csm.state=-3;
      csm->gui_id=0;
    }
  }
  if ((msg->msg==MSG_RECONFIGURE_REQ)&&(cfg_name==(char *)msg->data0))
  {
    csm->csm.state=-3;
  }
  return(1);
}

unsigned short maincsm_name_body[140];

const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM =
{
  {
  maincsm_onmessage,
  maincsm_oncreate,
#ifdef NEWSGOLD
  0,
  0, 
  0, 
  0,
#endif
  maincsm_onclose,
  sizeof(MAIN_CSM),
  1,
  &minus11
  },
  {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};



typedef struct
{
  unsigned short u;
  char dos;
  char win;
  char koi8;
} TUNICODE2CHAR;

const TUNICODE2CHAR unicode2char[]=
{
  // CAPITAL Cyrillic letters (base)
  0x410,0x80,0xC0,0xE1, // А
  0x411,0x81,0xC1,0xE2, // Б
  0x412,0x82,0xC2,0xF7, // В
  0x413,0x83,0xC3,0xE7, // Г
  0x414,0x84,0xC4,0xE4, // Д
  0x415,0x85,0xC5,0xE5, // Е
  0x416,0x86,0xC6,0xF6, // Ж
  0x417,0x87,0xC7,0xFA, // З
  0x418,0x88,0xC8,0xE9, // И
  0x419,0x89,0xC9,0xEA, // Й
  0x41A,0x8A,0xCA,0xEB, // К
  0x41B,0x8B,0xCB,0xEC, // Л
  0x41C,0x8C,0xCC,0xED, // М
  0x41D,0x8D,0xCD,0xEE, // Н
  0x41E,0x8E,0xCE,0xEF, // О
  0x41F,0x8F,0xCF,0xF0, // П
  0x420,0x90,0xD0,0xF2, // Р
  0x421,0x91,0xD1,0xF3, // С
  0x422,0x92,0xD2,0xF4, // Т
  0x423,0x93,0xD3,0xF5, // У
  0x424,0x94,0xD4,0xE6, // Ф
  0x425,0x95,0xD5,0xE8, // Х
  0x426,0x96,0xD6,0xE3, // Ц
  0x427,0x97,0xD7,0xFE, // Ч
  0x428,0x98,0xD8,0xFB, // Ш
  0x429,0x99,0xD9,0xFD, // Щ
  0x42A,0x9A,0xDA,0xFF, // Ъ
  0x42B,0x9B,0xDB,0xF9, // Ы
  0x42C,0x9C,0xDC,0xF8, // Ь
  0x42D,0x9D,0xDD,0xFC, // Э
  0x42E,0x9E,0xDE,0xE0, // Ю
  0x42F,0x9F,0xDF,0xF1, // Я
  // CAPITAL Cyrillic letters (additional)
  0x402,'_',0x80,'_', // _ .*.*
  0x403,'_',0x81,'_', // _ .*.*
  0x409,'_',0x8A,'_', // _ .*.*
  0x40A,'_',0x8C,'_', // _ .*.*
  0x40C,'_',0x8D,'_', // _ .*.*
  0x40B,'_',0x8E,'_', // _ .*.*
  0x40F,'_',0x8F,'_', // _ .*.*
  0x40E,0xF6,0xA1,'_', // Ў ...*
  0x408,0x4A,0xA3,0x4A, // _ .*.*
  0x409,0x83,0xA5,0xBD, // _ .*..
  0x401,0xF0,0xA8,0xB3, // Ё
  0x404,0xF2,0xAA,0xB4, // Є
  0x407,0xF4,0xAF,0xB7, // Ї
  0x406,0x49,0xB2,0xB6, // _ .*..
  0x405,0x53,0xBD,0x53, // _ .*.*
  // SMALL Cyrillic letters (base)
  0x430,0xA0,0xE0,0xC1, // а
  0x431,0xA1,0xE1,0xC2, // б
  0x432,0xA2,0xE2,0xD7, // в
  0x433,0xA3,0xE3,0xC7, // г
  0x434,0xA4,0xE4,0xC4, // д
  0x435,0xA5,0xE5,0xC5, // е
  0x436,0xA6,0xE6,0xD6, // ж
  0x437,0xA7,0xE7,0xDA, // з
  0x438,0xA8,0xE8,0xC9, // и
  0x439,0xA9,0xE9,0xCA, // й
  0x43A,0xAA,0xEA,0xCB, // к
  0x43B,0xAB,0xEB,0xCC, // л
  0x43C,0xAC,0xEC,0xCD, // м
  0x43D,0xAD,0xED,0xCE, // н
  0x43E,0xAE,0xEE,0xCF, // о
  0x43F,0xAF,0xEF,0xD0, // п
  0x440,0xE0,0xF0,0xD2, // р
  0x441,0xE1,0xF1,0xD3, // с
  0x442,0xE2,0xF2,0xD4, // т
  0x443,0xE3,0xF3,0xD5, // у
  0x444,0xE4,0xF4,0xC6, // ф
  0x445,0xE5,0xF5,0xC8, // х
  0x446,0xE6,0xF6,0xC3, // ц
  0x447,0xE7,0xF7,0xDE, // ч
  0x448,0xE8,0xF8,0xDB, // ш
  0x449,0xE9,0xF9,0xDD, // щ
  0x44A,0xEA,0xFA,0xDF, // ъ
  0x44B,0xEB,0xFB,0xD9, // ы
  0x44C,0xEC,0xFC,0xD8, // ь
  0x44D,0xED,0xFD,0xDC, // э
  0x44E,0xEE,0xFE,0xC0, // ю
  0x44F,0xEF,0xFF,0xD1, // я
  // SMALL Cyrillic letters (additional)
  0x452,'_',0x90,'_', // _ .*.*
  0x453,'_',0x83,'_', // _ .*.*
  0x459,'_',0x9A,'_', // _ .*.*
  0x45A,'_',0x9C,'_', // _ .*.*
  0x45C,'_',0x9D,'_', // _ .*.*
  0x45B,'_',0x9E,'_', // _ .*.*
  0x45F,'_',0x9F,'_', // _ .*.*
  0x45E,0xF7,0xA2,'_', // ў ...*
  0x458,0x6A,0xBC,0x6A, // _ .*.*
  0x491,0xA3,0xB4,0xAD, // _ .*..
  0x451,0xF1,0xB8,0xA3, // ё
  0x454,0xF3,0xBA,0xA4, // є
  0x457,0xF5,0xBF,0xA7, // ї
  0x456,0x69,0xB3,0xA6, // _ .*..
  0x455,0x73,0xBE,0x73, // _ .*.*
  0x0A0,'_',0xA0,0x20, // space .*..
  0x0A4,'_',0xA4,0xFD, // ¤   .*..
  0x0A6,'_',0xA6,'_', // ¦   .*.*
  0x0B0,0xF8,0xB0,0x9C, // °
  0x0B7,0xFA,0xB7,0x9E, // ·
  // 0x2022,,0x95,0x95, //    .*..
  // 0x2116,0xFC,0xB9,0x23, // №   ...*
  // 0x2219,,0xF9,0x9E, //    .*..
  // 0x221A,0xFB,,0x96, // v   ..*.
  // 0x25A0,0xFE,,0x94, // ¦
  0x0000,0,0,0
};

unsigned int char16to8(unsigned int c)
{
  const TUNICODE2CHAR *p=unicode2char;
  unsigned int i;
  if (c<32) return(' ');
  if (c<128) return(c);
  while((i=p->u))
  {
    if (c==i)
    {
      return(p->win);
    }
    p++;
  }
  c&=0xFF;
  if (c<32) return(' ');
  return(c);
}

void UpdateCSMname(const char *fname)
{
  char *s;
//  int i;
  WSHDR *ws=AllocWS(256);
  if ((s=strrchr(fname,'\\'))==0)
  {
    if ((s=strrchr(fname,'/'))==0)
    {
      if ((s=strrchr(fname,':'))==0) s=(char *)fname-1;
    }
  }
  s++;
  str_2ws(ws,s,128);
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"Config: %w",ws);
  FreeWS(ws);
}

void ErrorMsg(const char *msg)
{
  LockSched();
  ShowMSG(1,(int)msg);
  UnlockSched();
}


int main(const char *elf_name, const char *fname)
{
  char dummy[sizeof(MAIN_CSM)];
  int f;
  unsigned int ul;
  strncpy(cfg_name,fname,255);
  if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
  {
    size_cfg=lseek(f,0,S_END,&ul,&ul);
    lseek(f,0,S_SET,&ul,&ul);
    if (size_cfg<=0)
    {
      ErrorMsg("Zero lenght of .bcfg file!");
      fclose(f,&ul);
      return 0;
    }
    cfg=malloc((size_cfg+3)&(~3));
    if (fread(f,cfg,size_cfg,&ul)!=size_cfg)
    {
      ErrorMsg("Can't read .bcfg file!");
      fclose(f,&ul);
      mfree(cfg);
      return 0;
    }
    fclose(f,&ul);
  }
  else
  {
    ErrorMsg("Can't open .bcfg file!");
    return 0;
  }
  UpdateCSMname(fname);
  LockSched();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;
}

int create_ed(void)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  int scr_w,head_h;

  char *p=cfg;
  int n=size_cfg;
  CFG_HDR *hp;

  int i;
  unsigned int curlev=0;
  CFG_HDR *parent=NULL;
  CFG_HDR *parents[16];

  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());

  //Имя конфигурации
  ConstructEditControl(&ec,1,0x40,(WSHDR *)(&MAINCSM.maincsm_name),256);
  AddEditControlToEditQend(eq,&ec,ma); //EditControl 1
  parents[0]=NULL;

  total_items=0;
  while(n>=sizeof(CFG_HDR))
  {
    hp=(CFG_HDR*)p;
    cfg_h[total_items]=hp;
    //Добавляем заголовок итема
    wsprintf(ews,"%t:",hp->name);
    if (hp->type==CFG_LEVEL)
    {
      if (hp->min)
      {
        if ((curlev==level)&&(parent==levelstack[level]))
	{
	  ConstructEditControl(&ec,1,0x00,ews,256);
	  AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+2
	}
      }
      else
	if (curlev)
	{
	  parent=parents[curlev];
	  curlev--;
	}
    }
    else
    {
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,1,0x40,ews,256);
	AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+2
      }
    }
    n-=sizeof(CFG_HDR);
    p+=sizeof(CFG_HDR);
    switch(hp->type)
    {
    case CFG_UINT:
      n-=4;
      if (n<0)
      {
      L_ERRCONSTR:
        wsprintf(ews,"Unexpected EOF!!!");
      L_ERRCONSTR1:
        ConstructEditControl(&ec,1,0x40,ews,256);
        AddEditControlToEditQend(eq,&ec,ma);
        goto L_ENDCONSTR;
      }
      wsprintf(ews,_percent_u,*((unsigned int *)p));
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,6,0x40,ews,10);
	AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+3
      }
      p+=4;
      break;
    case CFG_INT:
      n-=4;
      if (n<0) goto L_ERRCONSTR;
      wsprintf(ews,_percent_d,*((int *)p));
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,6,0x40,ews,10);
	AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+3
      }
      p+=4;
      break;
    case CFG_STR_UTF8:
      n-=(hp->max+1+3)&(~3);
      if (n<0) goto L_ERRCONSTR;
      str_2ws(ews,p,hp->max);
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,3,0x40,ews,hp->max);
	AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+3
      }
      p+=(hp->max+1+3)&(~3);
      break;
    case CFG_STR_WIN1251:
      n-=(hp->max+1+3)&(~3);
      if (n<0) goto L_ERRCONSTR;
      wsprintf(ews,_percent_t,p);
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,3,0x40,ews,hp->max);
	AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+3
      }
      p+=(hp->max+1+3)&(~3);
      break;
    case CFG_CBOX:
      n-=hp->max*sizeof(CFG_CBOX_ITEM)+4;
      if (n<0) goto L_ERRCONSTR;
      i=*((int *)p);
      if (i>=hp->max)
      {
        wsprintf(ews,"Bad index in combobox!!!");
        goto L_ERRCONSTR1;
      }
      wsprintf(ews,_percent_t,((CFG_CBOX_ITEM*)(p+4))+i);
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructComboBox(&ec,7,0x40,ews,32,0,hp->max,i+1);
	AddEditControlToEditQend(eq,&ec,ma);
      }
      p+=hp->max*sizeof(CFG_CBOX_ITEM)+4;
      break;
    case CFG_STR_PASS:
      n-=(hp->max+1+3)&(~3);
      if (n<0) goto L_ERRCONSTR;
      wsprintf(ews,_percent_t,p);
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,3,0x50,ews,hp->max);
	AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+3
      }
      p+=(hp->max+1+3)&(~3);
      break;
    case CFG_COORDINATES:
      n-=8;
      if (n<0) goto L_ERRCONSTR;
      wsprintf(ews,"%d,%d",*((int *)p),*((int *)p+1));
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,9,0x40,ews,10);
	AddEditControlToEditQend(eq,&ec,ma); 
      }
      p+=8;
      break;
    case CFG_COLOR:
      n-=4;
      if (n<0) goto L_ERRCONSTR;
      wsprintf(ews,"%02X,%02X,%02X,%02X",*((char *)p),*((char *)p+1),*((char *)p+2),*((char *)p+3));
      if ((curlev==level)&&(parent==levelstack[level]))
      {
	ConstructEditControl(&ec,9,0x40,ews,12);
	AddEditControlToEditQend(eq,&ec,ma);           
      }
      p+=4;
      break;
    case CFG_LEVEL:
      if (n<0) goto L_ERRCONSTR;
      wsprintf(ews,_percent_t,"Enter");
      if (hp->min)
      {
	if ((curlev==level)&&(parent==levelstack[level]))
	{
	  EDITC_OPTIONS ec_options;
	  ConstructEditControl(&ec,8,0x40,ews,256);
	  SetPenColorToEditCOptions(&ec_options,2);
	  SetFontToEditCOptions(&ec_options,1);
	  CopyOptionsToEditControl(&ec,&ec_options);
	  AddEditControlToEditQend(eq,&ec,ma); //EditControl n*2+3
	  total_items++;
	}
	curlev++;
	parents[curlev]=parent;
	parent=hp;
      }
      continue;
    default:
      wsprintf(ews,"Unsupported item %d",hp->type);
      ConstructEditControl(&ec,1,0x40,ews,256);
      AddEditControlToEditQend(eq,&ec,ma);
      goto L_ENDCONSTR;
    }
    if ((curlev==level)&&(parent==levelstack[level])) total_items++;
  }
L_ENDCONSTR:
  scr_w=ScreenW();
  head_h=HeaderH();
  patch_header(&ed1_hdr,0,0,scr_w-1,head_h);
  patch_input(&ed1_desc,0,head_h+1,scr_w-1,ScreenH()-SoftkeyH()-1);
  return CreateInputTextDialog(&ed1_desc,&ed1_hdr,eq,1,0);
}
