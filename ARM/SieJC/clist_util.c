#include "../inc/swilib.h"
#include "main.h"
#include "jabber.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "string_util.h"
#include "roster_icons.h"
#include "history.h"

#ifdef STD_PALETTE
#define CURSOR 6                 // ad: цвет курсора

#define CURSOR_BORDER 20         // ad: цвет ободка курсора

#define CLIST_F_COLOR_0 1         // Цвет шрифта 
#define CLIST_F_COLOR_1 15        // Цвет шрифта (есть сообщения)
#define CONTACT_BG_0 0
#define CONTACT_BG_1 22
#else 
const RGBA CURSOR = {80, 80, 255, 100};
const RGBA CURSOR_BORDER = {200, 200, 200, 100};
const RGBA CLIST_F_COLOR_0 = {0, 0, 0, 100};
const RGBA CLIST_F_COLOR_1 = {0, 0, 170, 100};
const RGBA CONTACT_BG_0 = {255, 255, 255, 100};
const RGBA CONTACT_BG_1 = {220, 220, 220, 100};
#endif

CLIST* cltop = NULL;

char Display_Offline = 1;         // Отображать ли оффлайн-пользователей

#ifdef STD_PALETTE
char lineColor = 0;               // ad: цвет текущей строчки
char borderColor = 0;               // ad: цвет ободка текущей строчки
#else 
RGBA lineColor = {0, 0, 0, 0};               
RGBA borderColor = {0, 0, 0, 0};            
#endif

unsigned int NContacts = 0;       // Всего контактов (и ресурсов) в списке
unsigned int N_Disp_Contacts = 0; // Сколько из них должны отображаться 

unsigned int Active_page = 1;     // Текущая активная страница списка
unsigned int N_cont_disp=0;       // Сколько контактов на странице
unsigned int CursorPos = 1;       // Текущая позиция курсора

TRESOURCE* ActiveContact = NULL;

extern char logmsg[512];
#ifdef STD_PALETTE
extern const unsigned short PRES_COLORS[PRES_COUNT];
#else
extern const RGBA PRES_COLORS[PRES_COUNT];
#endif
extern char My_Presence;
extern const char* PRESENCES[PRES_COUNT];
extern JABBER_STATE Jabber_state;

