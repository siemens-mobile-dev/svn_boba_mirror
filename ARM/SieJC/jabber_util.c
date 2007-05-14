#include "../inc/swilib.h"
#include "history.h"
#include "main.h"
#include "clist_util.h"
#include "revision.h"
#include "jabber_util.h"
#include "string_util.h"
#include "xml_parser.h"
#include "base64.h"
#include "md5.h"
#include "jabber.h"
#include "bookmarks.h"
#include "serial_dbg.h"
#include "groups_util.h"


extern const char JABBER_SERVER[];
extern const char USERNAME[];
extern const char PASSWORD[];
extern const char RESOURCE[];
extern const char CMP_DATE[];
extern const char VERSION_NAME[];
extern const char VERSION_VERS[];
extern const char OS[];
extern const int USE_SASL;
extern const int USE_ZLIB;
extern char My_JID_full[];
extern char My_JID[];
extern char logmsg[];

MUC_ITEM *muctop = NULL;
BM_ITEM *BM_ROOT  = NULL;


extern JABBER_STATE Jabber_state;
const char* PRESENCES[PRES_COUNT] = {"online", "chat", "away", "xa", "dnd", "invisible", "unavailable", "error",
                                      "subscribe", "subscribed", "unsubscribe", "unsubscribed"};

const RGBA PRES_COLORS[PRES_COUNT] =
{
  {  0,   0, 127, 100},   // Online
  {  0, 255,   0, 100},   // Chat
  {  0,   0, 255, 100},   // Away
  {  0, 127,   0, 100},   // XA
  {255,   0,   0, 100},   // DND
  {127, 127, 127, 100},   // Invisible
  {170, 170, 170, 100},   // Offline
  {127, 127, 127, 100},   // Error
  {170, 170, 170, 100},   // Subscribe
  {170, 170, 170, 100},   // Subscribed
  {170, 170, 170, 100},   // Unsubscribe
  {170, 170, 170, 100}    // Unsubscribed
};

#define AFFS_CNT 5
#define ROLS_CNT 4

const char* JABBER_AFFS[] = {"none", "outcast", "member", "admin", "owner"};
const char* JABBER_ROLS[] = {"none", "visitor", "participant", "moderator"};

typedef struct
{
  char *nonce;
  char *cnonce;
  char *qop;
  char *rsp_auth;
}SASL_AUTH_DATA;


SASL_AUTH_DATA SASL_Auth_data = {NULL, NULL, NULL, NULL};

void Destroy_SASL_Ctx()
{
  if(SASL_Auth_data.nonce)mfree(SASL_Auth_data.nonce);
  if(SASL_Auth_data.cnonce)mfree(SASL_Auth_data.cnonce);
  if(SASL_Auth_data.qop)mfree(SASL_Auth_data.qop);
  if(SASL_Auth_data.rsp_auth)mfree(SASL_Auth_data.rsp_auth);
}

/*
  Посылка стандартного Jabber Iq
*/
//Context: HELPER
void SendIq(char* to, char* type, char* id, char* xmlns, char* payload)
{
  char* xmlq=malloc(1024);
  char *xmlq2=malloc(1024);
  char s_to[128];
  char s_id[40];
  if(id)
  {
    snprintf(s_id,40,"id='%s'", id);
  }
  else
  {
    strcpy(s_id, "");
  }
  sprintf(xmlq, "<iq type='%s' %s from='%s'", type, s_id, My_JID_full);
  if(to)
  {
    snprintf(s_to, 128, " to='%s'", to);
    strcat(xmlq, s_to);
  }
  if(payload)
  {
    strcat(xmlq, ">\r\n<query xmlns='%s'>\r\n%s\r\n</query>\r\n</iq>");
    sprintf(xmlq2, xmlq, xmlns, payload);
  }
  else
  {
    strcat(xmlq, ">\r\n<query xmlns='%s'/>\r\n</iq>");
    sprintf(xmlq2, xmlq, xmlns);
  }
  sprintf(xmlq2, xmlq, xmlns, payload);
  SendAnswer(xmlq2);
//  Log("IQ_OUT", xmlq2);
  mfree(xmlq2);
  mfree(xmlq);
}

/*
  Послать приветствие, на него сервер высылает ответный stream.
  После этого можно общаться с сервером
*/
// Context:HELPER
void Send_Welcome_Packet()
{

  void Send_Welcome_Packet_SASL();
  if(USE_SASL)
  {
    Send_Welcome_Packet_SASL();
    return;
  }
  char streamheader[]="<?xml version='1.0' encoding='UTF-8'?>\n"
    "<stream:stream to='%s' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' xml:lang='en'>";
  char* buf=malloc(256);
  sprintf(buf,streamheader,JABBER_SERVER);
  SendAnswer(buf);
  mfree(buf);
  LockSched();
  strcpy(logmsg,"Send Welcome");
  UnlockSched();
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif
}

void Send_Welcome_Packet_SASL()
{
  char streamheader[]="<?xml version='1.0' encoding='UTF-8'?>\n"
    "<stream:stream to='%s' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' xml:lang='en' version='1.0'>";
  char* buf=malloc(256);
  sprintf(buf,streamheader,JABBER_SERVER);
  SendAnswer(buf);
  mfree(buf);
  LockSched();
  strcat(logmsg,"\nSend Extended Welcome");
  UnlockSched();
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif
}


/*
  Послать дисконнект
*/
// Context:HELPER
void Send_Disconnect()
{
  char streamend[]="</stream:stream>";
  SendAnswer(streamend);
  strcpy(logmsg,"Send Disconnect");
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif
}

// Константы Iq-запросов
char auth_id[] = "SieJC_auth_req";
char rost_id[] = "SieJC_rost_req";
char vreq_id[] = "SieJC_vers_req";
char priv_id[] = "SieJC_priv_req";

