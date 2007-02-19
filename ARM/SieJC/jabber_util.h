/*
    SieNatJabber Project
    Функции работы с контакт-листом

    Специфический для джаббера разбор XML
*/

#ifndef _JABBER_UTIL_H_
  #define _JABBER_UTIL_H_

#include "xml_parser.h"

//#define MAX_MSG_LEN 4096
#define MAX_MSG_LEN 128 // Ага! Мало? Мне тоже мало! Позже пофиксим...

#define PRES_COUNT 12
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
void Send_Presence(short priority, char status, char* message);

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

// kibab612@jabber.ru/SieJC  => SieJC
char* Get_Resource_Name_By_FullJID(char* full_jid);

/*
Получить внутренний номер данного типа присутствия по строке с присутсвием
*/
unsigned short GetPresenceIndex(char* presence_str);

void SendMessage(char* jid, char* body);

// UTF8 для джаббера
char* Correct_UTF8_String(char* utf8_jid);

// А это вообще чтобы всем было хорошо (с) Чеботарёв А.А.
char* ANSI2UTF8(char* ansi_str, unsigned int maxlen);
#endif
