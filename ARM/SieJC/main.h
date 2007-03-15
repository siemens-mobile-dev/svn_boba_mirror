#ifndef _MAIN_H_
  #define _MAIN_H_

#include "jabber.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////// Некоторые дополнительные настройки проекта ////////////////
//#define LOG_ALL       // Включить протоколирование входящих / исходящих потоков

#define VIBRA_POWER 10 // Сила вибры для событий разного рода

#define USE_PNG_EXT   // Использовать ли расширение PNG для картинок (ELFLoader >2.0)
//#define STD_PALETTE
////////////////////////////////////////////////////////////////////////////////

#define TMR_SECOND 216

// Общие функции модуля MAIN.C

#define XML_BUFFER_SIZE 64*1024 // Размер буфера в байтах
#define REC_BUFFER_SIZE 512 // Размер буфера приёма в байтах
#define UNP_BUFFER_SIZE 100*1024 // Размер буфера распаковки в байтах

#ifdef STD_PALETTE
#define color(x) GetPaletteAdrByColorIndex(x)
#else
#define color(x) (char *)(&(x))
#endif

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
  JS_ZLIB_INIT_ACK,           // Ожидание подтверждения юзания ZLib
  JS_ZLIB_STREAM_INIT_ACK,    // Ожидание ответного ZLib-потока
  JS_SASL_NEGOTIATION,        // Обмен приветствиями SASL
  JS_SASL_NEG_ANS_WAIT,       // Ожидание ответа от сервера на посланный SASL
  JS_SASL_AUTH_ACK,           // Ожидание <success/> после ответа на rspauth
  JS_SASL_NEW_STREAM_ACK,     // Ожидание инициализации нового потока
  JS_SASL_RESBIND_ACK,        // Ожидание привязки ресурса
  JS_SASL_SESS_INIT_ACK,      // Ожидание инициализации сессии
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
  JABBER_GC_AFFILIATION aff;
  JABBER_GC_ROLE role;
}CONF_PRIV;

typedef struct
{
  char* name;                   // Отображаемое имя ресурса
  char* full_name;              // Полный JID (jid/resource)
  RES_TYPE entry_type;          // Тип записи (см. RES_TYPE)
  char status;                  // Статус
  char* status_msg;             // Статусное сообщение
  unsigned int has_unread_msg;  // Есть ли непрочитанные и сколько
  unsigned int total_msg_count; // Общее количество сообщений
  LOG_MESSAGE* log;             // Начало лога сообщений
  CONF_PRIV muc_privs;          // Тут для конференционных контактов - про роли
  void* next;                   // Следующий экземпляр
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
  char* conf_jid; //UTF-8
  void* next;
}MUC_ITEM;

// Для задания своих параметров присутсвия в сети
typedef struct
{
  short priority;
  char status;
  char* message;
}PRESENCE_INFO;

typedef struct 
{
  char r;
  char g;
  char b;
  char a;
}RGBA;

void SendAnswer(char *str);

void start_vibra();
#endif