/*
  Авторизация на Jabber-сервере
  Самая тупая, без извращений.
*/
//Context: HELPER
void Send_Auth()
{
  sprintf(My_JID, "%s@%s",USERNAME, JABBER_SERVER);
  sprintf(My_JID_full,"%s/%s",My_JID, RESOURCE);
  char* payload = malloc(256);
  sprintf(payload,"<username>%s</username>\n<password>%s</password>\n<resource>%s</resource>",USERNAME, PASSWORD, RESOURCE);
  SendIq(NULL, "set", auth_id, IQ_AUTH, payload);
  LockSched();
  strcpy(logmsg,"Send auth");
  UnlockSched();
}



//Context: HELPER
void _sendversionrequest(char *dest_jid)
{
  char typ[]=IQTYPE_GET;
  char iq_v[]=IQ_VERSION;

  SendIq(dest_jid, typ, vreq_id, iq_v, NULL);
  mfree(dest_jid);
}

// Послать запрос о версии пользователю с указанным JID
void Send_Version_Request(char *dest_jid)
{
  char *to=malloc(128);
  strcpy(to, dest_jid);
  SUBPROC((void*)_sendversionrequest,to);
}


/*
  Послать своё присутствие (в частности, после этого на нас вываливаются
  присутствия остальных, а мы появляемся в ресурсах своего контакта)

*/
// Context: HELPER
void Send_Presence(PRESENCE_INFO *pr_info)
{
  extern char My_Presence;
  My_Presence = pr_info->status;

  char* presence = malloc(1024);
  if(pr_info->message)
  {
    char presence_template[]="<presence><priority>%d</priority><show>%s</show><status>%s</status></presence>";
    snprintf(presence,1024,presence_template, pr_info->priority, PRESENCES[pr_info->status], pr_info->message);
  }
  else
  {
    char presence_template[]="<presence><priority>%d</priority><show>%s</show></presence>";
    snprintf(presence,1024,presence_template, pr_info->priority, PRESENCES[pr_info->status]);
  }
  SendAnswer(presence);

// MUC support
  MUC_ITEM* m_ex = muctop;
  while(m_ex)
  {
    if(pr_info->message)
    {
      char presence_template[]="<presence from='%s' to='%s'><show>%s</show><status>%s</status></presence>";
      snprintf(presence,1024,presence_template, My_JID_full, m_ex->conf_jid, PRESENCES[pr_info->status], pr_info->message);
    }
    else
    {
      char presence_template[]="<presence from='%s' to='%s'><show>%s</show></presence>";
      snprintf(presence,1024,presence_template, My_JID_full, m_ex->conf_jid, PRESENCES[pr_info->status]);
    }
    SendAnswer(presence);
    m_ex=m_ex->next;
  };

  mfree(presence);
  if(pr_info->message)mfree(pr_info->message);
  mfree(pr_info);
  LockSched();
  strcpy(logmsg,"Send presence");
  UnlockSched();
}

/*
  Послать запрос ростера
*/
// Context: HELPER
void Send_Roster_Query()
{
  SendIq(NULL, IQTYPE_GET, rost_id, IQ_ROSTER, NULL);
  LockSched();
  strcpy(logmsg,"Send roster Q");
  UnlockSched();
}

unsigned int m_num=0;


// Context: HELPER
void SendMessage(char* jid, IPC_MESSAGE_S *mess)
{
/*
  <message to='romeo@montague.net' id='message22'>
      <body>Art thou not Romeo, and a Montague?</body>
      <x xmlns='jabber:x:event'>
        <offline/>
        <delivered/>
        <displayed/>
        <composing/>
      </x>
    </message>
*/
  char* _jid=malloc(128);
  strcpy(_jid, jid);
  char mes_template[]="<message to='%s' id='SieJC_%d' type='%s'><body>%s</body></message>";
  char* msg_buf = malloc(MAX_MSG_LEN*2+200);
  if(mess->IsGroupChat)
  {
     sprintf(msg_buf, mes_template, _jid, m_num, MSGSTR_GCHAT, mess->body);
  }else sprintf(msg_buf, mes_template, _jid, m_num, MSGSTR_CHAT, mess->body);
  mfree(mess->body);
  mfree(mess);
  mfree(_jid);
#ifdef LOG_ALL
  LockSched();
  Log("MESS_OUT", msg_buf);
  UnlockSched();
#endif
  SendAnswer(msg_buf);
  mfree(msg_buf);
  m_num++;
}

// Context: HELPER
void Report_VersionInfo(char* id, char *to)
{
  char answer[200];
  char *ph_model = Get_Phone_Info(PI_MODEL);
  char *ph_sw = Get_Phone_Info(PI_SW_NUMBER);
  sprintf(answer, "<name>%s</name><version>%s-r%d (%s)</version><os>SIE-%s/%s %s</os>", VERSION_NAME, VERSION_VERS, __SVN_REVISION__, CMP_DATE, ph_model, ph_sw, OS);
  SendIq(to, IQTYPE_RES, id, IQ_VERSION, answer);

  mfree(id);
  mfree(to);
}

JABBER_SUBSCRIPTION GetSubscrType(char* subs)
{
  if(!subs)return SUB_NONE;
  if(!strcmp(subs,"none"))return SUB_NONE;
  if(!strcmp(subs,"both"))return SUB_BOTH;
  if(!strcmp(subs,"to"))return SUB_TO;
  if(!strcmp(subs,"from"))return SUB_FROM;

  return SUB_NONE;
}

char* Get_Resource_Name_By_FullJID(char* full_jid)
{
  char* pre_res = strstr(full_jid, "/");
  char* res_name = pre_res==NULL ? NULL : pre_res +1;
  return res_name;
}

//Context: HELPER
void Send_Initial_Presence_Helper()
{
  char ansi_msg[]="Online";
  char *message = ANSI2UTF8(ansi_msg, strlen(ansi_msg));
  PRESENCE_INFO *pr_info = malloc(sizeof(PRESENCE_INFO));
  pr_info->priority = 16;
  pr_info->status=PRESENCE_ONLINE;
  pr_info->message=message;
  Send_Presence(pr_info);
  Jabber_state = JS_ONLINE;
}

