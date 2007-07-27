#include "../inc/swilib.h"
#include "main.h"
#include "clist_util.h"
#include "history.h"
#include "message_list.h"
#include "jabber_util.h"
#include "string_util.h"
#include "serial_dbg.h"


#define MSG_START_X 1    //X-���������� ������ ��������� ������ ���������


//-------------�����. ����� ������ :)

RGBA MESSAGEWIN_BGCOLOR =         {255, 255, 255, 100}; // ����� ���
RGBA MESSAGEWIN_TITLE_BGCOLOR =   {  0,   0, 255, 100}; // ��� ���������
RGBA MESSAGEWIN_TITLE_FONT =      {255, 255, 255, 100}; // ���� ������ ���������
RGBA MESSAGEWIN_MY_BGCOLOR =      {200, 215, 255, 100}; // ���� ���� ��������� ���������
RGBA MESSAGEWIN_CH_BGCOLOR =      {233, 233, 233, 100}; // ���� ���� ��������� ���������
RGBA MESSAGEWIN_CURSOR_BGCOLOR =  {255, 255,   0, 100}; // ���� ���� �������
RGBA MESSAGEWIN_GCHAT_BGCOLOR_1 = {255, 255, 255, 100}; // �����������: ���� ���� ��������� ����������� 1
RGBA MESSAGEWIN_GCHAT_BGCOLOR_2 = {233, 233, 233, 100}; // �����������: ���� ���� ��������� ����������� 2
RGBA MESSAGEWIN_SYS_BGCOLOR =     {115, 170, 240, 100}; // ���� ���� ��������� �����������
RGBA MESSAGEWIN_STATUS_BGCOLOR =  {155, 255, 180, 100}; // ���� ���� ��������� ����� �������
RGBA MESSAGEWIN_CHAT_FONT =       {  0,   0,   0, 100}; // ���� ������ ���������

#define MESSAGEWIN_FONT FONT_SMALL



//------------------


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
int edmessage_id;
void menu_enter(USR_MENU_ITEM *item)
{
  if (item->type==0)
  {
    switch(item->cur_item)
    {
    case 0:
      wsprintf(item->ws,"%t","������� ������");
      break;
    }
  }
  if (item->type==1)
  {
    switch(item->cur_item)
    {
    case 0:
      GeneralFunc_flag1(edmessage_id, 1);
      break;
    }
  }   
}

EDITCONTROL ec_message;
GUI *ed_message_gui;

void DispSelectMenu();

int inp_onkey(GUI *gui, GUI_MSG *msg)
{
  if(msg->gbsmsg->submess==GREEN_BUTTON)
  {
    Terminate = 1;
    extern const char sndMsgSend[];
    Play(sndMsgSend);
    return 1;
  }

  if (msg->keys==0x18)
  {
    DispSelectMenu();
  }
  
#ifndef NEWSGOLD
  if (msg->keys==0x0FF0) //����� ���� �����
  {
    return(1);
  }
#endif

#ifdef NEWSGOLD
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    if (msg->gbsmsg->submess==ENTER_BUTTON)
    {
      EDIT_OpenOptionMenuWithUserItems(gui,menu_enter,gui,1); 
      return (-1);      
    }
  }
#endif  
  
  return(0); //Do standart keys

}

void inp_redraw(void *data)
{
}

char Mess_was_sent = 0;

