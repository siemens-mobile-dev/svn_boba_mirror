#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "ctype.h"
#include "base64.h"
#include "sf_mime.h"

extern const unsigned int ICON0;
extern const unsigned int ICON1;
extern const unsigned int ICON2;
extern const unsigned int ICON3;
extern const char HIST_PATH[];

int S_ICONS[5];

int displace_config;
typedef struct
{
  int type;
  char uid[44];
}MAIL_HIST;

#define FULL_MES 0   // normal mes
#define UNFULL_MES 1   // unfull mes
#define MES_DOWN 2   // mes for download
#define MES_DEL 3   // mes for delete


// ------------------------------- Defines ----------------------------------------
int ecq(const char *a, const char *b)
{
  register const unsigned char
    *us1 = (const unsigned char *)a,
    *us2 = (const unsigned char *)b;
  
  /* Can't test equality of NOTHING */
  if(us1 == NULL || us2 == NULL)
    return 0;
  while (tolower(*us1) == tolower(*us2++))
    if (*us1++ == '\0')
      return 1;
  return 0;
}

int ecnq(const char *a, const char *b,int n)
{
  register const unsigned char
    *us1 = (const unsigned char *)a,
    *us2 = (const unsigned char *)b;
  
  /* Can't test equality of NOTHING */
  if(n == NULL)
    return 0;  
  
  if(us1 == NULL || us2 == NULL)
    return 0;
  while (tolower(*us1++) == tolower(*us2++))
    if (--n==0)
      return 1;
  return 0;
}

// --------------------------------------------------------------------------------
static char *_sf_b64_buf = NULL;
static size_t _sf_b64_len = 0;


char * base64_decode(const char *str, size_t *size)
{
  char *output, *ou;
  unsigned int pr[6];
  int n;
  int doit;
  
  if(str == NULL)
    str = "";
  ou = output = malloc((size?*size:strlen(str)) + 1);
  if(output == NULL)
    /* ENOMEM? */
    return NULL;
  
  if(size)
    *size = 0;
  
  doit = 1;
  
  do
  {
    n = 0;
    while(n < 4)
    {
      unsigned char ch;
      ch = _sf_uc_bi[*(unsigned char *)str];
      if(ch < B64_UNUSED)
      {
        pr[n] = ch;
        n++;
        str++;
      }
      else
        if(ch == B64_UNUSED)
        {
          str++;
	}
        else
        {
          doit = 0;
          break;
        }
    }
    
    if(doit == 0 && n < 4)
    {
      pr[n+2] = pr[n+1] = pr[n]='\0';
    }
    
    *(ou+0) = (pr[0] << 2) | (pr[1] >> 4);
    *(ou+1) = (pr[1] << 4) | (pr[2] >> 2);
    *(ou+2) = (pr[2] << 6) | (pr[3] >> 0);
    
    ou = ou+((n * 3) >> 2);
  }
  while(doit);
  
  *ou = '\0';	/* NUL-terminate */
  
  _sf_b64_len = (ou - output);
  if(size)
    *size = _sf_b64_len;
  
  /*
   * Replace the old buffer.
  */
  //if(_sf_b64_buf)
    //mfree(_sf_b64_buf);
  return (_sf_b64_buf=output);
}



char * quoted_printable_decode(const char *str, size_t *size)
{
  char *buf;
  char *s;
  register int n;

  int first;
  int second;
  if(str == NULL) return NULL;
  
  /*
  * Allocate sufficient space to hold decoded string.
  */
  
  
  s=buf=malloc((size?*size:strlen(str)) + 1);
  if(buf == NULL)
    
    /* ENOMEM? */
    return NULL;

  for(; *str; str++)
  {
    if(*str == '_')
    {
      *s++ = ' ';
      continue;
    }
    if(*str == '=')
    {
      *s = '\0';
      n = *++str; 
      if(n == '\0')
      {
        str--;
        break;
      }
      if(str[1] == '\0')
        break;
      
      if (n>='0'&&n<='9')
        first = n - '0';
      
      if (n>='A'&&n<='F')
        first =n - 'A' + 10;
      
      if (n>='a'&&n<='f')
        first = n- 'a' + 10;
      
      if (n=='\r'||n=='\n')
      {
        *s++ = '=';
        *s++ = n;
        continue;
      }
      
      n = *++str;
      
      if (n>='0'&&n<='9')
        second = n - '0';
      
      
      if (n>='A'&&n<='F')
        second =n - 'A' + 10;
      
      if (n>='a'&&n<='f')
        second = n- 'a' + 10;
      
      if (n=='\r'||n=='\n')
      {
        *s++ = n;
        *s = ' ';
        continue;
      }
      *s=(first<<4)+second;
      s++;
      continue;
    }
    *s++ = *str;
  }
  
  *s = '\0';

//  if(_sf_qp_buf)
//    mfree(_sf_qp_buf);
  if(size) *size = (s - buf);
  return (buf);
}

