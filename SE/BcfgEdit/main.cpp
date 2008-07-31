#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"
#include "..\\include\cfg_items.h"
#include <errno.h>

#include "visual.h"
#include "datetime.h"
#include "main.h"

#define MESSAGE(__STR__) MessageBox(LGP_NULL,__STR__,0, 1 ,11000,(BOOK*)FindBook(isBcfgEditBook));


const PAGE_MSG bk_msglst_base[] @ "DYN_PAGE"  = 
{
  ELF_TERMINATE_EVENT,     TerminateElf,
  ELF_SHOW_INFO_EVENT,     ShowAuthorInfo,
  NIL_EVENT_TAG,           NULL
};

const PAGE_MSG bk_msglst_main[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    MainPageOnCreate,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_base = {"BcfgEdit_Base_Page",0,bk_msglst_base};
const PAGE_DESC bk_main = {"BcfgEdit_Main_Page",0,bk_msglst_main};


wchar_t cfgpath[128];
wchar_t cfgname[128];

//Указатель на буфер конфигурации
char *cfg;
//Длинна файла конфигурации
int size_cfg=0;

unsigned int level;
CFG_HDR *levelstack[16];

enum INPUT_TYPES {
  IT_REAL=0,
  IT_STRING=1,
  IT_INTEGER=2,
  IT_PHONE_NUMBER=3,
  IT_DIGITAL_PASS=4,
  IT_DIGITAL_IP=5,
  IT_URL=6,
  IT_UNSIGNED_DIGIT=9,
  IT_EXTRA_DIGIT=10,
  IT_ABC_OR_DIGIT=11,
  IT_EXTRA_DIGIT_2=12,
  IT_ABC_OR_DIGIT_2=13,
};

#pragma segment="ELFBEGIN"
void elf_exit(void){
  kill_data(__segment_begin("ELFBEGIN"), (void(*)(void*))mfree_adr());
}

unsigned long Crc32(unsigned char *buf, unsigned long len)
{
  unsigned long crc_table[256];
  unsigned long crc;
  for (int i = 0; i < 256; i++)
  {
    crc = i;
    for (int j = 0; j < 8; j++)
      crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
    crc_table[i] = crc;
  }
  crc = 0xFFFFFFFFUL;
  while (len--) 
    crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
  return crc ^ 0xFFFFFFFFUL;
};

void win12512unicode(wchar_t *ws, const char *s, int len)
{
  int c;
  while((c=*s++)&&((len--)>0))
  {
    if (c==0xA8) c=0x401;
    if (c==0xAA) c=0x404;
    if (c==0xAF) c=0x407;
    if (c==0xB8) c=0x451;
    if (c==0xBA) c=0x454;
    if (c==0xBF) c=0x457;
    if ((c>=0xC0)&&(c<0x100)) c+=0x350;
    *ws++=c;
  }
  *ws=0;
}

char *unicode2win1251(char *s, wchar_t *ws, int len)
{
  char *d=s;
  int c;
  while((c=*ws++)&&((len--)>0))
  {
    if (c==0x401) c=0xA8;
    if (c==0x404) c=0xAA;
    if (c==0x407) c=0xAF;
    if (c==0x451) c=0xB8;
    if (c==0x454) c=0xBA;
    if (c==0x457) c=0xBF;
    if ((c>=0x410)&&(c<0x450)) c-=0x350;
    *s++=c;
  }
  *s=0;
  return(d);
}
int getnumwidth(unsigned int num)
{
  int i=1;
  while(num>=10) 
  {
    num/=10;
    i++;
  }
  return (i);  
}

int SaveCfg()
{
  int f;
  int result=0;
  if ((f=_fopen(cfgpath,cfgname,0x204,0x180,0))>=0)
  {
    if (fwrite(f,cfg,size_cfg)==size_cfg) result=1;
    fclose(f);
  }
  return result;
}

int LoadCfg(wchar_t *path, wchar_t *fname)
{
  int f;
  FSTAT fstat;
  int result=0;
  if (path)
  {
    if (isFileExist(path,fname,&fstat)!=-1)
    {
      if ((f=_fopen(path,fname,0x001,0x180,0))>=0)
      {
        size_cfg=fstat.fsize;
        if (size_cfg<=0)
        {
          MESSAGE(STR("Zero lenght of .bcfg file!"));
        }
        else
        {
          cfg=new char[(size_cfg+3)&~3];
          if (fread(f,cfg,size_cfg)!=size_cfg)
          {
            MESSAGE(STR("Can't read .bcfg file!"));
            delete cfg;
            cfg=NULL;
          }
          else result=1;
        }
        fclose(f);
      }
    }
  }
  return result;
}

void SendReconfigEvent()
{
  RECONFIG_EVENT_DATA *reconf;
  if (wstrlen(cfgpath)<MAXELEMS(reconf->path) && wstrlen(cfgname)<MAXELEMS(reconf->name))
  {
    reconf=(RECONFIG_EVENT_DATA *)malloc(sizeof(RECONFIG_EVENT_DATA));
    wstrcpy(reconf->path,cfgpath);
    wstrcpy(reconf->name,cfgname);
    UI_Event_wData(ELF_RECONFIG_EVENT,reconf,(void(*)(void*))mfree_adr());
  }
}


GUI *CreateSelectBCFGMenu(int)
{
  return NULL;
}

void OnYesExitGui(BOOK * bk, void *)
{
  SaveCfg();
  SendReconfigEvent();
  FreeBook(bk);
}

void OnNoExitGui(BOOK * bk, void *)
{
  FreeBook(bk);
}

void OnBackExitGui(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  GUI_Free(myBook->yesno);
  myBook->yesno=NULL;
}

void CloseMyBook(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  CFG_HDR *hp;
  if (level)
  {
    hp=levelstack[level];
    levelstack[level]=NULL;
    level--;
    GUI_Free(myBook->bcfg);
    myBook->bcfg=NULL;
    create_ed(bk, hp);
  }
  else
  {
    if (myBook->old_crc==Crc32((unsigned char *)cfg,size_cfg))
    {
      FreeBook(bk);     
    }
    else
    {
      myBook->yesno=CreateYesNoQuestionVA(0,
                                          VAR_BOOK(bk),
                                          VAR_YESNO_PRE_QUESTION(myBook->changes_have_been_made),
                                          VAR_YESNO_QUESTION(myBook->save_before_exit),
                                          0);
      AddMSGHook(myBook->yesno,ACTION_YES,OnYesExitGui);
      AddMSGHook(myBook->yesno,ACTION_NO,OnNoExitGui);
      AddMSGHook(myBook->yesno,ACTION_BACK,OnBackExitGui);
    }
  }
}

void OnCloseCBoxGui(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  GUI_Free((GUI *)myBook->cbox_gui);
  myBook->cbox_gui=NULL;
}

void OnSelectCBoxGui(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  int item=OneOfMany_GetSelected(myBook->cbox_gui);
  CFG_HDR *hp=myBook->cur_hp;
  if (item<hp->max)  *((int *)((char *)hp+sizeof(CFG_HDR)))=item;
  GUI_Free((GUI *)myBook->cbox_gui);
  myBook->cbox_gui=NULL;
}

void CreateCBoxGui(MyBOOK *myBook)
{
  GUI_ONEOFMANY *om=CreateOneOfMany(&myBook->book);
  CFG_HDR *hp=myBook->cur_hp;
  wchar_t ustr[64];
  myBook->cbox_gui=om;
  win12512unicode(ustr,hp->name,MAXELEMS(ustr)-1);
  GuiObject_SetTitleText(om,Str2ID(ustr,0,SID_ANY_LEN));
  STRID *strid=new STRID[hp->max];
  CFG_CBOX_ITEM *cbox=((CFG_CBOX_ITEM *)((char *)hp+sizeof(CFG_HDR)+sizeof(int)));
  for (int i=0; i<hp->max; i++)
  {
    win12512unicode(ustr,cbox->cbox_text,MAXELEMS(ustr)-1);
    strid[i]=Str2ID(ustr,0,SID_ANY_LEN);
    cbox++;
  }
  OneOfMany_SetTexts(om,strid,hp->max);
  delete strid;
  OneOfMany_SetChecked(om,*((int *)((char *)hp+sizeof(CFG_HDR))));
  AddMSGHook(om,ACTION_BACK,OnCloseCBoxGui);
  AddMSGHook(om,ACTION_SELECT1,OnSelectCBoxGui);
  ShowWindow(om);
}


void OnBackCreateTextInputGui(BOOK * bk, u16 *string, int len)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  GUI_Free(myBook->text_input);
  myBook->text_input=NULL;
}