//Context: HELPER
void _enterconference(MUC_ENTER_PARAM *param)
{
  char magic_ex[]="<presence from='%s' to='%s/%s'><x xmlns='http://jabber.org/protocol/muc'><history maxstanzas='%d'/></x></presence>";
  char* magic = malloc(1024);
  sprintf(magic,magic_ex, My_JID_full, param->room_name,param->room_nick, param->mess_num);
  SendAnswer(magic);
  mfree(magic);
  mfree(param->room_nick);
  mfree(param->room_name);
  if(param->pass)mfree(param->pass);
  mfree(param);
}


// Входит в конференцию
// Все имена в UTF-8 :)
void Enter_Conference(char *room, char *roomnick, char N_messages)
{
  // Добавляем контакт конференции в ростер
  CLIST* Conference = CList_FindContactByJID(room);
  if(!Conference)
  {
    CList_AddContact(room,room, SUB_BOTH, 0, 129);
  }
  else
  {
    Conference->res_list->status=PRESENCE_ONLINE;
  }

  // Готовим структуру для передачи в HELPER
  MUC_ENTER_PARAM* par = malloc(sizeof(MUC_ENTER_PARAM));
//  par->room_nick =ANSI2UTF8(roomnick, strlen(roomnick)*2);
//  par->room_name = ANSI2UTF8(room, strlen(room)*2);

  par->room_nick =malloc(strlen(roomnick)*2);
  par->room_name = malloc(strlen(room)*2);
  strcpy(par->room_nick, roomnick);
  strcpy(par->room_name, room);

  par->pass=NULL;
  par->mess_num=N_messages;
  SUBPROC((void*)_enterconference, par);

  // Регистрируем конференцию в списке конференций
  MUC_ITEM* mi = malloc(sizeof(MUC_ITEM));
  mi->conf_jid = malloc(strlen(par->room_name)*2+strlen(par->room_nick)*2+2);
  sprintf(mi->conf_jid, "%s/%s", par->room_name, par->room_nick);
  mi->next=NULL;
  MUC_ITEM* m_ex = muctop;
  if(muctop)
  {
    while(m_ex->next){m_ex=m_ex->next;};
    m_ex->next = mi;
  }
  else
  {
    muctop = mi;
  }
}


//Context: HELPER
void _leaveconference(char *conf_jid)
{
  char pr_templ[] = "<presence from='%s' to='%s' type='unavailable'/>";
  char* pr=malloc(1024);
  sprintf(pr, pr_templ,My_JID_full,conf_jid);
  mfree(conf_jid);
  SendAnswer(pr);
  mfree(pr);
}

// Выходит из конференции
void Leave_Conference(char* room)
{
  // Ищем экземпляр контакта в списке для конференций
  MUC_ITEM* m_ex = muctop;
  while(m_ex)
  {
    if(strstr(m_ex->conf_jid, room))
    {
      char* cj = malloc(strlen(m_ex->conf_jid)*2+1);
      strcpy(cj, m_ex->conf_jid);
      SUBPROC((void*)_leaveconference, cj);
      break;
    }
    m_ex = m_ex->next;
  }

  MUC_ITEM* m_ex2 = muctop;
  if(muctop==m_ex && muctop!=NULL)
  {
    mfree(m_ex->conf_jid);
    muctop=m_ex->next;
    mfree(m_ex);
    muctop = NULL;
  }
  while(m_ex2)
  {
    if(m_ex2->next==m_ex)
    {
      m_ex2->next = m_ex->next;   // Выбиваем из цепочки
      mfree(m_ex->conf_jid);
      mfree(m_ex);
      break;
    }
    m_ex2 = m_ex2->next;
  }
  ShowMSG(1,(int)"Выход из MUC выполнен");
}


// Уничтожить список комнат
void MUCList_Destroy()
{
  LockSched();
  MUC_ITEM* cl=muctop;
  muctop=NULL;
  while(cl)
  {
    MUC_ITEM *p;
    mfree(cl->conf_jid);
    p=cl;
    cl=(MUC_ITEM*)(cl->next);
    mfree(p);
    p=NULL;
  }
  UnlockSched();
}



// Для вызова таймером
void Send_Presence_MMIStub()
{
  SUBPROC((void*)Send_Initial_Presence_Helper);
}

// Изменяет имя контакта в ростере
void ChangeRoster(XMLNode* items)
{
  XMLNode* rostEx = items;
  CLIST* Cont_Ex;
  char *name;
  char* w_subscr;
  char aname[]="jid";  // Затрахали Tool Internal Error
  char asub[]="subscribe";
  char aask[]="ask";
  char* jid;
  char w_subscr_flag;
  while(rostEx)
  {
    jid = XML_Get_Attr_Value(aname,rostEx->attr);
    JABBER_SUBSCRIPTION r_subscr=GetSubscrType(XML_Get_Attr_Value("subscription",rostEx->attr));
    name = XML_Get_Attr_Value("name",rostEx->attr);
    w_subscr = XML_Get_Attr_Value(aask,rostEx->attr);
    if(w_subscr)
    {
      w_subscr_flag = !strcmp(w_subscr, asub) ? 1: 0;
    }
    else
    {
      w_subscr_flag = 0;
    }
    Cont_Ex = CList_FindContactByJID(jid);
    if(Cont_Ex)
    {
      CList_ChangeContactParams(Cont_Ex,name,r_subscr, w_subscr_flag,0);
    }
    rostEx=rostEx->next;
  }
}

