#include "..\inc\swilib.h"

GBSTMR tmr_scroll;

#define TMR_SECOND 216


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


#ifdef ELKA
#define MAX_ESTR_LEN 9
#else
#define MAX_ESTR_LEN 13
#endif

#ifdef NEWSGOLD
#define MAX_RECORDS 5000
#define LEVEL1_RN	(41*41)
#define LEVEL2_RN	(41)
#else 
#define MAX_RECORDS 1024
#define LEVEL1_RN	(31)
#endif

#define wslen(ARG) (ARG->wsbody[0])

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

int (*old_ed_onkey)(GUI *gui, GUI_MSG *msg);
void (*old_ed_ghook)(GUI *gui, int cmd);
void (*old_ed_redraw)(void *data);

//0 - ждем появления диалога
//1 - диалог появился, зацепились, ждем зацепления за onRedraw
//2 - зацепили onRedraw, idle-режим
//3 - запрос поиска
//4 - отправлен запрос
//5 - производится поиск
volatile int hook_state=0;

volatile WSHDR *e_ws;

volatile int curpos; //Позиция курсора в списке

typedef struct
{
  void *next;
  WSHDR *name;
  WSHDR *num[4];
  WSHDR *icons;
}CLIST;

volatile CLIST *cltop; //Начало
volatile CLIST *clbot; //Конец

char dstr[4][40];

int menu_icons[8];
int utf_symbs[4];

#define USR_WIRE 0xE106
#define USR_MOBILE 0xE107
#define WORK_WIRE 0xE108
#define WORK_FAX 0xE109
#define USR_FAX 0xE10A
#define WORK_MOBILE 0xE10E

void InitIcons(void)
{
  menu_icons[0]=GetPicNByUnicodeSymbol(utf_symbs[0]=USR_WIRE); //12345
  menu_icons[2]=GetPicNByUnicodeSymbol(utf_symbs[1]=WORK_WIRE); //093
  menu_icons[4]=GetPicNByUnicodeSymbol(utf_symbs[2]=USR_MOBILE); //651
  menu_icons[6]=GetPicNByUnicodeSymbol(utf_symbs[3]=WORK_MOBILE); //884
}