/*
    Единственная процедура, которая занимается отрисовкой контакт-листа
*/
void CList_RedrawCList()
{
  // Определяем, скока контактов поместится на странице списка
  int font_y = GetFontYSIZE(CLIST_FONT)+2; // ad: думаю что нужно сделать чтобы была 1 строка... пусть контакт вылетает за пределы экрана
  int scr_w=ScreenW();
#ifdef USE_PNG_EXT
  char path_to_pic[128];
#else
  int icon_num=0;
#endif
  N_cont_disp = sdiv(font_y,ScreenH()-CLIST_Y1)-1;
  if(!cltop)return; 
   
  LockSched(); 
  N_Disp_Contacts = Display_Offline? CList_GetNumberOfUsers() : CList_GetNumberOfOnlineUsers();
  // Определяем количество страниц списка контактов
  int pages_number = sdiv(N_cont_disp, N_Disp_Contacts);
  if(N_cont_disp*pages_number<N_Disp_Contacts){pages_number++;};
  
  CLIST* ClEx = cltop;
  WSHDR* out_ws = AllocWS(256);
  int i=1;
  int start_y;
#ifdef STD_PALETTE
  int fcolor;
#else
  RGBA fcolor;
#endif
  TRESOURCE* resEx;

  char Alternation = 1;             // ad: состояние чередования

  WSHDR* ClEx_name = AllocWS(128);
  WSHDR* ResEx_name = AllocWS(128);
  
  while(ClEx)
  {
    if(ClEx->ResourceCount)
    {
      resEx = ClEx->res_list;
      while(resEx)
      {
        
        if((i>(Active_page-1)*N_cont_disp) && (Display_Offline  |  resEx->status!=PRESENCE_OFFLINE | resEx->has_unread_msg ))
        {
          if(i==CursorPos)
          {
            lineColor=CURSOR;
            borderColor=CURSOR_BORDER; //бортик курсора
            ActiveContact = resEx;
          } else{ 
            borderColor=lineColor=(Alternation==1)? CONTACT_BG_0 : CONTACT_BG_1;
          }
          
          //ascii2ws(ClEx_name, ClEx->name);
          utf8_2ws(ClEx_name, ClEx->name, 128);
          
          if(resEx->name)
          {
            ascii2ws(ResEx_name,resEx->name);
            utf8_2ws(ResEx_name,resEx->name, 128);
            if(resEx->entry_type==T_CONF_NODE)
            {
              wsprintf(out_ws,"%w", ResEx_name); // это участник конференции
            }
            else wsprintf(out_ws,"%w/%w", ClEx_name, ResEx_name); //Это просто ресурс
          }
          else
          {
            wsprintf(out_ws,"%w", ClEx_name);  //другой вид, имхо удобнее       
          }
         
                    
          start_y = CLIST_Y1 + (i - (Active_page-1)*N_cont_disp)*font_y;
          
          if(resEx->has_unread_msg){fcolor=CLIST_F_COLOR_0;}
          else {fcolor=PRES_COLORS[resEx->status];}

          DrawRoundedFrame(0,start_y+1,scr_w-1,start_y+font_y,0,0,0,
		   color(borderColor),  //ad: ободок
		   color(lineColor));   //ad: рисуем с чередованием... для наглядности
          
          CutWSTR(out_ws, CHAR_ON_LINE);
          DrawString(out_ws,16,start_y+2,scr_w-1,start_y+font_y,SMALL_FONT,0,color(fcolor),0);

#ifdef USE_PNG_EXT          
          Roster_getIcon(path_to_pic, ClEx, resEx);
          Roster_DrawIcon(1, start_y, (int)path_to_pic);   
#else
          if (resEx->has_unread_msg)
            icon_num=Roster_getIconByStatus(50); //иконка сообщения
         else 
           icon_num=Roster_getIcon(ClEx, resEx);
  
           Roster_DrawIcon(1, start_y, icon_num) ; //иконка сообщения
#endif          

          Alternation=(Alternation==1)?0:1; //ad: перещелкиваем чередование          
        }
        if(Display_Offline  |  resEx->status!=PRESENCE_OFFLINE | resEx->has_unread_msg)i++;
        resEx = resEx->next;
        if(i>Active_page*N_cont_disp)break;
      }
    }
    ClEx = ClEx->next;
    if(i>Active_page*N_cont_disp)break;
  }

//  sprintf(logmsg, "P=%d;C=%d;N=%d;ND=%d",Active_page, CursorPos,N_Disp_Contacts,N_cont_disp);
  if(Jabber_state==JS_ONLINE)sprintf(logmsg, "Self=%s",PRESENCES[My_Presence]);
//  sprintf(logmsg, "Jabber=%d",Jabber_state);
  UnlockSched();
  FreeWS(ClEx_name);
  FreeWS(ResEx_name);
  FreeWS(out_ws);
}


TRESOURCE* CList_GetActiveContact()
{
  return ActiveContact;
}

unsigned int CList_GetNumberOfOnlineUsers()
{
  unsigned int Online=0;
  CLIST* ClEx;
  TRESOURCE* ResEx;
  if(!(ClEx = cltop))return 0;
  while(ClEx)
  {
    ResEx = ClEx->res_list;
    while(ResEx)
    {
      if(ResEx->status!=PRESENCE_OFFLINE)Online++;
      ResEx=ResEx->next;
    }
    ClEx = ClEx->next;
  }
  return Online;
}

unsigned int CList_GetNumberOfUsers() //количество контактов
{
  return NContacts;
}

void CList_ToggleOfflineDisplay()
{
  Display_Offline = !Display_Offline;
  REDRAW();
}

// Убить список сообщений
void KillMsgList(LOG_MESSAGE* messtop)
{
  LOG_MESSAGE* cl=messtop;
  messtop=NULL;
  while(cl)
  {
    LOG_MESSAGE *p;
    mfree(cl->mess);
    if(cl->muc_author)mfree(cl->muc_author);
    p=cl;
    cl=cl->next;
    mfree(p);
  }     
}


