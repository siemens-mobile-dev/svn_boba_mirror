#include "../inc/swilib.h"
#include "main.h"
#include "clist_util.h"
#include "history.h"
#include "message_list.h"
#include "jabber_util.h"
#include "string_util.h"

char MsgList_Quit_Required = 0;

TRESOURCE* Resource_Ex = NULL;


int Message_gui_ID;


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


WSHDR* ws_eddata = NULL;
int Terminate=0;
//---------------------------------------------------------------------------
// Test edit dialog
//---------------------------------------------------------------------------
int inp_onkey(GUI *gui, GUI_MSG *msg)
{

  if(msg->gbsmsg->submess==GREEN_BUTTON)
  {
    Terminate = 1;
    return 1;
  }

  if (msg->keys==0x18)
  {
    return(-1); //do redraw
  }
  return(0); //Do standart keys

}

void inp_redraw(void *data)
{
}

void inp_ghook(GUI *gui, int cmd)
{
  static SOFTKEY_DESC sk={0x0018, 0x0000,(int)"Меню"};
  EDITCONTROL ec;
  if (cmd==2)
  {
    //Called after onCreate
  }
  if (cmd==7)
  {
//    curpos=EDIT_CURSOR_POS(gui);
    SetSoftKey(gui,&sk,0);    
    ExtractEditControl(gui,1,&ec);    
    wstrcpy(ws_eddata,ec.pWS);
  }
 
  if(Terminate) 
 {
   char* body=malloc(MAX_MSG_LEN*2);  // UTF
   zeromem(body, MAX_MSG_LEN*2);    
   ExtractEditControl(gui,1,&ec);    
   wstrcpy(ws_eddata,ec.pWS);
   ws_2str(ws_eddata, body, MAX_MSG_LEN); // WS из эдитконтрола => UTF-8
   //char m[100];
   //sprintf(m,"WL=%d, L=%d", wstrlen(ws_eddata), strlen(body));
   //ShowMSG(1,(int)m);
   char* hist = convUTF8_to_ANSI_STR(body);
   char* real_hist = *hist == 0x1F ? hist+1 : hist; 
   CList_AddMessage(Resource_Ex->full_name, MSG_ME, real_hist);
   mfree(hist);

   SUBPROC((void*)SendMessage,Resource_Ex->full_name, body);
   Terminate = 0;
 }
}

void inp_locret(void){}

SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

INPUTDIA_DESC inp_desc=
{
  1,
  inp_onkey,
  inp_ghook,
  (void *)inp_locret,
  0,
  &menu_skt,
  {0,22,131,153},
  4,
  100,
  101,
  0,
  0,
  0x40000000
};

HEADER_DESC inp_hdr={0,0,131,21,NULL,(int)"Новое...",0x7FFFFFFF};



// Открыть окно написания нового сообщения
void Init_Message(TRESOURCE* ContEx)
{
  Resource_Ex = ContEx;
  patch_header(&inp_hdr);
  patch_input(&inp_desc);
  ws_eddata = AllocWS(MAX_MSG_LEN);
  EDITCONTROL ec;
  void *ma=malloc_adr();
  void *eq;
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  ConstructEditControl(&ec,3,0x40,ws_eddata,MAX_MSG_LEN);
  AddEditControlToEditQend(eq,&ec,ma);
  CreateInputTextDialog(&inp_desc,&inp_hdr,eq,1,/*ws_eddata*/0);  
}

// Корень списка
DISP_MESSAGE* MessagesList = NULL;
unsigned int MessList_Count = 0;

// Убить список сообщений
void KillDisp(DISP_MESSAGE* messtop)
{
  DISP_MESSAGE* cl=messtop;
  LockSched();
  messtop = NULL;
  while(cl)
  {
    DISP_MESSAGE *p;
    if(cl->mess)mfree(cl->mess);
    p=cl;
    cl=cl->next;
    mfree(p);
  }
  MessList_Count = 0;
  UnlockSched();  
}

//===============================================================================================

