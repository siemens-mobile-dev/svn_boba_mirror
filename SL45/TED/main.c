#include "sl45.h"
#include "reg167.h"

/*unsigned long far tmr1;

void dummy_timer(void)
{
 static const STR estr[]=
 {
  "See you later!"
 };
 SendMessage((void far *)0x3B5E8,0x90,0);
 //SendMessage((void far *)0x3B5E8,0x88,0);
 setExtSIDTable(estr);
 DisplayMessageBoxV(1,0xE27);
 DisableTimerX(&tmr1);
}*/

unsigned int far terminated; //Признак закрытия диалога
int far dia_id;
unsigned int far editmode;

void sd_oncreate(void){doBack0A();}
void sd_onkey(void){}
void sd_oninit(void){}
void sd_onexit1(void){}
void sd_onexit2(void){}
static const struct DIALOG sdialog={sd_onkey,sd_oninit,sd_onexit1,sd_onexit2,sd_oncreate,0x10,0xc4};

volatile int far stk_fhandle;

//Флаг необходимости перерисовать экран
volatile unsigned char far draw_mode;
volatile unsigned char far font_size;

volatile unsigned int far clip_pasted;

void md_onCreate(void)
{
 char mmm[0x10];
 void DrawScreen(void);
 void FirstLoadFile(unsigned int fmt);

 pShowDialog(&sdialog,mmm); //Псевдо-диалог
 //Запрещаем IDLE таймер
 OnTimer1();
 OnTimer2();
 DrawScreen();
 if (terminated==2)
 {
  terminated=0;
  draw_mode=1;
 }
 else
 {
  FilesysICall_UI(FirstLoadFile,0xFFFF); //С поиском в истории и вызовом меню, если нет
 }
}

void md_onClose(void)
{
 void SaveHistory(void);
 editmode=0;
 terminated=1;
 FilesysICall(SaveHistory);
 //StartTimer2(&tmr1,100,dummy_timer);
}

int md_onKey(void far *data, struct MSG far *Msg);

static const unsigned int md_tab[]={0xFFF5};