// --------------------------------------------------------------------------------
const char mailer_db_name[]="mails.db";
const int minus11=-11;
unsigned short maincsm_name_body[140];
extern void kill_data(void *p, void (*func_p)(void *));
char* read_db_file(void);
void remake_mailmenu();
int cur_menu;
// --------------------------------------------------------------------------------
int main_menu_id;
int maillist_menu_id;
int options_menu_id;
volatile int request_remake_mailmenu;
volatile int request_close_mailmenu;
// -------------------------- Global Variables -------------------------------------
typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;



void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}
//-----------------------------------------------------------------------------------

int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Options"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

void get_eml_name(char* dest,int i)
{
  MAIL_HIST* fbuf;
  fbuf=(MAIL_HIST*)read_db_file();
  sprintf(dest,"%s%s.eml",HIST_PATH,fbuf[i].uid);
  mfree(fbuf);
}

// ----------------------------------------------------------------------------------

char*decodestr(char* str)
{
  char * buf=malloc(1024);
  zeromem(buf,1024);
  int i=0;
  int n=0;
  char* first;
  char* decode_start;
  char* decode_end;
  int cod; // 1 - win 1251 2 - koi8
  int type; // 1- base 64 2 - quote printable
  do
  {
    if (str[i]=='=')
    {
      if (str[i+1]=='?')
      {
        i+=2;
        first=str+i;
        if (ecnq(first,"windows-1251",sizeof("windows-1251")-1))
        {
          cod=1;
          i+=sizeof("windows-1251")-1;
          goto L_NEXT_CHECK;
        }
        
        if (ecnq(first,"koi8-r",sizeof("koi8-r")-1))
        {
          cod=2;
          i+=sizeof("koi8-r")-1;
        }
        else goto L_NEXT;
       
      L_NEXT_CHECK:
        
        if (str[i]=='?')
        {
          if (tolower(str[i+1])=='b')
          {
            type=1;
            goto L_FIND_END;
          }
          if (tolower(str[i+1])=='q')
          {
            type=2;
          }
          else goto L_NEXT;
        L_FIND_END:
          i+=3;
          decode_start=str+i;
          
          decode_end=strstr(decode_start,"?=");
          int decode_len=decode_end-decode_start;
          char*decodstr=malloc(decode_len+1);
          char* decoded_str;
          
          decodstr[decode_len]=0;
          strncpy(decodstr,decode_start,decode_len);
          switch(type)
          {
          case 1:
            decoded_str=base64_decode(decodstr,0);
            break;
            
          case 2:
            decoded_str=quoted_printable_decode(decodstr,0);
            break;
          }
          strcat(buf,decoded_str);
          i=decode_end-str+2;
          mfree(decoded_str);
          mfree(decodstr);
        }
        else goto L_NEXT;
        
      }
      else goto L_NEXT;
    }
    else
    {
    L_NEXT:
      buf[n++]=str[i++];
    }
  }
  while(str[i]);
  return buf;
}

int get_file_size(const char * fname)
{
  FSTATS fs;
  unsigned int err;
  if (GetFileStats(fname,&fs,&err)==-1) return (-1);
  else return (fs.size);
}

char* find_subject(int i)
{
  int f;
  unsigned int err;
  unsigned int f_size;
  char* eml_buf;
  char* first_in_subj;
  char* sec_in_subj;
  char* decoded_str;
  int subj_len;
  char eml_fname[128];
  char* subj;
  get_eml_name(eml_fname,i);
  if ((f_size=get_file_size(eml_fname))!=0xFFFFFFFF)
  {
    if ((f=fopen(eml_fname,A_ReadOnly,P_READ,&err))!=-1)
    {
      eml_buf=malloc(f_size+1);
      fread(f,eml_buf,f_size,&err);
      fclose(f,&err);
      eml_buf[f_size]=0;
      first_in_subj=strstr(eml_buf,"Subject: ");
      sec_in_subj=strstr(first_in_subj,"\r\n");
      subj_len=sec_in_subj-first_in_subj;
      subj=malloc(subj_len+1);
      subj[subj_len]=0;
      strncpy(subj,first_in_subj,subj_len);
      decoded_str=decodestr(subj);
      
      mfree(subj);
      mfree(eml_buf);
      return (decoded_str);
    }
  }
  return 0;
}