void FillRoster(XMLNode* items)
{
  XMLNode* rostEx = items;
  int i=0;
  char* name;
  char* w_subscr;
  char w_subscr_flag;
  while(rostEx)
  {
    JABBER_SUBSCRIPTION r_subscr=GetSubscrType(XML_Get_Attr_Value("subscription",rostEx->attr));
    name = XML_Get_Attr_Value("name",rostEx->attr);
    w_subscr = XML_Get_Attr_Value("ask",rostEx->attr);
    if(w_subscr)
    {
      w_subscr_flag = !strcmp(w_subscr, "subscribe") ? 1: 0;
    }
    else
    {
      w_subscr_flag = 0;
    }
    // Получаем группу контакта, заносим в список групп
    XMLNode *group = XML_Get_Child_Node_By_Name(rostEx, "group");
    int gr_id;
    if(group)
      if(!(gr_id = GetGroupID(group->value)))gr_id = AddGroup(group->value);

    CList_AddContact(XML_Get_Attr_Value("jid",rostEx->attr),
                          name,
                          r_subscr,
                          w_subscr_flag,
                          gr_id
                          );
   //if(name)mfree(name);

   rostEx=rostEx->next;
   i++;
  }
}


void KillBMList()
{
  LockSched();
  BM_ITEM* cl=BM_ROOT;
  BM_ROOT=NULL;
  while(cl)
  {
    BM_ITEM *p;
    mfree(cl->mucname);
    mfree(cl->nick);
    if(cl->pass)mfree(cl->pass);
    p=cl;
    cl=(BM_ITEM*)(cl->next);
    mfree(p);
    p=NULL;
  }
  UnlockSched();
}


/*
<storage xmlns='storage:bookmarks'>
  <conference jid='siepatchdb@conference.jabber.ru' name='siepatchdb@conference.jabber.ru' autojoin='1'>
    <nick>РљРѕС€РєРѕ</nick>
    <password/>
  </conference>

  <conference jid='bombusmod@conference.jabber.ru' name='bombusmod@conference.jabber.ru'>
    <nick>Kibab</nick>
  </conference>

  <conference jid='cx75planet@conference.jabber.ru' name='cx75planet@conference.jabber.ru'>
    <nick>Kibab</nick>
  </conference>
</storage>
*/
void Process_Bookmarks_Storage(XMLNode* nodeEx)
{
  XMLNode *elem = nodeEx->subnode;
  XMLNode *tmpnode;
  extern const char conference_t[];
  char jid[]="jid";
  char *n_name = NULL;
  char *c_name=NULL;
  char *c_nick=NULL;
  char *c_pass=NULL;

  while(elem)
  {
    n_name = elem->name;
    if(!n_name)return;
    if(!strcmp(n_name, conference_t))  // Элемент конференции
    {
      c_name = XML_Get_Attr_Value(jid,elem->attr);
      tmpnode = XML_Get_Child_Node_By_Name(elem, "nick");
      if(!tmpnode)return;
      c_nick = tmpnode->value;
      if(!c_nick)return;
      tmpnode = XML_Get_Child_Node_By_Name(elem, "password");
      if(tmpnode)
      {
        c_pass = tmpnode->value;
      }

      // Создаём очередной элемент списка
      BM_ITEM *bmitem = malloc(sizeof(BM_ITEM));
      bmitem->mucname = malloc(strlen(c_name)+1);
      strcpy(bmitem->mucname, c_name);

      bmitem->nick = malloc(strlen(c_nick)+1);
      strcpy(bmitem->nick, c_nick);

      if(c_pass)
      {
        bmitem->pass = malloc(strlen(c_pass)+1);
        strcpy(bmitem->pass, c_pass);
      }
      else bmitem->pass = NULL;
      bmitem->next = NULL;

      BM_ITEM *tmp=BM_ROOT;
      if(tmp)
        while(tmp->next)tmp = tmp->next;
      if(tmp)
      {
        tmp->next = bmitem;
      }
      else BM_ROOT = bmitem;
    }
    elem = elem->next;
  }

  Disp_BM_Menu();
}

//Context:HELPER
void _getbookmarkslist()
{
  static char priv_id[]="SieJC_priv_req";
  static char bm[]="<storage xmlns='storage:bookmarks'/>";
  char gget[]=IQTYPE_GET;
  char iqv[]=IQ_PRIVATE;
  SendIq(NULL, gget, priv_id, iqv, bm);
}

void Get_Bookmarks_List()
{
  if(!BM_ROOT)
  {
    SUBPROC((void*)_getbookmarkslist);
  }
  else Disp_BM_Menu();
}

