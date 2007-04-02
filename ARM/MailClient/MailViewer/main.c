#include "..\..\inc\swilib.h"
#include "..\conf_loader.h"
#include "..\mailclient.h"

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

#define wslen(ws) ws->wsbody[0]
//===============================================================================================
int S_ICONS[8];
char empty_str[]="";

// ------------------------------- Defines ----------------------------------------

const char wintranslation[128]=
{
  0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
  0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
  0xFF,0xF6,0xF7,0x5F,0xFD,0x83,0xB3,0x15,0xF0,0x63,0xF2,0x3C,0xBF,0x2D,0x52,0xF4,
  0xF8,0x2B,'I' ,'i' ,0xA3,0xE7,0x14,0xFA,0xF1,0xFC,0xF3,0x3E,0x5F,0x5F,0x5F,0xF5,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
};

const char koi8translation[128]=
{
  0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
  0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
  0xFF,0xF6,0xF7,0xF1,0xF3,0x5F,'i' ,0xF5,0xF0,0x63,0xF2,0x3C,0xBF,0xA3,0x52,0xF4,
  0xF8,0x2B,0x5F,0xF0,0xF2,0xE7,'I' ,0xF4,0xF1,0xFC,0xF3,0x3E,0x5F,0x83,0x5F,0xF5,

  0xEE,0xA0,0xA1,0xE6,0xA4,0xA5,0xE4,0xA3,0xE5,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,
  0xAF,0xEF,0xE0,0xE1,0xE2,0xE3,0xA6,0xA2,0xEC,0xEB,0xA7,0xE8,0xED,0xE9,0xE7,0xEA,
  0x9E,0x80,0x81,0x96,0x84,0x85,0x94,0x83,0x95,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,
  0x8F,0x9F,0x90,0x91,0x92,0x93,0x86,0x82,0x9C,0x9B,0x87,0x98,0x9D,0x99,0x97,0x9A
};

const unsigned short dos2unicode[128]=
{
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
  0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
  0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
  0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
  0x002D,0x002D,0x002D,0x00A6,0x002B,0x00A6,0x00A6,0x00AC,
  0x00AC,0x00A6,0x00A6,0x00AC,0x002D,0x002D,0x002D,0x00AC,
  0x004C,0x002B,0x0054,0x002B,0x002D,0x002B,0x00A6,0x00A6,
  0x004C,0x0433,0x00A6,0x0054,0x00A6,0x003D,0x002B,0x00A6,
  0x00A6,0x0054,0x0054,0x004C,0x004C,0x002D,0x0433,0x002B,
  0x002B,0x002D,0x002D,0x002D,0x002D,0x00A6,0x00A6,0x002D,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
  0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
  0x0401,0x0451,0x0404,0x0454,0x0407,0x0457,0x040E,0x045E,
  0x00B0,0x2022,0x00B7,0x0076,0x2116,0x00A4,0x00A6,0x00A0
};



unsigned int char8to16(int c, int type)
{
  if (c>=128)
  {
    switch(type)
    {
    case 1:
      //Win->Dos
      c=wintranslation[c-128];
      break;
    case 2:
      //Koi8->Dos
      c=koi8translation[c-128];
      break;
    case 3:
      break;
      //Dos
    }
    if (c<128) return(c);
    return(dos2unicode[c-128]);
  }
  return(c);
}


void ascii2ws(WSHDR *ws, const char *s)
{
  char c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c,1));
  }
}

void koi2ws(WSHDR *ws, const char *s)
{
  char c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c,2));
  }
}

void dos2ws(WSHDR *ws, const char *s)
{
  int c;
  CutWSTR(ws,0);
  while((c=*s++))
  {
    wsAppendChar(ws,char8to16(c,3));
  }
}  

// --------------------------------------------------------------------------------
extern char *unmime_header(const char *encoded_str);
// --------------------------------------------------------------------------------
const char mailer_db_name[]="mails.db";
const char _percent_t[]="%t";
const int minus11=-11;
unsigned short maincsm_name_body[140];



int displace_config;
// --------------------------------------------------------------------------------
volatile int main_menu_id;
volatile int maillist_menu_id;
volatile int options_menu_id;
volatile int view_mail_id;
volatile int request_recount_mailmenu;
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

// ----------------------------------------------------------------------------------
#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}