// Убить список ресурсов
void KillResourceList(TRESOURCE* res_list)
{
  TRESOURCE* cl=res_list;
  while(cl)
  {
    TRESOURCE *p;
    if(cl->name)mfree(cl->name);
    mfree(cl->full_name);
    if(cl->status_msg)mfree(cl->status_msg);
    if(cl->log)KillMsgList(cl->log);
    p=cl;
    cl=cl->next;
    mfree(p);
    p=NULL;
  }     
}

// Получить дескриптор контакта по FullJID (JID вместе с ресурсом)
CLIST* CList_FindContactByJID(char* jid)
{
  LockSched();
  // Преобразуем жид в нижний регистр
//  char lc_jid[128];
//  strcpy(lc_jid, jid);
//  str2lower(lc_jid);
  CLIST* ClEx = cltop;
  while(ClEx)
  {
//    if(strstr(lc_jid,ClEx->JID)==lc_jid)
    if(stristr(jid,ClEx->JID)==jid)
    {
      UnlockSched();
      return ClEx;
    }
    ClEx = ClEx->next;
  }
  UnlockSched();
  return NULL;
}

// Добавляет в список контактов системное сообщение
// Полезно, когда происходят действия с подпиской
void CList_AddSystemMessage(char* jid, char status, char* status_msg)
{
  if(status<=PRESENCE_ERROR)
  {
    if(!status_msg)return;
    CList_AddMessage(jid, MSG_STATUS, status_msg);
  }
  if(status == PRESENCE_UNSUBSCRIBED)
  {
    CList_AddMessage(jid, MSG_SYSTEM, "Authorization was removed!");
  }
  if(status==PRESENCE_SUBSCRIBED)
  {
    CList_AddMessage(jid, MSG_SYSTEM, "Authorization was granted");    
  }
}

void CList_AddSystemMessageA(char* jid, char status, char* ansi_status_msg)
{
  char *utf8_msg=ANSI2UTF8(ansi_status_msg, 512);
  CList_AddSystemMessage(jid, status, utf8_msg);
  mfree(utf8_msg);
}

// Узнать, есть ли уже такой ресурс у контакта, по FullJID
TRESOURCE* CList_IsResourceInList(char* jid)
{
  CLIST* ClEx = CList_FindContactByJID(jid);
  if(!ClEx)return NULL;
  LockSched();
  TRESOURCE* ResEx = ClEx->res_list;
  while(ResEx)
  {
    if(!stricmp(ResEx->full_name, jid))
    {
      UnlockSched();
      return ResEx;
    }
    ResEx = ResEx->next;
  }
  UnlockSched();
  return NULL;
}


// Поменять статус у контакта 
void CList_Ch_Status(TRESOURCE* resource,
                       char status,
                       char* status_msg
                       )
{
  LockSched();
  if(resource->status_msg)
  {
    mfree(resource->status_msg);
    resource->status_msg = NULL;
  }
  if(status_msg)
  {
    resource->status_msg = malloc(strlen(status_msg)+1);
    strcpy(resource->status_msg, status_msg);
  }
  resource->status = status;
  UnlockSched();
  CList_AddSystemMessage(resource->full_name, status, status_msg);
}


// Делаем оффлайнами все ресурсы заданного контакта. Useful для конференций.
void CList_MakeAllResourcesOFFLINE(CLIST* ClEx)
{
  if(!ClEx)return;
  TRESOURCE* ResEx = ClEx->res_list;
  
  while(ResEx)
  {
     CList_Ch_Status(ResEx,
                       PRESENCE_OFFLINE,
                       NULL
                       );
    ResEx = ResEx->next;
  }
}