void inp_ghook(GUI *gui, int cmd)
{
  static SOFTKEY_DESC sk={0x0018, 0x0000,(int)"����"};
#ifndef NEWSGOLD
  static const SOFTKEY_DESC sk_cancel={0x0FF0,0x0000,(int)"�������"};
#endif
  EDITCONTROL ec;
  if (cmd==2)
  {
    //Called after onCreate
  }
  if (cmd==7)
  {
    ExtractEditControl(gui,1,&ec);
    wstrcpy(ws_eddata,ec.pWS);    
#ifdef NEWSGOLD
    SetSoftKey(gui,&sk,0);
#else
   SetSoftKey(gui,&sk,1);
   if (ec.pWS->wsbody[0]==0)
      SetSoftKey(gui,&sk_cancel,SET_SOFT_KEY_N==0?1:0);   
#endif
  }

  if(cmd==0xA)
  {
    DisableIDLETMR();   // ��������� ������ ������ �� ��������
  }

  if(Terminate)
  {
    ExtractEditControl(gui,1,&ec);
    wstrcpy(ws_eddata,ec.pWS);
    //size_t xz = wstrlen(ws_eddata)*2;
    if(wstrlen(ws_eddata))
    {
      //char* body =  utf16_to_utf8((char**)ws_eddata,&xz);
      //body[xz]='\0';
      int res_len;
      char* body = malloc(wstrlen(ws_eddata)*2+1);
      ws_2utf8(ws_eddata, body, &res_len, wstrlen(ws_eddata)*2+1);
      body = realloc(body, res_len+1);
      body[res_len]='\0';
      char is_gchat = Resource_Ex->entry_type== T_CONF_ROOT ? 1: 0;
      char part_str[]="/part";

      if(!is_gchat)
      {
        CList_AddMessage(Resource_Ex->full_name, MSG_ME, body);
      }
      else
        if(strstr(body, part_str)==body)  // ���� � ������
        {
          CLIST* room=CList_FindContactByJID(CList_GetActiveContact()->full_name);
          Leave_Conference(room->JID);
          CList_MakeAllResourcesOFFLINE(room);
          Terminate = 0;
          Mess_was_sent = 1;
          mfree(body);
          return;
        }
      IPC_MESSAGE_S *mess = malloc(sizeof(IPC_MESSAGE_S));
      mess->IsGroupChat = is_gchat;
      mess->body = Mask_Special_Syms(body);
      mfree(body);
      SUBPROC((void*)SendMessage,Resource_Ex->full_name, mess);
      REDRAW();
      Mess_was_sent = 1;
    }
    else MsgBoxError(1,(int)"������ ������� ������ ���������");
    Terminate = 0;
  }

  if(cmd==0x03)     // onDestroy
  {
    FreeWS(ws_eddata);
    ws_eddata = NULL;
    //Send composing CANCELATION
    if(!Mess_was_sent)SUBPROC((void*)CancelComposing,Resource_Ex->full_name);
    Mess_was_sent = 0;
  }
}

void inp_locret(void){}

SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"�����"},
  {0x0001,0x0000,(int)"�����"},
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

HEADER_DESC inp_hdr={0,0,0,0,NULL,(int)"�����...",LGP_NULL};



// ������� ���� ��������� ������ ���������

void Init_Message(TRESOURCE* ContEx, char *init_text)
{
  Resource_Ex = ContEx;

  //Send composing
  SUBPROC((void*)SendComposing,Resource_Ex->full_name);

  patch_header(&inp_hdr);
  patch_input(&inp_desc);
  ws_eddata = AllocWS(MAX_MSG_LEN);
  if(init_text)
  {
    utf8_2ws(ws_eddata, init_text, MAX_MSG_LEN);
  }
  EDITCONTROL ec;
  void *ma=malloc_adr();
  void *eq;
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  ConstructEditControl(&ec,3,0x40,ws_eddata,MAX_MSG_LEN);
  AddEditControlToEditQend(eq,&ec,ma);
  edmessage_id=CreateInputTextDialog(&inp_desc,&inp_hdr,eq,1,0);
}
////////////////////////////////////////////////////////////////////////////////
DISP_MESSAGE* MessagesList = NULL;      // ������ ������
unsigned int DispMessList_Count = 0;        // ���������� ���������
unsigned int OLD_MessList_Count = 0;    // ����������� ���������� ���������
unsigned int Cursor_Pos = 0;            // ������� ������� � ������


// ����� ������ ���������
void KillDisp(DISP_MESSAGE* messtop)
{
  DISP_MESSAGE* cl=messtop;
  LockSched();
  messtop = NULL;
  int cnt=0;
  while(cl)
  {
    DISP_MESSAGE *p;
    if(cl->mess)FreeWS(cl->mess);
    p=cl;
    cl=cl->next;
    mfree(p);
    cnt++;
  }
  DispMessList_Count = 0;
  OLD_MessList_Count = 0;
  UnlockSched();
}

//===============================================================================================

