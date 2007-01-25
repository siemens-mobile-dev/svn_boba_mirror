#ifndef _MAIN_H_
  #define _MAIN_H_

// Общие функции модуля MAIN.C
//void SendAnswer(int dummy, TPKT *p);

//void patch_header(HEADER_DESC* head);
//void patch_input(INPUTDIA_DESC* inp);

// Константы статусов
#define IS_OFFLINE 0
#define IS_INVISIBLE 1
#define IS_AWAY 2
#define IS_NA 3
#define IS_OCCUPIED 4
#define IS_DND 5
#define IS_ONLINE 6
#define IS_FFC 7
#define IS_MSG 8
#define IS_UNKNOWN 9


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



#endif