int strcmp_nocase(const char *s,const char *d)
{
  int cs;
  int ds;
  do
  {
    cs=*s++;
    if (cs&0x40) cs&=0xDF;
    ds=*d++;
    if (ds&0x40) ds&=0xDF;
    cs-=ds;
    if (cs) break;
  }
  while(ds);
  return(cs);
}

int strncmp_nocase(const char *s1,const char *s2,unsigned int n)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))&&(--n)) if (!c) break;
  return(i);
}

volatile ML_VIEW *mails;

void FreeMailDB()
{
  ML_VIEW *ml_cur=(ML_VIEW *)mails;
  LockSched();
  mails=0;
  UnlockSched();
  ML_VIEW *ml_prev;
  while (ml_cur)
  {
    mfree(ml_cur->uidl);
    mfree(ml_cur->header);
    mfree(ml_cur->subject);
    mfree(ml_cur->from);
    mfree(ml_cur->to);
    mfree(ml_cur->content_type);
    mfree(ml_cur->content_encoding);
    ml_prev=ml_cur;
    ml_cur=ml_cur->next;
    mfree(ml_prev);
  }
}

void InitHeaders();
int InitMailDB()
{
  char fname[256];
  int f;
  MAIL_DB mail_db;
  char *uidl;
  int n=0;
  ML_VIEW *ml_cur=(ML_VIEW *)&mails;
  unsigned int err;
  strcpy(fname,EML_PATH);
  if (fname[strlen(fname)-1]!='\\')
  {
    strcat(fname,"\\");
  }
  strcat(fname,"mails.db");
  FreeMailDB();
  if ((f=fopen(fname,A_BIN+A_ReadOnly,P_READ,&err))==-1)
  {
    mails=0;
    return(n);
  }
  while (fread(f,&mail_db,sizeof(MAIL_DB),&err)==sizeof(MAIL_DB))
  {
    if(mail_db.magic!=MDB_MAGIC) break;
    ml_cur=ml_cur->next=malloc(sizeof(ML_VIEW));
    ml_cur->next=0;
    ml_cur->state=mail_db.state;
    ml_cur->is_read=mail_db.is_read;
    ml_cur->mail_size=mail_db.mail_size;
    if (mail_db.uidl_len)
    {
      uidl=malloc(mail_db.uidl_len+1);
      uidl[mail_db.uidl_len]=0;
      fread(f,uidl,mail_db.uidl_len,&err);
      ml_cur->uidl=uidl;
    }
    else ml_cur->uidl=0;
    ml_cur->subject=0;
    ml_cur->header=0;
    ml_cur->from=0;
    ml_cur->to=0;
    ml_cur->content_type=0;
    ml_cur->content_encoding=0;
    ml_cur->content_type=0;
    n++;
  }
  fclose(f,&err);
  InitHeaders();
  return (n);
}

void write_mail_DB()
{
  int f;
  unsigned int err;
  char fname[128];
  ML_VIEW *ml_list=(ML_VIEW *)&mails;
  MAIL_DB mail_db;
  strcpy(fname,EML_PATH);
  strcat(fname,"mails.db");
  if ((f=fopen(fname,A_WriteOnly+A_BIN+A_Create+A_Truncate,P_WRITE,&err))==-1)
    return;
  while((ml_list=ml_list->next))
  {
    mail_db.magic=MDB_MAGIC;
    mail_db.uidl_len=(ml_list->uidl)?strlen(ml_list->uidl):NULL;
    mail_db.state=ml_list->state;
    mail_db.is_read=ml_list->is_read;
    mail_db.mail_size=ml_list->mail_size;
    fwrite(f,&mail_db,sizeof(MAIL_DB),&err);
    if (mail_db.uidl_len) fwrite(f,ml_list->uidl,mail_db.uidl_len,&err);
  }
  fclose(f,&err);
}

ML_VIEW *find_mlist_ByID(int id)
{
  ML_VIEW *ml_cur=(ML_VIEW *)(&mails);
  int i=0;
  while ((ml_cur=ml_cur->next))
  {
    if (i++==id) return (ml_cur);
  }
  return (0);
}

#pragma inline
int get_mlist_N()
{
  ML_VIEW *ml_cur=(ML_VIEW *)(&mails);
  int i=0;
  while ((ml_cur=ml_cur->next))
    i++;
  return i;
}

