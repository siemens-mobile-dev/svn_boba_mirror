/*
*===========================================================================
*                       Модуль интерфейса проверки событий
*===========================================================================
*/
#include "..\deleaker\mem2.h"
#include "..\include\Lib_Clara.h"
#include "..\include\Dir.h"
#include "header\structs.h"
#include "header\lng.h"

extern LIST *events;

#pragma swi_number=0x109
__swi __arm  void MMIPROC (void(*PROC)(EVENT *), EVENT *);
void SUBPROC_(void(*PROC)(EVENT *),EVENT *p1, void * p2);

void action_processor(EVENT *ev);

GUI_LIST *g=0;

wchar_t *items[] = {
  L"Сообщение",
  L"Запуск эльфа",
  L"Ярлык",
  L"Java",
  L"Выключение телефона",
  L"Яркость", 
  L"Фонарик: уст. яркость",
  L"Профиль",
  L"Рестарт",
  L"Музыка: воспр.", 
  L"Музыка: воспр. с громкостью",
  L"Обои: установить",
  L"Вибра",
  L"ГЭ: уст. заметку",
  L"ГЭ: уст. оператора",
  L"Закрыть книгу",
  L"Вкл. музыку/радио",
  L"Поменять расписание",
  L"Обновить",
  L"Звук: выкл",
  L"Звук: вкл", 
  L"ИК: вкл",
  L"ИК: выкл", 
  L"ИК: мигнуть",
  L"Фонарик: мигнуть",
  L"(W580): отключить диоды",
  L"(W580): включить диоды", 
  L"Оранж.диод: мигнуть", 
  L"Оранж.диод: вкл. ",
  L"Оранж.диод: выкл",
  L"Важное событие", 
  L"Звонок: уст.громкость",
  L"Звонок: вибрация",
  L"Звонок: уст.файл", 
  L"Звонок: способ приёма",
  L"Звонок: вкл.нарастание",
  L"Звонок: уст. приём вызовов",
  L"Голосовой вызов",
  L"Сменить тему",
  0,0,0
};

int onLBMessage(GUI_MESSAGE * msg)
{
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    int item=GUIonMessage_GetCreatedItemIndex(msg);
    EVENT* evt=(EVENT *)ListElement_GetByIndex(events,item);
    SetMenuItemText0(msg,Str2ID(items[evt->type],0,SID_ANY_LEN));
    break;
  }
  return(1);
};

void OnSelect1Gui(BOOK *bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(g);
  EVENT *ev=(EVENT*)ListElement_GetByIndex(events,item);
  MMIPROC(action_processor,ev);
};

void OnSelect2Gui(BOOK *bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(g);
  wchar_t text[1024];
  EVENT *ev=(EVENT*)ListElement_GetByIndex(events,item);
  snwprintf(text, 1023, L"%d-%d-%d-%d-%d-%d-%d\n\n%d-%d-%d-%d-%d-%d-%d\n\n%d-%d-%d-%d-%d-%d-%d\n%d:%d:%d", ev->days[0], ev->days[1], ev->days[2], ev->days[3], ev->days[4], ev->days[5], ev->days[6], ev->remdays[0], ev->remdays[1], ev->remdays[2], ev->remdays[3], ev->remdays[4], ev->remdays[5], ev->remdays[6], ev->remdays2[0], ev->remdays2[1], ev->remdays2[2], ev->remdays2[3], ev->remdays2[4], ev->remdays2[5], ev->remdays2[6], ev->tm_start, ev->ask_before, ev->ask_after);
  MessageBox(0x6FFFFFFF,Str2ID(text,0,SID_ANY_LEN),0,1,0,0);
};
void OnBackGui(BOOK * bk, GUI* )
{
  GUI_Free(g);
  g=0;
  MyBOOK *mbk=(MyBOOK*)bk;
  mbk->g_test=0;
  BookObj_Hide(bk, 0);
};

GUI_LIST * CreateGuiList(BOOK * book)
{
  GUI_LIST * lo=0;
  if (lo=CreateListObject(book,0))
  {
    SetNumOfMenuItem(lo,events->FirstFree);
    SetCursorToItem(lo,0);
    ListMenu_SetOnMessages(lo,onLBMessage);
    SetMenuItemStyle(lo,3);
    GuiObject_SetTitleText(lo,Str2ID(lng[LNG_EVENTS_TEST],0,SID_ANY_LEN));
    GUIObject_Softkey_SetAction(lo,ACTION_BACK, OnBackGui);
    GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,OnSelect1Gui);
    GUIObject_Softkey_SetAction(lo,0,OnSelect2Gui);
    GUIObject_Softkey_SetText(lo,0,Str2ID(lng[LNG_INFO],0,SID_ANY_LEN));
  }
  return(lo);
};

GUI_LIST *create_ed(BOOK *book)
{
  if (events)
  {
    MyBOOK *mbk=(MyBOOK*)book;
    g=CreateGuiList(book);
    mbk->g_test=g;
    ShowWindow(g);
    BookObj_Show(book, 0);
    BookObj_SetFocus(book, 0);
  }
  return(g);
};