/*
 Обработка входящих Iq-запросов
*/
void Process_Iq_Request(XMLNode* nodeEx)
{
  char* iqtype = NULL;
  char* id = NULL;
  char *from = NULL;
  char gget[]=IQTYPE_GET;
  char gres[]=IQTYPE_RES;
  char gerr[]=IQTYPE_ERR;
  char gset[]=IQTYPE_SET;
  const char iq_version[]=IQ_VERSION;

  iqtype = XML_Get_Attr_Value("type",nodeEx->attr);
  from = XML_Get_Attr_Value("from",nodeEx->attr);
  id = XML_Get_Attr_Value("id",nodeEx->attr);

// Проверяем наличие обязательных атрибутов
if(!iqtype) return;

if(!strcmp(gget,iqtype)) // Iq type = get
{
  XMLNode* query;
  if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;
  char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
  if(!q_type)return;
  // Тут мы знаем XMLNS поступившего запроса
  if(!strcmp(q_type,iq_version))
  {
    // jabber:iq:version
    if(from)
    {
        // Создаем переменные, чтобы в них записать данные
        // и безопасно уничтожить в HELPER

      char* loc_id = NULL;
      if(id)
        {
          loc_id=malloc(strlen(id)+1);
          strcpy(loc_id,id);
        }
        char* loc_from=malloc(strlen(from)+1);
        strcpy(loc_from,from);
        SUBPROC((void*)Report_VersionInfo,loc_id, loc_from);

    }
  }
}


// Обработка  Iq type = result
if(!strcmp(gres,iqtype))
{
  char bind_id[]="SieJC_bind_req";
  char sess_id[]="SieJC_sess_req";

  if(!strcmp(id, bind_id))
  {
    Jabber_state = JS_SASL_SESS_INIT_ACK;
    SASL_Init_Session();
  }

  if(!strcmp(id,auth_id) || !strcmp(id, sess_id))   // Авторизация либо конец инициализации сессии
  {
    Jabber_state = JS_AUTH_OK;
    CList_AddContact(My_JID, "(Me)", SUB_BOTH,0,0);
    SUBPROC((void*)Send_Roster_Query);
  }

  if(!strcmp(id,rost_id))   // Запрос ростера
  {
    XMLNode* query;
    if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;
    char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
    if(!q_type)return;
    if(!strcmp(q_type,IQ_ROSTER))
    {
      // jabber:iq:roster
      FillRoster(query->subnode);
        // Через секунду запросим презенсы
      extern GBSTMR TMR_Send_Presence;
      GBS_StartTimerProc(&TMR_Send_Presence, TMR_SECOND*1, Send_Presence_MMIStub);
    }
  }

  if(!strcmp(id,vreq_id))   // Запрос версии (ответ)
  {
    XMLNode* query;
    if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;
    char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
    if(!q_type)return;
    if(!strcmp(q_type,IQ_VERSION))
    {
      char no_os[]="(no data)";
      char* vers_os_str;
      XMLNode *cl_name=XML_Get_Child_Node_By_Name(query, "name");
      XMLNode *cl_version=XML_Get_Child_Node_By_Name(query, "version");
      XMLNode *cl_os=XML_Get_Child_Node_By_Name(query, "os");
      if(cl_os)
      {
        vers_os_str = cl_os->value;
      }
      else
      {
        vers_os_str=no_os;
      }
      //Формируем сообщение
      char *reply=malloc(512);
      snprintf(reply, 512,"Version Info:\nName:%s\nVersion:%s\nOS:%s",cl_name->value, cl_version->value, vers_os_str);
      CList_AddMessage(from, MSG_SYSTEM, reply);
      ShowMSG(1,(int)reply);
      mfree(reply);
    }

  }


  if(!strcmp(id,priv_id))
  {
    XMLNode* query;
    if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;
    char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
    if(!q_type)return;
    if(!strcmp(q_type,IQ_PRIVATE))
    {
      XMLNode *bm = XML_Get_Child_Node_By_Name(query, "storage");
      if(!bm)return;
      char *xmlns = XML_Get_Attr_Value("xmlns", bm->attr);
      if(!xmlns)return;
      if(!strcmp(xmlns, XMLNS_BOOKMARKS))Process_Bookmarks_Storage(bm);
    }
  }

}

// Обработка  Iq type = set
if(!strcmp(gset,iqtype))
{
    XMLNode* query;
    if(!(query = XML_Get_Child_Node_By_Name(nodeEx, "query")))return;
    char* q_type = XML_Get_Attr_Value("xmlns", query->attr);
    if(!q_type)return;

    if(!strcmp(q_type,IQ_ROSTER))
    {
      // jabber:iq:roster
      ChangeRoster(query->subnode);
    }
}


if(!strcmp(gerr,iqtype)) // Iq type = error
{

  // Анализируем ошибку
  XMLNode* error = XML_Get_Child_Node_By_Name(nodeEx, "error");
  if(!error)return;
  char* errcode = XML_Get_Attr_Value("code", error->attr);
//  Jabber_state = JS_ERROR;
  if(errcode)sprintf(logmsg,"ERR:%s",errcode);
  MsgBoxError(1,(int)logmsg);
  if(!strcmp(id,auth_id))
  {
    Jabber_state = JS_AUTH_ERROR;
    strcat(logmsg,"\nAuth error!");
  }

}

}