char *find_eol(char *s)
{
  int c;
  s--;
  do
  {
    s++;
    c=*s;
  }
  while((c)&&(c!=10)&&(c!=13));
  return s;
}

int get_fsize(int f)
{
  int cur_pos;
  int fsize;
  unsigned int err;
  cur_pos=lseek(f,0,S_CUR,&err,&err);
  fsize=lseek(f,0,S_END,&err,&err);
  lseek(f,cur_pos,S_SET,&err,&err);
  return (fsize);
}

const char eml_not_found[]="Error! File not found!";

void InitHeaders()
{
  int f;
  unsigned int err;
  char fname[128];
  char *buf, *dec_str;
  int fsize;
  char *_eol;
  ML_VIEW *ml_cur=(ML_VIEW *)&mails;
  while((ml_cur=ml_cur->next))
  {
    if (ml_cur->header)
    {
      mfree(ml_cur->header);
      ml_cur->header=0;
    }
    snprintf(fname,127,"%s%s.eml",EML_PATH,ml_cur->uidl);
    if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&err))==-1)
    {
      continue;
    }
    fsize=get_fsize(f);
    buf=malloc(fsize+1);
    fread(f,buf,fsize,&err);
    fclose(f,&err);
    buf[fsize]=0;
    _eol=strstr(buf,"\r\n\r\n");
    if (_eol)
    {
      *_eol=0;
      buf=realloc(buf,strlen(buf)+1);
    }
    dec_str=unmime_header(buf);
    mfree(buf);
    ml_cur->header=dec_str;
  }
}
    
char *get_string_from_header(ML_VIEW *ml_list, char *str)
{
  char *p;
  char *end;
  char *subject;
  int len;
  if (!ml_list) return (0);
  if (ml_list->header)
  {
    p=ml_list->header;
    p-=2;
    do
    {
      p+=2;
      end=strstr(p,"\r\n");
      len=strlen(str);
      if (!strncmp_nocase(p, str,len))
      {
        if (end)
        {
          subject=malloc((end-p)+1);
          strncpy(subject,p+len,end-(p+len));
          subject[end-(p+len)]=0;
        }
        else 
        {
          subject=malloc(strlen(p)+1);
          strcpy(subject,p);
        }
        return (subject);
      }
      if (!end) return (0);
      p=end;
    }
    while(*p!=0);
  }
  return 0;
}

char *get_subject(ML_VIEW *ml_list)
{
  char *subject;
  if (ml_list->subject) return ml_list->subject;
  subject=get_string_from_header(ml_list, "subject:");
  return (ml_list->subject=subject);
}

char *get_from(ML_VIEW *ml_list)
{
  char *from;
  if (ml_list->from) return ml_list->from;
  from=get_string_from_header(ml_list, "from:");
  return (ml_list->from=from);
}

char *get_to(ML_VIEW *ml_list)
{
  char *to;
  if (ml_list->to) return ml_list->to;
  to=get_string_from_header(ml_list, "to:");
  return (ml_list->to=to);
}

char *get_content_type(ML_VIEW *ml_list)
{
  char *content_type;
  if (ml_list->content_type) return ml_list->content_type;
  content_type=get_string_from_header(ml_list, "content-type:");
  return (ml_list->content_type=content_type);
}

char *get_content_encoding(ML_VIEW *ml_list)
{
  char *content_encoding;
  if (ml_list->content_encoding) return ml_list->content_encoding;
  content_encoding=get_string_from_header(ml_list, "content-transfer-encoding:");
  return (ml_list->content_encoding=content_encoding);
}
//----------------------------------------------------------------------------------------------
HEADER_DESC ed1_hdr={0,0,0,0,NULL,(int)"Просмотр",LGP_NULL};

void ed1_locret(void){}

int ed1_onkey(GUI *data, GUI_MSG *msg)
{
  return(0); 
}
void ed1_ghook(GUI *data, int cmd)
{
}
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
  0,
  0
};

typedef struct{
  void *next;
  int content_type;
  int transfer_encoding;
  int charset;
  int offset;
  int size;
  char ct[128];
  char te[128];
}MAIL_PART;

