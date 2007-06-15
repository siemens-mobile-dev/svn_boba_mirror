#ifndef _MAIN_H_
  #define _MAIN_H_

// Общие функции модуля MAIN.C
void SendAnswer(int dummy, TPKT *p);
void ascii2ws(WSHDR *ws, const char *s);
void CreateEditChat(CLIST *t);

extern int connect_state;

typedef struct
{
  CLIST *ed_contact;
  GUI *ed_chatgui;
  int ed_answer;
  int requested_decrement_total_unread;
  int loaded_templates;
}EDCHAT_STRUCT;

extern unsigned long  strtoul (const char *nptr,char **endptr,int base);


//void patch_header(HEADER_DESC* head);
//void patch_input(INPUTDIA_DESC* inp);

enum ISTATUS {IS_OFFLINE,IS_INVISIBLE,IS_AWAY,IS_NA,IS_OCCUPIED,IS_DND,IS_DEPRESSION,IS_EVIL,IS_HOME,IS_LUNCH,
IS_WORK,IS_ONLINE,IS_FFC,IS_MSG,IS_UNKNOWN,IS_GROUP,IS_GROUPOFF,IS_NULLICON,
//
ICON_ADDCONTACT,
ICON_HEAD,
ICON_LOGO,
ICON_PING,
ICON_SETTINGS,
TOTAL_ICONS,
//
IS_ANY=0xFFFFFFFF,IS_FILTER_T9KEY=0xFFFFFFFE};

/*
// Константы статусов
#define IS_OFFLINE 0
#define IS_INVISIBLE 1
#define IS_AWAY 2
#define IS_NA 3
#define IS_OCCUPIED 4
#define IS_DND 5
#define IS_DEPRESSION 6
#define IS_EVIL 7
#define IS_HOME 8
#define IS_LUNCH 9
#define IS_WORK 10
#define IS_ONLINE 11
#define IS_FFC 12
#define IS_MSG 13
#define IS_UNKNOWN 14
#define IS_GROUP 15
//#define IS_GROUPOFF 16
#define IS_NULLICON 17
#define IS_ANY 0xFFFFFFFF
#define IS_FILTER_T9KEY 0xFFFFFFFE
*/


// Константы операций (взаимодействие с сервером шлюза)
#define T_REQLOGIN 1
#define T_SENDMSG 2
#define T_RECVMSG 3
#define T_LOGIN 4
#define T_ERROR 6
#define T_CLENTRY 7
#define T_STATUSCHANGE 9
#define T_AUTHREQ 10
#define T_REQINFOSHORT 11
#define T_ADDCONTACT 12
#define T_SSLRESP 13
#define T_AUTHGRANT 14
#define T_MY_STATUS_CH 15   // Изменение моего статуса
//Сервер (аол) получил сообщение
#define T_SRV_ACK 16
//Клиент получил сообщение
#define T_CLIENT_ACK 17

#define T_ECHO 18
#define T_ECHORET 19

#define T_GROUPID 20
#define T_GROUPFOLLOW 21

#endif