TRESOURCE* CList_AddResourceWithPresence(char* jid, char status, char* status_msg)
{
  TRESOURCE* qq = CList_IsResourceInList(jid);
  
  // Если такой ресурс уже есть, его не добавляем. 
  // Нужно ему статус поменять.
  if(qq)
  {
    CList_Ch_Status(qq, status, status_msg);
    return qq;
  }
  CLIST* ClEx = cltop;
  LockSched();
  while(ClEx)
  {
    
    if(stristr(jid,ClEx->JID)==jid) // Ага, именно так, ибо это соответствует началу строки!
    {
      TRESOURCE* ResEx=malloc(sizeof(TRESOURCE));//ClEx->res_list;
      char *resname_ptr=Get_Resource_Name_By_FullJID(jid);
      if(resname_ptr)
      {
        ResEx->name = malloc(strlen(resname_ptr)+1);
        strcpy(ResEx->name, resname_ptr);
      }
      else
      {
        ResEx->name = NULL;
      }
      ResEx->full_name = malloc(strlen(jid)+1);
      strcpy(ResEx->full_name, jid);
      
      if(status_msg)
      {
        ResEx->status_msg = malloc(strlen(status_msg)+1);
        strcpy(ResEx->status_msg, status_msg);
      }
      else
      {
        ResEx->status_msg = NULL;
      }
            
      ResEx->status = status;
      ResEx->muc_privs.aff = AFFILIATION_NONE;
      ResEx->muc_privs.role=  ROLE_NONE; 
      if(ClEx->res_list->entry_type!=T_CONF_ROOT){ ResEx->entry_type = T_NORMAL;}
      else{ResEx->entry_type = T_CONF_NODE;}
      ResEx->has_unread_msg=0;
      ResEx->total_msg_count=0;
      ResEx->log = NULL;
      
      // Удаляем псевдоресурс, если он есть
      if(ClEx->res_list->entry_type==T_VIRTUAL && ClEx->res_list->has_unread_msg==0)
      {
        KillResourceList(ClEx->res_list);
        ClEx->ResourceCount=0;
        NContacts--;
        ClEx->res_list=NULL;
      }    
      
      TRESOURCE* existing_res=ClEx->res_list;
      
      if(!existing_res)
      {
        ClEx->res_list = ResEx;
        ClEx->ResourceCount=1;
      }
      else
      {
        while(existing_res->next){existing_res=existing_res->next;}
        existing_res->next = ResEx;
        ClEx->ResourceCount++;
      }
      NContacts++;
      ResEx->next=NULL;
      CursorPos = 1;
      Active_page=1;
      UnlockSched();
      CList_AddSystemMessage(ResEx->full_name, status, ResEx->status_msg);
      return ResEx;
    }
    ClEx = ClEx->next;
  }
CursorPos = 1;
Active_page=1;
UnlockSched();  
return NULL;
}

void CList_ChangeContactParams(CLIST* Cont_Ex,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char wants_subscription,
                          char group)
{

  if(!Cont_Ex)return;
  if(Cont_Ex->name)mfree(Cont_Ex->name);
  // Имя может быть не заполнено
  if(name)
  {
    Cont_Ex->name = malloc(strlen(name)+1);
    strcpy(Cont_Ex->name, name);
  }
  else
  {
    Cont_Ex->name = malloc(strlen(Cont_Ex->JID)+1);
    strcpy(Cont_Ex->name, Cont_Ex->JID);
  }
  Cont_Ex->subscription = subscription;
  Cont_Ex->wants_subscription = wants_subscription;
  Cont_Ex->group = group;
}

// Пишет роли контакта в конфе в структуру
void CList_MUC_SetRole(char* jid, CONF_PRIV priv)
{
  TRESOURCE* ResEx = CList_IsResourceInList(jid);
  if(!ResEx)
  {
    return;
  }
  if(ResEx->entry_type!=T_CONF_NODE)return;
  ResEx->muc_privs.aff=priv.aff;
  ResEx->muc_privs.role=priv.role;
  //ShowMSG(1,(int)"Found OK");
}