static const struct DIALOG_WSK main_dia=
{
 md_onKey,
 md_onCreate,
 md_onClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

static const char bmpDiskAccess[8]={0xFC,0x86,0xB3,0xA9,0xB1,0xA9,0x81,0xFF};
static const struct ImageHdr far imgDiskAccess = {8,8,0x1,bmpDiskAccess};

//Указатель блока в верхнем стеке
int far u_disk; //Дисковый указатель верхнего стека (в блоках по STKSZ50)
//Указатель блока в нижнем стеке
int far d_disk; //Дисковый указатель нижнего стека (в блоках по STKSZ50)

unsigned int far usp; //Указатель на верхний стек
unsigned int far dsp; //Указатель на нижний стек

//Флаг работы с диском
#define FIRSTLOAD 1
#define LOAD_UP 2
#define LOAD_DOWN 4
#define SAVE_UP 8
#define SAVE_DOWN 16
#define SAVE_FILE 32

unsigned char far disk_access;

volatile unsigned char far cursor_cnt;
volatile unsigned char far disable_exit;

volatile unsigned char far win_dos_koi;
unsigned char far cursor_off;

const unsigned char clipfile[64]="A:\\Trash\\TED.clipb";
const unsigned char stkfile[64]="A:\\Trash\\TED.tmp";
const unsigned char snapshootfile[64]="A:\\Trash\\TED.snapshoot";
const unsigned char historyfile[64]="A:\\TED\\history";
const unsigned char fntfilename[64]="A:\\TED\\%d.fnt";

//Строка, на которую надо установить позицию в самом начале
unsigned long far seek_to_line;
//Общее количество строк
unsigned long far total_line;
//Номер текущей строки (курсор)
unsigned long far curline;
//Текущий символ для редактирования (курсор)
unsigned int far curpos;
//Номер верхней отображаемой строки
unsigned long far viewline;
//Номер самого левого отображаемого символа
unsigned int far viewpos;

unsigned int far max_y;
unsigned int far max_x;

//---------------------------------
// Состояние конвертора
//---------------------------------
unsigned int far CSM;
//Символ конца строки в конверторе
unsigned int far eolsymb;

//Прототипы конверторов
unsigned int ConvertFormat(int fin,int fs,int fmt);
unsigned int ConvertSimple(int fin,int fs);

//Размер стеков
#define STKSZ (16384)
//Размер, при котором происходит запись во временный файл
#define STKMAX (16384-3000)
//Размер, при котором происходит чтение из временного файла
#define STKMIN (3000)
//Размер блока во временном файле
#define STKSZ50 (8192)

#pragma class fb=CFAR1
unsigned char ustk[STKSZ]; //верхний стек строк
unsigned char dstk[STKSZ]; //нижний стек строк, строки хранятся в обратном порядке
unsigned char far dbat[256]; //Таблица распределения блоков нижнего стека во временном файле
unsigned char far ubat[256]; //Таблица распределения блоков верхнего стека во временном файлe
unsigned char far bfree[256]; //Таблица занятости блоков (0 - свободен)
unsigned char far font[2048]; //Буфер для шрифта
unsigned char far editline[256]; //Буфер для редактирования строки
unsigned char far filename[128];
struct 
{
 unsigned char name[128]; //Имя файла
 unsigned char font; //Размер шрифта
 unsigned char fmt; //Тип форматера
 unsigned char codepage; //Кодировка
 unsigned char cursor_off; //Если 1 - выключен курсор при просмотре
 unsigned long line; //Номер строки для перехода
 unsigned long total; //Общее количество. Если не совпадает, переход по отношению
 unsigned long bookm1;
 unsigned long bookm2;
 unsigned long bookm3;
 unsigned long bookm4;
} far HISTORY;
void huge * far estr_methods[9];

unsigned char far myscr[13*80];

//typedef huge void p_redraw(void far *data);
//p_redraw * far old_estr_redraw;

#pragma default_attributes

static const struct ImageHdr far MyScrHdr = {101,80,0x1,myscr};

const far char wintranslation[128]=
{
 0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
 0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
 0xFF,0xF6,0xF7,0x5F,0xFD,0x83,0xB3,0x15,0xF0,0x63,0xF2,0x3C,0xBF,0x2D,0x52,0xF4,
 0xF8,0x2B,'I' ,'i' ,0xA3,0xE7,0x14,0xFA,0xF1,0xFC,0xF3,0x3E,0x5F,0x5F,0x5F,0xF5,
 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
 0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
 0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
 0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
};

const far char koi8translation[128]=
{
 0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
 0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
 0xFF,0xF6,0xF7,0xF1,0xF3,0x5F,'i' ,0xF5,0xF0,0x63,0xF2,0x3C,0xBF,0xA3,0x52,0xF4,
 0xF8,0x2B,0x5F,0xF0,0xF2,0xE7,'I' ,0xF4,0xF1,0xFC,0xF3,0x3E,0x5F,0x83,0x5F,0xF5,

 0xEE,0xA0,0xA1,0xE6,0xA4,0xA5,0xE4,0xA3,0xE5,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,
 0xAF,0xEF,0xE0,0xE1,0xE2,0xE3,0xA6,0xA2,0xEC,0xEB,0xA7,0xE8,0xED,0xE9,0xE7,0xEA,
 0x9E,0x80,0x81,0x96,0x84,0x85,0x94,0x83,0x95,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,
 0x8F,0x9F,0x90,0x91,0x92,0x93,0x86,0x82,0x9C,0x9B,0x87,0x98,0x9D,0x99,0x97,0x9A
};

const far unsigned int dos2unicode[128]=
{
 0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
 0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
 0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
 0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
 0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
 0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
 0x002D,0x002D,0x002D,0x00A6,0x002B,0x00A6,0x00A6,0x00AC,
 0x00AC,0x00A6,0x00A6,0x00AC,0x002D,0x002D,0x002D,0x00AC,
 0x004C,0x002B,0x0054,0x002B,0x002D,0x002B,0x00A6,0x00A6,
 0x004C,0x0433,0x00A6,0x0054,0x00A6,0x003D,0x002B,0x00A6,
 0x00A6,0x0054,0x0054,0x004C,0x004C,0x002D,0x0433,0x002B,
 0x002B,0x002D,0x002D,0x002D,0x002D,0x00A6,0x00A6,0x002D,
 0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
 0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
 0x0401,0x0451,0x0404,0x0454,0x0407,0x0457,0x040E,0x045E,
 0x00B0,0x2022,0x00B7,0x0076,0x2116,0x00A4,0x00A6,0x00A0
};

unsigned int char8to16(char c)
{
 if (c<32) return('_');
 if (c>=128)
 {
  switch(win_dos_koi)
  {
   case 1:
    //Win->Dos
    c=wintranslation[c-128];
    break;
   case 2:
    //Koi8->Dos
    c=koi8translation[c-128];
    break;
  }
  return(dos2unicode[c-128]);
 }
 return(c);
}

// RECODING TABLE
//utf16,cp866,cp1251,koi8
//const far unsigned int unicode2char[]=
typedef struct
{
 unsigned int u;
 unsigned char dos;
 unsigned char win;
 unsigned char koi8;
} TUNICODE2CHAR;

const far TUNICODE2CHAR unicode2char[]=
{
// CAPITAL Cyrillic letters (base)
0x410,0x80,0xC0,0xE1, // А
0x411,0x81,0xC1,0xE2, // Б
0x412,0x82,0xC2,0xF7, // В
0x413,0x83,0xC3,0xE7, // Г
0x414,0x84,0xC4,0xE4, // Д
0x415,0x85,0xC5,0xE5, // Е
0x416,0x86,0xC6,0xF6, // Ж
0x417,0x87,0xC7,0xFA, // З
0x418,0x88,0xC8,0xE9, // И
0x419,0x89,0xC9,0xEA, // Й
0x41A,0x8A,0xCA,0xEB, // К
0x41B,0x8B,0xCB,0xEC, // Л
0x41C,0x8C,0xCC,0xED, // М
0x41D,0x8D,0xCD,0xEE, // Н
0x41E,0x8E,0xCE,0xEF, // О
0x41F,0x8F,0xCF,0xF0, // П
0x420,0x90,0xD0,0xF2, // Р
0x421,0x91,0xD1,0xF3, // С
0x422,0x92,0xD2,0xF4, // Т
0x423,0x93,0xD3,0xF5, // У
0x424,0x94,0xD4,0xE6, // Ф
0x425,0x95,0xD5,0xE8, // Х
0x426,0x96,0xD6,0xE3, // Ц
0x427,0x97,0xD7,0xFE, // Ч
0x428,0x98,0xD8,0xFB, // Ш
0x429,0x99,0xD9,0xFD, // Щ
0x42A,0x9A,0xDA,0xFF, // Ъ
0x42B,0x9B,0xDB,0xF9, // Ы
0x42C,0x9C,0xDC,0xF8, // Ь
0x42D,0x9D,0xDD,0xFC, // Э
0x42E,0x9E,0xDE,0xE0, // Ю
0x42F,0x9F,0xDF,0xF1, // Я
// CAPITAL Cyrillic letters (additional)
0x402,'_',0x80,'_', // _ .*.*
0x403,'_',0x81,'_', // _ .*.*
0x409,'_',0x8A,'_', // _ .*.*
0x40A,'_',0x8C,'_', // _ .*.*
0x40C,'_',0x8D,'_', // _ .*.*
0x40B,'_',0x8E,'_', // _ .*.*
0x40F,'_',0x8F,'_', // _ .*.*
0x40E,0xF6,0xA1,'_', // Ў ...*
0x408,0x4A,0xA3,0x4A, // _ .*.*
0x409,0x83,0xA5,0xBD, // _ .*..
0x401,0xF0,0xA8,0xB3, // Ё
0x404,0xF2,0xAA,0xB4, // Є
0x407,0xF4,0xAF,0xB7, // Ї
0x406,0x49,0xB2,0xB6, // _ .*..
0x405,0x53,0xBD,0x53, // _ .*.*
// SMALL Cyrillic letters (base)
0x430,0xA0,0xE0,0xC1, // а
0x431,0xA1,0xE1,0xC2, // б
0x432,0xA2,0xE2,0xD7, // в
0x433,0xA3,0xE3,0xC7, // г
0x434,0xA4,0xE4,0xC4, // д
0x435,0xA5,0xE5,0xC5, // е
0x436,0xA6,0xE6,0xD6, // ж
0x437,0xA7,0xE7,0xDA, // з
0x438,0xA8,0xE8,0xC9, // и
0x439,0xA9,0xE9,0xCA, // й
0x43A,0xAA,0xEA,0xCB, // к
0x43B,0xAB,0xEB,0xCC, // л
0x43C,0xAC,0xEC,0xCD, // м
0x43D,0xAD,0xED,0xCE, // н
0x43E,0xAE,0xEE,0xCF, // о
0x43F,0xAF,0xEF,0xD0, // п
0x440,0xE0,0xF0,0xD2, // р
0x441,0xE1,0xF1,0xD3, // с
0x442,0xE2,0xF2,0xD4, // т
0x443,0xE3,0xF3,0xD5, // у
0x444,0xE4,0xF4,0xC6, // ф
0x445,0xE5,0xF5,0xC8, // х
0x446,0xE6,0xF6,0xC3, // ц
0x447,0xE7,0xF7,0xDE, // ч
0x448,0xE8,0xF8,0xDB, // ш
0x449,0xE9,0xF9,0xDD, // щ
0x44A,0xEA,0xFA,0xDF, // ъ
0x44B,0xEB,0xFB,0xD9, // ы
0x44C,0xEC,0xFC,0xD8, // ь
0x44D,0xED,0xFD,0xDC, // э
0x44E,0xEE,0xFE,0xC0, // ю
0x44F,0xEF,0xFF,0xD1, // я
// SMALL Cyrillic letters (additional)
0x452,'_',0x90,'_', // _ .*.*
0x453,'_',0x83,'_', // _ .*.*
0x459,'_',0x9A,'_', // _ .*.*
0x45A,'_',0x9C,'_', // _ .*.*
0x45C,'_',0x9D,'_', // _ .*.*
0x45B,'_',0x9E,'_', // _ .*.*
0x45F,'_',0x9F,'_', // _ .*.*
0x45E,0xF7,0xA2,'_', // ў ...*
0x458,0x6A,0xBC,0x6A, // _ .*.*
0x491,0xA3,0xB4,0xAD, // _ .*..
0x451,0xF1,0xB8,0xA3, // ё
0x454,0xF3,0xBA,0xA4, // є
0x457,0xF5,0xBF,0xA7, // ї
0x456,0x69,0xB3,0xA6, // _ .*..
0x455,0x73,0xBE,0x73, // _ .*.*
0x0A0,'_',0xA0,0x20, // space .*..
0x0A4,'_',0xA4,0xFD, // ¤   .*..
0x0A6,'_',0xA6,'_', // ¦   .*.*
0x0B0,0xF8,0xB0,0x9C, // °
0x0B7,0xFA,0xB7,0x9E, // ·
// 0x2022,,0x95,0x95, //    .*..
// 0x2116,0xFC,0xB9,0x23, // №   ...*
// 0x2219,,0xF9,0x9E, //    .*..
// 0x221A,0xFB,,0x96, // v   ..*.
// 0x25A0,0xFE,,0x94, // ¦
0x0000,0,0,0
};

unsigned char char16to8(unsigned int c)
{
 const TUNICODE2CHAR far *p=unicode2char;
 unsigned int i;
 if (c<32) return(' ');
 if (c<128) return(c);
 while((i=p->u))
 {
  if (c==i)
  {
   switch(win_dos_koi)
   {
   case 1:
    //Win
    return(p->win);
   case 2:
    //Koi8
	return(p->koi8);
   }
   return(p->dos);
  }
  p++;
 }
 c&=0xFF;
 if (c<32) return(' ');
 return(c);
}

//static const struct ImageHdr far imgScreen = {96,80,0x1,&screen};
//--------------------------------------------------------------------------
//Печать символа
void DrawChar(char c,int x,int y)
{
 unsigned int d; //Куда рисуем
 unsigned int s; //Откуда рисуем
 int i;
 char ms;
 char md;
 if (c>=128)
 switch(win_dos_koi)
 {
  case 1:
   //Win->Dos
   c=wintranslation[c-128];
   break;
  case 2:
   //Koi8->Dos
   c=koi8translation[c-128];
   break;
 }
 #pragma asm
	PUSH	DPP2
 #pragma endasm
 switch(font_size)
 {
  case 4:
   //Обрабатываем фонт размером 4
   s=_pof(font)+(c<<3);
   d=y*(8*13)+(x>>1)+0x8000; //0E:160C - Основной экран
   i=8;
   ms=0xF0;
   md=0x0F;
   if (x&1) {ms=0x0F;md=0xF0;}
   //Печать в тетраде
   #pragma asm(@w1=s, @w2=d, @b3, @b4, @w5=i, @b6=ms, @b7=md )
    ADD		@w2,#POF _myscr
	MOV		DPP2,#PAG _myscr ;//0E:160C - Основной экран
	MOV		DPP0,#PAG _font
	_x_x_loop1:	    
	MOVB	@b3,[@w2]
	ANDB	@b3,@b7
	MOVB	@b4,[@w1]
	ANDB	@b4,@b6
	ORB		@b3,@b4
	MOVB	[@w2],@b3
	ADD		@w2,#0Dh
	ADD		@w1,#01h
	SUB		@w5,#01h
	JMPR	cc_NZ,_x_x_loop1
   #pragma endasm()
   break;
  case 6:
   //Обрабатываем фонт размером 6
   s=_pof(font)+(c<<3);
   d=y*(8*13)+((x>>2)*3)+0x8000; //0E:160C - Основной экран
   i=8;
   switch(x&3)
   {
	case 0:
     #pragma asm(@w1=s, @w2=d, @b3, @b4, @w5=i)
    ADD		@w2,#POF _myscr
	 	MOV		DPP2,#PAG _myscr ;//0E:160C - Основной экран
		MOV		DPP0,#PAG _font
		_x_x_loop2:	    
		MOVB	@b3,[@w2]
		ANDB	@b3,#003h
		MOVB	@b4,[@w1]
		ORB		@b3,@b4
		MOVB	[@w2],@b3
		ADD		@w2,#0Dh
		ADD		@w1,#01h
		SUB		@w5,#01h
		JMPR	cc_NZ,_x_x_loop2
     #pragma endasm()
	 break;
	case 1:
     #pragma asm(@w1=s, @w2=d, @b3, @w5=i)
    ADD		@w2,#POF _myscr
	 	MOV		DPP2,#PAG _myscr ;//0E:160C - Основной экран
		MOV		DPP0,#PAG _font
		_x_x_loop3:	    
		MOVB	@b3,[@w2]
		ANDB	@b3,#0FCh
		MOVB	RL1,[@w1]
		MOVB	RH1,#0
		SHR		R1,#6
		ORB		@b3,RL1
		MOVB	[@w2],@b3
		ADD		@w2,#1
		MOVB	@b3,[@w2]
		ANDB	@b3,#00Fh
		MOVB	RL1,[@w1]
		SHL		R1,#2
		ORB		@b3,RL1
		MOVB	[@w2],@b3
		ADD		@w2,#0Ch
		ADD		@w1,#01h
		SUB		@w5,#01h
		JMPR	cc_NZ,_x_x_loop3
     #pragma endasm()
	 break;
	case 2:
	 d++;
     #pragma asm(@w1=s, @w2=d, @b3, @w5=i)
    ADD		@w2,#POF _myscr
	 	MOV		DPP2,#PAG _myscr;//0E:160C - Основной экран
		MOV		DPP0,#PAG _font
		_x_x_loop4:	    
		MOVB	@b3,[@w2]
		ANDB	@b3,#0F0h
		MOVB	RL1,[@w1]
		MOVB	RH1,#0
		SHR		R1,#4
		ORB		@b3,RL1
		MOVB	[@w2],@b3
		ADD		@w2,#1
		MOVB	@b3,[@w2]
		ANDB	@b3,#03Fh
		MOVB	RL1,[@w1]
		SHL		R1,#4
		ORB		@b3,RL1
		MOVB	[@w2],@b3
		ADD		@w2,#0Ch
		ADD		@w1,#01h
		SUB		@w5,#01h
		JMPR	cc_NZ,_x_x_loop4
     #pragma endasm()
	 break;
	case 3:
	 d+=2;
     #pragma asm(@w1=s, @w2=d, @b3,@w5=i)
    ADD		@w2,#POF _myscr
	 	MOV		DPP2,#PAG _myscr ;//0E:160C - Основной экран
		MOV		DPP0,#PAG _font
		_x_x_loop5:	    
		MOVB	@b3,[@w2]
		ANDB	@b3,#0C0h
		MOVB	RL1,[@w1]
		MOVB	RH1,#0
		SHR		R1,#2
		ORB		@b3,RL1
		MOVB	[@w2],@b3
		ADD		@w2,#0Dh
		ADD		@w1,#01h
		SUB		@w5,#01h
		JMPR	cc_NZ,_x_x_loop5
     #pragma endasm()
	 break;
   }
   break;
  case 8:
   //Обрабатываем фонт размером 8
   s=_pof(font)+(c<<3);
   d=y*(8*13)+x+0x8000; //0E:160C - Основной экран
   i=8;
   #pragma asm(@w1=s, @w2=d, @b3, @w5=i )
    ADD		@w2,#POF _myscr
	MOV		DPP0,#PAG _font
	MOV		DPP2,#PAG _myscr ;//0E:160C - Основной экран
	NOP
	_x_x_loop6:	   
	MOVB	@b3,[@w1]
	MOVB	[@w2],@b3
	ADD		@w2,#0Dh
	ADD		@w1,#01h
	SUB		@w5,#01h
	JMPR	cc_NZ,_x_x_loop6
   #pragma endasm()
   break;
  default:
   break;
 }
 #pragma asm
	POP		DPP2
 #pragma endasm
}

//Печать строки из буфера текста
void drawStkStr(unsigned char *p, unsigned int y, unsigned int vp, int ep)
{
 unsigned int i=0;
 unsigned char c;
 do
 {
  c=*p;
  if (c)
  {
   if (ep>=0) editline[ep++]=c;
   p++;
   if (vp!=0) vp--; else DrawChar(c,i++,y);
  }
  else
  {
   DrawChar(' ',i++,y);
  }
 }
 while(i<max_x);
 if (ep>=0) while((c=*p++)) editline[ep++]=c; //Добиваем остаток строки
}

void drawFrmStkStr(unsigned char *p, unsigned int y, unsigned int vp, int ep)
{
 unsigned int i=0;
 unsigned char c;
 unsigned long spcsum;
 unsigned long spcadd;
 unsigned int spcs=0;

 //Надо посчитать пробелы
 i=0;
 if (*p)
 {
  i=1; //Считаем со второго символа
  while((c=p[i])) //Пока не конец строки
  {
   if (c==' ') spcs++; //Считаем пробелы
   i++;
  }
 }
 if (i>max_x)
  i=0; //Не добавляем пробелы, строка длиннее, чем экран
 else
  i=max_x-i; //Теперь в i - общее количество добавляемых пробелов
 spcadd=0;
 spcsum=0;
 if (spcs)
 {
  spcadd=((unsigned long)i<<16)/spcs;
  spcsum=((unsigned long)i<<16)%spcs;
 }
 spcsum+=spcadd; //Начальное значение

 i=0;
 c=*p;
 if (c)
 {
  if (ep>=0) editline[ep++]=c;
  if (vp!=0) vp--; else DrawChar(c,i++,y);
  p++;
 }
 else goto L2;
 do
 {
  c=*p;
  if (c)
  {
   if (ep>=0) editline[ep++]=c;
   if (vp!=0) vp--; else DrawChar(c,i++,y);
   if (c==' ')
   {
      //Добавляем пробелы
      if (spcsum>=65536)
      {
       spcsum-=65536;
	   continue;
      }
      spcsum+=spcadd;
   }
   p++;
  }
  else
  {
   L2:
   DrawChar(' ',i++,y);
  }
 }
 while(i<max_x);
 if (ep>=0) while((c=*p++)) editline[ep++]=c; //Добиваем остаток строки
}

//Поиск начала строки в верхнем стеке
extern unsigned int bl_us(unsigned int pos);
extern unsigned int bl_ds(unsigned int pos);

//Перенос из верхнего стека в нижний (на строку вверх)
extern void move_up(void);
//Перенос из нижнего стека в верхний (на строку вниз)
extern void move_dw(void);
//---------------------------------------
// Работа с таблицей блоков
//---------------------------------------
//Поиск первого свободного блока
unsigned char FindFreeBlock(void)
{
 unsigned char i=0;
 do
 {
  if (!bfree[i])
  {
   bfree[i]=1;
   break;
  }
 }
 while(++i);
 return(i);
}

//---------------------------------------
// Загрузка с диска нового фрагмента
//---------------------------------------
void DoDiskAccess(unsigned int flag)
{
 int f;
 unsigned char b;
 long ul;

 if ((f=stk_fhandle)==-1)
 {
  stk_fhandle=f=FileOpen((STR)stkfile,_O_RDWR+_O_BINARY,_S_IREAD); //Файл верхнего стека
 }
 if (disk_access&LOAD_UP)
 {
  memcpy(ustk+STKSZ50,ustk,usp);
  b=ubat[u_disk--];
  bfree[b]=0;
  FileSeek(f,_mulu32(STKSZ50,b),0,&ul);
  FileRead(f,ustk,STKSZ50);
  usp+=STKSZ50;
 }
 if (disk_access&LOAD_DOWN)
 {
  memcpy(dstk+(dsp-STKSZ50),dstk+dsp,STKSZ-dsp);
  b=dbat[d_disk--];
  bfree[b]=0;
  FileSeek(f,_mulu32(STKSZ50,b),0,&ul);
  FileRead(f,dstk+STKSZ50,STKSZ50);
  dsp-=STKSZ50;
 }
 if (disk_access&SAVE_UP)
 {
  b=FindFreeBlock();
  ubat[++u_disk]=b;
  FileSeek(f,_mulu32(STKSZ50,b),0,&ul);
  FileWrite(f,ustk,STKSZ50);
  memcpy(ustk,ustk+STKSZ50,usp-=STKSZ50);
 }
 if (disk_access&SAVE_DOWN)
 {
  b=FindFreeBlock();
  dbat[++d_disk]=b;
  FileSeek(f,_mulu32(STKSZ50,b),0,&ul);
  FileWrite(f,dstk+STKSZ50,STKSZ50);
  memcpy(dstk+(dsp+STKSZ50),dstk+dsp,STKSZ50-dsp);
  dsp+=STKSZ50;
 }
 disk_access=0;
 if (flag)
 {
  FileClose(f);
  stk_fhandle=-1;
 }
 if ((!terminated)&&flag) SendMessage((void far *)0x3B5E8,0xF8,dia_id);
 return;
}

//------------------------------------------
// Проверка необходимости чтения/записи с диска/на диск
//------------------------------------------
void CheckDiskAccess(void)
{
 if ((usp<STKMIN)&&(u_disk>=0)) disk_access|=LOAD_UP;
 if ((dsp>STKMAX)&&(d_disk>=0)) disk_access|=LOAD_DOWN;
 if (usp>STKMAX) disk_access|=SAVE_UP;
 if (dsp<STKMIN) disk_access|=SAVE_DOWN;
}

//-------------------------------------------------------
// Работа с историей
//-------------------------------------------------------
unsigned int SearchHistory(void)
{
 unsigned int history_pos=0;
 int fin;
 int i;
 if ((fin=FileOpen((STR)historyfile,_O_RDONLY+_O_BINARY,0))!=-1)
 {
  for(;;)
  {
   i=FileRead(fin,&HISTORY,sizeof(HISTORY));
   if (i<sizeof(HISTORY)) goto L_NO_HISTORY;
   if (!strcmp(HISTORY.name,filename))
   {
   	history_pos|=0x8000;
    break; //Нашли
   }
   history_pos++;
  }
  L_NO_HISTORY:
  FileClose(fin);
 }
 return(history_pos);
}

//Записать историю, вызывается в контексте MMC_Filesystem
void SaveHistory(void)
{
 int f;
 long ul;
 char ss[sizeof(HISTORY)];
 unsigned int history_pos;

 memcpy(ss,&HISTORY,sizeof(HISTORY));
 history_pos=SearchHistory()&0x7FFF;
 memcpy(&HISTORY,ss,sizeof(HISTORY));
 memcpy(HISTORY.name,filename,128);
 HISTORY.font=font_size;
 HISTORY.line=curline;
 HISTORY.total=total_line;
 HISTORY.codepage=win_dos_koi;
 HISTORY.cursor_off=cursor_off;
 if ((f=FileOpen((STR)historyfile,_O_RDWR+_O_BINARY,_S_IREAD))==-1)
  f=FileOpen((STR)historyfile,_O_RDWR+_O_BINARY+_O_CREAT,_S_IREAD);
 if (f!=-1)
 {
  FileSeek(f,sizeof(HISTORY)*history_pos,0,&ul);
  FileWrite(f,&HISTORY,sizeof(HISTORY));
  FileClose(f);
 }
 if (terminated)
 {
  //Записываем snap-shoot
  if ((f=FileOpen((STR)snapshootfile,_O_BINARY+_O_CREAT+_O_RDWR,_S_IEXEC))!=-1)
  {
   FileWrite(f,(void far *)0x80000,16384);
   FileWrite(f,(void far *)0x84000,16384);
   FileWrite(f,(void far *)0x88000,16384);
   FileWrite(f,(void far *)0x8C000,16384);
   FileWrite(f,(void far *)0x90000,16384);
   FileClose(f);
  }
 }
}
//------------------------------------------
// Собственно редактор
//------------------------------------------
void GotoLine(void) //Переход на строку seek_to_line, вызывается в контексте MMC_Filesystem
{
// unsigned int l;
// unsigned int p;
 for(;;)
 {
  disk_access=0; //Закончили дисковые операции
  CheckDiskAccess();
  if (disk_access) DoDiskAccess(0);
  if (seek_to_line==curline) break; //Пришли
  if (seek_to_line<curline)
  {
   //Пытаемся идти вверх
   if (!curline) break; //начало!
   if (curline==viewline) viewline--; //Если первая строка экрана
   curline--;
   move_up();
  }
  else
  {
    //Пытаемся идти вниз
   if (dsp==STKSZ) break; //Конец текста
   curline++;
   if ((curline-viewline)==max_y)
   {
    viewline++;
   }
   move_dw();
  }
 }
 if (stk_fhandle!=-1)
 {
  FileClose(stk_fhandle);
  stk_fhandle=-1;
 }
 draw_mode=1; //Перерисовываем
 if (!terminated) SendMessage((void far *)0x3B5E8,0xF8,dia_id);
}

void LineUp(void)
{
// unsigned int l;
// unsigned int p;
 if (curline) //Не начало!
 {
  if (curline==viewline) viewline--; //Если первая строка экрана
  curline--;
  move_up();
 }
}

void LineDw(void)
{
// unsigned int l;
// unsigned int p;
 if (dsp==STKSZ) return; //Конец текста
 curline++;
 if ((curline-viewline)==max_y)
 {
  viewline++;
 }
 move_dw();
}

void PageUp(unsigned int lines)
{
 unsigned int i;
// unsigned int l;
// unsigned int p;
 if (curline<lines)
 {
  i=curline;
  viewline=0;
  curline=0;
 }
 else
 {
  i=lines;
  curline-=i;
  if (viewline>i) viewline-=i; else viewline=0;
 }
 while(i)
 {
  move_up();
  i--;
 }
}

void PageDw(unsigned int lines)
{
 unsigned int i;
// unsigned int l;
// unsigned int p;
 i=lines;
 do
 {
  if (dsp==STKSZ) break; //Конец текста
  viewline++;
  curline++;
  move_dw();
 }
 while(--i);
}

void ToEOL(void)
{
 unsigned int i=strlen(editline);
 if (i>255) curpos=0; else curpos=i;
 if (curpos>=max_x) viewpos=curpos-max_x+1; else viewpos=0;
}

void ToBOL(void)
{
 curpos=0;
 viewpos=0;
}

void WordLeft(void)
{
 if (curpos==0) return;
 for(;;)
 {
  if ((--curpos)==0) break;
  if ((editline[curpos-1]==' ')&&(editline[curpos]!=' ')) break;
 }
 while(curpos<viewpos)
 {
  if (viewpos>8) viewpos-=8; else viewpos=0;
 }
}

void WordRight(void)
{
 if (!editline[curpos]) return;
 for(;;)
 {
  curpos++;
  if (!editline[curpos]) break;
  if ((editline[curpos-1]==' ')&&(editline[curpos]!=' ')) break;
 }
 while((curpos-viewpos)>=max_x)
 {
  viewpos+=8;
 }
}

void DrawInfo(void)
{
 char s[256];
 FillRect(0,0,101,80,0);
 xsprintf(s,(STR)"Time:\n%02d:%02d\n"
 "Current line %lu\nTotal lines %lu\n\nCurrent file:",
 _hour,_minute,curline,total_line);
 DrawString(0,0,101,56,(STR)s,0);
 DrawString(0,56,101,24,filename,0);
}

//=============================================================================
//Вывод на экран
//=============================================================================
void DrawScreen(void)
{
 unsigned int y;
 unsigned int p;
 unsigned char c;
 int f;
 char s[32];
 unsigned int my;
 if (disk_access)
 {
  drawImage(0,0,8,8,&imgDiskAccess);
  if (draw_mode==2)
  {
   xsprintf(s,(STR)"Goto line %lu...",curline);
   FillRect(8,0,101-8,8,0);
   DrawString(8,0,101-8,8,(STR)s,0);
   goto L_WELLCOME2;
  }
  if (disk_access==SAVE_FILE)
  {
   xsprintf(s,(STR)"Saving...");
   goto L_W1;
  }
  if (disk_access==FIRSTLOAD)
  {
   xsprintf(s,(STR)"Line %lu...",curline);
   L_W1:
   FillRect(8,0,101-8,8,0);
   DrawString(8,0,101-8,8,(STR)s,0);
   goto L_WELLCOME2;
  }
  if (draw_mode==255) goto L_WELLCOME;
 }
 else
 {
  switch(draw_mode)
  {
   case 1:
    my=max_y;
    if (editmode) my-=4;
    memset(editline,0,256);
    //Перерисовываем весь экран
	y=curline-viewline;
	p=usp;
	if (dsp!=STKSZ) c=dstk[dsp]; else c=0; //Первый символ текущей строки - если буква - форматируем
	while(y)
	{
	 //Есть строки выше текущей
     p=bl_us(p);
     if ((c>' ')&&(!(HISTORY.fmt&0x80))&&(cursor_off))
	 drawFrmStkStr(ustk+p,--y,viewpos,-1);
     else
     drawStkStr(ustk+p,--y,viewpos,-1);
	 c=ustk[p];
    }
    y=curline-viewline;
	//Теперь печатаем от текущей вниз
	p=dsp;
	f=0;
	while(y!=(my))
	{
     if (p==STKSZ)
	 {
	  drawStkStr((STR)"<<EOF>>",y++,0,-1);
      while(y!=my) drawStkStr("",y++,viewpos,-1);
      break;
	 }
	 else
	 {
	  unsigned int p1;
      p1=bl_ds(p);
	  if (p1!=STKSZ) c=dstk[p1]; else c=0; //Посл. строка не расширяется
      if ((c>' ')&&(!(HISTORY.fmt&0x80))&&(cursor_off))
 	  drawFrmStkStr(dstk+p,y++,viewpos,f);
	  else
	  drawStkStr(dstk+p,y++,viewpos,f);
	  p=p1;
	  f=-1;
	 }
    }
    //DrawCursor(curpos-viewpos,curline-viewline);
	{
	 //Рисуем скролл-бар
	 char far *d=myscr+12; //Последний байт
	 if (total_line)
	 {
      y=((editmode?40:72)*curline)/total_line;
	 }
	 else
	 {
	  y=0;
	 }
	 p=0;
	 do
	 {
	  if ((p<y)||(p>(y+7)))
	  {
	   *d=p&1?0x50:0x28;
      }
	  else
	  {
	   *d=0x78;
      }
	  d+=13;
	  p++;
     }
	 while(p<80);
    }
	if (editmode)
	 drawImage(0,16,101,48,&MyScrHdr);
	else
	 drawImage(0,0,101,80,&MyScrHdr);
	cursor_cnt=3;
	goto L_CURSOR;
   case 2:
    //Процесс перехода на строку
    {
     FillRect(0,0,101,8,0);
     xsprintf(s,(STR)"Goto line %lu...",curline);
     DrawString(0,0,101,8,(STR)s,0);
	 goto L_WELLCOME2;
    }
    //return;
   case 4:
    //Экран информации
	DrawInfo();
	return;
   case 255:
    //Экран приветствия
	L_WELLCOME:
    FillRect(0,0,101,8,0);
	draw_mode=0;
	L_WELLCOME2:
	FillRect(0,8,101,80-8,0);
	DrawString(0,16,101,48,(STR)"Text viewer/editor\nversion 2.0\nCopyright(C)2005/2006\nby Rst7/CBSIE",0);
	DrawString(0,48,101,8,*(char far *)(0x3B960)==1?(STR)"26MHz":(STR)"13MHz",0);
	{
	 DrawString(0,56,101,24,filename,0);
    }
	return;
   case 0:
    //Курсор
	if (cursor_cnt)
	{
	 cursor_cnt--;
	 break;
    }
	L_CURSOR:
    {
     unsigned int x=curpos-viewpos;
     unsigned int y=curline-viewline;
	 my=max_y;
	 if (editmode)
	 {
	  my-=2;
	  y+=2;
	 }
     if ((x<max_x)&&(y<my)&&(!cursor_off))
     {
      FillRect(x*font_size,y*8,font_size,8,1);
     }
	}
	break;
  }
  draw_mode=0;
 } 
 return;
}

void DrawLoadMenu(void);
void DrawSoftMenu(void);


unsigned int IsFocus(void far *data)
{
 int id;
 if (((int far *)data)[0x10]) return 0; //Есть меню поверх
 if (((int far *)data)[0x0A]) return 0; //Есть модальное окно поверх
 id=(*((int far * far *)(0x37*0x4000L+0x327A)))[6];
 if (((int far *)data)[6]!=id) return 0; //Есть диалог поверх
 //if (((int far *)data)[6]!=GetBotDialogWSK_ID()) return 0; //Есть диалог поверх
 return(1);
}

void doCurRight(void)
{
    if (cursor_off)
	{
	 if (curpos>=(256-8)) return;
	 curpos+=8;
	 viewpos+=8;
	 draw_mode=1;
	 return;
    }
    if (curpos>=256) return;
    curpos++;
    if ((curpos-viewpos)>=max_x)
    {
     viewpos+=8;
    }
    draw_mode=1;
}

void doCurLeft(void)
{
    if (cursor_off)
	{
	 if (curpos<8)
	  curpos=0;
	 else
	  curpos-=8;
	 if (viewpos<8)
	  viewpos=0;
	 else
	  viewpos-=8;
	 draw_mode=1;
	 return;
    }
    if (curpos==0) return;
    draw_mode=1;
    curpos--;
    if (curpos>=viewpos) return;
    if (viewpos>8) viewpos-=8; else viewpos=0;
}

void insline(void)
{
 dstk[--dsp]=0;
 total_line++;
 GeneralFunc(1);
}

void delline(void)
{
 if (dsp!=STKSZ)
 {
  dsp=bl_ds(dsp);
  total_line--;
 }
 GeneralFunc(1);
}

void splitline(void)
{
 if ((curpos<=strlen(dstk+dsp))&&(dsp!=STKSZ))
 {
  while(curpos)
  {
   ustk[usp++]=dstk[dsp++];
   curpos--;
  }
  viewpos=0;
  ustk[usp++]=0;
  total_line++;
  curline++;
 }
 GeneralFunc(1);
}

void joinlines(void)
{
 unsigned int p=bl_ds(dsp);
 if (p!=STKSZ)
 {
  p--;
  while(p!=dsp)
  {
   dstk[p]=dstk[p-1];
   p--;
  }
  dsp++;
  total_line--;
 }
 GeneralFunc(1);
}

void instime(void)
{
 usp+=xsprintf(ustk+usp,"%02d:%02d",_hour,_minute)+1;
 curline++;
 total_line++;
 GeneralFunc(1);
}

void insdate(void)
{
 usp+=xsprintf(ustk+usp,"%02d-%02d-%04d",_day,_month,_year)+1;
 curline++;
 total_line++;
 GeneralFunc(1);
}

void paste(void)
{
 clip_pasted=1;
 GeneralFunc(1);
}

//void cliperase(void)
//{
// FileErase((STR)clipfile);
//}

void clearclipb(void)
{
// FilesysICall(cliperase);
 clip_pasted=1;
 GeneralFunc(1);
}

static const STR emenu_ExtendedSTR[]=
{
 "Insert line",
 "Delete line",
 "Split line",
 "Join lines",
 "Insert time",
 "Insert date",
 "Paste",
 "Clear clipb"
};

static const struct ITEMSDATA far emenu_ITEMS[]=
{
 NULL,0xE27,0xE27,0,(void far *)0xBAFC06,3,0xD0,
 NULL,0xE28,0xE28,0,(void far *)0xBAFC06,3,0xD0,
 NULL,0xE29,0xE29,0,(void far *)0xBAFC06,3,0xD0,
 NULL,0xE2A,0xE2A,0,(void far *)0xBAFC06,3,0xD0,
 NULL,0xE2B,0xE2B,0,(void far *)0xBAFC06,3,0xD0,
 NULL,0xE2C,0xE2C,0,(void far *)0xBAFC06,3,0xD0,
 NULL,0xE2D,0xE2D,0,(void far *)0xBAFC06,3,0xD0,
 NULL,0xE2E,0xE2E,0,(void far *)0xBAFC06,3,0xD0
};

static const struct ITEMSHNDL far emenu_HNDLS[]=
{
 insline,
 delline,
 splitline,
 joinlines,
 instime,
 insdate,
 paste,
 clearclipb
};

static const struct MENUHEADER far emenu_HDR=
{
 5,9,95,21,NULL,0x2F5,0x7FFF 
};

static const struct MENUSTRUCT far emenu_STRUCT=
{
 NULL,NULL,NULL,NULL,
 (char far*)0xBAFC02,(char far*)0xBAFBFC,
 0x48,0x00,
 NULL,
 (struct ITEMSDATA far*)&emenu_ITEMS,
 (struct ITEMSHNDL far*)&emenu_HNDLS,
 8
};

int e_OnKey2(void far *data, int far *msg)
{
 int p=msg[1];
 if (p==0x2F)
 {
  //Создаем меню редактора
  setExtSIDTable(emenu_ExtendedSTR);
  CreateMenu02(menu_options,0,0,
               (struct MENUSTRUCT far*)&emenu_STRUCT,
               (struct MENUHEADER far*)&emenu_HDR,
               0,0,0,0);
 }
 if ((p==0x21)||(p==0x20)||(p==0x25)||(p==0x24)||(p==0x0E)||(p==0x0D)||(p==0x2F))
 {
  editmode=p;
  return(1);
 }
 data=data;
 return(0);
}

void e_redraw(void far *data)
{
 WS_HDR far *es;
 unsigned int sz;
 unsigned int p;
 int ecp;

 if (editmode==1)
 {
  ((int far *)data)[20]=curpos+1;
  editmode=2;
 }
 ecp=((int far *)data)[20];
 ecp--;
 while(ecp>curpos) doCurRight();
 while(ecp<curpos) doCurLeft();

 es=((void far * far *)data)[8];
 sz=WSTR_Len(es)-11;
 p=0;
 while((p<255)&&(p<sz))
 {
  int wc=WString_GetChar(es,p+12);
  editline[p]=char16to8(wc);
  p++;
 }
 editline[p]=0;
 if ((sz=dsp)==STKSZ) total_line++;
 sz=bl_ds(sz); //Ищем начало сл. строки
 sz-=p+1; //Вновь добавляемая строка
 strcpy(dstk+(dsp=sz),editline); //Добавляем
 draw_mode=1;
 DrawScreen();
}

void e_OnKey(void far* data, int param)
{
 void far * far *mp;
 mp=Find_Pointer0A_ItemInMenuITEMSQ_by_ID(data,4);
 memcpy(estr_methods,(void far *)(0xE06944),sizeof(estr_methods));
 estr_methods[0]=e_redraw;
 mp[1]=estr_methods;
 if (param==7)
 {
  SetSoftKeyID(data,0,1);
 }
} 

void e_OnKey3(void){}

static const DIALOG_HEADER e_header = { 0, 0, 101, 12, (char far*)0xBAFB8A, 0x2A3, 0x7FFF };
static const INPUTDIA_HDNL e_hndl = { 
 1, 0,
 e_OnKey2, //InputDiaOn2, //OnKeyUp????
 e_OnKey,
 e_OnKey3,
 NULL,
 (char far*)0xB8CF84,		//0xBAFBFC,
 0, 16, 101, 32,
 0, //Шрифт
 2, //Количество строк
 0, 0, 0, 0, 0, 0
};

void CreateEditDialog(void)
{
 INPUT_DATA data;
 WS_HDR far* ws_hdr2;
 unsigned int p;
 char c;

 ws_hdr2=AllocWSTR(256+11,MMIMalloc);
 CutWSTR(ws_hdr2,0);
 ClearInputData(&data);
 SetInputData(&data,NULL,256);
 SetInputType(&data,3);
 AddInputToWSTR(ws_hdr2,&data);
 if ((p=dsp)!=STKSZ)
 {
  while((c=dstk[p]))
  {
   AddCharToWSTR(ws_hdr2,char8to16(c));
   p++;
  }
 }
 if ((curline-viewline)>2)
 {
  *(int far *)(&e_hndl.y)=16;
  *(int far *)(&e_hndl.h)=32;
 }
 else
 {
  *(int far *)(&e_hndl.y)=48;
  *(int far *)(&e_hndl.h)=64;
 }
 CreateInputTextDialog((INPUTDIA_HDNL far*)&e_hndl, (DIALOG_HEADER far*)&e_header, ws_hdr2, 1);
}

void add_to_clip(unsigned char far *s)
{
 int f=FileOpen((STR)clipfile,
 clip_pasted?_O_CREAT+_O_RDWR+_O_TRUNC:_O_CREAT+_O_RDWR+_O_APPEND
 ,_S_IREAD);
 if (f!=-1)
 {
  FileWrite(f,s,strlen(s));
  FileClose(f);
 }
 EX_heap_free_with_lock(s);
 clip_pasted=0;
}

//------------------------------------------------------------------------------
// Осн. диалог
//------------------------------------------------------------------------------
int md_onKey(void far *data, struct MSG far *Msg)
{
 if ((Msg->Msg==0xF8)&&(Msg->Param[0]==dia_id))
 {
  if (IsFocus(data))
  {
   if (editmode>2) goto L_EDIT; //Перестартуем редактор
   DrawScreen();
  }
  return(0);
 }
 if (!IsFocus(data)) return(1);
 if ((Msg->Msg==0x90)||(Msg->Msg==0x88))
 {
  if (editmode>2)
  {
   //Вышли из редактора стрелками вверх/вниз, выполняем	переход
   switch(editmode)
   {
    case 0x0E:
	 PageUp(6);
	 break;
    case 0x21:
	case 0x25:
	 //Стрелка вверх
	 LineUp();
	 break;
	case 0x0D:
	 PageDw(6);
	 break;
	case 0x20:
	case 0x24:
	 //Стрелка вниз
	 LineDw();
	 break;
	case 0x2F:
	 //Создано меню
	 break;
   }
   CheckDiskAccess();
   if (disk_access) 
   {
    FilesysICall_UI(DoDiskAccess,1);
	return(0);
   }
   goto L_EDIT;
  }
  editmode=0; //Выключаем редактор
  //Если вышли из меню не выбрав пункт загрузки
  if (HISTORY.fmt==255)
  {
   if (disable_exit) //Если выбирали шрифт
   {
	disable_exit=0; //Не выходим
	return(1);
   }
   goto L_EXIT;
  }
  //Перерисовывание после меню, если ничего не изменили
  if (!draw_mode) draw_mode=1;
  DrawScreen();
  return(0); //Обработали сообщение 0x90
 }
 //if (Msg->Msg==0xB7) return(0); //Убить скроллер имени
 if (Msg->Msg==0xB8)
 {
  DrawScreen(); //Перерисовываем по таймеру
  return(1);
 }
 if (disk_access) return(1); //Если дисковые операции
 if (draw_mode&&(draw_mode!=4)) return(1); //Если еще рисуем
 if (Msg->Msg==KEY_UP)
 {
  if (draw_mode==4)
  {
   draw_mode=1;
   DrawScreen();
  }
  return(0);
 }
 if ((Msg->Msg==KEY_DOWN)||(Msg->Msg==LONG_PRESS)) 
 {
  switch(Msg->Param[0])
  {
   case RED_BUTTON:
    L_EXIT:
    CloseDialogWithSoftKeys(((int far *)data)[6]);
	//((unsigned int far *)data)[7]=0xFFFD;
	//((unsigned int far *)data)[8]=0x0000;
    return(0);
   case GREEN_BUTTON:
    L_EDIT:
    editmode=1;
	cursor_off=0;
	while ((curline-viewline)>=6) viewline++; //Если ниже чем 6 строк - перемещаемся
	CreateEditDialog();
	return(0);
   case RIGHT_SOFT:
    FilesysICall(SaveHistory);
    DrawLoadMenu();
    return(0);
   case LEFT_SOFT:
    DrawSoftMenu();
	return(0);
   case UP_BUTTON:
    if (cursor_off)
	 PageUp(1);
	 else
     LineUp();
    draw_mode=1;
    break;
   case RECORD_BUTTON:
    if (cursor_off||(dsp==STKSZ)) break;
	else
	{
	 unsigned int i=strlen(dstk+dsp); //Длина тек. строки
	 unsigned char far *s;
	 s=EX_heap_malloc_with_lock(i+2); //Т.к. добавляем 0 и 0d
	 if (s)
	 {
	  strcpy(s,dstk+dsp);
	  s[i]=0x0D;
	  s[i+1]=0x00;
	  FilesysICall_FP(add_to_clip,s);
     }
	}
   case DOWN_BUTTON:
    if (cursor_off)
	 PageDw(1);
	 else
     LineDw();
    draw_mode=1;
    break;
   case VOL_UP_BUTTON:
   case '2':
    PageUp(max_y);
    draw_mode=1;
    break;
   case VOL_DOWN_BUTTON:
   case '8':
    PageDw(max_y);
    draw_mode=1;
    break;
   case '5':
    cursor_off^=1;
	draw_mode=1;
	break;
   case LEFT_BUTTON:
    if (Msg->Msg!=LONG_PRESS)
	{
     doCurLeft();
     break;
	}
   case '4':
    //На слово влево
    WordLeft();
	cursor_off&=0xFE;
	draw_mode=1;
    break;
   case RIGHT_BUTTON:
    if (Msg->Msg!=LONG_PRESS)
	{
     doCurRight();
     break;
	}
   case '6':
    //На слово вправо
    WordRight();
	cursor_off&=0xFE;
	draw_mode=1;
    break;
   case '9':
    ToEOL();
	cursor_off&=0xFE;
	draw_mode=1;
	break;
   case '7':
    ToBOL();
	cursor_off&=0xFE;
	draw_mode=1;
	break;
   case '0':
    draw_mode=4;
	break;
   case '#':
    if (win_dos_koi>1) win_dos_koi=0; else win_dos_koi++;
    draw_mode=1;
    break;
   default:          
    break;
  }
  CheckDiskAccess();
  if (disk_access) FilesysICall_UI(DoDiskAccess,1);
  DrawScreen();
  return(0);
 }
 return(1); //Оставить для остальных
}

//===================================================================
// Меню загрузки файла
//===================================================================
void sf_OnKey(char far* data, int param)
{
 WS_HDR far* ESTR;
 unsigned int p=0;
 unsigned int sz;

 extern void savetext(void);

 if (param!=0xB) return;
 if (sub_E70BAC(data)) return;
 ESTR=GetDataPointer_ITEM04(data);
 sz=WSTR_Len(ESTR)-11;
 p=0;
 while((p<255)&&(p<sz))
 {
  int wc=WString_GetChar(ESTR,p+12);
  if ((wc>=0x410)&&(wc<0x450)) wc-=0x350;
  filename[p]=wc;
  p++;
 }
 filename[p]=0;
 HISTORY.fmt=0; //Грузим его теперь как DirectLoad
 disk_access=SAVE_FILE;
 FilesysICall(savetext);
 //memcpy(HISTORY.name,filename,sizeof(HISTORY.name));
}

static const DIALOG_HEADER sf_header = { 0, 0, 101, 12, (char far*)0xBAFB8A, 0xE2F, 0x7FFF };
static const INPUTDIA_HDNL sf_hndl = { 
 1, 0,
 NULL, //InputDiaOn2, //OnKeyUp????
 sf_OnKey,
 NULL,
 NULL,
 (char far*)0xBAFBFC,
 0, 16, 101, 64,
 3, //Шрифт
 2, //Количество строк
 0, 0, 0, 0, 0, 0
};

void CreateSaveAsDialog(void)
{
 unsigned int wstr[128];
 WS_HDR ws_hdr1;
 INPUT_DATA data;
 WS_HDR far* ws_hdr2;
 ClearWideString(&ws_hdr1,(unsigned int far*)&wstr,128);
 CutWSTR(&ws_hdr1,0);
 ws_hdr2=AllocWSTR(128+11,MMIMalloc);
 CutWSTR(ws_hdr2,0);
 ClearInputData(&data);
 SetInputData(&data,&ws_hdr1,127);
 SetInputType(&data,3);
 AsciizToWSTR(&ws_hdr1,filename);
 AddInputToWSTR(ws_hdr2,&data);
 CreateInputTextDialog((INPUTDIA_HDNL far*)&sf_hndl, (DIALOG_HEADER far*)&sf_header, ws_hdr2, 1);
}

static const STR loadmenu_ExtendedSTR[]=
{
 "Load options",
 "Font size=4",
 "Font size=6",
 "Font size=8",
 "Direct load",
 "Load & Format",
 "EOL->EOL SPC",
 "Extend lines",
 "Save as..."
};

static const struct ITEMSDATA far loadmenu_ITEMS[]=
{//0xB867CE
 (char far*)0xBAFC22, 0xE28,0xE28, 0,NULL,3,0x53A,
 (char far*)0xBAFC22, 0xE29,0xE29, 0,NULL,3,0x53A,
 (char far*)0xBAFC22, 0xE2A,0xE2A, 0,NULL,3,0x53A,
 (char far*)0xBAFC22, 0xE2B,0xE2B, 0,NULL,3,0x53A,
 (char far*)0xBAFC22, 0xE2C,0xE2C, 0,NULL,3,0x53A,
 (char far*)0xBAFC22, 0xE2D,0xE2D, 0,NULL,3,0x53A,
 (char far*)0xBAFC22, 0xE2E,0xE2E, 0,NULL,3,0x53A,
 (char far*)0xBAFC22, 0xE2F,0xE2F, 0,NULL,3,0x53A
};


void loadfont(int flag)
{
 char fn_font[32];
 int fin;
 xsprintf(fn_font,(STR)fntfilename,font_size);
 if ((fin=FileOpen(fn_font,_O_RDONLY+_O_BINARY,0))!=-1)
 {
  FileRead(fin,font,sizeof(font));
  FileClose(fin);
 }
 max_x=96/font_size;
 max_y=80/8;
 if(flag)
 {
  disk_access=0;
  draw_mode=1;
  if (!terminated) SendMessage((void far *)0x3B5E8,0xF8,dia_id); //Перерисовываем
 }
}

void setfont(char sz)
{
 font_size=sz;
 if (disk_access==FIRSTLOAD) //Пришли из первой загрузки
 {
  disable_exit=1;
  DisplayMessageBoxV(1,0x55A);
  return;
 }
 disk_access=FIRSTLOAD;
 draw_mode=255; //Экран приветствия
 FilesysICall_I(loadfont,1);
 GeneralFunc(1);
}

void load_setfont4(void){setfont(4);}
void load_setfont6(void){setfont(6);}
void load_setfont8(void){setfont(8);}

void FirstLoadFile(unsigned int fmt);

void load_direct(void)
{
 draw_mode=255;
 disk_access=FIRSTLOAD;
 HISTORY.fmt=0;
 GeneralFunc(1);
 FilesysICall_UI(FirstLoadFile,0);
}

void load_format(void)
{
 draw_mode=255;
 disk_access=FIRSTLOAD;
 HISTORY.fmt=1;
 GeneralFunc(1);
 FilesysICall_UI(FirstLoadFile,1);
}

void load_eolspc(void)
{
 draw_mode=255;
 disk_access=FIRSTLOAD;
 HISTORY.fmt=2;
 GeneralFunc(1);
 FilesysICall_UI(FirstLoadFile,2);
}

void load_pad(void)
{
 HISTORY.fmt^=0x80;
 GeneralFunc(1);
}

void loadmenu_ITEMPROC(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 switch(Curr)
 {
  case 0: if (font_size==4) i=1; else i=0; break;
  case 1: if (font_size==6) i=1; else i=0; break;
  case 2: if (font_size==8) i=1; else i=0; break;
  case 3: if ((HISTORY.fmt&0x7F)==0) i=1; else i=0; break;
  case 4: if ((HISTORY.fmt&0x7F)==1) i=1; else i=0; break;
  case 5: if ((HISTORY.fmt&0x7F)==2) i=1; else i=0; break;
  case 6: if (!(HISTORY.fmt&0x80)) i=1; else i=2; break;
  case 7: i=4; break;
 }
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void load_save(void)
{
 GeneralFunc(1);
 CreateSaveAsDialog();
}

static const struct ITEMSHNDL far loadmenu_HNDLS[]=
{
 load_setfont4,
 load_setfont6,
 load_setfont8,
 load_direct,
 load_format,
 load_eolspc,
 load_pad,
 load_save
};

static const struct MENUHEADER far loadmenu_HDR=
{
 0x00,0x00,0x65,0x0C,(char far*)0xB88DB0,0xE27,0x7FFF 
};

static const struct MENUSTRUCT far loadmenu_STRUCT=
{
 NULL,NULL,NULL,NULL,
 (char far*)0xBAFC06,(char far*)0xBAFBFC,
 0x01,0x00,
 loadmenu_ITEMPROC,
 (struct ITEMSDATA far*)&loadmenu_ITEMS,
 (struct ITEMSHNDL far*)&loadmenu_HNDLS,
 8
};

void DrawLoadMenu(void)
{
 setExtSIDTable(loadmenu_ExtendedSTR);
 CreateMenu02(menu_full,0,0,
              (struct MENUSTRUCT far*)&loadmenu_STRUCT,
              (struct MENUHEADER far*)&loadmenu_HDR,
              0,0,0,0);
}

//-------------------------------------------------------------------
//===================================================================
// Ввод строки
//===================================================================
void InDigitStrProcess(STR s, char key, unsigned int protect_len, unsigned int edit_len)
{
 unsigned int l=strlen(s);
 unsigned int l1=l-protect_len;
 if ((key>='0')&&(key<='9')&&(l1<edit_len))
 {
  s[l]=key; s[l+1]=0;
 }
 if ((key==LEFT_SOFT)&&(l1!=0))
 {
  s[l-1]=0;
 }
 FillRect(0,0,101,80,0);
 DrawString(0,0,101,56,s,3);
 DrawString(0,56,101,12,(STR)"Digit - input",3);
 DrawString(0,68,50,12,(STR)"Clear",3);
 DrawString(51,68,50,12,(STR)"Ok",3);
}

static const STR gl_01[]=
{
 "Go to line\nnumber:\n\0\0\0\0\0",
 "Go to percent\n\0\0\0"
};

unsigned int far gl_num;

unsigned long my_atoi(STR s)
{
 unsigned long r=0;
 for(;;)
 {
  unsigned char c=*s-'0';
  if (c>9) break;
  r=r*10+c;
  s++;
 }
 return(r);
}

void gl_oncreate(void)
{
 int i=gl_num;
 InDigitStrProcess(gl_01[i],0,i?14:19,i?3:5);
}

int gl_onkey(void far* data, struct MSG far* Msg)
{
 int i=gl_num;
 if (IsFocus(data))
 {
  InDigitStrProcess(gl_01[i],0,i?14:19,i?3:5);
  if (Msg->Msg==KEY_DOWN)
  {
   switch(Msg->Param[0])
   {
    case RIGHT_SOFT:
    case GREEN_BUTTON:
     if (i)
	 {
	  if (!gl_01[1][14]) return(0);
	  seek_to_line=total_line*my_atoi(gl_01[1]+14)/100;
     }
	 else
	 {
	  if (!gl_01[0][19]) return(0);
	  seek_to_line=my_atoi(gl_01[0]+19);
     }
     draw_mode=2; //Переходим на нужную строку
     FilesysICall(GotoLine);
    case RED_BUTTON:
     CloseDialogWithSoftKeys(((int far *)data)[6]);
 	return(0);
   }
   InDigitStrProcess(gl_01[i],Msg->Param[0],i?14:19,i?3:5);
   return(0);  
  }
  if ((Msg->Msg==KEY_UP)||(Msg->Msg==LONG_PRESS)) return(0);
 }
 return(1);
}

//===================================================================
// Меню Goto...
//===================================================================
void goto_l(unsigned long l)
{
 seek_to_line=l;
 draw_mode=2; //Переходим на нужную строку
 FilesysICall(GotoLine);
 GeneralFunc(1);
}

void goto_top(void)
{
 goto_l(0);
}

void goto_bot(void)
{
 goto_l(0xFFFFFFFF);
}

void gl_onclose(void){}
static const struct DIALOG_WSK gl_dialog=
{gl_onkey,gl_oncreate,gl_onclose,0x2C, sizeof(md_tab)/sizeof(unsigned int),md_tab};

void goto_line(void)
{
 unsigned char mmm[0x2C];
 gl_num=0; //Признак перехода по строке
 GeneralFunc(1);
 CreateDialogWithSoftKeys(&gl_dialog,mmm,0);
}

void goto_percent(void)
{
 unsigned char mmm[0x2C];
 gl_num=1; //Признак перехода по процентам
 GeneralFunc(1);
 CreateDialogWithSoftKeys(&gl_dialog,mmm,0);
}

void goto_last_saved(void)
{
 goto_l(HISTORY.line);
}

void goto_book1(void)
{
 goto_l(HISTORY.bookm1);
}
void goto_book2(void)
{
 goto_l(HISTORY.bookm2);
}
void goto_book3(void)
{
 goto_l(HISTORY.bookm3);
}
void goto_book4(void)
{
 goto_l(HISTORY.bookm4);
}

static const struct ITEMSDATA far gotomenu_ITEMS[]=
{
 NULL, 0xE2B,0xE2B, 0,NULL,3,0x2BC,
 NULL, 0xE2C,0xE2C, 0,NULL,3,0x2BC,
 NULL, 0xE2D,0xE2D, 0,NULL,3,0x2BC,
 NULL, 0xE2E,0xE2E, 0,NULL,3,0x2BC,
 NULL, 0xE2F,0xE2F, 0,NULL,3,0x2BC,
 NULL, 0xE30,0xE30, 0,NULL,3,0x2BC,
 NULL, 0xE31,0xE31, 0,NULL,3,0x2BC,
 NULL, 0xE32,0xE32, 0,NULL,3,0x2BC,
 NULL, 0xE33,0xE33, 0,NULL,3,0x2BC
};

static const struct ITEMSHNDL far gotomenu_HNDLS[]=
{
 goto_top,
 goto_line,
 goto_percent,
 goto_bot,
 goto_last_saved,
 goto_book1,
 goto_book2,
 goto_book3,
 goto_book4
};

static const struct MENUHEADER far gotomenu_HDR=
{
 0x00,0x00,0x65,0x0C,(char far*)0xB88DB0,0xE28,0x7FFF 
};

static const struct MENUSTRUCT far gotomenu_STRUCT=
{
 NULL,NULL,NULL,NULL,
 (char far*)0xBAFC06,(char far*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (struct ITEMSDATA far*)&gotomenu_ITEMS,
 (struct ITEMSHNDL far*)&gotomenu_HNDLS,
 9
};

void goto_menu(void)
{
 GeneralFunc(1);
 CreateMenu02(menu_full,0,0,
              (struct MENUSTRUCT far*)&gotomenu_STRUCT,
              (struct MENUHEADER far*)&gotomenu_HDR,
              0,0,0,0);
}

//===================================================================
// Меню Bookmark...
//===================================================================
void set_book1(void)
{
 HISTORY.bookm1=curline;
 GeneralFunc(1);
}

void set_book2(void)
{
 HISTORY.bookm2=curline;
 GeneralFunc(1);
}

void set_book3(void)
{
 HISTORY.bookm3=curline;
 GeneralFunc(1);
}

void set_book4(void)
{
 HISTORY.bookm4=curline;
 GeneralFunc(1);
}

static const struct ITEMSDATA far bookmenu_ITEMS[]=
{
 NULL, 0xE30,0xE30, 0,NULL,3,0x2BC,
 NULL, 0xE31,0xE31, 0,NULL,3,0x2BC,
 NULL, 0xE32,0xE32, 0,NULL,3,0x2BC,
 NULL, 0xE33,0xE33, 0,NULL,3,0x2BC
};

static const struct ITEMSHNDL far bookmenu_HNDLS[]=
{
 set_book1,
 set_book2,
 set_book3,
 set_book4
};

static const struct MENUHEADER far bookmenu_HDR=
{
 0x00,0x00,0x65,0x0C,(char far*)0xB88DB0,0xE29,0x7FFF 
};

static const struct MENUSTRUCT far bookmenu_STRUCT=
{
 NULL,NULL,NULL,NULL,
 (char far*)0xBAFC06,(char far*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (struct ITEMSDATA far*)&bookmenu_ITEMS,
 (struct ITEMSHNDL far*)&bookmenu_HNDLS,
 4
};

void bookm_menu(void)
{
 GeneralFunc(1);
 CreateMenu02(menu_full,0,0,
              (struct MENUSTRUCT far*)&bookmenu_STRUCT,
              (struct MENUHEADER far*)&bookmenu_HDR,
              0,0,0,0);
}
//===================================================================
// Меню по левому софту
//===================================================================

void search_menu(void)
{
}

static const STR softmenu_ExtendedSTR[]=
{
 "TED menu",
 "Goto...", //8
 "Set Bookmark...", //9
 "Search...", //A
 "Top", //B
 "Line...", //C
 "Percent...", //D
 "Bottom", //E
 "Last saved", //F
 "BM1 000000", //30
 "BM2 000000", //31
 "BM3 000000", //32
 "BM4 000000", //33
};

static const struct ITEMSDATA far softmenu_ITEMS[]=
{
 NULL, 0xE28,0xE28, 0,NULL,3,0x2BC,
 NULL, 0xE29,0xE29, 0,NULL,3,0x2BC,
 NULL, 0xE2A,0xE2A, 0,NULL,3,0x2BC
};

static const struct ITEMSHNDL far softmenu_HNDLS[]=
{
 goto_menu,
 bookm_menu,
 search_menu
};

static const struct MENUHEADER far softmenu_HDR=
{
 0x00,0x00,0x65,0x0C,(char far*)0xB88DB0,0xE27,0x7FFF 
};

static const struct MENUSTRUCT far softmenu_STRUCT=
{
 NULL,NULL,NULL,NULL,
 (char far*)0xBAFC06,(char far*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (struct ITEMSDATA far*)&softmenu_ITEMS,
 (struct ITEMSHNDL far*)&softmenu_HNDLS,
 3
};

void DrawSoftMenu(void)
{
 static const char lfrm[]="%lu";
 xsprintf((STR)(softmenu_ExtendedSTR[9]+4),(STR)lfrm,HISTORY.bookm1);
 xsprintf((STR)(softmenu_ExtendedSTR[10]+4),(STR)lfrm,HISTORY.bookm2);
 xsprintf((STR)(softmenu_ExtendedSTR[11]+4),(STR)lfrm,HISTORY.bookm3);
 xsprintf((STR)(softmenu_ExtendedSTR[12]+4),(STR)lfrm,HISTORY.bookm4);
 setExtSIDTable(softmenu_ExtendedSTR);
 CreateMenu02(menu_full,0,0,
              (struct MENUSTRUCT far*)&softmenu_STRUCT,
              (struct MENUHEADER far*)&softmenu_HDR,
              0,0,0,0);
}

//======================================================================

//Загрузка куска входного файла
unsigned int FL_loader(int fin, unsigned int p)
{
 unsigned int i;
 if (p==0xFFFF)
 {
  //Первая загрузка
  i=FileRead(fin,dstk,STKSZ);
  if (i<STKSZ)
  {
   //Загрузили меньше 16 кб
   dstk[i]=0; //Маркер конца файла
  }
  p=0;
 }
 if (p>STKSZ50) //Если указатель перешел за границу половины буфера
 {
  memcpy(dstk,dstk+STKSZ50,STKSZ50); //Копируем конец буфера в начало
  p-=STKSZ50; //Новый указатель
  i=FileRead(fin,dstk+STKSZ50,STKSZ50); //Следующие 8 кБ
  if (i<STKSZ50) dstk[i+STKSZ50]=0; //Меньше 8кБ - маркер конца файла
 }
 return(p);
}

//Запись верхнего стека при начальной загрузке
unsigned int FL_saver(int fs, unsigned int p)
{
 unsigned char b;
 long ul;
 if (p>=STKSZ50)
 {
  b=FindFreeBlock();
  ubat[++u_disk]=b;
  FileSeek(fs,_mulu32(STKSZ50,b),0,&ul);
  FileWrite(fs,ustk,STKSZ50);
  p-=STKSZ50;
  memcpy(ustk,ustk+STKSZ50,p);
 }
 return(p);
}

//==================================================================
// Первая загрузка файла
// TODO:
// - разбиение на строки с учетом переноса по словам
// - юникод
//==================================================================
void FirstLoadFile(unsigned int fmt)
{
 int fin;
 int fs;

 u_disk=-1; //Дисковый указатель верхнего стека
 d_disk=-1; //Дисковый указатель нижнего стека
 memset(ubat,0,sizeof(ubat)); //Прочищаем таблицу блоков верхнего стека
 memset(dbat,0,sizeof(dbat)); //Прочищаем таблицу блоков нижнего стека
 memset(bfree,0,sizeof(bfree)); //Прочищаем таблицу занятых блоков
 usp=0; //Указатель на верхний стек
 dsp=STKSZ; //Указатель на нижний стек
 viewpos=0;
 curpos=0;
 curline=0;
 CSM=0;

 draw_mode=255;
 if (fmt==0xFFFF)
 {
  //Ищем историю
  if (SearchHistory()&0x8000)
  {
   font_size=HISTORY.font;
   fmt=HISTORY.fmt;
   win_dos_koi=HISTORY.codepage;
   cursor_off=HISTORY.cursor_off;
  }
  else
  {
   win_dos_koi=0xFF; //Неизвестный
   font_size=4; //Минимальный шрифт
   memset(&HISTORY.line,0,4*6); //Все на самом верху
   HISTORY.cursor_off=cursor_off=0; //Включить курсор
   HISTORY.total=1;
   HISTORY.fmt=255; //Первый запуск!!!!
   DrawLoadMenu(); //Определяем, как грузить через меню
   SendMessage(pid_MMI,0x85,0); //Перерисовываем
   return;
  }
 }
 if ((fmt&0x7F)>2) fmt=0;
 switch(font_size)
 {
  case 4:
  case 6:
  case 8:
   break;
  default:
   font_size=4;
  break;
 }
 //Загружаем шрифт
 loadfont(0);

 //Конвертируем все строки в верхний стек
 fs=FileOpen((STR)stkfile,_O_CREAT+_O_RDWR+_O_BINARY,_S_IREAD); //Файл верхнего стека

 if ((fin=FileOpen((STR)filename,_O_RDONLY+_O_BINARY,0))!=-1)
 {
  switch(fmt&0x7F)
  {
   case 1:
   case 2:
     usp=ConvertFormat(fin,fs,fmt&0x7F);
     break;
   case 0:
   default:
     if (win_dos_koi==0xFF) win_dos_koi=0;
     usp=ConvertSimple(fin,fs);
	 break;
  }
  FileClose(fin);
 }
 FileClose(fs);
 total_line=viewline=curline; //Находимся в последней строке
 HISTORY.font=font_size;
 HISTORY.fmt=fmt;
 memcpy(HISTORY.name,filename,sizeof(HISTORY.name));
 //Расчитываем, куда перейти
 if (HISTORY.total==total_line)
 {
  seek_to_line=HISTORY.line; //Прямой переход
 }
 else
 {
  if (HISTORY.total)
   seek_to_line=(total_line*HISTORY.line)/HISTORY.total; //По отношению
  else
   seek_to_line=0;
   HISTORY.line=seek_to_line;
 }
 draw_mode=2; //Переходим на нужную строку
 GotoLine();
}

void EraseSnapShoot(void)
{
 FileErase((STR)snapshootfile);
}

void LoadSnapShoot(void)
{
 char mmm[0x2C];
 int f;
 if ((f=FileOpen((STR)snapshootfile,_O_BINARY+_O_RDONLY,0))!=-1)
 {
  FileRead(f,(void far *)0x80000,16384);
  FileRead(f,(void far *)0x84000,16384);
  FileRead(f,(void far *)0x88000,16384);
  FileRead(f,(void far *)0x8C000,16384);
  FileRead(f,(void far *)0x90000,16384);
  FileClose(f);
  EraseSnapShoot();
  terminated=2;
  stk_fhandle=-1;
  dia_id=CreateDialogWithSoftKeys(&main_dia,mmm,0); //Создаем основной диалог
  SendMessage(pid_MMI,0x85,0); //Перерисовываем
 }
}

void xmain(STR file_to_load)
{
 char mmm[0x2C];
 filename[0]=0;
 if (*((int far *)0x3B1DC)!=0x3870) goto LOAD_SNAP;
 if (_pag(file_to_load)==0x35)
 {
  if (strlen(file_to_load)<0x7F)
  {
   strcpy((STR)filename,file_to_load);
   goto LOAD_TEXT;
  }
 }
 LOAD_SNAP:
 FilesysICall(LoadSnapShoot);
 return;
 LOAD_TEXT:
 FilesysICall(EraseSnapShoot);
 clip_pasted=1;
 draw_mode=255; //Приветствие
 disk_access=FIRSTLOAD;
 font_size=4;
 terminated=0;
 editmode=0;
 stk_fhandle=-1;
 dia_id=CreateDialogWithSoftKeys(&main_dia,mmm,0); //Создаем основной диалог
 SendMessage(pid_MMI,0x85,0); //Перерисовываем
}