enum {MULTIPART, APPLICATION, TEXT};
int get_ctype_index(char *str)
{
  if (*str==' ')
  {
    while (*str==' ') str++;
  }
  if (!strncmp_nocase(str,"multipart",9))
    return MULTIPART;
  
  if (!strncmp_nocase(str,"application",11))
    return APPLICATION; 
  
  if (!strncmp_nocase(str,"text",4))
    return TEXT;
  
  return TEXT;
}

enum {BIT8, BASE64, QPRINTABLE, BIT7};
int get_ctencoding_index(char *str)
{
  if (*str==' ')
  {
    while (*str==' ') str++;
  }
  if (!strncmp_nocase(str,"8bit",4))
    return BIT8;
  
  if (!strncmp_nocase(str,"base64",6))
    return BASE64; 
  
  if (!strncmp_nocase(str,"quoted-printable",16))
    return QPRINTABLE;
  
  if (!strncmp_nocase(str,"7bit",4))
    return BIT8;
  
  return BIT8;
}

enum {WIN_1251, KOI8_R, ISO_8859_5};
int get_charset(char *charset)
{
  if (!strncmp_nocase(charset,"windows-1251",12))
    return WIN_1251;
  
  if (!strncmp_nocase(charset,"\"windows-1251\"",14))
    return WIN_1251;
    
  if (!strncmp_nocase(charset,"koi8-r",6))
    return KOI8_R; 
  
  if (!strncmp_nocase(charset,"\"koi8-r\"",8))
    return KOI8_R; 
    
  if (!strncmp_nocase(charset,"ISO-8859-5",10))
    return ISO_8859_5;
  
  if (!strncmp_nocase(charset,"\"ISO-8859-5\"",12))
    return ISO_8859_5;
  
  return WIN_1251;
}
#pragma inline 
char *strchr_nocase(const char *s, int c) /* Copied from strchr.c */
{ 
  int ch = toupper(c);
  
  for (; toupper(*s) != ch; ++s)
  if (*s == '\0')
    return (0);
  return ((char *)s);
}

#pragma inline 
char *strstr_nocase(const char *s1, const char *s2) /* Copied from strstr.c */
{  
  if (*s2 == '\0')
    return ((char *)s1);
  
  for (; (s1 = strchr_nocase(s1, *s2)) != 0; ++s1)
  {    
    const char *sc1, *sc2;
    
    for (sc1 = s1, sc2 = s2; ; )
    if (*++sc2 == '\0')
      return ((char *)s1);
    else if (toupper(*++sc1) != toupper(*sc2))
      break;
  }
  return (0);
}

char *cstrcpy(char *s1, const char *s2)
{
  char *s = s1;
  int c=*s2++;
  while(c!=0 && c!='\r' && c!='\n')
  {
    *s++=c;
    c=*s2++;
  }
  *s=0;
  return (s1);
}
extern char * base64_decode(const char *str, size_t *size);
extern char * quoted_printable_decode(const char *str, size_t *size);
extern void koi2win(char*d,char *s);
extern void iso885952win(char*d,char *s);

const char default_ctype[]="Content-Type: text/plain; charset=\"windows-1251\"";
const char default_transfere[]="Content-Transfer-Encoding: 8bit";

