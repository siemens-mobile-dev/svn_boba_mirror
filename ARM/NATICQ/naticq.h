#ifndef _NATICQ_H_
  #define _NATICQ_H_
/*
  Типы данных, общие для всех модулей проекта
*/
typedef struct
{
  void *next;
  void *prev;
  unsigned int isgroup;
  unsigned int uin;
  unsigned int group;
  char name[64];
  unsigned short state;
  int isunread;
  char *log;
  char *answer;
  char *last_log;
  int isactive;
  int msg_count;
}CLIST;

typedef struct
{
  unsigned long uin;
  unsigned short type;
  unsigned short data_len;
}PKT;

typedef struct
{
  PKT pkt;
  char data[16384];
}TPKT;

#ifdef NEWSGOLD
#define CBOX_CHECKED 0xE116
#define CBOX_UNCHECKED 0xE117
#else
#define CBOX_CHECKED 0xE10B
#define CBOX_UNCHECKED 0xE10C
#endif


#endif
