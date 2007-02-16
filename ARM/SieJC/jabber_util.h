/*
    SieNatJabber Project
    Функции работы с контакт-листом

    Специфический для джаббера разбор XML
*/

#ifndef _JABBER_UTIL_H_
  #define _JABBER_UTIL_H_

#include "xml_parser.h"

/*
  Посылка стандартного Jabber Iq
*/
void SendIq(char* to, char* type, char* id, char* xmlns, char* payload);

/*
  Послать приветствие, на него сервер высылает ответный stream.
  После этого можно общаться с сервером
*/
// Context:HELPER
void Send_Welcome_Packet();

/*
  Послать дисконнект
*/
void Send_Disconnect();

/*
  Авторизация на Jabber-сервере
  Самая тупая, без извращений.
*/
void Send_Auth();

/*
  Послать своё присутствие (в частности, после этого на нас вываливаются 
  присутствия остальных, а мы появляемся в ресурсах своего контакта)
*/
// Context: HELPER
void Send_Presence();

/*
  Послать запрос ростера
*/
// Context: HELPER
void Send_Roster_Query();

/*
 Обработка входящих Iq-запросов
*/
void Process_Iq_Request(XMLNode* nodeEx);


/*
Презенсы :)
*/
void Process_Presence_Change(XMLNode* node);

/*
Входящие сообщения
*/
void Process_Incoming_Message(XMLNode* nodeEx);

char* Get_Resource_Name_By_FullJID(char* full_jid);

#endif
