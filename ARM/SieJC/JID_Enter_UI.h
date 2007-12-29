/*
    SieNatJabber Project
    Вход в конференцию
*/

#ifndef _JID_UTIL_H_
  #define _JID_UTIL_H_
typedef struct {
  char jid_sub;
  char jid_ask;
  char jid_del;
  char jid_add;
} JIDENTER_SETTINGS;
// Показать диалог Add/Change. если ClEx=NULL то будет добавление контакта
void Disp_JID_Enter_Dialog(CLIST* ClEx);

#endif