void OnOkCreateUnsignedNumberGui(BOOK * bk, wchar_t *string, int len)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  CFG_HDR *hp=myBook->cur_hp;
  wchar_t ustr[64];
  unsigned int ui;
  *_Geterrno()=0;
  ui=wcstoul(string,0,10);
  if (ui<hp->min || ui>hp->max || *_Geterrno()==ERANGE)
  {
    snwprintf(ustr,MAXELEMS(ustr)-1,L"min: %u\nmax: %u",hp->min, hp->max);
    MessageBox(LGP_NULL,Str2ID(ustr,0,SID_ANY_LEN),0, 1 ,5000, bk);
  }
  else
  {
    *((unsigned int *)((char *)hp+sizeof(CFG_HDR)))=ui;
    GUI_Free(myBook->text_input);
    myBook->text_input=NULL;
  }
}


void CreateUnsignedNumberInput(MyBOOK *myBook)
{
  wchar_t ustr[64];
  CFG_HDR *hp=myBook->cur_hp;
  STRID text, header_name;
  
  snwprintf(ustr,MAXELEMS(ustr)-1,L"%u",*((unsigned int *)((char *)hp+sizeof(CFG_HDR))));
  text=Str2ID(ustr,0,SID_ANY_LEN);
  win12512unicode(ustr,hp->name,MAXELEMS(ustr)-1);
  header_name=Str2ID(ustr,0,SID_ANY_LEN);
  myBook->text_input=(GUI *)CreateStringInput(0,
                                              VAR_HEADER_TEXT(header_name),
                                              VAR_STRINP_MAX_LEN(getnumwidth(hp->max)),
                                              VAR_STRINP_MODE(IT_UNSIGNED_DIGIT),
                                              VAR_BOOK(myBook),
                                              VAR_STRINP_ENABLE_EMPTY_STR(0),
                                              VAR_STRINP_TEXT(text),
                                              VAR_PREV_ACTION_PROC(OnBackCreateTextInputGui),
                                              VAR_OK_PROC(OnOkCreateUnsignedNumberGui),
                                              0);
}