void ParseMailBody(void *eq,ML_VIEW *ml_list, void *ma, WSHDR *headers)
{
  WSHDR *ws;
  MAIL_PART *top=0, *bot, *prev;
  EDITCONTROL ec;
  EDITC_OPTIONS ec_options;
  char *content_type, *content_encoding;
  int f;
  unsigned int err;
  char fname[128];
  char *eml, *end_header;
  int size, eml_size;
  
  
  top=malloc(sizeof(MAIL_PART));
  bot=top;
  top->next=0;
  top->offset=0;

  content_type=get_content_type(ml_list);
  if (!content_type) top->content_type=TEXT;
  else
  {
    top->content_type=get_ctype_index(content_type);
  } 
  
  content_encoding=get_content_encoding(ml_list);
  if (!content_encoding) top->transfer_encoding=BIT8;
  else
  {
    top->transfer_encoding=get_ctencoding_index(content_encoding);
  }   
  
  strcpy(top->ct,content_type?content_type:default_ctype);
  strcpy(top->te,content_encoding?content_encoding:default_transfere);
  snprintf(fname,127,"%s%s.eml",EML_PATH,ml_list->uidl);
  f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&err);
  if (f==-1)
  {
    mfree(top);
    return;
  }
  size=get_fsize(f);
  if (!size)
  {
    fclose(f, &err);
    mfree(top);
    return;
  }
  eml=malloc(size+1);
  fread(f, eml, size, &err);
  fclose(f, &err);
  eml[size]=0;
  end_header=strstr(eml, "\r\n\r\n");
  if (!end_header || (end_header+4)==(eml+size))
  {
    mfree(eml);
    mfree(top);
    return;
  }
  strcpy(eml,end_header+4);
  size=strlen(eml);
  top->size=size;
  eml=realloc(eml, size+1);
  while(bot)
  {
    char boundary[32];
    size_t buf_size;
    char *p, *buf, *b_end;
    int c, i;
    switch(bot->content_type)
    {
    case TEXT:
      p=bot->ct;
      p=strstr_nocase(p,"charset=");

      if (p)
      {
        bot->charset=get_charset(p+8);
      }
      else bot->charset=WIN_1251;
      switch(bot->transfer_encoding)
      {
      default:
      case BIT8:
        buf=malloc(bot->size+1);
        strncpy(buf,eml+bot->offset,size);
        buf[size]=0;
        break;
         
      case BASE64:
        buf_size=bot->size;
        buf=base64_decode(eml+bot->offset, &buf_size);
        break;
          
      case QPRINTABLE:
        buf_size=bot->size;
        buf=quoted_printable_decode(eml+bot->offset, &buf_size);
        break;  
      }
      switch(bot->charset)
      {
      case WIN_1251:
      default:
        break;
        
      case KOI8_R:
        koi2win(buf, buf);
        break;
        
      case ISO_8859_5:
        iso885952win(buf, buf);
        break;
      }
      ws=AllocWS(strlen(buf));
      ascii2ws(ws, buf);
        
      PrepareEditControl(&ec);
      ConstructEditControl(&ec,3,0x40,ws,ws->wsbody[0]);
      PrepareEditCOptions(&ec_options);
      SetFontToEditCOptions(&ec_options,1);
      CopyOptionsToEditControl(&ec,&ec_options);
      AddEditControlToEditQend(eq,&ec,ma); 
      FreeWS(ws);
      mfree(buf);
      break;
      
    case MULTIPART:
      p=ml_list->header;
      if (!p) break;
      p=strstr_nocase(p,"boundary=");
      if (!p) break;
      p+=9;
      if (*p=='\"')
      {
        while(*p=='\"') p++;
      }
      for (i=0; (c=p[i])!='\"';i++)
      {
        boundary[i]=c;
      }
      boundary[i]=0;
      p=strstr_nocase(eml,boundary);
      while(p)
      {
        p+=i;
        if (!strncmp(p,"--\r\n",4)) break;
        prev=bot;
        while(prev->next) prev=prev->next;
        prev=prev->next=malloc(sizeof(MAIL_PART));
        prev->next=0;
        buf=strstr(p,"\r\n\r\n");
        buf+=4;
        prev->offset=buf-eml;
        
        b_end=strstr_nocase(p,boundary);
        prev->size=b_end-buf;
        
        content_type=strstr_nocase(p,"content-type:");
        if (content_type && content_type<buf)
        {
          cstrcpy(prev->ct,content_type+13);          
        }
        else strcpy(prev->ct,default_ctype);
        prev->content_type=get_ctype_index(prev->ct);
        
        content_encoding=strstr_nocase(p,"Content-Transfer-Encoding:");
        if (content_encoding && content_encoding<buf)
        {
          cstrcpy(prev->te,content_encoding+26);          
        }
        else strcpy(prev->te,default_transfere);
        prev->transfer_encoding=get_ctencoding_index(prev->te);
        p=b_end;
        
      }      
    }
    bot=bot->next;
  }
  bot=top;
  while(bot)
  {
    prev=bot;
    bot=bot->next;
    mfree(prev);
  }
  mfree(eml);
}
  

