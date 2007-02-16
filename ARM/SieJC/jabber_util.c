#include "../inc/swilib.h"
#include "SieJC.h"
#include "history.h"
#include "main.h"
#include "clist_util.h"
#include "xml_parser.h"
#include "jabber.h"

extern const char JABBER_HOST[];
extern const unsigned int JABBER_PORT;
extern const char USERNAME[];  
extern const char PASSWORD[];
extern const char RESOURCE[];
extern const char VERSION_NAME[];
extern const char VERSION_VERS[];
extern const char OS[];
extern char My_JID_full[];
extern char My_JID[];
extern char logmsg[];

extern JABBER_STATE Jabber_state;


/*
  Посылка стандартного Jabber Iq
*/
//Context: HELPER
void SendIq(char* to, char* type, char* id, char* xmlns, char* payload)
{
  char* xmlq=malloc(1024);
  char *xmlq2=malloc(1024);
  char s_to[40];
  sprintf(xmlq, "<iq type='%s' id='%s' from='%s'", type, id, My_JID_full);
  if(to)
  {
    sprintf(s_to, " to='%s'", to);
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
  sprintf(buf,streamheader,JABBER_HOST);
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

// Эти константы надо будет заменить на динамические
char auth_id[] = "SieJC_auth_req";
char rost_id[] = "SieJC_rost_req";

/*
  Авторизация на Jabber-сервере
  Самая тупая, без извращений.
*/
//Context: HELPER
void Send_Auth()
{
  sprintf(My_JID, "%s@%s",USERNAME, JABBER_HOST);
  sprintf(My_JID_full,"%s/%s",My_JID, RESOURCE);
  char* payload = malloc(256);
  sprintf(payload,"<username>%s</username>\n<password>%s</password>\n<resource>%s</resource>",USERNAME, PASSWORD, RESOURCE);
  SendIq(NULL, "set", auth_id, IQ_AUTH, payload);  
  LockSched();
  strcpy(logmsg,"Send auth");
  UnlockSched();
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif 
}

void Send_VReq()
{
  char to[]="mastermind@jabber.org/Psi";
  char typ[]="get";
  char iqid[]="SieJC_VR";
  char iq_v[]=IQ_VERSION;
  SendIq(to, typ, iqid, iq_v, NULL);
  return;
}


/*
  Послать своё присутствие (в частности, после этого на нас вываливаются 
  присутствия остальных, а мы появляемся в ресурсах своего контакта)
*/
// Context: HELPER
void Send_Presence()
{
  char presence[]="<presence><priority>-100</priority><status></status></presence>";
  SendAnswer(presence);
  LockSched();
  strcpy(logmsg,"Send presence");
  UnlockSched();
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif 
}

/*
  Послать запрос ростера
*/
// Context: HELPER
void Send_Roster_Query()
{
  SendIq(NULL, "get", rost_id, IQ_ROSTER, NULL); 
  LockSched();
  strcpy(logmsg,"Send roster Q");
  UnlockSched();
#ifdef LOG_ALL
  Log("USER->", logmsg);
#endif  
}

// Context: HELPER
void Report_VersionInfo(char* id, char *to)
{
  char answer[100];
 
  sprintf(answer, "<name>%s</name><version>%s</version><os>%s</os>", VERSION_NAME, VERSION_VERS, OS);
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
  char* res_name = (char*)(strstr(full_jid, "/") +1);
  return res_name;
}

void FillRoster(XMLNode* items)
{
  XMLNode* rostEx = items;
  int i=0;
  while(rostEx)
  {
    
    JABBER_SUBSCRIPTION r_subscr=GetSubscrType(XML_Get_Attr_Value("subscription",rostEx->attr));

    CList_AddContact(XML_Get_Attr_Value("jid",rostEx->attr),
                          XML_Get_Attr_Value("name",rostEx->attr),
                          r_subscr,
                          0
                          );
    
   rostEx=rostEx->next;
   i++;
  }
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
  const char iq_version[]=IQ_VERSION;

  iqtype = XML_Get_Attr_Value("type",nodeEx->attr);
  from = XML_Get_Attr_Value("from",nodeEx->attr);
  id = XML_Get_Attr_Value("id",nodeEx->attr);
  
// Проверяем наличие обязательных атрибутов
if(!iqtype || !id) return;

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
        char* loc_id=malloc(strlen(id)+1);
        strcpy(loc_id,id);        
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
    CList_AddContact(My_JID, "(Me)", SUB_BOTH,0);
    SUBPROC((void*)Send_Roster_Query);
    //Send_Presence();
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

if(!strcmp(gerr,iqtype)) // Iq type = error
{

  // Анализируем ошибку
  XMLNode* error = XML_Get_Child_Node_By_Name(nodeEx, "error");
  if(!error)return;
  char* errcode = XML_Get_Attr_Value("code", error->attr);
  Jabber_state = JS_ERROR;
  if(errcode)sprintf(logmsg,"ERR:%s",errcode);

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
  // ВРЕМЕННО, потом надо заменить на код!
  char status = 0;
  char* msg=NULL;
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
  }  
}