char CurrentPage=1;
char lines_on_page;
char MaxPages=1;
// Обслуживание созданного GUI
void mGUI_onRedraw(GUI *data)
{
  
  KillDisp(MessagesList);
  ParseMessagesIntoList(Resource_Ex);
  
  Terminate = 0;
  // Расчёт количества строк на одной странице
  unsigned short FontSize = GetFontYSIZE(SMALL_FONT);
  lines_on_page = sdiv(FontSize, ScreenH() - HIST_DISP_OFS);
  MaxPages = sdiv(lines_on_page,MessList_Count);
  if(lines_on_page*MaxPages<MessList_Count)MaxPages++;
  // Заголовок окна
  DrawRoundedFrame(0,0,ScreenW()-1,FontSize*2+1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(MESSAGEWIN_TITLE_BGCOLOR));
  
  DrawRoundedFrame(0,FontSize+2,ScreenW()-1,ScreenH()-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(MESSAGEWIN_BGCOLOR));

  // Делаем типо название окошка... :)
  WSHDR* ws_title = AllocWS(256);
  //str_2ws(ws_title, Resource_Ex->full_name,strlen(Resource_Ex->full_name));
  ascii2ws(ws_title, Resource_Ex->full_name);

  DrawString(ws_title,1,1,ScreenW()-1,FontSize+1,SMALL_FONT,0,GetPaletteAdrByColorIndex(MESSAGEWIN_TITLE_FONT),GetPaletteAdrByColorIndex(23));  
  
  DISP_MESSAGE* ml = MessagesList;
  int i_ctrl=0;
  int i = 0;
  char MsgBgClolor;
  while(ml)
  {
    if((i_ctrl>=(CurrentPage-1)*lines_on_page) && (i_ctrl<CurrentPage*lines_on_page))
    {
      //str_2ws(ws_title,ml->mess,strlen(ml->mess));
      
      //str_2ws(ws_title,ml->mess,CHAR_ON_LINE);
      switch(ml->mtype)
      {
      case MSG_ME:{MsgBgClolor=MESSAGEWIN_MY_BGCOLOR;break;}        
      case MSG_CHAT:{MsgBgClolor=MESSAGEWIN_CH_BGCOLOR;break;}              
      case MSG_SYSTEM:{MsgBgClolor=MESSAGEWIN_SYS_BGCOLOR;break;}                    
      case MSG_STATUS:{MsgBgClolor=MESSAGEWIN_STATUS_BGCOLOR;break;}                    
      }
      DrawRoundedFrame(0,HIST_DISP_OFS+i*FontSize,ScreenW()-1,HIST_DISP_OFS+(i+1)*FontSize,0,0,0,
		   GetPaletteAdrByColorIndex(MsgBgClolor),
		   GetPaletteAdrByColorIndex(MsgBgClolor));
    
      //DrawString(ws_title,1,20+i*FontSize,ScreenW()-1,20+(i+1)*FontSize,SMALL_FONT,0,GetPaletteAdrByColorIndex(MESSAGEWIN_TITLE_FONT),GetPaletteAdrByColorIndex(23));      
      DrawString(ml->mess,1,20+i*FontSize,ScreenW()-1,20+(i+1)*FontSize,SMALL_FONT,0,GetPaletteAdrByColorIndex(MESSAGEWIN_TITLE_FONT),GetPaletteAdrByColorIndex(23));      
      i++;
    }
    ml = ml->next;
    i_ctrl++;
  }  
  FreeWS(ws_title);
  Resource_Ex->has_unread_msg =0; // Непрочитанных сообщений больше нет
}

void mGUI_onCreate(GUI *data, void *(*malloc_adr)(int))
{
  data->state=1;
  MsgList_Quit_Required=0;
}

void mGUI_onClose(GUI *data, void (*mfree_adr)(void *))
{
  KillDisp(MessagesList);
  if(ws_eddata)
  {
    FreeWS(ws_eddata);
    ws_eddata = NULL;
  }  
  data->state=0;
}

void mGUI_onFocus(GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  data->state=2;
}

void mGUI_onUnfocus(GUI *data, void (*mfree_adr)(void *))
{
  if (data->state!=2) return;
  data->state=1;
}

void DbgInfo()
{
  char q[200];
  sprintf(q,"MCnt=%d; P=%d, MP=%d; LP=%d",MessList_Count,CurrentPage, MaxPages,lines_on_page);
  ShowMSG(1,(int)q);
}

