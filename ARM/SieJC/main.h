#ifndef _MAIN_H_
  #define _MAIN_H_

#include "jabber.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////// Некоторые дополнительные настройки проекта ////////////////
//#define LOG_ALL       // Включить протоколирование входящих / исходящих потоков

#define HAS_MMCARD    // Есть ли флешка у телефона. При включении этой опции
                      // запись логов будет идти на флеш-карту, что быстрее.
#define VIBRA_POWER 10 // Сила вибры для событий разного рода

#define USE_PNG_EXT   // Использовать ли расширение PNG для картинок (ELFLoader >2.0)
#define PATH_TO_PIC "4:\\ZBin\\SieJC\\img\\"
////////////////////////////////////////////////////////////////////////////////

#ifdef HAS_MMCARD
#define DEFAULT_DISC '4'
#else
#define DEFAULT_DISC '0'
#endif

#define TMR_SECOND 216

// Общие функции модуля MAIN.C

#define XML_BUFFER_SIZE 64*1024 // Размер буфера в байтах
#define REC_BUFFER_SIZE 512 // Размер буфера приёма в байтах

typedef struct
{
  int buf_size;
  char* xml_buffer;
} IPC_BUFFER;


typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;

typedef enum
{
  JS_NOT_CONNECTED, 
  JS_CONNECTED_STATE, 
  JS_AUTH_OK, 
  JS_AUTH_ERROR, 
  JS_ONLINE,
  JS_ERROR
} JABBER_STATE;

////////////////////////////////
// Кой-какие типы сообщений из http://www.xmpp.org/rfcs/rfc3921.html и + свои
typedef enum
{
  MSG_NORMAL, // типа "нормал", по дефолту, See RFC
  MSG_ME,     // от меня контакту
  MSG_CHAT,   // от контакта ко мне
  MSG_GCHAT,   // конференция
  MSG_SYSTEM,  // всякого рода сервисные сообщения
  MSG_STATUS   // Статусные сообщения
} MESS_TYPE;


// Типы узлов в списке контактов
typedef enum
{   
  T_NORMAL,       // Обычный (просто ресурс)
  T_VIRTUAL,      // Создаётся при создании контакта, для удаления
  T_CONF_ROOT,    // Корень конференции (место доставки публичных сообщений)
  T_CONF_NODE     // Участник конференции (отображаем только "ресурс" жида) 
}RES_TYPE;

typedef struct
{
  MESS_TYPE mtype;
  char* mess;
  void *next; 
}LOG_MESSAGE;

typedef struct
{
  char* name;
  char* full_name;
  RES_TYPE entry_type;
  char status;
  char* status_msg;
  unsigned int has_unread_msg;
  unsigned int total_msg_count;
  LOG_MESSAGE* log;
  void* next;
} TRESOURCE;

typedef struct
{
  char* JID;
  int ResourceCount;
  TRESOURCE* res_list;
  char* name;
  JABBER_SUBSCRIPTION subscription;
  char wants_subscription;
  char group;     // >128 -> конференции 
  void *next;
} CLIST;


// Для списка конференций
typedef struct
{
  char* conf_jid;
  void* next;
}MUC_ITEM;

void SendAnswer(char *str);

void start_vibra(int lvibra_count);
#endif
