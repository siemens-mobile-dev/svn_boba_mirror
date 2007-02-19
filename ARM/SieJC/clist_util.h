/*
    SieNatJabber Project
    Функции работы с контакт-листом

    Данный API обязан присутствовать во ВСЕХ вариантах контакт-листа
*/

#ifndef _CLIST_UTIL_H_
  #define _CLIST_UTIL_H_

#include "main.h"

// Количество символов на строке
#define CHAR_ON_LINE 20           // ad: цвет оффлайнеров

#define CURSOR 13                 // ad: цвет курсора

#define CLIST_FONT SMALL_FONT     // Шрифт контакт-листа
#define CLIST_Y1 20               // ad: меньше сделал (26->20) вроде 

#define CLIST_F_COLOR_0 1         // Цвет шрифта 
#define CLIST_F_COLOR_1 15         // Цвет шрифта (есть сообщения)

// Отрисовать список контактов
  void CList_RedrawCList();

// Получить дескриптор контакта по FullJID (JID вместе с ресурсом)
  CLIST* CList_FindContactByJID(char* jid);

// Добавить к листу контакт. Возвращает структуру созданного контакта.
  CLIST* CList_AddContact(char* jid,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char wants_subscription,
                          char group
                          );

// Изменить параметры контакта (получена подписка и пр)

  void CList_ChangeContactParams(CLIST* Cont_Ex,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char wants_subscription,
                          char group);
  


// Добавить сообщение в список сообщений контакта
  void CList_AddMessage(char* jid, MESS_TYPE mtype, char* mtext);

  TRESOURCE* CList_AddResourceWithPresence(char* jid, char status, char* status_msg);

  TRESOURCE* CList_GetActiveContact();
  
// Уничтожить список контактов  
  void CList_Destroy();

// Изменить режим отображения контактов оффлайн
  void CList_ToggleOfflineDisplay();

// Получить число онлайн-юзверей
  unsigned int CList_GetNumberOfOnlineUsers();

void CList_Display_Popup_Info(TRESOURCE* ResEx);
  
// Управление курсором
  void CList_MoveCursorUp();
  void CList_MoveCursorDown();

  void CList_MoveCursorHome();
  void CList_MoveCursorEnd();
  
#endif