char* read_db_file(void)
{
  char db_fname[128];
  snprintf(db_fname,sizeof(db_fname),"%s%s",HIST_PATH,mailer_db_name);
  int f;
  unsigned int err;
  char* buf;
  int fsize;
  
  if ((f=(fopen(db_fname,A_ReadOnly+A_BIN,P_READ,&err)))==-1) return 0;
  {
    
    if ((fsize=get_file_size(db_fname))==-1) return 0;
    {
      buf=malloc(fsize+1);
      buf[fsize]=0;
      fread(f,buf,fsize,&err);
      fclose(f,&err);
      return buf;
    }
  }
}

void write_db_file(MAIL_HIST*buf,int fsize)
{
  char db_fname[128];
  snprintf(db_fname,sizeof(db_fname),"%s%s",HIST_PATH,mailer_db_name);
  int f;
  unsigned int err;
  if ((f=(fopen(db_fname,A_ReadWrite+A_Truncate+A_BIN,P_READ+P_WRITE,&err)))==-1) return;
  {
    fwrite(f,buf,fsize,&err);
    fclose(f,&err);
    return;
  }
}

int GetIconIndex(i)
{
  MAIL_HIST* fbuf;
  fbuf=(MAIL_HIST*)read_db_file();
  int l=fbuf[i].type;
  mfree(fbuf);
  
  switch (l)
  {
  case 0:
    return 0;
  case 1:
    return 1;
  case 2:
    return 2;
  case 3:
    return 3;
  }
  return 0;
}

// ---------------------------------------------------------------------------------

void setstate(int state)
{
  char db_fname[128];
  MAIL_HIST* fbuf;
  int fsize;
  snprintf(db_fname,sizeof(db_fname),"%s%s",HIST_PATH,mailer_db_name);
  if ((fsize=get_file_size(db_fname))==-1) return;
  fbuf=(MAIL_HIST*)read_db_file();
  fbuf[cur_menu].type=state;
  write_db_file(fbuf,fsize);
  mfree(fbuf);   
}


int options_menu_onkey(void)
{
  return 0;
}

void options_menu_ghook(void)
{
}

void set_state_download()
{
  setstate(MES_DOWN);
  GeneralFuncF1(1);
}

void set_state_delete()
{
  setstate(MES_DEL);
  GeneralFuncF1(1);
}

void delete_record()
{
  char db_fname[128];
  MAIL_HIST* fbuf;
  int fsize;
  int num_rec;
  MAIL_HIST* endbuf;
  
  
  snprintf(db_fname,sizeof(db_fname),"%s%s",HIST_PATH,mailer_db_name);
  if ((fsize=get_file_size(db_fname))==-1) return;
  num_rec=fsize/sizeof(MAIL_HIST);
  
  fbuf=(MAIL_HIST*)read_db_file();
  endbuf=fbuf+num_rec;
  memcpy(&fbuf[cur_menu],&fbuf[cur_menu+1],(endbuf-&fbuf[cur_menu+1])*sizeof(MAIL_HIST));
  write_db_file(fbuf,fsize-sizeof(MAIL_HIST));
  mfree(fbuf);
  GeneralFuncF1(1);
  remake_mailmenu();
}

void back()
{
  GeneralFuncF1(1);
}


HEADER_DESC options_menuhdr={0,0,0,0,NULL,(int)"Опции",0x7FFFFFFF};

MENUITEM_DESC options_menu_ITEMS[4]=
{
  {NULL,(int)"Download",0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)"Delete",0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)"Delete rec",0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)"Back",0x7FFFFFFF,0,NULL,0,0x59D},
};

void *options_menu_HNDLS[4]=
{
  (void *)set_state_download,
  (void *)set_state_delete,
  (void *)delete_record,
  (void *)back,  
};


MENU_DESC options_menu_STRUCT=
{
  8,(void *)options_menu_onkey,(void*)options_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  NULL,
  options_menu_ITEMS,   //Items
  options_menu_HNDLS,   //Procs
  4  //n
};



void create_options_menu(int i)
{
  cur_menu=i;
  options_menuhdr.rc.x=3;
  options_menuhdr.rc.y=0x18;
  options_menuhdr.rc.x2=ScreenW()-6;
  options_menuhdr.rc.y2=0x18+HeaderH();
  
  options_menu_id=CreateMenu(1,0,&options_menu_STRUCT,&options_menuhdr,0,4,0,0);
}

// ----------------------------------------------------------------------------------

HEADER_DESC maillist_menuhdr={0,0,131,21,NULL,(int)"Входящие",0x7FFFFFFF};