// Добавить к листу контакт. Возвращает структуру созданного контакта.
CLIST* CList_AddContact(char* jid,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char wants_subscription,
                          char group)
{
  
  str2lower(jid);
  CLIST* Cont_Ex = malloc(sizeof(CLIST));

  // Имя может быть не заполнено
  if(name)
  {
    Cont_Ex->name = malloc(strlen(name)+1);
    strcpy(Cont_Ex->name, name);
  }
  else
  {
    Cont_Ex->name = malloc(strlen(jid)+1);
    strcpy(Cont_Ex->name, jid);
  }

  Cont_Ex->JID = malloc(strlen(jid)+1);
  strcpy(Cont_Ex->JID, jid);
  Cont_Ex->subscription = subscription;
  Cont_Ex->wants_subscription = wants_subscription;
  Cont_Ex->group = group;
  Cont_Ex->next=NULL;
   
  TRESOURCE* ResEx = malloc(sizeof(TRESOURCE));
  ResEx->log=NULL;
  ResEx->next=NULL;
  ResEx->status_msg=NULL;
  ResEx->has_unread_msg=0;
  ResEx->total_msg_count=0;
  if(group & 0x80)
  {
    ResEx->entry_type=T_CONF_ROOT; // Корень конференции
    ResEx->status=PRESENCE_ONLINE;
    ShowMSG(1,(int)"Conference created OK");
  }
  else
  {
    ResEx->entry_type=T_VIRTUAL; // По этому признаку потом его убъём
    ResEx->status=PRESENCE_OFFLINE;
  }
  ResEx->name = NULL;
  ResEx->full_name = malloc(strlen(jid)+1);
  strcpy(ResEx->full_name, jid);
//  strcat(ResEx->full_name, "/");
//  strcat(ResEx->full_name, ResEx->name);
  Cont_Ex->res_list=ResEx;
  Cont_Ex->ResourceCount=1;
  
  LockSched();
  if(!cltop)
  {
    cltop = Cont_Ex;
  }
  else
  {
    CLIST* tmp=cltop;
    while(tmp->next){tmp=tmp->next;}
    tmp->next=Cont_Ex;
  }

  NContacts++;
  UnlockSched();
  CursorPos = 1;
  Active_page=1;
  return Cont_Ex;
}