int CurrentPage=1;
int CurrentMessage = 0;
int CurrentMessage_Lines = 0;
int lines_on_page;
int MaxPages=1;
unsigned short FontSize;
// ������������ ���������� GUI

// ������ ����������� ������� :)
void Calc_Pages_Data()
{
  MaxPages = sdiv(lines_on_page,DispMessList_Count);
  if(lines_on_page*MaxPages<DispMessList_Count)MaxPages++;
}

// ������ ����������� ������� :)
void Calc_Pages_Data_1()
{
  if((Cursor_Pos-1-(CurrentMessage_Lines)<=(CurrentPage-1)*lines_on_page) && CurrentPage>1)
  {
    CurrentPage--;
    //Cursor_Pos--;
    //ShowMSG(1,(int)"Q");
    return;
  }
  else
  {
    //char q[20];
    //sprintf(q,"Cur=%d > %d", Cursor_Pos-1-(CurrentMessage_Lines), (CurrentPage-1)*lines_on_page);
    //ShowMSG(1,(int)q);
  }
}


void Calc_Pages_Data_2()
{
  if(Cursor_Pos+1>CurrentPage*lines_on_page)
  {
    CurrentPage++;
    //Cursor_Pos++;
  }
}

void mGUI_onRedraw(GUI *data)
{

  if(Resource_Ex->total_msg_count!=OLD_MessList_Count)
  {
    //KillDisp(MessagesList);
    ParseMessagesIntoList(Resource_Ex);
    OLD_MessList_Count = Resource_Ex->total_msg_count;
  }

  Terminate = 0;
  // ������ ���������� ����� �� ����� ��������

  Calc_Pages_Data();
  // ��������� ����
  DrawRoundedFrame(0,SCR_START,ScreenW()-1,SCR_START+FontSize*2+1,0,0,0,
                   0,
                   color(MESSAGEWIN_TITLE_BGCOLOR));

  DrawRoundedFrame(0,SCR_START+FontSize+2,ScreenW()-1,ScreenH()-1,0,0,0,
                   0,
                   color(MESSAGEWIN_BGCOLOR));

  // ������ ���� �������� ������... :)
  WSHDR* ws_title = AllocWS(256);
  utf8_2ws(ws_title, Resource_Ex->full_name, 256);

  DrawString(ws_title,1,SCR_START+1,ScreenW()-1,SCR_START+FontSize+1,MESSAGEWIN_FONT,0,color(MESSAGEWIN_TITLE_FONT),0);

  DISP_MESSAGE* ml = MessagesList;
  int i_ctrl=1;

  int i = 0;
  RGBA MsgBgColor;
  CurrentMessage_Lines = 0;
  while(ml)
  {
    if((i_ctrl>(CurrentPage-1)*lines_on_page) && (i_ctrl<=CurrentPage*lines_on_page))
    {

      switch(ml->mtype)
      {
      case MSG_ME:      {MsgBgColor=MESSAGEWIN_MY_BGCOLOR;break;}
      case MSG_CHAT:    {MsgBgColor=MESSAGEWIN_CH_BGCOLOR;break;}
      case MSG_SYSTEM:  {MsgBgColor=MESSAGEWIN_SYS_BGCOLOR;break;}
      case MSG_STATUS:  {MsgBgColor=MESSAGEWIN_STATUS_BGCOLOR;break;}
      case MSG_GCHAT:
        {
          MsgBgColor=ml->log_mess_number %2==0? MESSAGEWIN_GCHAT_BGCOLOR_1 : MESSAGEWIN_GCHAT_BGCOLOR_2;
          break;
        }
      }
      if(CurrentMessage==ml->log_mess_number)
      {
        Cursor_Pos=i_ctrl;
      }
      if(Cursor_Pos==i_ctrl)
      {
        MsgBgColor = MESSAGEWIN_CURSOR_BGCOLOR;
        DISP_MESSAGE *mln = ml->next;
        if(mln)
          if(CurrentMessage==mln->log_mess_number) Cursor_Pos++;                    // ��������� ������� �������
        CurrentMessage_Lines++;
      }

      DrawRoundedFrame(0,SCR_START+FontSize+2+i*FontSize,ScreenW()-1,SCR_START+FontSize+2+(i+1)*FontSize,0,0,0,
                       color(MsgBgColor),
                       color(MsgBgColor));

      DrawString(ml->mess,MSG_START_X,SCR_START+FontSize+2+i*FontSize,ScreenW()-1,SCR_START+FontSize+2+(i+1)*FontSize*2,MESSAGEWIN_FONT,0,color(MESSAGEWIN_CHAT_FONT),0);
      i++;
    }
    ml = ml->next;
    i_ctrl++;
  }
  FreeWS(ws_title);
  Resource_Ex->has_unread_msg =0; // ������������� ��������� ������ ���

}

