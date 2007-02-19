#include "../inc/swilib.h"
#include "history.h"
#include "main.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "string_util.h"
#include "xml_parser.h"
#include "jabber.h"


extern const char JABBER_SERVER[];
extern const char USERNAME[];  
extern const char PASSWORD[];
extern const char RESOURCE[];
extern const char CMP_DATE[];
extern const char VERSION_NAME[];
extern const char VERSION_VERS[];
extern const char OS[];
extern char My_JID_full[];
extern char My_JID[];
extern char logmsg[];

extern JABBER_STATE Jabber_state;

const char* PRESENCES[PRES_COUNT] = {"online", "unavailable", "error", "chat", "away", "xa", "dnd", "invisible",
                                      "subscribe", "subscribed", "unsubscribe", "unsubscribed"};
const unsigned short PRES_COLORS[PRES_COUNT]  = {15,        21,             14,     16,     3,      18,   2,      20, 21,21,21,21}; //цвет оффлайнов изменил


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
    to =  ANSI2UTF8(to, 128);
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
  char streamheader[]="<?xml version='1.0' encoding='UTF-8'?>\n<stream:stream to='%s' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' xml:lang='en'>";
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

/*
  Послать дисконнект
*/
// Context:HELPER
void Send_Disconnect()
{
//  connect_state = 0;
  char streamend[]="</stream:stream>";
  SendAnswer(streamend);
  strcpy(logmsg,"Send Disconnect");
#ifdef LOG_ALL
  Log("CONN",logmsg);
#endif 
}

// Константы Iq-запросов авторизации и ростера
char auth_id[] = "SieJC_auth_req";
char rost_id[] = "SieJC_rost_req";

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

void Send_VReq()
{
  char to[]="kibab612@jabber.ru/Miranda_Home";
  char typ[]="get";
  char iqid[]="SieJC_VR";
  char iq_v[]=IQ_VERSION;
  SendIq(to, typ, iqid, iq_v, NULL);
  return;
}


/*
  Послать своё присутствие (в частности, после этого на нас вываливаются 
  присутствия остальных, а мы появляемся в ресурсах своего контакта)

Параметр message НЕ ДОЛЖЕН уничтожаться в вызывающем контексте!
*/
// Context: HELPER
void Send_Presence(short priority, char status, char* message)
{
  extern char My_Presence;
  My_Presence = status;
  char presence_template[]="<presence><priority>%d</priority><show>%s</show><status>%s</status></presence>";
  char* presence = malloc(1024);
  snprintf(presence,1024,presence_template, priority, PRESENCES[status], message);
  SendAnswer(presence);
  mfree(presence);
  mfree(message);
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

/*
  Обеспечивает преобразование кривого UTF-8 Сименса в UTF-8 для Jabber
*/
char* Correct_UTF8_String(char* utf8_str)
{
  int l = strlen(utf8_str)*2;
  // ^ так нельзя делать цикл, строка на самом длиннее, чем strlen
  int j=0;
  int i=0;
  char character = *utf8_str;
  while(character!='\0')
  {
    if(character!=0x1F)
    {
      utf8_str[j]=character;
      j++;      
    }
    i++;
    character = *(utf8_str+i);
  }
  utf8_str[j]='\0';
  utf8_str = realloc(utf8_str, j+1);
  return utf8_str;
}


char* ANSI2UTF8(char* ansi_str, unsigned int maxlen)
{
  WSHDR* ws_str = AllocWS(maxlen);
  ascii2ws(ws_str, ansi_str);
  char* utf8_str = malloc(maxlen*2+1);
  ws_2str(ws_str, utf8_str, maxlen);
  FreeWS(ws_str);
  utf8_str = Correct_UTF8_String(utf8_str);
  return utf8_str;
}

// Context: HELPER
void SendMessage(char* jid, char* body)
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
 
  // Извратимся с JID 
//  char IsNonLatin=0; // Нет латинских символов
//  int jlen=strlen(jid);
//  for(int i=0;i<jlen;i++)
//  {
//    if(*(jid+i)>127)IsNonLatin=1;
//  }

  char* utf8_jid = ANSI2UTF8(jid, 128);
  
  body = Correct_UTF8_String(body);
  char mes_template[]="<message to='%s' id='SieJC_%d' type='chat'><body>%s</body></message>";
  char* msg_buf = malloc(MAX_MSG_LEN*2+200);
  sprintf(msg_buf, mes_template, utf8_jid, m_num, body);
  mfree(body);
  mfree(utf8_jid);
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
  sprintf(answer, "<name>%s</name><version>%s (compile date: %s)</version><os>%s</os>", VERSION_NAME, VERSION_VERS, CMP_DATE, OS);
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
  char ansi_msg[]="Тута я :)!";
  char *message = ANSI2UTF8(ansi_msg, strlen(ansi_msg));
  Send_Presence(16, PRESENCE_ONLINE, message);
}

// пока это вместо всего:)))))
//Context: HELPER
void Send_Away_Presence_Helper()
{
  TTime now_time;
  TDate now_date;
  GetDateTime(&now_date,&now_time);
  char away_msg_template[]="Отсутствую с %02d:%02d, буду позже.";
  snprintf(away_msg_template,strlen(away_msg_template), away_msg_template, now_time.hour,now_time.min);
  char *message = ANSI2UTF8(away_msg_template, strlen(away_msg_template));
  Send_Presence(-16, PRESENCE_AWAY, message);
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
    name = XML_Get_Attr_Value("name",rostEx->attr);//convUTF8_to_ANSI(XML_Get_Attr_Value("name",rostEx->attr));  
    w_subscr = XML_Get_Attr_Value("ask",rostEx->attr);
    if(w_subscr)
    {
      w_subscr_flag = !strcmp(w_subscr, "subscribe") ? 1: 0;
    }
    else
    {
      w_subscr_flag = 0;
    }
    CList_AddContact(XML_Get_Attr_Value("jid",rostEx->attr),
                          name,
                          r_subscr,
                          w_subscr_flag,
                          0
                          );
   //if(name)mfree(name);
    
   rostEx=rostEx->next;
   i++;
  }
  // Через секунду запросим презенсы
  extern GBSTMR TMR_Send_Presence;
  GBS_StartTimerProc(&TMR_Send_Presence, TMR_SECOND*1, Send_Presence_MMIStub);
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
  if(!strcmp(id,auth_id))   // Авторизация
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
  Jabber_state = JS_ERROR;
  if(errcode)sprintf(logmsg,"ERR:%s",errcode);
  ShowMSG(1,(int)logmsg);
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
  CList_AddResourceWithPresence(from, status, msg);
}

/*
Входящие сообщения
*/
void Process_Incoming_Message(XMLNode* nodeEx)
{
  XMLNode* msgnode = XML_Get_Child_Node_By_Name(nodeEx,"body");
  if(msgnode)
  if(msgnode->value)
  {
    char* m = malloc(100+strlen(msgnode->value));
    sprintf(m,"%s: %s", XML_Get_Attr_Value("from",nodeEx->attr), msgnode->value);
    ShowMSG(1,(int)m);
    mfree(m);
    CList_AddMessage(XML_Get_Attr_Value("from",nodeEx->attr), MSG_CHAT, msgnode->value);
    start_vibra(1);
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
