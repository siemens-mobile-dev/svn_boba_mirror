#ifndef _MAIN_H_
  #define _MAIN_H_

#ifdef NEWSGOLD
#define DEFAULT_DISC '4'
#else
#define DEFAULT_DISC '0'
#endif

// Общие функции модуля MAIN.C

#define XML_BUFFER_SIZE 64*1024 // Размер буфера в килобабах
#define REC_BUFFER_SIZE 512 // Размер буфера приёма в байтах

typedef struct
{
  int buf_size;
  char* xml_buffer;
} IPC_BUFFER;

typedef enum
{
  JS_NOT_CONNECTED, 
  JS_CONNECTED_STATE, 
  JS_AUTH_OK, 
  JS_AUTH_ERROR, 
  JS_ONLINE
} JABBER_STATE;

/*
From Miranda Jabber_list.h:
*/

typedef enum {
	SUB_NONE,
	SUB_TO,
	SUB_FROM,
	SUB_BOTH
} JABBER_SUBSCRIPTION;

typedef enum {
	AFFILIATION_NONE,
	AFFILIATION_OUTCAST,
	AFFILIATION_MEMBER,
	AFFILIATION_ADMIN,
	AFFILIATION_OWNER
} JABBER_GC_AFFILIATION;

typedef enum {
	ROLE_NONE,
	ROLE_VISITOR,
	ROLE_PARTICIPANT,
	ROLE_MODERATOR
} JABBER_GC_ROLE;
////////////////////////////////
typedef enum
{
  MSG_ME,     // от меня контакту
  MSG_CHAT,   // от контакта ко мне
  MSG_GCHAT   // конференция
} MESS_TYPE;

typedef struct
{
  char mtype;
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

#endif