int create_view(ML_VIEW *ml_list)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  EDITC_OPTIONS ec_options;
  char *from, *to, *subject;
  WSHDR *ws, *headers;
  
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  headers=AllocWS(100);
  from=get_from(ml_list);
  if (from)
  {
    if (*from==' ') from++;
    ws=AllocWS(strlen(from));
    ascii2ws(ws,from);
    ascii2ws(headers,"From:");
    
    ConstructEditControl(&ec,1,0x40,headers,headers->wsbody[0]);
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    
    ConstructEditControl(&ec,3,0x40,ws,ws->wsbody[0]);
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma); 
    FreeWS(ws);
  }
  
  to=get_to(ml_list);
  if (to)
  {
    if (*to==' ') to++;
    ws=AllocWS(strlen(to));
    ascii2ws(ws,to);
    ascii2ws(headers,"To:");
    
    ConstructEditControl(&ec,1,0x40,headers,headers->wsbody[0]);
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    
    ConstructEditControl(&ec,3,0x40,ws,ws->wsbody[0]);
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma); 
    FreeWS(ws);
  }
  
  subject=get_subject(ml_list);
  if (subject)
  {
    if (*subject==' ') subject++;
    ws=AllocWS(strlen(subject));
    ascii2ws(ws,subject);
    ascii2ws(headers,"Subject:");
    
    ConstructEditControl(&ec,1,0x40,headers,headers->wsbody[0]);
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma);
    
    ConstructEditControl(&ec,3,0x40,ws,ws->wsbody[0]);
    PrepareEditCOptions(&ec_options);
    SetFontToEditCOptions(&ec_options,1);
    CopyOptionsToEditControl(&ec,&ec_options);
    AddEditControlToEditQend(eq,&ec,ma); 
    FreeWS(ws);
  }
  ascii2ws(headers,"---------------------");
  ConstructEditControl(&ec,1,0x40,headers,headers->wsbody[0]);
  PrepareEditCOptions(&ec_options);
  SetFontToEditCOptions(&ec_options,1);
  CopyOptionsToEditControl(&ec,&ec_options);
  AddEditControlToEditQend(eq,&ec,ma);  
  
  ParseMailBody(eq,ml_list,ma,headers);    
  FreeWS(headers);
  patch_header(&ed1_hdr);
  patch_input(&ed1_desc);  
  return CreateInputTextDialog(&ed1_desc,&ed1_hdr,eq,1,0);
}  
  
//----------------------------------------------------------------------------------------------
      
int GetIconIndex(ML_VIEW *m_list)
{
  if (!m_list) return (7);
  int l=m_list->state;
  switch (l)
  {
  case M_FULL_LOADED:
    if (m_list->is_read)
      return 1;
    else return 0;
  case M_HEADERS_LOADED:
    return 2;
  case M_LOAD_FULL:
    return 5;
  case M_DELETE:
    return 6;
  }
  return 0;
}

// ---------------------------------------------------------------------------------


void set_state_for_all(int state)
{
  ML_VIEW *mail_cur=(ML_VIEW *)&mails;
  while((mail_cur=mail_cur->next))
  {
    mail_cur->state=state;
  }
}

void set_state_download(void *gui)
{
  ML_VIEW *cur_menu;
  cur_menu=MenuGetUserPointer(gui);
  cur_menu->state=M_LOAD_FULL;
  GeneralFuncF1(2);
}

void set_state_delete(void *gui)
{
  ML_VIEW *cur_menu;
  cur_menu=MenuGetUserPointer(gui);
  cur_menu->state=M_DELETE;
  GeneralFuncF1(2);
}

void set_state_download_all()
{
  set_state_for_all(M_LOAD_FULL);
  GeneralFuncF1(2);
}

void set_state_delete_all()
{
  set_state_for_all(M_DELETE);
  GeneralFuncF1(2);
}

void delete_record(void *gui)
{ 
  ML_VIEW *cur_menu;
  cur_menu=MenuGetUserPointer(gui);
  ML_VIEW *mail_cur=(ML_VIEW *)&mails;
  ML_VIEW *ml_prev=mail_cur;
  while((mail_cur=mail_cur->next))
  {
    if(!(strcmp(mail_cur->uidl,cur_menu->uidl)))
    {
      ml_prev->next=mail_cur->next;
      mfree(mail_cur->uidl);
      mfree(mail_cur->header);
      mfree(mail_cur->subject);
      mfree(mail_cur->from);
      mfree(mail_cur->to);
      mfree(mail_cur->content_type);
      mfree(mail_cur->content_encoding);
      mfree(mail_cur);
      request_recount_mailmenu=1;
      break;        
    }
    ml_prev=mail_cur;
  }
  GeneralFuncF1(2);
}
    
  