int maillist_menu_onkey(void *data, GUI_MSG *msg)
{
  char eml_name[128];
  WSHDR* ws;
  int i;
  int keycode;
  keycode=msg->keys;
  i=GetCurMenuItem(data);
  switch(keycode)
  {
  case 0x18:
    //    GeneralFunc_F1(1);
    create_options_menu(i);
    return(-1);
    
  case 0x3D:
    get_eml_name(eml_name,i);
    ws=AllocWS(128);
    str_2ws(ws,eml_name,128);
    ExecuteFile(ws,0,0);
    FreeWS(ws);
    return(-1);
  } 
  return(0);
}

void maillist_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
    if (request_close_mailmenu)
    {
      request_close_mailmenu=0;
      GeneralFunc_flag1(maillist_menu_id,1);
    }
  }
}

void maillist_menu_iconhndl(void *data, int curitem, int *unk)
{
  MAIL_HIST* buf;
  buf=(MAIL_HIST*)read_db_file();
  char* subj;
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  ws=AllocMenuWS(data,50);
  
  if ((subj=find_subject(curitem)))
  {
    ws=AllocMenuWS(data,strlen(subj));
    wsprintf(ws,"%t",subj+sizeof("Subject: ")-1);
    mfree(subj);
  }
  else 
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,"%t","Ошибка");
  }
  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(curitem));
  mfree(buf);
  

}


MENU_DESC maillist_menu=
{
  8,(void *)maillist_menu_onkey,(void*)maillist_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  (void *)maillist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};




//------------------------------------------------------------------------------------

void main_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

int main_menu_onkey(void *data, GUI_MSG *msg)
{
  if (msg->keys==0x18)
  {
    return(-1);
  }

  return(0);
}


void CreateMailList(void)
{
  char dbfname[128];
  int fsize;
  int mails_num;
  snprintf(dbfname,sizeof(dbfname),"%s%s",HIST_PATH,mailer_db_name);
  if ((fsize=get_file_size(dbfname))!=-1)
  {
    mails_num=fsize/sizeof(MAIL_HIST);
    maillist_menu_id=CreateMenu(0,0,&maillist_menu,&maillist_menuhdr,0,mails_num,0,0);
  }  
}

void remake_mailmenu(void)
{
  if (maillist_menu_id)
  {
    request_remake_mailmenu=1;
    if (IsGuiOnTop(maillist_menu_id))
    {
      GeneralFunc_flag1(maillist_menu_id,1);
    }
    else
    {
      request_close_mailmenu=1;
    }
  }
  else
  {
    CreateMailList();
  }
}


void Incoming(void)
{
  CreateMailList();
}

void Outgoing(void)
{
}


void Options(void)
{
  WSHDR* ws;
  ws=AllocWS(128);
  wsprintf(ws,"%u:\\ZBin\\etc\\EMailClient.bcfg",displace_config);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
}

void Exit(void)
{
  GeneralFuncF1(1);
}

HEADER_DESC mainmenu_HDR={0,0,0,0,NULL,(int)"E-Mail Client...",0x7FFFFFFF};


MENUITEM_DESC mainmenu_ITEMS[4]=
{
  {NULL,(int)"Входящие",0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)"Исходящие",0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)"Настройки",0x7FFFFFFF,0,NULL,0,0x59D},
  {NULL,(int)"Выход",0x7FFFFFFF,0,NULL,0,0x59D},
};

void *mainmenu_HNDLS[6]=
{
  (void *)Incoming,
  (void *)Outgoing,
  (void *)Options,
  (void *)Exit,
};


MENU_DESC mainmenu_STRUCT=
{
  8,(void *)main_menu_onkey,(void*)main_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  NULL,
  mainmenu_ITEMS,   //Items
  mainmenu_HNDLS,   //Procs
  4   //n
};

// ------------------------  Creating CSM -------------------------------- //
void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  mainmenu_HDR.rc.x2=ScreenW();
  mainmenu_HDR.rc.y2=HeaderH();
  csm->gui_id=main_menu_id=CreateMenu(0,0,&mainmenu_STRUCT,&mainmenu_HDR,0,4,0,0);
}

void maincsm_onclose(CSM_RAM *csm)
{
  SUBPROC((void *)ElfKiller);
}


int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
    if ((int)msg->data0==maillist_menu_id)
    {
      maillist_menu_id=0;
      if (request_remake_mailmenu)
      {
        request_remake_mailmenu=0;
        CreateMailList();
      }
    }
  }
  return(1);
}



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


void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"*.eml File Viewer");
}


int main()
{
  InitConfig();
  
  S_ICONS[0]=ICON0;
  S_ICONS[1]=ICON1;
  S_ICONS[2]=ICON2;
  S_ICONS[3]=ICON3;
  
  LockSched();
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
}