void mGUI_onCreate(GUI *data, void *(*malloc_adr)(int))
{
  data->state=1;
  MsgList_Quit_Required=0;
}

void mGUI_onClose(GUI *data, void (*mfree_adr)(void *))
{
  KillDisp(MessagesList);
  data->state=0;
}

void mGUI_onFocus(GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();   // ��������� ������ ������ �� ��������
  data->state=2;
}

void mGUI_onUnfocus(GUI *data, void (*mfree_adr)(void *))
{
  if (data->state!=2) return;
  data->state=1;
}

LOG_MESSAGE *GetCurMessage()
{
  unsigned int i=0;
  LOG_MESSAGE* log =Resource_Ex->log;
  while(log)
  {
    i++;
    if(i==CurrentMessage)break;
    log = log->next;
  }
  return log;
}

void DbgInfo()
{
  char q[200];
  sprintf(q,"Messlist_cnt=%d\nCurMessage=%d\nLines=%d\nCursor=%d, CurPage=%d",DispMessList_Count, CurrentMessage, CurrentMessage_Lines,Cursor_Pos, CurrentPage);
  ShowMSG(2,(int)q);
}

int mGUI_onKey(GUI *data, GUI_MSG *msg)
{
  if(MsgList_Quit_Required)return 1; //���������� ����� GeneralFunc ��� ���. GUI -> �������� GUI

  //DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case '0':
      {
        // ����� ������ ���������
        //KillDisp(MessagesList);
        KillMsgList(Resource_Ex->log);
        Resource_Ex->log = NULL;
        Resource_Ex->has_unread_msg=0;
        Resource_Ex->total_msg_count=0;
        return 1;
      }

#ifndef NEWSGOLD
    case RED_BUTTON:
#endif
    case RIGHT_SOFT:
      {
        return 1;
      }

    case LEFT_SOFT:
      {
        DbgInfo();
        break;
      }

    case ENTER_BUTTON:
      {
        LOG_MESSAGE* log =GetCurMessage();
        if(log)
        {
          unsigned int l = strlen(log->mess);
          char *init_text = malloc(l+3+1);
          zeromem(init_text,l+3+1);
          init_text[0] = '>';
          init_text[1] = '>';
          strcat(init_text, log->mess);
          init_text[2+ l] = '\n';
          init_text[3+ l] = '\0';
          Init_Message(Resource_Ex, init_text);
          mfree(init_text);
        }
        break;
      }
    case '2':
    case UP_BUTTON:
      {
        Calc_Pages_Data_1();
        CurrentMessage_Lines = 0;
        if(Cursor_Pos>1)Cursor_Pos--;
        if(CurrentMessage>1)CurrentMessage--;
        REDRAW();
        break;
      }
    case '8':
    case DOWN_BUTTON:
      {
        CurrentMessage_Lines = 0;
        if(Cursor_Pos<DispMessList_Count)Cursor_Pos++;
        if(CurrentMessage<Resource_Ex->total_msg_count)CurrentMessage++;
        Calc_Pages_Data_2();
        REDRAW();
        break;
      }
    case '1'://� ������ ������ ���������
      {
        CurrentMessage_Lines = 0;
        Cursor_Pos=1;
        CurrentMessage=1;
        CurrentPage=1;
        REDRAW();
        break;
      }
    case '9'://� ����� ������ ���������
      {
        CurrentMessage_Lines = 0;
        Cursor_Pos=DispMessList_Count;
        CurrentMessage=Resource_Ex->total_msg_count;
        CurrentPage=MaxPages;
        REDRAW();
        break;
      }  
   /* case VOL_UP_BUTTON://�������� �����
      {
        if (CurrentPage>1) 
        {
          int cp=CurrentPage;
          for(;;)
          {
            CurrentMessage_Lines = 0;
            Cursor_Pos--;
            CurrentMessage--;
            Calc_Pages_Data_1();
            if (CurrentPage+1==cp)
            {
              Cursor_Pos--;
              CurrentMessage--;             
              break;            
            }
          }

          //CurrentPage--;
          //Cursor_Pos-=lines_on_page;
          //CurrentMessage-=lines_on_page;
        }
        REDRAW();
        break;
      }
    case VOL_DOWN_BUTTON://�������� ����
      {
        if (CurrentPage<MaxPages) 
        {
          int cp=CurrentPage;
          for(;;)
          {
            CurrentMessage_Lines = 0;
            Cursor_Pos++;
            CurrentMessage++;
            Calc_Pages_Data_2();
            if (CurrentPage-1==cp) break;
          }
          
          //CurrentPage++;
          //Cursor_Pos+=lines_on_page;
          //CurrentMessage+=lines_on_page;
        }
        REDRAW();
        break;
      } */     
    case RIGHT_BUTTON:
      {
        LOG_MESSAGE *msg = GetCurMessage();
        if(msg)
          if(msg->mtype==MSG_GCHAT)
          {
            unsigned int au_nick_len = strlen(msg->muc_author);
            char *init_text = malloc(au_nick_len+3);
            zeromem(init_text, au_nick_len+3);
            strcpy(init_text, msg->muc_author);
            init_text[au_nick_len]=':';
            init_text[au_nick_len+1]=' ';
            init_text[au_nick_len+2]='\0';
            Init_Message(Resource_Ex, init_text);
            mfree(init_text);
          }
        break;
      }

    case GREEN_BUTTON:
      {
        Init_Message(Resource_Ex, NULL);
        break;
      }
    }
  }
  if (msg->gbsmsg->msg==LONG_PRESS)
  {
    switch (msg->gbsmsg->submess)
    {
    case '2':
    case UP_BUTTON:
      {
        Calc_Pages_Data_1();
        CurrentMessage_Lines = 0;
        if(Cursor_Pos>1)Cursor_Pos--;
        if(CurrentMessage>1)CurrentMessage--;
        REDRAW();
        break;
      }
    case '8':
    case DOWN_BUTTON:
      {
        CurrentMessage_Lines = 0;
        if(Cursor_Pos<DispMessList_Count)Cursor_Pos++;
        if(CurrentMessage<Resource_Ex->total_msg_count)CurrentMessage++;
        Calc_Pages_Data_2();
        REDRAW();
        break;
      }
    }
  }
  return(0);
}


