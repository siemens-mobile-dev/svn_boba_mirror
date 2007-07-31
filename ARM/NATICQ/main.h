#ifndef _MAIN_H_
  #define _MAIN_H_

// Общие функции модуля MAIN.C
void SendAnswer(int dummy, TPKT *p);
void ascii2ws(WSHDR *ws, const char *s);
void CreateEditChat(CLIST *t);
void set_my_status(void);
void set_my_xstatus(void);

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

#define T_MY_XSTATUS_CH 22

//Мой ответ Чемберлену (подтверждение доставки)
#define T_MSGACK 23

#define T_XTEXT_REQ 24
#define T_XTEXT_ACK 25
#define T_XTEXT_SET 26


#endif