//Уничтожить список
void FreeCLIST(void)
{
  LockSched();
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  clbot=0;
  UnlockSched();
  while(cl)
  {
    CLIST *p;
    FreeWS(cl->name);
    FreeWS(cl->num[0]);
    FreeWS(cl->num[1]);
    FreeWS(cl->num[2]);
    FreeWS(cl->num[3]);
    FreeWS(cl->icons);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

//-----------------------------------------------------
//Поиск подстроки в строке по методу Т9
//-----------------------------------------------------
int CompareStrT9(WSHDR *ws, WSHDR *ss)
{
  int spos=1;
  int wpos=1;
  int c;

  //Таблица ключей для поиска текста
  static const char key[256]=
    "11111111111111111111111111111111"   
      "10001**0***0000*012345678900***0"
	"0222333444555666777788899991*110"
	  "122233344455566677778889999111*1"
	    "11111111111111111111111111111111"
	      "11111111311111111111111131111111"
		"22223333444455566677778888899999"
		  "22223333444455566677778888899999";

  if (!wslen(ss)) return(1); //Пустая строка всегда пляшет ;)
  if (!wslen(ws)) return(0); //А пустая исходная не стреляет ;)
  do
  {
    c=ws->wsbody[wpos];
    if ((wpos==1)&&(c=='+'))
    {
      wpos=2; //Пропуск первого '+'
      continue;
    }
    if (!spos)
    {
      //Ищем пробел
      if (c==' ') spos=1; //Начинаем заново
    }
    else
    {
      //Преобразуем в код кнопки
      if ((c>=0x410)&&(c<0x450)) c-=0x350;
      if ((c==0x401)) c=0xA8;
      if ((c==0x451)) c=0xB8;
      c&=0xFF;
      c=key[c];
      if (c==ss->wsbody[spos])
      {
	spos++;
	if (spos>wslen(ss)) return(1); //Все совпало
      }
      else spos=0; //Ищем новое слово
    }
    wpos++;
  }
  while(wpos<=wslen(ws));
  return(0);
}

//=====================================================
// Конструктор списка
//=====================================================
void ConstructList(void)
{
  int fin;
  unsigned int ul;
  char recname[128];

  AB_UNPRES ur;
  void *buffer;
  
#pragma pack(1)
  struct {
#ifdef NEWSGOLD
    long dummy1;
    short dummy2;
    char bitmap[MAX_RECORDS/8];
#else
    long dummy1;
    char bitmap[MAX_RECORDS/8];    
#endif    
  } ABmain;
#pragma pack()

  int rec=0;
  int fsz;
  int total=0;
  CLIST contact;

  WSHDR *sws=AllocWS(50);

  FreeCLIST(); //Уничтожаем пред. список
  LockSched();
  if (e_ws) wstrcpy(sws,(WSHDR *)e_ws);
  if (hook_state==4) hook_state=5;
  UnlockSched();
  zeromem(&contact,sizeof(contact));
  if ((buffer=malloc(65536)))
  {
    zeromem(&ABmain,sizeof(ABmain));
    if ((fin=fopen("0:\\System\\apo\\addr\\main",A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
    {
      if (fread(fin,&ABmain,sizeof(ABmain),&ul)>=194)
      {
	fclose(fin,&ul);
	do
	{
	  if (ABmain.bitmap[rec>>3]&(0x80>>(rec&7)))
	  {

            #ifdef NEWSGOLD
	    //Запись есть в битмапе
            unsigned int rl1;
	    unsigned int rl2;
	    unsigned int rl3;
	    rl1=rec/LEVEL1_RN;
	    rl2=(rec%LEVEL1_RN)/LEVEL2_RN;
	    rl3=rec%LEVEL2_RN;
	    snprintf(recname,128,"0:\\System\\apo\\addr\\data\\%02d\\%02d\\%02d",rl1,rl2,rl3);
            #else
	    unsigned int rl1=rec/LEVEL1_RN;
	    unsigned int r12=rec%LEVEL1_RN;
	    snprintf(recname,128,"0:\\System\\apo\\addr\\%02x\\%02x",rl1,r12);            
            #endif             
	    if ((fin=fopen(recname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
	    {
	      zeromem(&ur,sizeof(ur));
              fsz=lseek(fin,0,S_END,&ul,&ul);
              lseek(fin,0,S_SET,&ul,&ul);
  	      fread(fin,buffer,fsz,&ul);
	      fclose(fin,&ul);
              #ifdef NEWSGOLD
	      UnpackABentry(&ur,((char *)buffer+8),fsz,0x28);
              #else
              UnpackABentry(&ur,((char *)buffer+4),fsz-4,0x28);
              #endif
	      int i=0;
	      zeromem(&contact,sizeof(contact));
	      while(i<ur.number_of_records)
	      {
		AB_UNPRES_ITEM *r=ur.record_list+i;
		if (r->no_data!=1)
		{
		  switch(GetTypeOfAB_UNPRES_ITEM(r->item_type))
		  {
		  case 0x05:
                    #ifdef NEWSGOLD
                    if (r->item_type==0x81)
                    #else
                    if (r->item_type==0x23||r->item_type==0x24)
                    #endif   
		    {
                      #ifdef NEWSGOLD
		      if (r->data)
		      {
			wstrcpy(contact.name=AllocWS(50),(WSHDR *)(r->data));
			*((int *)(&contact.next))|=CompareStrT9(contact.name,sws);
		      }
                      #else
                      if (r->data)
		      { 
                        if (!contact.name)
                        {
			 wstrcpy(contact.name=AllocWS(50),(WSHDR *)(r->data));
			 *((int *)(&contact.next))|=CompareStrT9(contact.name,sws);
                        }
                        else
                        {
                         wsAppendChar(contact.name,',');
                         wsAppendChar(contact.name,' ');
                         wstrcat(contact.name,(WSHDR *)(r->data));
                         *((int *)(&contact.next))|=CompareStrT9(contact.name,sws);
                        }
		      }
                      #endif 
		    }
		    break;
		  case 0x01:
		    {
		      PKT_NUM *p=(PKT_NUM*)r->data;
		      unsigned int n=r->item_type;
		      int j;
		      int c;
		      WSHDR *ws;
                      #ifdef NEWSGOLD
		      n-=0x62;
                      #else
                      n-=0x2A;
                      #endif 
		      if (n<4)
		      {
			if (p)
			{
			  ws=contact.num[n]=AllocWS(50);
			  j=0;
			  if (p->format==0x91) wsAppendChar(ws,'+');
			  while(j<p->data_size)
			  {
			    c=(p->data[j])&0x0F;
			    if (c!=0x0F) wsAppendChar(ws,c+'0'); else break;
			    c=(p->data[j]>>4)&0x0F;
			    if (c!=0x0F) wsAppendChar(ws,c+'0'); else break;
			    j++;
			  }
			  *((int *)(&contact.next))|=CompareStrT9(ws,sws);
			}
		      }
		      break;
		    }
		  }
		}
		i++;
	      }
	      if (!contact.next)
	      {
		FreeWS(contact.name);
		FreeWS(contact.num[0]);
		FreeWS(contact.num[1]);
		FreeWS(contact.num[2]);
		FreeWS(contact.num[3]);
	      }
	      else
	      {
		//Добавляем иконки телефонов
		contact.icons=AllocWS(10);
		if (contact.num[0]) wsAppendChar(contact.icons,utf_symbs[0]);
		if (contact.num[1]) wsAppendChar(contact.icons,utf_symbs[1]);
		if (contact.num[2]) wsAppendChar(contact.icons,utf_symbs[2]);
		if (contact.num[3]) wsAppendChar(contact.icons,utf_symbs[3]);
	      }
	      FreeUnpackABentry(&ur,mfree_adr());
	      if (hook_state!=5) goto L_STOP;
	      LockSched();
	      if ((hook_state==5)&&(contact.next))
	      {
		//Добавляем в список
		CLIST *p=malloc(sizeof(contact));
		CLIST *b=(CLIST *)clbot;
		contact.next=0;
		memcpy(p,&contact,sizeof(contact));
		if (b)
		{
		  //Не первый
		  b->next=p;
		  clbot=p;
		}
		else
		{
		  //Первый
		  cltop=p;
		  clbot=p;
		}
		if (curpos<2)
		{
		  if (total<5) REDRAW();
		}
		else
		{
		  if ((unsigned int)(total-(curpos-2))<5) REDRAW();
		}
		total++;
	      }
	      UnlockSched();
	    }
	  }
	  rec++;
	}
	while(rec<MAX_RECORDS);
      }
      else
      {
	fclose(fin,&ul);
      }
    }
  L_STOP:
    mfree(buffer);
  }
  if (contact.next)
  {
    FreeWS(contact.name);
    FreeWS(contact.num[0]);
    FreeWS(contact.num[1]);
    FreeWS(contact.num[2]);
    FreeWS(contact.num[3]);
  }
  LockSched();
  if (hook_state==5) hook_state=2; else FreeCLIST();
  UnlockSched();
  FreeWS(sws);
}

#pragma optimize=no_inline
void f_dummy(void){}

volatile int scroll_disp;
volatile int max_scroll_disp;

void scroll_timer_proc(void)
{
  int i=max_scroll_disp;
  if (i)
  {
    if (scroll_disp>=i)
    {
      scroll_disp=0;
      GBS_StartTimerProc(&tmr_scroll,TMR_SECOND,scroll_timer_proc);
    }
    else
    {
      scroll_disp++;
      GBS_StartTimerProc(&tmr_scroll,scroll_disp!=i?TMR_SECOND>>3:TMR_SECOND,scroll_timer_proc);
    }
    REDRAW();
  }
}

void DisableScroll(void)
{
  GBS_DelTimer(&tmr_scroll);
  max_scroll_disp=0;
  scroll_disp=0;
}

void my_ed_redraw(void *data)
{
  //  WSHDR *ews=(WSHDR*)e_ws;
  int i=curpos-2;
  int cp;
  CLIST *cl=(CLIST *)cltop;
  old_ed_redraw(data);

  if (!cl) return;

  if (!e_ws) return;

  if (e_ws->wsbody[0]<MAX_ESTR_LEN) //Ее длина <MAX_ESTR_LEN
  {
    int y=ScreenH()-SoftkeyH()-(GetFontYSIZE(FONT_MEDIUM)+1)*5-5;

    DrawRoundedFrame(1,y,ScreenW()-2,ScreenH()-SoftkeyH()-2,0,0,0,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(7));

    if (i<0) cp=curpos; else cp=2;
    while(i>0)
    {
      if (!cl) break;
      cl=(CLIST *)(cl->next);
      i--;
    }
    i=0;
    do
    {
      int dy=i*(GetFontYSIZE(FONT_MEDIUM)+1)+y;
      if (!cl) break;
      if (i!=cp)
      {
	DrawScrollString(cl->name,3,dy+4,ScreenW()-4,dy+3+GetFontYSIZE(FONT_MEDIUM),1,FONT_MEDIUM,0x80,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(23));
      }
      else
      {
	int icons_size=Get_WS_width(cl->icons,FONT_MEDIUM_BOLD);
	{
	  int i=Get_WS_width(cl->name,FONT_MEDIUM_BOLD);
	  i-=(ScreenW()-7-icons_size);
	  if (i<0)
	  {
	    DisableScroll();
	  }
	  else
	  {
	    if (!max_scroll_disp)
	    {
	      GBS_StartTimerProc(&tmr_scroll,TMR_SECOND,scroll_timer_proc);
	    }
	    max_scroll_disp=i;
	  }
	}
	DrawRoundedFrame(2,dy+2,ScreenW()-3,dy+3+GetFontYSIZE(FONT_MEDIUM_BOLD),0,0,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(3));
	DrawScrollString(cl->name,3,dy+4,ScreenW()-5-icons_size,dy+3+GetFontYSIZE(FONT_MEDIUM_BOLD),scroll_disp+1,FONT_MEDIUM_BOLD,0x80,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
	DrawString(cl->icons,ScreenW()-4-icons_size,dy+4,ScreenW()-5,dy+3+GetFontYSIZE(FONT_MEDIUM_BOLD),FONT_MEDIUM_BOLD,0x80,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
      }
      cl=(CLIST *)cl->next;
      i++;
    }
    while(i<5);
  }
}

void ChangeRC(GUI *gui)
{
#ifdef ELKA
  static const RECT rc={6,80,234,140};
#else
  static const RECT rc={6,40,126,100};
#endif
  if (e_ws)
  {
    if (wslen(e_ws)>=MAX_ESTR_LEN) return;
  }
  if (!gui) return;
  char *p=(char *)gui;
  p+=DISPLACE_OF_EDGUI;
  gui=*((GUI **)p);
  if (!gui) return;
  void **m=GetDataOfItemByID(gui,4);
  if (gui->canvas) memcpy(gui->canvas,&rc,sizeof(rc));
  if (!m) return;
  if (m[0]) memcpy(m[0],&rc,sizeof(rc));
}


const int menusoftkeys[]={0,1,2};

const SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Select"},
  {0x0001,0x0000,(int)"Back"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

const SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

int is_sms_need=0;
WSHDR *ews;
const char *snum;

void edsms_locret(void){}

int edsms_onkey(GUI *data, GUI_MSG *msg)
{
  EDITCONTROL ec;
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    if (msg->gbsmsg->submess==GREEN_BUTTON)
    {
      ExtractEditControl(data,2,&ec);
      WSHDR *sw=AllocWS(ec.pWS->wsbody[0]);
      wstrcpy(sw,ec.pWS);
      SendSMS(sw,snum,0x4209,MSG_SMS_RX-1,6);
      return(1);
    }
  }
  //-1 - do redraw
  return(0); //Do standart keys
  //1: close
}

void edsms_ghook(GUI *data, int cmd)
{
}

HEADER_DESC edsms_hdr={0,0,131,21,NULL,(int)"Write SMS",LGP_NULL};

INPUTDIA_DESC edsms_desc=
{
  1,
  edsms_onkey,
  edsms_ghook,
  (void *)edsms_locret,
  0,
  &menu_skt,
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
  0x40000000 // Поменять местами софт-кнопки
};

void VoiceOrSMS(const char *num)
{
  if (!is_sms_need)
  {
    MakeVoiceCall(num,0x10,0x20C0);
  }
  else
  {
    void *ma=malloc_adr();
    void *eq;
    EDITCONTROL ec;
    snum=num;
    PrepareEditControl(&ec);
    eq=AllocEQueue(ma,mfree_adr());
    wsprintf(ews,"SMS to %s:",num);
    ConstructEditControl(&ec,1,0x40,ews,64);
    AddEditControlToEditQend(eq,&ec,ma);
    //wsprintf(ews,percent_t,"");
    CutWSTR(ews,0);
    ConstructEditControl(&ec,3,0x40,ews,1024);
    AddEditControlToEditQend(eq,&ec,ma);
    patch_header(&edsms_hdr);
    patch_input(&edsms_desc);
    CreateInputTextDialog(&edsms_desc,&edsms_hdr,eq,1,0);
  }
}

void goto_1(void)
{
  VoiceOrSMS(dstr[0]);
}

void goto_2(void)
{
  VoiceOrSMS(dstr[1]);
}

void goto_3(void)
{
  VoiceOrSMS(dstr[2]);
}

void goto_4(void)
{
  VoiceOrSMS(dstr[3]);
}

void *gotomenu_HNDLS[4]=
{
  (void *)goto_1,
  (void *)goto_2,
  (void *)goto_3,
  (void *)goto_4
};

int gotomenu_onkey(GUI *data, GUI_MSG *msg)
{
  if ((msg->gbsmsg->msg==KEY_DOWN)&&(msg->gbsmsg->submess==GREEN_BUTTON))
  {
    msg->keys=0x18;
  }
  return(0);
}

const MENUITEM_DESC gotomenu_ITEMS[9]=
{
  {menu_icons+0,(int)dstr[0],LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {menu_icons+2,(int)dstr[1],LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {menu_icons+4,(int)dstr[2],LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {menu_icons+6,(int)dstr[3],LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};

const HEADER_DESC gotomenu_HDR={0,0,131,21,/*icon*/0,(int)"Select number...",LGP_NULL};

MENU_DESC gotomenu_STRUCT=
{
  8,(void *)gotomenu_onkey,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  1,
  NULL,
  gotomenu_ITEMS,
  gotomenu_HNDLS,
  4
};

int my_ed_onkey(GUI *gui, GUI_MSG *msg)
{
  int key=msg->gbsmsg->submess;
  int m=msg->gbsmsg->msg;
  int r;
  int i=0;

  CLIST *cl=(CLIST *)cltop;
  
  is_sms_need=0;
  if ((key==RIGHT_BUTTON)&&(m==KEY_DOWN))
  {
    EDITCONTROL ec;
    ExtractEditControl(gui,1,&ec);
    if (ec.pWS->wsbody[0]==EDIT_GetCursorPos(gui)-1)
    {
//      ShowMSG(1,(int)"Try to write SMS!");
      is_sms_need=1;
    }
  }
  if (key==GREEN_BUTTON||is_sms_need)
  {
    int to_remove[5];
    int remove=0;
    DisableScroll();
    if (!cl) goto L_OLDKEY;
    while(i!=curpos)
    {
      i++;
      cl=(CLIST *)cl->next;
      if (!cl) goto L_OLDKEY;
    }
    //Теперь cl указывает на вход
    i=0;
    r=0;
    do
    {
      if (cl->num[r]) {ws_2str(cl->num[r],dstr[r],39);i=r;}
      else
      {
	to_remove[++remove]=r;
      }
      r++;
    }
    while(r<4);
    to_remove[0]=remove;
    if (remove==3) //Только один номер
    {
      VoiceOrSMS(dstr[i]);
      return(1); //Закрыть нах
    }
    if (remove==4) goto L_OLDKEY; //Нет вообще телефонов
    //Количество номеров больше 1, рисуем меню
    {
      HEADER_DESC *head=(HEADER_DESC *)&gotomenu_HDR;
      head->rc.x=0;
      head->rc.y=YDISP;
      head->rc.x2=ScreenW()-1;
      head->rc.y2=HeaderH()+YDISP;
    }
    CreateMenu(0,0,&gotomenu_STRUCT,&gotomenu_HDR,0,4,0,to_remove);
    return(0);
  }
  if ((key==UP_BUTTON)||(key==DOWN_BUTTON))
  {
    //Не обрабатываем редактором вверх/вниз
    msg->keys=0;
    if ((m==KEY_DOWN)||(m==LONG_PRESS))
    {
      if (key==UP_BUTTON)
      {
	if (curpos) curpos--;
      }
      if (key==DOWN_BUTTON)
      {
	if (cl)
	{
	  do
	  {
	    cl=(CLIST *)cl->next;
	    if (!cl) break;
	    i++;
	  }
	  while(i<=curpos);
	}
	curpos=i;
      }
    }
    r=-1; //Перерисовать
  }
  else
  {
    #ifdef NEWSGOLD
    if ((key>='0'&&key<='9')||(key=='*')||(key=='#')||(key==RIGHT_SOFT))
    #else
    if ((key>='0'&&key<='9')||(key=='*')||(key=='#')||(key==LEFT_SOFT))
    #endif  
    {
      if (m==KEY_DOWN)
      {
	if (hook_state>=2) //Возможно изменение строки ввода, требуется поиск
	{
	  hook_state=3;
	  FreeCLIST();
	}
      }
    }
  L_OLDKEY:
    r=old_ed_onkey(gui,msg);
  }
  ChangeRC(gui);
  return(r);
}

void my_ed_ghook(GUI *gui, int cmd)
{
  static void *methods[16];
  void **m=GetDataOfItemByID(gui,4);
  if ((hook_state==1)/*&&(cmd==7)*/)
  {
    //Цепляем метод onRedraw
    if (m)
    {
      if (m[1])
      {
	memcpy(methods,m[1],sizeof(methods));
	old_ed_redraw=(void (*)(void *))(methods[0]);
	methods[0]=(void *)my_ed_redraw;
	m[1]=methods;
	hook_state=3; //Запрос поиска
      }
    }
  }
  old_ed_ghook(gui, cmd);
  if (cmd==7)
  {
    EDITCONTROL ec;
    ExtractEditControl(gui,1,&ec);
    //Новая строка поиска
    if ((e_ws=ec.pWS)->wsbody[0]<MAX_ESTR_LEN) //Ее длина <MAX_ESTR_LEN
    {
      if (hook_state==3)
      {
	curpos=0;
	hook_state=4;
	SUBPROC((void *)ConstructList);
      }
    }
  }
  ChangeRC(gui);
}

void DoSplices(GUI *gui)
{
  static INPUTDIA_DESC my_ed;
  memcpy(&my_ed,gui->definition,sizeof(INPUTDIA_DESC));
  old_ed_onkey=my_ed.onKey;
  old_ed_ghook=my_ed.global_hook_proc;
  my_ed.onKey=my_ed_onkey;
  my_ed.global_hook_proc=my_ed_ghook;
  gui->definition=&my_ed;
  scroll_disp=0;
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
#define edialgui_id (((int *)data)[DISPLACE_OF_EDGUI_ID/4])
  int csm_result;

  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==edialgui_id)
    {
      hook_state=0;
      e_ws=0;
      FreeCLIST();
      DisableScroll();
    }
  }
  csm_result=old_icsm_onMessage(data,msg); //Вызываем старый обработчик событий
  if (IsGuiOnTop(edialgui_id)) //Если EDialGui на самом верху
  {
    GUI *igui=GetTopGUI();
    if (igui) //И он существует (а не в проекте ;))
    {
      if (!hook_state)
      {
	//Не было диалога
	DoSplices(igui);
	hook_state=1;
      }
    }
  }
  return(csm_result);
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  FreeWS(ews);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;
  icsmd.onMessage=MyIDLECSM_onMessage;
  icsm->constr=&icsmd;
  UnlockSched();
  ews=AllocWS(1024);
  InitIcons();
  return 0;
}
