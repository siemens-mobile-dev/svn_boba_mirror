#ifndef _MAILCLIENT_H_
  #define _MAILCLIENT_H_
/*
  Типы данных, общие для всех модулей проекта
*/

extern long  strtol (const char *nptr,char **endptr,int base);
extern unsigned long  strtoul (const char *nptr,char **endptr,int base);

extern const char MAIL_LOGIN[];
extern const char MAIL_PASS[];
extern const char EML_PATH[];
extern const int DEFAULT_ACTION;
extern const int IF_DEL_AUX;
extern const char POP3_ADRESS[];
extern const unsigned int POP3_PORT;
extern const char SAVE_AS_FOLDER[];
extern const int DEL_AFTER_LOAD;
extern void kill_data(void *p, void (*func_p)(void *));
extern const char SMSFORWARD[];

extern const char I_UNREAD[];
extern const char I_READ[];
extern const char I_HEADER[];
extern const char I_HEADER_ATT[];
extern const char I_MES_UNREAD_ATT[];
extern const char I_MES_READ_ATT[];
extern const char I_MES_DOWN[];
extern const char I_MES_DEL[];

#define M_LOAD_HEADER 0
#define M_LOAD_FULL 1
#define M_HEADERS_LOADED 2
#define M_FULL_LOADED 3
#define M_DELETE 4

#define POP_GREETING 0
#define POP_VER_LOGIN 1
#define POP_VER_PASS 2
#define POP_GET_STAT 3
#define POP_GET_UIDL_LIST 4
#define POP_GET_LIST 5
#define POP_PROCESS_LIST 6
#define POP_RECEIVE_HEADERS 7
#define POP_RECEIVE_MESSAGE 8
#define POP_DELETE_MESSAGE 9

#define MDB_MAGIC 0x01024244



//IPC messages

#define IPC_PING 0
#define IPC_CHECK_MAILBOX 1
#define IPC_CHANGE_STATE 2
//states:
//1 - elf turn on
//2 - elf turn off
#define IPC_LOADING_FINISHED 3

#pragma pack(1)
typedef struct
{
  unsigned int magic;
  unsigned short uidl_len;
  unsigned char state;
  unsigned char is_read;  
  unsigned int mail_size;
}MAIL_DB;
#pragma pack()

typedef struct
{
  void *next;
  char *uidl;
  unsigned int state;
  unsigned int is_read;
  unsigned int mail_size;
  int num_in_list;
}ML_DAEMON;

typedef struct
{
  void *next;
  char *uidl;
  unsigned int state;
  unsigned int is_read;
  unsigned int is_attach;
  unsigned int mail_size;
  char *header;
  char *subject;
  char *from;
  char *to;
  char *content_type;
  char *content_encoding;
}ML_VIEW;

typedef struct
{
  int connect_state;
  int pop_state;
  int in_pop3;
  int pop3_recv;
  int pop3_del;
  int total_recv;
  int total_send;
  char log[256];
}POP_STAT;
#endif
