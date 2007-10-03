/*
    SieNatJabber Project
    Функции работы с vCard

    Работа с присланной нодой vCard, сохранение фотки
*/

#ifndef _VCARD_H_
  #define _VCARD_H_

#include "xml_parser.h"

// Обработчик ноды vCard
void Process_vCard(char *from, XMLNode *vCard);

// Показать фотку из вкарда
void vCard_Photo_Display(char *path);

#endif