/*
Презенсы :)
*/
void Process_Presence_Change(XMLNode* node)
{
  CONF_PRIV priv;
  char Req_Set_Role=0;
  char* from = XML_Get_Attr_Value("from",node->attr);
  if(!from)return;

  char status;
  char* msg=NULL;
  char* pr_type = XML_Get_Attr_Value("type",node->attr);
  if(pr_type)
  {
    status = GetPresenceIndex(pr_type);
  }
  else
  {
    XMLNode* status_node = XML_Get_Child_Node_By_Name(node,"show");
    if(!status_node)
    {
      status = PRESENCE_ONLINE;
    }
    else
    {
      status = GetPresenceIndex(status_node->value);
    }

    XMLNode* statusmsg_node = XML_Get_Child_Node_By_Name(node,"status");
    if(statusmsg_node)msg = statusmsg_node->value;
  }

   // Предусматриваем случай, что послано нам что-то от конференции. Это важно.
    XMLNode* x_node = XML_Get_Child_Node_By_Name(node,"x");
    if(x_node)
    {
    if(!strcmp(XML_Get_Attr_Value("xmlns", x_node->attr), XMLNS_MUC)) // Послано от конференции
    {
      CLIST* Conference = CList_FindContactByJID(from);
      // Получаем дочерний узел error (ибо нацелены на обработку именно ошибок)
      XMLNode* err_node = XML_Get_Child_Node_By_Name(node,"error");
      if(err_node)  // Есть ошибка!
      {
        // Хочу текст ошибки
        XMLNode* err_desc = XML_Get_Child_Node_By_Name(err_node,"text");
        if(err_desc->value)msg = err_desc->value;
        MsgBoxError(1,(int)err_desc->value);
        CList_AddSystemMessage(Conference->JID,PRESENCE_OFFLINE, err_desc->value);
      }

    }
// Иар заебал
char loc_actor[]="actor";
char loc_jid[]="jid";
char loc_reason[]="reason";
#define MAX_STATUS_LEN 512
static char r[MAX_STATUS_LEN];       // Статик, чтобы не убило её при завершении процедуры

    if(!strcmp(XML_Get_Attr_Value("xmlns", x_node->attr), XMLNS_MUC_USER)) // Послано от конференции в пользователя
    {

      // Получим экземпляр конфы, в которой всё происходит
      CLIST* Conference = CList_FindContactByJID(from);
      if(!Conference)
      {
        return;
      }
      char* nick = Get_Resource_Name_By_FullJID(from);

      // Тут можно обрабатывать события входа/выхода в конфу
      // Ибо сообщается, кто вошёл (модер ли, админ...)
      XMLNode* item = XML_Get_Child_Node_By_Name(x_node,"item");
      if(status!=PRESENCE_OFFLINE) // Вход с любым статусом
      {
        TRESOURCE* ResEx = CList_IsResourceInList(from);
        char* affiliation = XML_Get_Attr_Value("affiliation", item->attr);
        char* role =  XML_Get_Attr_Value("role", item->attr);
        priv.aff = (JABBER_GC_AFFILIATION)GetAffRoleIndex(affiliation);
        priv.role = (JABBER_GC_ROLE)GetAffRoleIndex(role);

        if(ResEx)
        {
        if(ResEx->status!=PRESENCE_OFFLINE)
        {
          if(!(ResEx->muc_privs.aff==priv.aff && ResEx->muc_privs.role==priv.role))
          {
            sprintf(r, "%s is now %s and %s [%d->%d, %d->%d]", nick, affiliation, role, ResEx->muc_privs.aff, priv.aff, ResEx->muc_privs.role, priv.role);
            Req_Set_Role = 1;
          }
          else
          {
            if(msg)
            {
              snprintf(r, MAX_STATUS_LEN, "%s changed status to %s (%s)", nick, PRESENCES[status], msg);
            }
            else
            {
              sprintf(r, "%s changed status to %s", nick, PRESENCES[status]);
            }
          }
        }
        else
        {
          sprintf(r, "%s joined as %s and %s", nick, affiliation, role);
          Req_Set_Role = 1;
        }

        }
        else
        {
          sprintf(r, "%s joined as %s and %s", nick, affiliation, role);
          Req_Set_Role = 1;
        }

        CList_AddSystemMessage(Conference->JID,PRESENCE_ONLINE, r);
      }


      if(status==PRESENCE_OFFLINE) // Выход
      {
        sprintf(r, "%s left us :(", nick);
        CList_AddSystemMessage(Conference->JID,PRESENCE_OFFLINE, r);
        priv.role = ROLE_NONE;
        Req_Set_Role = 1;
      }


      // Получаем дочерний узел статуса
      XMLNode* sstatus = XML_Get_Child_Node_By_Name(x_node,"status");
      if(sstatus)  // Есть статус!
      {
        // Получаем код статуса
        char* st_code=XML_Get_Attr_Value("code", sstatus->attr);
        sprintf(r,"%s - [Unknown action]",nick);
        // Разные коды статусов - разное варенье:)
        if(!strcmp(st_code, MUCST_KICKED)) sprintf(r, MUCST_R_KICK,nick); // Сообщение о кике
        if(!strcmp(st_code, MUCST_BANNED)) sprintf(r, MUCST_R_BAN, nick); // Сообщение о бане
        if(!strcmp(st_code, MUCST_CHNICK)) sprintf(r, MUCST_R_CHNICK, nick,  XML_Get_Attr_Value("nick", item->attr)); // Сообщение о смене ника
        //sprintf(r,r,nick);
        XMLNode* item = XML_Get_Child_Node_By_Name(x_node,"item");
        if(item)
        {
          XMLNode* actor = XML_Get_Child_Node_By_Name(item,loc_actor); // Вдруг сервис был настолько любезен, что соообщил исполнителя
          if(actor)strcat(r, XML_Get_Attr_Value(loc_jid, actor->attr));

          XMLNode* reason = XML_Get_Child_Node_By_Name(item,loc_reason); // А вдруг нам даже сказали, за что?
          if(reason)
          {
            strcat(r, "; Reason: ");
            int l = strlen(r);
            if(MAX_STATUS_LEN-l-1>0)strncat(r, reason->value, MAX_STATUS_LEN-l-1);
          }


        }
        char *ansi_r = convUTF8_to_ANSI_STR(r);
        MsgBoxError(1,(int)ansi_r);
        mfree(ansi_r);
        CList_AddSystemMessage(Conference->JID,status, r);
        msg = r;
      }

    }

    }
  CList_AddResourceWithPresence(from, status, msg);
  if(Req_Set_Role) CList_MUC_SetRole(from, priv);
}

MESS_TYPE Get_Message_Type(char* mess_type_str)
{
  char m_chat[]=MSGSTR_CHAT;
  char m_gchat[]=MSGSTR_GCHAT;
  if(!mess_type_str)return MSG_NORMAL;
  if(!strcmp(mess_type_str,m_chat ))return MSG_CHAT;
  if(!strcmp(mess_type_str,m_gchat ))return MSG_GCHAT;
  return MSG_NORMAL;
}

//Context: HELPER
void _mucadmincmd(char* room, char* iq_payload)
{
  //char* to, char* type, char* id, char* xmlns, char* payload
  char admin_iqid[]="SieJC_adm";
  char iqtypeset[]=IQTYPE_SET;
  SendIq(room, iqtypeset, admin_iqid, XMLNS_MUC_ADMIN, iq_payload);
  mfree(iq_payload);
  mfree(room);
}

// Исполнение административных команд
void MUC_Admin_Command(char* room_name, char* room_jid, MUC_ADMIN cmd, char* reason)
{
  char* payload = malloc(1024);
  char *_room_name = malloc(strlen(room_name)+1);
  strcpy(_room_name, room_name);
  char payload_tpl[]="<item nick='%s' %s='%s'><reason>%s</reason></item>";
  char it[20];
  char val[20];
  char aff[]="affiliation";
  char role[]="role";
  switch(cmd)
  {
  case ADM_KICK:
    {
      strcpy(it,role);
      strcpy(val,JABBER_ROLS[ROLE_NONE]);
      break;
    }
  case ADM_VOICE_REMOVE:
    {
      strcpy(it,role);
      strcpy(val,JABBER_ROLS[ROLE_VISITOR]);
      break;
    }
  case ADM_VOICE_GRANT:
    {
      strcpy(it,role);
      strcpy(val,JABBER_ROLS[ROLE_PARTICIPANT]);
      break;
    }

  case ADM_BAN:
    {
      strcpy(it,aff);
      strcpy(val,JABBER_AFFS[AFFILIATION_OUTCAST]);
      break;
    }

  }

  snprintf(payload, 1023, payload_tpl, room_jid, it, val, reason);
//  ShowMSG(1,(int)payload);
  SUBPROC((void*)_mucadmincmd, _room_name, payload);
}