void back()
{
  GeneralFuncF1(1);
}



HEADER_DESC options_menuhdr={0,0,0,0,NULL,(int)"Опции",LGP_NULL};

MENUITEM_DESC options_menu_ITEMS[6]=
{
  {NULL,(int)"Догрузить",     LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Удалить",       LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Удалить все",   LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Удалить запись",LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Догрузить все", LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Назад",         LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
};

void *options_menu_HNDLS[6]=
{
  (void *)set_state_download,
  (void *)set_state_delete,
  (void *)set_state_delete_all,
  (void *)delete_record,
  (void *)set_state_download_all,
  (void *)back,  
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
  6 //n
};

void create_options_menu(ML_VIEW *i)
{
  options_menuhdr.rc.x=3;
  options_menuhdr.rc.y=0x18;
  options_menuhdr.rc.x2=ScreenW()-6;
  options_menuhdr.rc.y2=0x18+0x13;
  
  options_menu_id=CreateMenu(1,0,&options_menu_STRUCT,&options_menuhdr,0,6,i,0);
}

// ----------------------------------------------------------------------------------

HEADER_DESC maillist_menuhdr={0,0,131,21,NULL,(int)"Входящие",LGP_NULL};


int maillist_menu_onkey(void *data, GUI_MSG *msg)
{
  ML_VIEW *mail_cur;
  int i;
  int keycode;
  keycode=msg->keys;
  i=GetCurMenuItem(data);
  mail_cur=find_mlist_ByID(i);
  if (!mail_cur) return (0);
  switch(keycode)
  {
  case 0x18:
    //    GeneralFunc_F1(1);
    create_options_menu(mail_cur);
    return(-1);
    
  case 0x3D:
    mail_cur->is_read=1;
    view_mail_id=create_view(mail_cur);
    return(-1);
  } 
  return(0);
}

void maillist_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
    if (request_recount_mailmenu)
    {
      request_recount_mailmenu=0;
      Menu_SetItemCountDyn(data,get_mlist_N());
    }
  }
}

void maillist_menu_iconhndl(void *data, int curitem, int *unk)
{
  ML_VIEW *mail_cur;
  WSHDR *ws;
  char *subject;
  mail_cur=find_mlist_ByID(curitem);
  if (!mail_cur) return;
  void *item=AllocMenuItem(data);
  
  subject=get_subject(mail_cur);
  if (subject)
  {
    ws=AllocMenuWS(data,strlen(subject));
    wsprintf(ws,"%t",subject);
  }
  else    
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,"%t","Ошибка");
  }
  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(mail_cur));
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
  InitMailDB();
  int mails_num=get_mlist_N();
  if (mails_num)
  {
    maillist_menu_id=CreateMenu(0,0,&maillist_menu,&maillist_menuhdr,0,mails_num,0,0);
  }
  else ShowMSG(1,(int)"Nothing loaded!");
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
  str_2ws(ws,successed_config_filename,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
}

void Exit(void)
{
  GeneralFuncF1(1);
}

HEADER_DESC mainmenu_HDR={0,0,0,0,NULL,(int)"MailViewer",LGP_NULL};


MENUITEM_DESC mainmenu_ITEMS[4]=
{
  {NULL,(int)"Входящие",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Исходящие",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Настройки",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,(int)"Выход",LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
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
  FreeMailDB();
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
    }
    if ((int)msg->data0==options_menu_id)
    {
      options_menu_id=0;
      if ((int)msg->data1==2)
      {
        write_mail_DB();
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"MailViewer");
}


int main()
{
  MAIN_CSM main_csm;
  InitConfig();
  
  S_ICONS[0]=(int)I_UNREAD;
  S_ICONS[1]=(int)I_READ;
  S_ICONS[2]=(int)I_HEADER;
  S_ICONS[3]=(int)I_HEADER_ATT;
  S_ICONS[4]=(int)I_MES_ATT;
  S_ICONS[5]=(int)I_MES_DOWN;
  S_ICONS[6]=(int)I_MES_DEL;

  LockSched();
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  UnlockSched();
}