extern void kill_data(void *p, void (*func_p)(void *));

// ���
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
  (void *)kill_data, //onDestroy,	//Destroy
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

// �������� ��������� ��������� DISP_MESSAGE ��� ����������� �� ������
void ParseMessagesIntoList(TRESOURCE* ContEx)
{
  if(!ContEx)return;
  int parsed_counter = 0; // ������� ��� ���� ���������� (=OLD_MessList_Count)
  LOG_MESSAGE* MessEx= ContEx->log;
  int cnt=0;
  int Scr_width = ScreenW() - 2 -1;    // � ����� ������� ������� ������
  int Curr_width=0;
  int sym_width;
  char IsCaret = 0; // �������� �� ������ ��������� ������
  //  int chars;
  DISP_MESSAGE* Disp_Mess_Ex, *tmp;
  if(!MessEx)return;
  LockSched();

  WSHDR *temp_ws_1=NULL;
  WSHDR *temp_ws_2=NULL;

  // ���� �� ���� ����������
  while(MessEx)
  {

    if(parsed_counter>=OLD_MessList_Count)
    {
      temp_ws_1 = AllocWS(strlen(MessEx->mess)*2);
      utf8_2ws(temp_ws_1, MessEx->mess, strlen(MessEx->mess)*2);
      //temp_ws_2 = AllocWS(CHAR_ON_LINE*2); WTF?
      temp_ws_2 = AllocWS(200); //����, ��� �����
      int l=wstrlen(temp_ws_1);

      //char q[40];
      //sprintf(q,"UTF_len=%d, WSTR_len=%d", strlen(MessEx->mess),l);
      //ShowMSG(2,(int)q);

      int i=0;
      unsigned short *wschar;
      unsigned short symb;
      cnt=0;
      for(i=1;i<=l;i++)
      {
        wschar = temp_ws_1->wsbody+i;
        symb = *wschar;
        IsCaret = symb==0x000A || symb==0x000D || symb==0x00A0 ? 1 : 0;
        sym_width = GetSymbolWidth(symb,MESSAGEWIN_FONT);
        if(!IsCaret && symb!=0x0 && (/*cnt<CHAR_ON_LINE ||*/ Curr_width + sym_width <= Scr_width))
        {
          Curr_width+=sym_width;
          wsAppendChar(temp_ws_2, symb);
          cnt++;
        }
        if(IsCaret || (Curr_width + sym_width>Scr_width) || i==l) // ������� ������
        {
          Disp_Mess_Ex = malloc(sizeof(DISP_MESSAGE));
          Disp_Mess_Ex->mess = AllocWS(cnt);
          wstrcpy(Disp_Mess_Ex->mess, temp_ws_2);
          CutWSTR(temp_ws_2, 0);
          Disp_Mess_Ex->mtype = MessEx->mtype;
          Disp_Mess_Ex->log_mess_number=parsed_counter+1;
          if(!MessagesList){MessagesList =Disp_Mess_Ex;Disp_Mess_Ex->next=NULL;}
          else
          {
            tmp= MessagesList;
            while(tmp->next)
            {
              tmp = tmp->next;
            }
            tmp->next = Disp_Mess_Ex;
            Disp_Mess_Ex->next=NULL;
          }
          cnt=0;
          DispMessList_Count++;
          Curr_width = 0;
        }
      }
      FreeWS(temp_ws_1);
      FreeWS(temp_ws_2);
    }
    MessEx = MessEx->next;
    parsed_counter++;
  }
  UnlockSched();
}