void OnOkCreateSignedNumberGui(BOOK * bk, wchar_t *string, int len)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  CFG_HDR *hp=myBook->cur_hp;
  wchar_t ustr[64];
  int i;
  *_Geterrno()=0;
  i=wcstol(string,0,10);
  if (i<(int)hp->min || i>(int)hp->max || *_Geterrno()==ERANGE)
  {
    snwprintf(ustr,MAXELEMS(ustr)-1,L"min: %d\nmax: %d",hp->min,hp->max);
    MessageBox(LGP_NULL,Str2ID(ustr,0,SID_ANY_LEN),0, 1 ,5000, bk);
  }
  else
  {
    *((int *)((char *)hp+sizeof(CFG_HDR)))=i;
    GUI_Free(myBook->text_input);
    myBook->text_input=NULL;
  }
}

void CreateSignedNumberInput(MyBOOK *myBook)
{
  wchar_t ustr[64];
  int min,max,k1,k2;
  CFG_HDR *hp=myBook->cur_hp;
  STRID text, header_name;
  snwprintf(ustr,MAXELEMS(ustr)-1,L"%d",*((int *)((char *)hp+sizeof(CFG_HDR))));
  text=Str2ID(ustr,0,SID_ANY_LEN);
  win12512unicode(ustr,hp->name,MAXELEMS(ustr)-1);
  header_name=Str2ID(ustr,0,SID_ANY_LEN);
  min=(k1=(int)hp->min)>=0?k1:(-k1)*10;
  max=(k2=(int)hp->max)>=0?k2:(-k2)*10;
  myBook->text_input=(GUI *)CreateStringInput(0,
                                              VAR_HEADER_TEXT(header_name),
                                              VAR_STRINP_MAX_LEN(getnumwidth(min>max?min:max)),
                                              VAR_STRINP_MODE(IT_INTEGER),
                                              VAR_BOOK(myBook),
                                              VAR_STRINP_ENABLE_EMPTY_STR(0),
                                              VAR_STRINP_TEXT(text),
                                              VAR_PREV_ACTION_PROC(OnBackCreateTextInputGui),
                                              VAR_OK_PROC(OnOkCreateSignedNumberGui),
                                              0);
}


