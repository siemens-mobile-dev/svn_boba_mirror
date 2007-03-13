#ifndef _NATICQ_H_
  #define _NATICQ_H_
/*
  Типы данных, общие для всех модулей проекта
*/

typedef struct
{
  void *next;
  unsigned int uin;
  char name[64];
  unsigned short state;
  int isunread;
  char *log;
  char *answer;
  char *last_log;
  int isactive;
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
#endif