// ���������� ������ ���������
void Display_Message_List(TRESOURCE* ContEx)
{
  if(!ContEx)return;
  // �������������
  OLD_MessList_Count = 0;
  MessagesList = NULL;
  DispMessList_Count = 0;
  Resource_Ex = ContEx;
  FontSize = GetFontYSIZE(MESSAGEWIN_FONT);
  lines_on_page = sdiv(FontSize, ScreenH() - HIST_DISP_OFS);
  ParseMessagesIntoList(Resource_Ex);
  Calc_Pages_Data();
  CurrentPage = MaxPages;
  OLD_MessList_Count = Resource_Ex->total_msg_count;
  Cursor_Pos = DispMessList_Count;
  CurrentMessage = OLD_MessList_Count;

  // ������� ���
  GUI *mess_gui=malloc(sizeof(GUI));
  zeromem(mess_gui, sizeof(GUI));
  patch_rect((RECT*)&mCanvas,0,0,ScreenW()-1,ScreenH()-1);
  mess_gui->canvas=(void *)(&mCanvas);
  mess_gui->flag30=2;
  mess_gui->methods=(void *)mgui_methods;
  mess_gui->item_ll.data_mfree=(void (*)(void *))mfree_adr();
  Message_gui_ID = CreateGUI(mess_gui);
}

//=================================== ������� ===========================================

int Commands_Menu_ID;
int Select_Menu_ID;

int Mode;
//0 - �������
//1 - ������� ���������
//2 - ������

char **commands_lines; //������ ���������� �� ������
int cmd_num=0;

void FreeCommands(void)
{
  if (commands_lines) mfree(commands_lines);
  commands_lines=NULL;
}

extern const char COMMANDS_PATH[];
extern const char MESSAGES_PATH[];

