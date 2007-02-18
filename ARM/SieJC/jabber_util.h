/*
    SieNatJabber Project
    Функции работы с контакт-листом

    Специфический для джаббера разбор XML
*/

#ifndef _JABBER_UTIL_H_
  #define _JABBER_UTIL_H_

#include "xml_parser.h"

#define PRES_COUNT 8
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

// kibab612@jabber.ru/SieJC  => SieJC
char* Get_Resource_Name_By_FullJID(char* full_jid);

/*
Получить внутренний номер данного типа присутствия по строке с присутсвием
*/
unsigned short GetPresenceIndex(char* presence_str);

/*
  Преобразование буфера данных из кодировки UTF-8 в ANSI
IN:
  - tmp_out: куда положить результат. Буфер уже должен существовать
             и в нем должно быть достаточно места
  - UTF8_str: откуда брать данные для преобразования
  - size: сколько длина буфера для преобразования (UTF8_str)
  - fact - куда положить итоговый размер данных в буфере

OUT:  результирующий буфер. 
*/
void* convUTF8_to_ANSI(char* tmp_out, char *UTF8_str, unsigned int size, int* fact);

char* convUTF8_to_ANSI_STR(char *UTF8_str);

void SendMessage(char* jid, char* body);
#endif