int mGUI_onKey(GUI *data, GUI_MSG *msg)
{
  if(MsgList_Quit_Required)return 1; //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI

  //DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
      case RIGHT_SOFT:
      {
        return 1;
      }
      
    case LEFT_SOFT:
      {
        DbgInfo();
        break;
      }
      
      case UP_BUTTON:
        {
          if(CurrentPage>1)CurrentPage--;
          REDRAW();
          break;         
        }
    case DOWN_BUTTON:
      {
          if(CurrentPage<MaxPages)CurrentPage++;
          REDRAW();
          break;         
      }
    case GREEN_BUTTON:
      {
        Init_Message(Resource_Ex);
        break;
      }
    }    
  }
  return(0);
}


int mGUI_onDestroy(void){return(0);}

// ГУЙ
int mGUI_method8(void){return(0);}

int mGUI_method9(void){return(0);}

const void * const mgui_methods[11]={
  (void *)mGUI_onRedraw,	//Redraw
  (void *)mGUI_onCreate,	//Create
  (void *)mGUI_onClose,	//Close
  (void *)mGUI_onFocus,	//Focus
  (void *)mGUI_onUnfocus,	//Unfocus
  (void *)mGUI_onKey,	//OnKey
  0,
  (void *)mGUI_onDestroy, //onDestroy,	//Destroy
  (void *)mGUI_method8,
  (void *)mGUI_method9,
  0
};
////////////////////////////////////////////////////////////////////////////////
#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}


const RECT mCanvas={0,0,0,0};

void ParseMessagesIntoList(TRESOURCE* ContEx)
{
  MessList_Count = 0;
  MessagesList = NULL;
  MaxPages=1;
  if(!ContEx)return;

  LOG_MESSAGE* MessEx= ContEx->log;
  int cnt=0;
  int chars;
  DISP_MESSAGE* Disp_Mess_Ex, *tmp;  
  if(!MessEx)return;
  LockSched();

  char* msg_buf = malloc(CHAR_ON_LINE+1);

  // Цикл по всем сообщениям
  while(MessEx)
  {
    int l=strlen(MessEx->mess);
    cnt=0;
    while(l>0)
    {
      Disp_Mess_Ex = malloc(sizeof(DISP_MESSAGE));
      chars = l>CHAR_ON_LINE ? CHAR_ON_LINE : l;
      Disp_Mess_Ex->mess = AllocWS(chars);
      //Disp_Mess_Ex->mess = malloc(chars+1);
      //zeromem(Disp_Mess_Ex->mess, chars+1);
      //strncpy(Disp_Mess_Ex->mess, MessEx->mess + cnt*CHAR_ON_LINE, chars);
      strncpy(msg_buf, MessEx->mess + cnt*CHAR_ON_LINE, chars);
      ascii2ws(Disp_Mess_Ex->mess, msg_buf);
      Disp_Mess_Ex->mtype = MessEx->mtype;
      if(!MessagesList){MessagesList =Disp_Mess_Ex;tmp=Disp_Mess_Ex;tmp->next=NULL;}
      else
      {
        tmp->next = Disp_Mess_Ex;
        tmp = Disp_Mess_Ex;
        tmp->next=NULL;
      }
      l-=CHAR_ON_LINE;
      MessList_Count++;
      cnt++;
    }
    MessEx = MessEx->next;
  }
  mfree(msg_buf);
  UnlockSched();
}


// Отобразить список сообщений
void Display_Message_List(TRESOURCE* ContEx)
{
  if(!ContEx)return;
  Resource_Ex = ContEx;
  ParseMessagesIntoList(Resource_Ex);
  CurrentPage=1;
  GUI *mess_gui=malloc(sizeof(GUI));
  zeromem(mess_gui, sizeof(GUI));
  patch_rect((RECT*)&mCanvas,0,0,ScreenW()-1,ScreenH()-1);
  mess_gui->canvas=(void *)(&mCanvas);
  mess_gui->flag30=2;
  mess_gui->methods=(void *)mgui_methods;
  mess_gui->item_ll.data_mfree=(void (*)(void *))mfree_adr();
  Message_gui_ID = CreateGUI(mess_gui);
}
