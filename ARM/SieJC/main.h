#ifndef _MAIN_H_
  #define _MAIN_H_

#include "jabber.h"


// Пока желательно оставить так
//#define LOG_ALL

#ifdef NEWSGOLD
#define DEFAULT_DISC '4'
#else
#define DEFAULT_DISC '0'
#endif

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
typedef enum
{
  MSG_ME,     // от меня контакту
  MSG_CHAT,   // от контакта ко мне
  MSG_GCHAT   // конференция
} MESS_TYPE;

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
  char virtual;
  char status;
  char* status_msg;
  unsigned int has_unread_msg;
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
  char group;
  void *next;
} CLIST;


void SendAnswer(char *str);

#endif