// Добавить сообщение в список сообщений контакта
void CList_AddMessage(char* jid, MESS_TYPE mtype, char* mtext)
{
  TTime now_time;
  TDate now_date;
  GetDateTime(&now_date,&now_time);
  char datestr[200];
  char IsMe = strstr(mtext,"/me ")==mtext ? 1 : 0; // Флаг наличия /me
  if(mtype==MSG_ME)
  {
    extern char My_JID[128];
    sprintf(datestr, "%s: %02d:%02d %02d-%02d\r\n",My_JID,now_time.hour,now_time.min,now_date.day,now_date.month);
  }
  else
  {
    sprintf(datestr, "%s: %02d:%02d %02d-%02d\r\n",jid,now_time.hour,now_time.min,now_date.day,now_date.month);
  }

  
  CLIST* contEx = CList_FindContactByJID(jid);
  if(!contEx)
  {
    Log("MESS_LOST",mtext);
    return;
  }
  TRESOURCE* cont = (contEx->group & 0x80 && (mtype==MSG_GCHAT || mtype==MSG_SUBJECT)) ? contEx->res_list : CList_IsResourceInList(jid);
  if(!cont)
  {
    // У контакта нет ресурсов или такого ресурса нет. Добавляем на первый же.
    // Такая ситуация, скорее всего, возникает, если у контакта ресурсов не 
    // бывает в принципе (MRIM)
    cont=contEx->res_list;
  }
  
  if(!cont->total_msg_count && mtype==MSG_STATUS && cont->entry_type!=T_CONF_ROOT)return;  // Не записываем статусные сообщения, если нет беседы
  
  if(mtype!=MSG_ME && mtype!=MSG_STATUS)cont->has_unread_msg++;
  LOG_MESSAGE* mess = malloc(sizeof(LOG_MESSAGE));
  char timestamp[]="[%02d:%02d] ";
  snprintf(timestamp, 12, timestamp, now_time.hour, now_time.min);
  switch(mtype)
  {
  case MSG_GCHAT:
  {
    char* conf_nickname = Get_Resource_Name_By_FullJID(jid);
    if(!conf_nickname)
    {
      extern const char _empty[];
      conf_nickname=(char*)&_empty;
    }
    mess->mess = malloc(strlen(mtext)+strlen(conf_nickname)+strlen(timestamp)+2+1);
    mess->muc_author = malloc(strlen(conf_nickname)+1);
    strcpy(mess->muc_author, conf_nickname);
    strcpy(mess->mess, timestamp);
    if(IsMe)
    {
      strcat(mess->mess, "*");
      strcat(mess->mess, conf_nickname);
      strcat(mess->mess, mtext+3);  // пропуск /me
    }
    else
    {
      strcat(mess->mess, conf_nickname);
      strcat(mess->mess, ": ");
      strcat(mess->mess, mtext);
    }
    break;
  }
  case MSG_SUBJECT:
    {
      char* conf_nickname = Get_Resource_Name_By_FullJID(jid);
      char subj_set_to[] = " has set the subject: ";
      mess->mess = malloc(strlen(mtext)+strlen(conf_nickname)+strlen(timestamp)+strlen(subj_set_to)+1);
      mess->muc_author = malloc(strlen(conf_nickname)+1);
      strcpy(mess->muc_author, conf_nickname);
      strcpy(mess->mess, timestamp);
      strcat(mess->mess, conf_nickname);
      strcat(mess->mess, subj_set_to);
      strcat(mess->mess, mtext);
      break;      
    }
  default:
  {
    mess->mess = malloc(strlen(mtext)+strlen(timestamp)+1);
    strcpy(mess->mess, timestamp);
    strcat(mess->mess, mtext);
    mess->muc_author = NULL;
  }
  }
  mess->mtype=mtype;
  LockSched();
  if(!cont->log)
  {
    cont->log = mess;
  }
  else
  {
    LOG_MESSAGE* tmp=cont->log;
    while(tmp->next){tmp=tmp->next;}
    tmp->next=mess;
  }
  cont->total_msg_count++;
  mess->next=NULL;
  UnlockSched();
  
  extern const int WRITE_HISTORY, WRITE_MUC_HISTORY;
  if((WRITE_HISTORY && !(mtype==MSG_GCHAT || mtype==MSG_SUBJECT)) || (WRITE_MUC_HISTORY && (mtype==MSG_GCHAT || mtype==MSG_SUBJECT)))
  {
    char *ansi_text = convUTF8_to_ANSI_STR(mtext);  
    Add2History(CList_FindContactByJID(jid), datestr,ansi_text);
    mfree(ansi_text);
  }
}

/*
//Уничтожить список
void FreeCLIST(void)
{
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  while(cl)
  {
    CLIST *p;
//    if (cl->log) mfree(cl->log);
//    if (cl->answer) mfree(cl->answer);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

*/

// Уничтожить список контактов  
void CList_Destroy()
{
  LockSched();
  CLIST* cl=cltop;
  cltop=NULL;
  while(cl)
  {
    CLIST *p;
    if(cl->res_list) KillResourceList(cl->res_list);
    cl->ResourceCount=0;
    mfree(cl->JID);
    mfree(cl->name);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
    p=NULL;
  }  
  UnlockSched();
}


// Отображение всякого спама про контакт
void CList_Display_Popup_Info(TRESOURCE* ResEx)
{
  if(!ResEx)return;
  char msg_ex[]="JID: %s\nStatus:%s";
  char* msg = malloc(200);
  extern const char* JABBER_AFFS[];
  extern const char* JABBER_ROLS[];
  if(ResEx->status_msg)
  {
    char *ansi_statusmsg = convUTF8_to_ANSI_STR(ResEx->status_msg);
    ShowMSG(0,(int)ansi_statusmsg);
    mfree(ansi_statusmsg);
  }
  snprintf(msg,200,msg_ex,ResEx->full_name, PRESENCES[ResEx->status]);
  char *ansi_msg=convUTF8_to_ANSI_STR(msg);
  ShowMSG(0, (int)ansi_msg);
  mfree(ansi_msg);
  if(ResEx->entry_type==T_CONF_NODE)
  {
    snprintf(msg,1024,"Aff:%s,\nRole:%s",JABBER_AFFS[ResEx->muc_privs.aff], JABBER_ROLS[ResEx->muc_privs.role]);
    ShowMSG(0, (int)msg);
  }

  mfree(msg);
}