void OnOkCreateWinOrPassGui(BOOK * bk, wchar_t *string, int len)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  CFG_HDR *hp=myBook->cur_hp;
  wchar_t ustr[64];
  if (len<hp->min || len>hp->max)
  {
    snwprintf(ustr,MAXELEMS(ustr)-1,L"min_string_len: %d\nmax_string_len: %d",hp->min,hp->max);
    MessageBox(LGP_NULL,Str2ID(ustr,0,SID_ANY_LEN),0, 1 ,5000, bk);
  }
  else
  {
    unicode2win1251((char *)hp+sizeof(CFG_HDR),string,hp->max);
    GUI_Free(myBook->text_input);
    myBook->text_input=NULL;
  }
}

void CreateWinOrPassSI(MyBOOK *myBook, int is_pass)
{
  wchar_t *ustr;
  CFG_HDR *hp=myBook->cur_hp;
  int len;
  STRID text, header_name;
  len=hp->max;
  if (len<63) len=63;
  ustr=new wchar_t[len+1];
  win12512unicode(ustr,hp->name,len);
  header_name=Str2ID(ustr,0,SID_ANY_LEN);
  win12512unicode(ustr,(char *)hp+sizeof(CFG_HDR),len);
  text=Str2ID(ustr,0,SID_ANY_LEN);
  myBook->text_input=(GUI *)CreateStringInput(0,
                                              VAR_HEADER_TEXT(header_name),
                                              VAR_STRINP_MIN_LEN(hp->min),
                                              VAR_STRINP_MAX_LEN(hp->max),
                                              VAR_STRINP_MODE(IT_STRING),
                                              VAR_BOOK(myBook),
                                              VAR_STRINP_TEXT(text),
                                              VAR_PREV_ACTION_PROC(OnBackCreateTextInputGui),
                                              VAR_OK_PROC(OnOkCreateWinOrPassGui),
                                              VAR_STRINP_IS_PASS_MODE(is_pass),
                                              0);
  delete ustr;
}