/*
Входящие сообщения
*/
void Process_Incoming_Message(XMLNode* nodeEx)
{
  char Is_subj=0;
  XMLNode* msgnode = XML_Get_Child_Node_By_Name(nodeEx,"body");
  if(!msgnode)
  {
    msgnode = XML_Get_Child_Node_By_Name(nodeEx,"subject");
    Is_subj = 1;
  }
  if(msgnode)
  if(msgnode->value)
  {
    MESS_TYPE msgtype = Get_Message_Type(XML_Get_Attr_Value("type",nodeEx->attr));

    // Не показываем попапы для групчата, ибо достаёт трындец как
    if(msgtype!=MSG_GCHAT)
    {
      char* m = malloc(128+5+strlen(msgnode->value));
      sprintf(m,"%s: %s", XML_Get_Attr_Value("from",nodeEx->attr), msgnode->value);
      char *ansi_m=convUTF8_to_ANSI_STR(m);
      ShowMSG(1,(int)ansi_m);
      mfree(m);
      mfree(ansi_m);
    }
    if(Is_subj)
    {
      msgtype = MSG_SUBJECT;
    }
    CList_AddMessage(XML_Get_Attr_Value("from",nodeEx->attr), msgtype, msgnode->value);
    extern volatile int vibra_count;
    Vibrate(1);
  }
}

/*
Получить внутренний номер данного типа присутствия по строке с присутсвием
*/
unsigned short GetPresenceIndex(char* presence_str)
{
  if(!presence_str)return 0;
  for(int i=0;i<PRES_COUNT;i++)
  {
    if(!strcmp(presence_str, PRESENCES[i]))return i;
  }
  return 0;
}

/*
Получить внутренний номер данного типа роли/полномочий по строке с присутсвием
*/
unsigned short GetAffRoleIndex(char* str)
{
  if(!str)return 0;
  int i;
  for(i=0;i<AFFS_CNT;i++)
  {
    if(!strcmp(str, JABBER_AFFS[i]))return i;
  }
  for(i=0;i<ROLS_CNT;i++)
  {
    if(!strcmp(str, JABBER_ROLS[i]))return i;
  }

  return 0;
}


/*
    Ниже функции авторизации для SASL-метода
*/

// Сообщить серверу об использовании аунтитификации MD5-DIGEST
//Context:HELPER
void Use_Md5_Auth_Report()
{
  char s[]="<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>";
  Jabber_state = JS_SASL_NEGOTIATION;
  SendAnswer(s);
}

// Открываем новый поток к серверу по окончании авторизации
void SASL_Open_New_Stream()
{
  Jabber_state = JS_SASL_NEW_STREAM_ACK;
  SUBPROC((void*)Send_Welcome_Packet_SASL);
}

// Выполняем Resource Binding
void SASL_Bind_Resource()
{
  sprintf(My_JID, "%s@%s",USERNAME, JABBER_SERVER);
  sprintf(My_JID_full,"%s/%s",My_JID, RESOURCE);

  sprintf(logmsg, "Resource binding");
  REDRAW();
static char bind_tpl[]="<iq type='set' id='SieJC_bind_req'>"
                  "<bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'>"
                  "<resource>SieJC</resource>"
                  "</bind>"
                  "</iq>";
  Jabber_state = JS_SASL_RESBIND_ACK;
  SUBPROC((void*)SendAnswer, bind_tpl);
}

// Инициализация сессии
void SASL_Init_Session()
{
  sprintf(logmsg, "Session Init");
  REDRAW();

static char sess_init_tpl[]="<iq type='set' id='SieJC_sess_req'>"
                  "<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/>"
                  "</iq>";

  Jabber_state = JS_SASL_SESS_INIT_ACK;
  SUBPROC((void*)SendAnswer, sess_init_tpl);
}

void Decode_Challenge(char *challenge)
{
  char *decoded_challenge = malloc(1024);
  base64_decode(challenge, decoded_challenge);
  SASL_Auth_data.nonce = Get_Param_Value(decoded_challenge, "nonce",1);
  SASL_Auth_data.qop   = Get_Param_Value(decoded_challenge, "qop",1);

//SASL_Auth_data.nonce = malloc(128);strcpy(SASL_Auth_data.nonce,"455564019");
//SASL_Auth_data.qop = malloc(128);strcpy(SASL_Auth_data.qop,"auth");

  mfree(decoded_challenge);
  char *cnonce= malloc(60);
  strcpy(cnonce, "7425da72aliuf242765");
  SASL_Auth_data.cnonce = cnonce;
}

void mkhex(md5_byte_t digest[16], char *hex_output)
{
  for (int di = 0; di < 16; ++di)sprintf(hex_output + di * 2, "%02x", digest[di]);
}



//Context:HELPER
void _sendandfree(char *str)
{
  SendAnswer(str);
  mfree(str);
}

char ans[]="<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>";

void Process_Auth_Answer(char *challenge)
{
  char *decoded_challenge = malloc(256);
  base64_decode(challenge, decoded_challenge);
  SASL_Auth_data.rsp_auth   = Get_Param_Value(decoded_challenge, "rspauth",0);
  mfree(decoded_challenge);
  Jabber_state = JS_SASL_AUTH_ACK;
  SUBPROC((void*)SendAnswer,ans);
}