// бегаем по контактам с сообщениями
void nextUnread()
{
  if (CList_GetUnreadMessages()==0) return; //выйдем если нет непрочитанных
  CLIST* ClEx;
  TRESOURCE* ResEx;
  if(!(ClEx = cltop))return;
LockSched();
  while(ClEx) //идем по списку контактов
    {
      ResEx = ClEx->res_list;
      while(ResEx) //идем по списку ресурсов
      {
          if(ResEx->has_unread_msg) { //если есть непрочитанное
            if (CList_GetActiveContact()!=ResEx) { //если мы не стоим на этом контакте
              //нужна еще какая-то проверка, я что-то не учитываю, поэтому иногда ведет себя странно...
              MoveCursorTo(ResEx);
              break;
            }
          }
        ResEx=ResEx->next; //следующий ресурс
      }
      ClEx = ClEx->next; //следующий контакт
    }
UnlockSched();  
}

unsigned int CList_GetUnreadMessages() //количество непрочитанных сообщений
{
  unsigned int unread=0;
  CLIST* ClEx;
  TRESOURCE* ResEx;
  if(!(ClEx = cltop))return 0;
  while(ClEx)
  {
    ResEx = ClEx->res_list;
    while(ResEx)
    {
      if(ResEx->has_unread_msg) {
        unread+=ResEx->has_unread_msg;
      }
      ResEx=ResEx->next;
    }
    ClEx = ClEx->next;
  }
  return unread;
};


// Ставим курсор на нужный ресурс
void MoveCursorTo(TRESOURCE* NewResEx)
{
  if(!NewResEx)return;
  
  CLIST* ClEx;
  TRESOURCE* ResEx;
  if(!(ClEx = cltop))return;
  
  int pos=0;
  while(ClEx)
  {
    ResEx = ClEx->res_list;
    while(ResEx)
    {
      if(ResEx==NewResEx) {
        //ставим курсор на клиента
        CursorPos=pos+1;
        //тут надо бы еще проверит на какой мы странице.. а то вдруг не там и курсор проебется :-D
        // сначала считаем на какой странице должен быть курсор
        if (CursorPos>N_cont_disp) {
          Active_page = sdiv(N_cont_disp, CursorPos)+1;
        } else {
          Active_page=1;
        }               
        REDRAW();
        break;
      }
      if (Display_Offline) {
        pos++;
      } else if (ResEx->status!=PRESENCE_OFFLINE) {
        pos++;
      }
      ResEx=ResEx->next;
    }
    ClEx = ClEx->next;
  }
};


// Управление курсором
void CList_MoveCursorUp()
{
  if(!N_Disp_Contacts)return;
  if(CursorPos==1)
  {
    CursorPos=N_Disp_Contacts;
    if (N_cont_disp==N_Disp_Contacts) {
      Active_page = 1; 
    } else {
      Active_page = sdiv(N_cont_disp, N_Disp_Contacts)+1;
    }
  }
  else
  {
    CursorPos--;
    if(CursorPos<=(Active_page-1)*N_cont_disp){Active_page--;}
  }
  REDRAW();
};

void CList_MoveCursorDown()
{
  if(!N_Disp_Contacts)return;
  CursorPos++;
  if(CursorPos>N_Disp_Contacts){CursorPos=1;Active_page=1;}
  if(Active_page*N_cont_disp<CursorPos){Active_page++;}
  REDRAW();
};

void CList_MoveCursorHome()
{
  if(!N_Disp_Contacts)return;
  CursorPos =1;
  Active_page = 1;
  REDRAW();  
};

void CList_MoveCursorEnd()
{
  if(!N_Disp_Contacts)return;
  CursorPos = N_Disp_Contacts;
  Active_page = sdiv(N_cont_disp, N_Disp_Contacts)+1;
  REDRAW();  
};