int LoadCommands(void)
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
  FreeCommands();
  
  switch (Mode)
  {
    case 0:
      strcpy(fn,COMMANDS_PATH);
    break;
    case 1:
      strcpy(fn,MESSAGES_PATH);
    break;
    case 2:
    break;    
  }
  //strcpy(fn,COMMANDS_PATH);
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
void UpdateCommandsMenu_header(void)
{
  switch (Mode)
  {
    case 0:
      strcpy(clm_hdr_text,"����� �������...");
    break;
    case 1:
      strcpy(clm_hdr_text,"����� �������...");
    break;
    case 2:
      strcpy(clm_hdr_text,"����� ������...");
    break;    
  }
}

HEADER_DESC cmd_menuhdr={0,0,131,21,NULL,(int)clm_hdr_text,LGP_NULL};

int cmd_menusoftkeys[]={0,1,2};

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
  FreeCommands();
  GeneralFunc_flag1(Commands_Menu_ID,1);
}

SOFTKEY_DESC cmd_menu_sk[]=
{
  {0x0018,0x0000,(int)"�����"},
  {0x0001,0x0000,(int)"������"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB cmd_menu_skt=
{
  cmd_menu_sk,3
};

void cmd_menuitemhandler(void *data, int curitem, void *unk)
{
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  extern const char percent_t[];
  ws=AllocMenuWS(data,strlen(commands_lines[curitem]));
  //wsprintf(ws,percent_t,commands_lines[curitem]);
  ascii2ws(ws,commands_lines[curitem]);
  SetMenuItemText(data, item, ws, curitem);
}

void cmd_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

static int cmd_menu_keyhook(void *data, GUI_MSG *msg)
{
  if ((msg->keys==0x18)||(msg->keys==0x3D))
  {
    if (cmd_num) SetCmdToEditMessage(commands_lines[GetCurMenuItem(data)]);
       else
         {
           FreeCommands();
           GeneralFunc_flag1(Commands_Menu_ID,1);           
         }
  }
  
  if (msg->keys==0x01)
  {
    FreeCommands();
    GeneralFunc_flag1(Select_Menu_ID,1);
  }  
  return(0);
}

static const MENU_DESC cmd_menu=
{
  0,cmd_menu_keyhook,cmd_menu_ghook,NULL,
  cmd_menusoftkeys,
  &cmd_menu_skt,
  8,
  cmd_menuitemhandler,
  NULL, //menuitems,
  NULL, //menuprocs,
  0
};

HEADER_DESC cmd_menuhdr;

void DispCommandsMenu()
{
  UpdateCommandsMenu_header();
  cmd_num=LoadCommands();
  patch_header(&cmd_menuhdr);
  Commands_Menu_ID = CreateMenu(0,0,&cmd_menu,&cmd_menuhdr,0,cmd_num,0,0);
}

//================================== ���� ������ ========================================
/*#ifdef NEWSGOLD
  #define SEL_MANU_ITEMS_NUM 4
#else*/
  #define SEL_MANU_ITEMS_NUM 3
/*#endif*/

HEADER_DESC sel_menuhdr={0,0,131,21,NULL,(int)"�����...",LGP_NULL};

int sel_menusoftkeys[]={0,1,2};

static const char * const sel_menutexts[SEL_MANU_ITEMS_NUM]=
{
  "�������",
  "������� ���������",
  "������"
  /*#ifdef NEWSGOLD
  , "������� ������"  
  #endif*/
};

SOFTKEY_DESC sel_menu_sk[]=
{
  {0x0018,0x0000,(int)"�����"},
  {0x0001,0x0000,(int)"�����"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB sel_menu_skt=
{
  cmd_menu_sk,3
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
    /*#ifdef NEWSGOLD
    switch (GetCurMenuItem(data))
    {
    case 0: case 1: case 2:
      {
        Mode = GetCurMenuItem(data);
        DispCommandsMenu();
      }
    break;
    case 3:
      {
        GeneralFunc_flag1(edmessage_id,1);
      }
    break;
    }
    #else*/
        Mode = GetCurMenuItem(data);
        DispCommandsMenu();    
    /*#endif*/
      
    GeneralFunc_flag1(Select_Menu_ID,1);
  }
  
  /*if (msg->keys==0x01)
  {
    GeneralFunc_flag1(Select_Menu_ID,1);
  }  */
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