void onEnterPressed(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  COLOR_TYPE color;
  int item=GetFocusetListObjectItem(myBook->bcfg);
  myBook->cur_hp=(CFG_HDR *)ListElement_GetByIndex(myBook->list,item);
  switch(myBook->cur_hp->type)
  {
  case CFG_UINT:
    CreateUnsignedNumberInput(myBook);
    break;
  case CFG_INT:
    CreateSignedNumberInput(myBook);
    break;
  case CFG_STR_WIN1251:
    CreateWinOrPassSI(myBook,0);
    break;
  case CFG_CBOX:
    CreateCBoxGui(myBook);
    break;
  case CFG_STR_PASS:
    CreateWinOrPassSI(myBook,1);
    break;
  case CFG_COORDINATES:
    CreateEditCoordinatesGUI(myBook);
    break;
  case CFG_COLOR:
    color.char_color=((char *)myBook->cur_hp+sizeof(CFG_HDR));
    CreateEditColorGUI(myBook, color,0);
    break;
  case CFG_LEVEL:
    level++;
    levelstack[level]=myBook->cur_hp;
    GUI_Free(myBook->bcfg);
    myBook->bcfg=NULL;
    create_ed(bk, NULL);
    return;
  case CFG_CHECKBOX:
    int str_id, a;
    a=(*((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)))=!*((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR))));
    str_id=(a?myBook->check_box_checked:myBook->check_box_unchecked)+0x78000000;
    ListMenu_SetSecondLineText((GUI_LIST *)myBook->bcfg,item,str_id);
    break;
  case CFG_TIME:
    BookObj_CallPage(&myBook->book,&bk_time_input);
    break;
  case CFG_DATE:
    BookObj_CallPage(&myBook->book,&bk_date_input);
    break;
  case CFG_COLOR_INT:
    color.int_color=*((unsigned int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
    CreateEditColorGUI(myBook, color,1);
    break;
  case CFG_FONT:
    CreateFontSelectGUI(myBook);
    break;
  default:
    return;
  }
}



STRID GetSubItemText(MyBOOK * myBook, CFG_HDR *hp)
{
  wchar_t ustr[64];
  STRID str_id=LGP_NULL;
  if (hp)
  {
    switch(hp->type)
    {
    case CFG_UINT:
      snwprintf(ustr,MAXELEMS(ustr)-1,L"%u",*((unsigned int *)((char *)hp+sizeof(CFG_HDR))));
      str_id=Str2ID(ustr,0,SID_ANY_LEN);
      break;
    case CFG_INT:
      snwprintf(ustr,MAXELEMS(ustr)-1,L"%d",*((int *)((char *)hp+sizeof(CFG_HDR))));
      str_id=Str2ID(ustr,0,SID_ANY_LEN);
      break;      
    case CFG_STR_WIN1251:
      win12512unicode(ustr,((char *)hp+sizeof(CFG_HDR)),MAXELEMS(ustr)-1);
      str_id=Str2ID(ustr,0,SID_ANY_LEN);
      break;
    case CFG_CBOX:
      win12512unicode(ustr,((char *)hp+sizeof(CFG_HDR)+sizeof(int)+*((int *)((char *)hp+sizeof(CFG_HDR)))*sizeof(CFG_CBOX_ITEM)),MAXELEMS(ustr)-1);
      str_id=Str2ID(ustr,0,SID_ANY_LEN);
      break;
    case CFG_STR_PASS:
      win12512unicode(ustr,"********",MAXELEMS(ustr)-1);
      str_id=Str2ID(ustr,0,SID_ANY_LEN);
      break;
    case CFG_COORDINATES:
      int x,y;
      x=*((int *)((char *)hp+sizeof(CFG_HDR)));
      y=*((int *)((char *)hp+sizeof(CFG_HDR)+sizeof(int)));
      snwprintf(ustr,MAXELEMS(ustr)-1,L"%03d,%03d",x,y);
      str_id=Str2ID(ustr,0,SID_ANY_LEN);
      break;
    case CFG_COLOR:
      {
        char *color;
        color=(char *)hp+sizeof(CFG_HDR);
        snwprintf(ustr,MAXELEMS(ustr)-1,L"%02X,%02X,%02X,%02X",color[0],color[1],color[2],color[3]);
        str_id=Str2ID(ustr,0,SID_ANY_LEN);
      }
      break;
    case CFG_LEVEL:
      {
        win12512unicode(ustr,"[Enter]",MAXELEMS(ustr)-1);
        str_id=Str2ID(ustr,0,SID_ANY_LEN);
        
      }
      break;
    case CFG_CHECKBOX:
      str_id=(*((int *)((char *)hp+sizeof(CFG_HDR)))?myBook->check_box_checked:myBook->check_box_unchecked)+0x78000000;
      break;
    case CFG_TIME:
      {
        TIME *time=((TIME *)((char *)hp+sizeof(CFG_HDR)));
        snwprintf(ustr,MAXELEMS(ustr)-1,L"%02d:%02d",time->hour,time->min);
        str_id=Str2ID(ustr,0,SID_ANY_LEN);
      }
      break;
    case CFG_DATE:
      {
        DATE *date=((DATE *)((char *)hp+sizeof(CFG_HDR)));
        snwprintf(ustr,MAXELEMS(ustr)-1,L"%02d.%02d.%04d",date->day,date->mon,date->year);
        str_id=Str2ID(ustr,0,SID_ANY_LEN);
      }
      break;
    case CFG_COLOR_INT:
      {
        unsigned int color;
        color=*((unsigned int *)((char *)hp+sizeof(CFG_HDR)));
        snwprintf(ustr,MAXELEMS(ustr)-1,L"%02X,%02X,%02X,%02X",COLOR_GET_R(color),COLOR_GET_G(color),COLOR_GET_B(color),COLOR_GET_A(color));
        str_id=Str2ID(ustr,0,SID_ANY_LEN);
      }
      break;
    case CFG_FONT:
      str_id=Str2ID(Font_GetNameByFontId(*((int *)((char *)hp+sizeof(CFG_HDR)))),0,SID_ANY_LEN);
      break;
    case CFG_STR_UTF8:
    case CFG_UTF8_STRING:
    case CFG_RECT:
      snwprintf(ustr,MAXELEMS(ustr)-1,L"Type %d is not supporting yet",hp->type);
      str_id=Str2ID(ustr,0,SID_ANY_LEN);
      break;
    }
  }
  return str_id;
}

