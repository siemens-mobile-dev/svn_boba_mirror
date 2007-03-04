/*
    SieNatJabber Project
    ‘ункции показа сообщений контактов    
*/
#ifndef _MESSAGE_LIST_H_
  #define _MESSAGE_LIST_H_

// —мещение, начина€ с которого рисовать хистори
#define HIST_DISP_OFS 14

// ÷вета. ћного цветов :)
//общий фон
#define MESSAGEWIN_BGCOLOR 0
//заголовок
#define MESSAGEWIN_TITLE_BGCOLOR 21
//цвет шрифта
#define MESSAGEWIN_TITLE_FONT 1

//исход€щие
#define MESSAGEWIN_MY_BGCOLOR 0
//вход€щие
#define MESSAGEWIN_CH_BGCOLOR 22
#define MESSAGEWIN_GHAT_BGCOLOR_1 13
#define MESSAGEWIN_GHAT_BGCOLOR_2 10
#define MESSAGEWIN_SYS_BGCOLOR 0
#define MESSAGEWIN_STATUS_BGCOLOR 0


/*
0 - белый
1 - черный
2 - красный
3 - синий
4 - зеленый
5 - малиновый
6 - €рко-синий )
7 - желтый
8 - темно-розовый
9 - голубой
10 - €рко-зеленый
11 - сиреневый
12 - €рко-голубой
13 - €рко-желтый
14 - бордовый
15 - темно-синий
16 - €довито-зеленый
17 - темно-сиреневый
18 - лазурно-темно-зеленый )
19 - коричнево-поносный )
20 - насыщенно-серый
21 - серый
22 - светло-серый
23 - прозрачный
*/

// Ќечто, что будет использоватьс€ дл€ вывода строк
typedef struct
{
  MESS_TYPE mtype;
  unsigned short log_mess_number;
  WSHDR* mess;
  void* next;
}DISP_MESSAGE;

// ќтобразить список сообщений
void Display_Message_List(TRESOURCE* ContEx);

// ќткрыть окно написани€ нового сообщени€
void Init_Message(TRESOURCE* ContEx);

void ParseMessagesIntoList(TRESOURCE* ContEx);
#endif