void Send_Login_Packet()
{
  md5_state_t state;
  md5_byte_t digest[16];
  md5_byte_t  A1[16], A2[16], Response[16];
  char colon_t[]=":";
  char _00000001[]="00000001";
  char hex_output[16*2 + 1];
  char A1_HEX[16*2 + 1];
  char A2_HEX[16*2 + 1];
  char R_HEX[16*2 + 1];

  char *digest_uri = malloc(128);
  char realm[]="";
  char *User_Realm_Pass = malloc(256);
  zeromem(digest_uri, 128);
  snprintf(digest_uri, 127, "AUTHENTICATE:xmpp/%s", JABBER_SERVER);

  md5_init(&state);
  md5_append(&state, (const md5_byte_t *)USERNAME, strlen(USERNAME));
  md5_append(&state, (const md5_byte_t *)colon_t,1);
  md5_append(&state, (const md5_byte_t *)realm, strlen(realm));
  md5_append(&state, (const md5_byte_t *)colon_t,1);
  md5_append(&state, (const md5_byte_t *)PASSWORD, strlen(PASSWORD));
  md5_finish(&state, digest);
  mkhex(digest, hex_output);

  md5_init(&state);
  md5_append(&state, (const md5_byte_t *)digest, 16);      // (MD5(user:realm:pass)
  md5_append(&state, (const md5_byte_t *)colon_t,1);            // :
  md5_append(&state, (const md5_byte_t *)SASL_Auth_data.nonce,strlen(SASL_Auth_data.nonce));
  md5_append(&state, (const md5_byte_t *)colon_t,1);            // :
  md5_append(&state, (const md5_byte_t *)SASL_Auth_data.cnonce,strlen(SASL_Auth_data.cnonce));
  md5_finish(&state, A1);
  mkhex(A1, A1_HEX);

  md5_init(&state);
  md5_append(&state, (const md5_byte_t *)digest_uri, strlen(digest_uri));
  md5_finish(&state, A2);
  mkhex(A2, A2_HEX);

  md5_init(&state);
  md5_append(&state, (const md5_byte_t *)A1_HEX, strlen(A1_HEX));
  md5_append(&state, (const md5_byte_t *)colon_t,1);
  md5_append(&state, (const md5_byte_t *)SASL_Auth_data.nonce,strlen(SASL_Auth_data.nonce));
  md5_append(&state, (const md5_byte_t *)colon_t,1);
  md5_append(&state, (const md5_byte_t *)_00000001, strlen(_00000001));
  md5_append(&state, (const md5_byte_t *)colon_t,1);
  md5_append(&state, (const md5_byte_t *)SASL_Auth_data.cnonce,strlen(SASL_Auth_data.cnonce));
  md5_append(&state, (const md5_byte_t *)colon_t,1);
  md5_append(&state, (const md5_byte_t *)SASL_Auth_data.qop,strlen(SASL_Auth_data.qop));
  md5_append(&state, (const md5_byte_t *)colon_t,1);
  md5_append(&state, (const md5_byte_t *)A2_HEX, strlen(A2_HEX));
  md5_finish(&state, Response);
  mkhex(Response, R_HEX);

  char *Response_STR = malloc(1024);
  zeromem(Response_STR, 1024);
  char Res_tpl[]=
    "username=\"%s\",realm=\"%s\",nonce=\"%s\",nc=00000001,cnonce=\"%s\","
                "qop=auth,digest-uri=\"xmpp/%s\",response=\"%s\",charset=utf-8";
  snprintf(Response_STR, 1024, Res_tpl,
           USERNAME,
           realm,
           SASL_Auth_data.nonce,
           SASL_Auth_data.cnonce,
           JABBER_SERVER,
           R_HEX
           );
  char *Result_Resp;
  base64_encode(Response_STR, strlen(Response_STR),&Result_Resp);
  char resp_full_tpl[]="<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>%s</response>";
  char *resp_full = malloc(1024);
  zeromem(resp_full, 1024);
  snprintf(resp_full, 1023, resp_full_tpl, Result_Resp);
  Jabber_state=JS_SASL_NEG_ANS_WAIT;
  SUBPROC((void*)_sendandfree, resp_full);
  mfree(Result_Resp);
  mfree(Response_STR);
  mfree(digest_uri);
  mfree(User_Realm_Pass);
}

// Обработка ошибок SASL
void SASL_Process_Error(XMLNode *nodeEx)
{
  Jabber_state=JS_ERROR;
  XMLNode *err = nodeEx->subnode;
  if(err)
  {
    strcpy(logmsg, "SASL error!\n");
    strcat(logmsg, err->name);  // Не юзер-френдли
    if(!strcmp(err->name, SASLERR_NOTAUTH))strcat(logmsg, "\nBad password");
  }
}
////////////////////////////////////////////////////////////////////////////////

// Запрос компрессии у сервера
void Compression_Ask()
{
static  char zlib_ask[]="<compress xmlns='http://jabber.org/protocol/compress'>"
                  "<method>zlib</method>"
                  "</compress>";
  Jabber_state = JS_ZLIB_INIT_ACK;
  SUBPROC((void*)SendAnswer,zlib_ask);
  strcat(logmsg, "\nUsing ZLib ack");
}

//Context:HELPER
void Compression_Send_Header()
{
  char cMethod = 8;
  char cInfo = 3;
  char cm = (char) (cMethod | (cInfo << 4));
  extern int sock;
  send(sock,&cm,1,0);
  char flags = 0;// bez dictu a fastest
  if ((cm * 256 + flags) % 31 != 0)
  {
    flags += 31 - ((cm * 256 + flags) % 31);
  }
  send(sock,&flags,1,0);
}


//Context:HELPER
void Send_New_stream()
{
  Jabber_state = JS_ZLIB_STREAM_INIT_ACK;
  Compression_Send_Header();
  Send_Welcome_Packet_SASL();
}

//GBSTMR Newstream;
// Инициализация нового потока (уже сжатого)
void Compression_Init_Stream()
{
  strcat(logmsg, "\nOK, ZLib enable...");
  extern char Is_Compression_Enabled;
  Is_Compression_Enabled = 1;
  SUBPROC((void*)Send_New_stream);
}
// EOL,EOF