// устанавливаем тексты в пунктах меню
int onLBMessage(GUI_MESSAGE * msg)
{
  MyBOOK * myBook = (MyBOOK *) FindBook(isBcfgEditBook);
  wchar_t ustr[32];
  int item;
  int str_id;
  CFG_HDR *hp;
  switch(msg->msg)
  {
    // onCreateListItem
  case 1:
    item=GUIonMessage_GetCreatedItemIndex(msg);
    hp=(CFG_HDR *)ListElement_GetByIndex(myBook->list,item);
    win12512unicode(ustr,hp->name,MAXELEMS(ustr)-1);
    SetMenuItemText0(msg,Str2ID(ustr,0,SID_ANY_LEN));
    SetMenuItemText2(msg,Str2ID(L"BcfgEdit v1.0\n(c) Rst7, KreN",0,SID_ANY_LEN));
    str_id=GetSubItemText(myBook, hp);
    if (str_id==LGP_NULL) str_id=Str2ID (L"Хрень",0,SID_ANY_LEN);
    SetMenuItemText1(msg,str_id);
  }
  return(1);
};


STRID GetParentName()
{
  wchar_t ustr[32];
  if (level)
  {
    CFG_HDR *hp=levelstack[level];
    win12512unicode(ustr,hp->name,MAXELEMS(ustr)-1);    
  }
  else
  {
    wstrncpy(ustr,cfgname,MAXELEMS(ustr)-1);
  }
  return (Str2ID(ustr,0,MAXELEMS(ustr)-1));
}

