/*
    SieNatJabber Project
    Функции работы с контакт-листом

    Данный API обязан присутствовать во ВСЕХ вариантах контакт-листа
*/

#ifndef _CLIST_UTIL_H_
  #define _CLIST_UTIL_H_

#include "main.h"

#define CLIST_FONT SMALL_FONT   // Шрифт контакт-листа
#define CLIST_Y1 26
// Начало вывода контакт-листа
#define CLIST_F_COLOR_0 1         // Цвет шрифта 
#define CLIST_F_COLOR_1 15         // Цвет шрифта (есть сообщения)

// Отрисовать список контактов
  void CList_RedrawCList();

// Добавить к листу контакт. Возвращает структуру созданного контакта.
  CLIST* CList_AddContact(char* jid,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char group
                          );

// Поменять статус у контакта 
  void CList_Ch_Status(char* jid,
                       unsigned short status,
                       char* status_msg
                       );

// Получить дескриптор контакта по JID
  TRESOURCE* CList_FindContactByJID(char* jid);

// Добавить сообщение в список сообщений контакта
  void CList_AddMessage(char* jid, MESS_TYPE mtype, char* mtext);

  TRESOURCE* CList_AddResourceWithPresence(char* jid, char status, char* status_msg);

// Уничтожить список контактов  
  void CList_Destroy();

// Управление курсором
  void CList_MoveCursorUp();
  void CList_MoveCursorDown();

  void CList_MoveCursorHome();
  void CList_MoveCursorEnd();
  
#endif