// создание меню
GUI_LIST * CreateGuiList(MyBOOK * bk, int set_focus)
{
  GUI_LIST * lo;
  lo=CreateListObject(&bk->book,0);
  bk->bcfg=(GUI *)lo;
  GuiObject_SetTitleText(lo,GetParentName());
  SetNumOfMenuItem(lo,bk->list->FirstFree);
  OneOfMany_SetonMessage((GUI_ONEOFMANY*)lo,onLBMessage);
  SetCursorToItem(lo,set_focus);
  SetMenuItemStyle(lo,3);
  AddMSGHook(lo,ACTION_BACK, CloseMyBook);
  AddMSGHook(lo,ACTION_SELECT1,onEnterPressed);
  return(lo);
};


GUI *create_ed(BOOK *book, CFG_HDR *need_to_focus)
{
  MyBOOK *mbk=(MyBOOK *)book;
  char *p=cfg;
  int n=size_cfg;
  CFG_HDR *hp;
  int need_to_jump=0;
  LIST *list=mbk->list;
  while(list->FirstFree)   ListElement_Remove(list,0); 
  GUI *gui=NULL;
  
  int i;
  unsigned int curlev=0;
  CFG_HDR *parent=NULL;
  CFG_HDR *parents[16];
  
  while(n>=sizeof(CFG_HDR))
  {
    hp=(CFG_HDR*)p;
    if (hp->type==CFG_LEVEL)
    {
      if (hp->min)
      {
        if ((curlev==level)&&(parent==levelstack[level]))
	{
	  ListElement_Add(list,hp);
	}
      }
      else if (curlev)
      {
        parent=parents[curlev--];
      }
    }
    else
    {
      if ((curlev==level)&&(parent==levelstack[level]))
      {
        ListElement_Add(list,hp);
      }
    }
    n-=sizeof(CFG_HDR);
    p+=sizeof(CFG_HDR);
    switch(hp->type)
    {
    case CFG_UINT:
      n-=sizeof(unsigned int);
      if (n<0)
      {
      L_ERRCONSTR:
      L_ERRCONSTR1:
        goto L_ENDCONSTR;
      }
      p+=sizeof(unsigned int);
      break;
    case CFG_INT:
      n-=sizeof(int);
      if (n<0) goto L_ERRCONSTR;
      p+=sizeof(int);
      break;
    case CFG_STR_UTF8:
      n-=(hp->max+1+3)&(~3);
      if (n<0) goto L_ERRCONSTR;
      p+=(hp->max+1+3)&(~3);
      break;
    case CFG_STR_WIN1251:
      n-=(hp->max+1+3)&(~3);
      if (n<0) goto L_ERRCONSTR;
      p+=(hp->max+1+3)&(~3);
      break;

    case CFG_UTF8_STRING:
      n-=(hp->max+1+3)&(~3);
      if (n<0) goto L_ERRCONSTR;
      p+=(hp->max+1+3)&(~3);
      break;      
      
    case CFG_CBOX:
      n-=hp->max*sizeof(CFG_CBOX_ITEM)+4;
      if (n<0) goto L_ERRCONSTR;
      i=*((int *)p);
      if (i>=hp->max)
      {
        goto L_ERRCONSTR1;
      }
      p+=hp->max*sizeof(CFG_CBOX_ITEM)+4;
      break;
    case CFG_STR_PASS:
      n-=(hp->max+1+3)&(~3);
      if (n<0) goto L_ERRCONSTR;
      p+=(hp->max+1+3)&(~3);
      break;
    case CFG_COORDINATES:
      n-=8;
      if (n<0) goto L_ERRCONSTR;
      p+=8;
      break;
    case CFG_COLOR:
      n-=4;
      if (n<0) goto L_ERRCONSTR;
      p+=4;
      break;
    case CFG_LEVEL:
      if (n<0) goto L_ERRCONSTR;
      if (hp->min)
      {
	if ((curlev==level)&&(parent==levelstack[level]))
	{
          if (need_to_focus)
          {
            if (need_to_focus==hp)  need_to_jump=list->FirstFree-1;
          }
	}
	curlev++;
	parents[curlev]=parent;
	parent=hp;
      }
      continue;
    case CFG_CHECKBOX:
      n-=sizeof(int);
      if (n<0) goto L_ERRCONSTR;
      p+=sizeof(int);
      break;

    case CFG_TIME:
      n-=sizeof(DATE);
      if (n<0) goto L_ERRCONSTR;
      p+=sizeof(DATE);
      break;
      
    case CFG_DATE:
      n-=sizeof(TIME);
      if (n<0) goto L_ERRCONSTR;
      p+=sizeof(TIME);
      break;

    case CFG_RECT:
      n-=sizeof(RECT);
      if (n<0) goto L_ERRCONSTR;
      p+=sizeof(RECT);
      break;
      
    case CFG_COLOR_INT:
      n-=sizeof(int);
      if (n<0) goto L_ERRCONSTR;
      p+=sizeof(int);
      break;
    case CFG_FONT:
      n-=sizeof(int);
      if (n<0) goto L_ERRCONSTR;
      p+=sizeof(int);
      break;
      
    default:
      goto L_ENDCONSTR;
    }
  }
L_ENDCONSTR:
  gui=(GUI *)CreateGuiList(mbk, need_to_jump);
  ShowWindow(gui);
  return(gui);
}


static int TerminateElf(void * ,BOOK *book)
{
  FreeBook(book);
  return(1);
}


static int ShowAuthorInfo(void *mess ,BOOK *book)
{
  MSG * msg = (MSG *)mess;
  MessageBox(LGP_NULL,STR("\nBcfgEdit v1.0\n(c) Rst7, KreN"),0, 1 ,5000,msg->book);
  return(1);
}

static int MainPageOnCreate(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int find_cfg=1;
  level=0;
  int icon_id;
  cfg=NULL;
  mbk->list=List_New();
  memset(levelstack,0,sizeof(levelstack));
  
  textidname2id(IDN_CHANGES_HAVE_BEEN_MADE,-1,&mbk->changes_have_been_made);
  textidname2id(IDN_SAVE_BEFORE_EXIT,-1,&mbk->save_before_exit);
  iconidname2id(IDN_CHECKBOX_UNCHECKED_ICON,-1,&icon_id);
  mbk->check_box_unchecked=icon_id;
  iconidname2id(IDN_CHECKBOX_CHECKED_ICON,-1,&icon_id);
  mbk->check_box_checked=icon_id;
  if (*cfgpath=='/') // Вероятно передали указатель на файл
  {
    if (LoadCfg(cfgpath, cfgname))
    {
      mbk->old_crc=Crc32((unsigned char *)cfg,size_cfg);
      create_ed((BOOK *)mbk, 0);
      find_cfg=0;
    }
  }
  if (find_cfg)
  {
    mbk->sel_bcfg=CreateSelectBCFGMenu(0);
  }
  return (1);
}

// при закрытии книги
static void onMyBookClose(BOOK * book)
{
  MyBOOK *mbk=(MyBOOK *)book;
  delete cfg;
  List_Free(mbk->list);
  SUBPROC(elf_exit);
}

int isBcfgEditBook(BOOK * struc)
{
  return(struc->onClose==(void*)onMyBookClose);
}

int main(wchar_t *elfname, wchar_t *path, wchar_t *fname)
{
  MyBOOK * myBook=new MyBOOK;
  memset(myBook,0,sizeof(MyBOOK));
  if (path) wstrncpy(cfgpath,path,MAXELEMS(cfgpath)); else *cfgpath=0;
  if (fname) wstrncpy(cfgname,fname,MAXELEMS(cfgname)); else *cfgname=0;
  if (!CreateBook(myBook,onMyBookClose,&bk_base,"BcfgEdit",-1,0))
  {
    delete myBook;
    SUBPROC(elf_exit);
    return (0);    
  }
  BookObj_GotoPage((BOOK *)myBook,&bk_main);
  return 0;
}


