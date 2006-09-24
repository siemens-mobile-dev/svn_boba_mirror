//---------------------------------------------
// Програмное обеспечение прибора ПУ-А1
// Copyright (C) 2001-2005 by Rst7
//---------------------------------------------

volatile char DBG1;

//Определяет ПУ-А или ПУ-А1
#include "devdef.h"

#define __8_BIT_ASCII_WANTED

#define _NOP __no_operation

// Подключение стандартных файлов описания
#include "stdio.h"

#define puts !!!ERROR!!!
#define putc !!!ERROR!!!

// Переназначение процедур обмена с EEPROM
#undef _EEGET
#undef _EEPUT
//void WriteEEPROM(unsigned int adr, unsigned char val);
//char ReadEEPROM(unsigned int adr);
//#define _EEPUT(ADR,VAL)  { WriteEEPROM((unsigned int)(ADR),(unsigned char)(VAL)); }
//#define _EEGET(VAR, ADR) { (VAR)=ReadEEPROM((unsigned int)(ADR)); }

#include "pgmspace.h"
typedef char __flash * STR;

#include "iom128.h"
#include "inavr.h"
#include "setjmp.h"

#define _CLI __disable_interrupt
#define _SEI __enable_interrupt


#define USR UCSR0A
#define UCR UCSR0B
#define UDR UDR0
#define UBRR UBRR0L
#define WaitTX while(PINE&2)

#include "ctype.h"
#include "can.h"

#undef tiny
#define tiny

//-----------------------------
// Примитивы для работы с 45??021
//-----------------------------

// Разрешение сигналом CS
#define FlashOn PORTB&=0xFE
// Запрет сигналом CS
#define FlashOff PORTB|=0x01
// Ожидание завершения обмена по SPI
#define WaitSPI while(!(SPSR&0x80))
// Ожидание окончания операции в 45??021
#define WaitFlash while(!(ReadFlashStatus()&0x80))

// Переход на новую строку (для принтера)
#define PRINT_CRLF {putchar(13);putchar(10);}

//-----------------------------
// Описание данных в EEPROM процессора
//-----------------------------
// Номер ПУ
__eeprom __no_init char PU_ID @0x06;
// Статус МРВ1, 0x80 - включен
__eeprom __no_init char MRV1_STAT @0x01;
// Статус МРВ2, 0x80 - включен
__eeprom __no_init char MRV2_STAT @0x02;
// Статус ТК, 0x80 - включен
__eeprom __no_init char TK_STAT @0x03;

// Конфигурация ПУ
// бит0=0 Большие/маленькие буквы
// бит0=1 Только большие буквы
// бит1=0 Английские/русские буквы
// бит1=1 Только русские буквы
// бит2=0 Реле ОТКАЗ
// бит2=1 Реле ПОЖАР 120С
// бит3=0 Реле ПОЖАР до сброса
// бит3=1 Реле ПОЖАР до звук откл.
// бит4=0 Звук включен
// бит4=1 Звук отключен
// бит5=0 Дублированные RS0/RS1
// бит5=1 Независимые RS0/RS1
#define INDEPENDED_RS (COPY_PU_STAT&0x20)
// бит6=0 Нет дубл. ПУ
// бит6=1 Ecть дубл. ПУ
#define DOUBLE_PU (COPY_PU_STAT&0x40)
// бит7=0 По умолчанию - мастер
#define IS_DOUBLER_DEFAULT (COPY_PU_STAT&0x80)

__eeprom __no_init char PU_STAT @0x05;

__eeprom __no_init char PU_STAT2 @0x04;

#define IS_ALLWAYS_RESET (PU_STAT2&1)
#define IS_NOLOG_ACK (PU_STAT2&2)
#define IS_TAMPER_DISABLED (PU_STAT2&4)

//Результат тестирования
//0 - тест кнопки "Звук откл."
//1 - тест кнопки "Стрелка влево"
//2 - тест кнопки "Стрелка вправо"
//3 - тест кнопки "Стрелка вверх
//4 - тест кнопки "Стрелка вниз
//5 - тест кнопки "Сброс"
//6 - тест PC клавиатуры "1"
//7 - тест PC клавиатуры "2"
//8 - тест PC клавиатуры "3"
//9 - тест PC клавиатуры "4"
//10 - тест PC клавиатуры "5"
//11 - тест PC клавиатуры "6"
//12 - тест PC клавиатуры "7"
//13 - тест PC клавиатуры "8"
//14 - тест PC клавиатуры "9"
//15 - тест PC клавиатуры "0"
//16 - тест RS0/RS1
//17 - тест RS2
//18 - тест принтера

__eeprom __no_init char TEST_STAT @0x07;
volatile char TSTS; //Копия TEST_STAT
volatile char TSTDBG; //Отладочная печать тестирования
__flash STR testm1[]=
{
  //012345678901234567890123
  (STR)"Нажми 0 на PC клавиатуре", //6
  (STR)"Нажми 1 на PC клавиатуре", //7
  (STR)"Нажми 2 на PC клавиатуре", //8
  (STR)"Нажми 3 на PC клавиатуре", //9
  (STR)"Нажми 4 на PC клавиатуре", //10
  (STR)"Нажми 5 на PC клавиатуре", //11
  (STR)"Нажми 6 на PC клавиатуре", //12
  (STR)"Нажми 7 на PC клавиатуре", //13
  (STR)"Нажми 8 на PC клавиатуре", //14
  (STR)"Нажми 9 на PC клавиатуре", //15
  (STR)"Не тестирован RS0/RS1! Статус: %03d", //16
  (STR)"Не тестирован RS2!", //17
  (STR)"Не тестирован принтер!"  //18
};

__flash char lpt_tab[]=
{
  0x00,0x2B,0x2B,0xFF,0x2B,0xFF,0xFF,0xFF,0x2F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x02,0x80,0x82,0x08,0x0A,0x88,0x8A,0x20,0x22,0xA0,0xA2,0x28,0x2A,0xA8,0xAA,
  0x2B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x01,0x03,0x81,0x83,0x09,0x0B,0x89,0x8B,0x21,0x23,0xA1,0xA3,0x29,0x2B,0xA9,0xAB
};

// Время работы прибора в часах, 0 - Время закончилось, 65535 - работать вечно
__eeprom __no_init unsigned int TRIALTIME @0x0C;

// Данные по БР-А, 0x80 - включен
__eeprom __no_init char PPKP_AREA[64] @0x10;

// Данные по зонам
// Таймаут для зон
__eeprom __no_init char ZONETO_AREA[256] @0x050;
// Количество необходимых сработок для зон
__eeprom __no_init char ZONEMF_AREA[256] @0x150;
// Точки срабатывания, подробнее смотри ZoneJob
__eeprom __no_init unsigned int ZONEPOINT_AREA[1024] @0x250;
// Пароль второго уровня доступа
__eeprom __no_init unsigned int PWDlevel2[2] @0xA50;
// Пароль третьего уровня доступа
__eeprom __no_init unsigned int PWDlevel3[2] @0xA58;
// Таблица перекодировки кодов событий для ТК
__eeprom __no_init char TKtable[256] @0xA60;

// Статус ВПК, 0x80 - включен
__eeprom __no_init char VPK_STAT @0xB60;
// Статус ВСТ, 0x80 - включен
__eeprom __no_init char VST_STAT @0xB61;
// Статус ДВП, 0x80 - включен
__eeprom __no_init char DVP_STAT @0xB62;
// Статус ВТК, 0x80 - включен
__eeprom __no_init char VTK_STAT @0xB63;

// Данные по внешним исполнительным устройствам (БРА), 0x80 - включено
__eeprom __no_init char BRA_AREA [32] @0xB70;
// Данные по АПС, 0x80 - включен
__eeprom __no_init char UPS_AREA [32] @0xB90;
// Задержки по зонам
__eeprom __no_init char ZONEDELAY_AREA[1024] @0xC00;

//---------------------------------------------------------------
// Описание структур
//---------------------------------------------------------------

//Структура конфигурации извещателя, подробнее смотри протокол передачи сообщений прибора ПУ
struct TSensorConfig
{
  char GeoPos[24];
  char Zone1;
  char Type;
  char Zone2;
  char TCode;
  char Zone4;
  char Zone3;
  int Led;
};

// Структура события в системе, подробнее смотри протокол передачи сообщений прибора ПУ
struct TLogEvent
{
  char PPKP;
  char Sensor;
  char Event;
  char Event2;
  unsigned long Second;
};

// Пакет для внешних приборов, смотри протокол передачи сообщений прибора ПУ
struct TDVPpacket
{
  char Packet_Type;

  struct TLogEvent ev;       //8

  struct TSensorConfig sc;   //32

  char TKEvent;
  char LED;
  char LEDflash;
  char SndMode;
  char FreeSpace[4];

} DVPpacket;

//---------------------------------------------------------------
// Текстовые сообщения
//---------------------------------------------------------------

//Таблица перекодировки русских букв
static __flash char Win2LCDTable[]=
{
  'A',0xa0,'B',0xa1,0xe0,'E',0xa3,0xa4,
  0xa5,0xa6,'K',0xa7,'M','H','O',0xa8,
  'P','C','T',0xa9,0xaa,'X',0xe1,0xab,
  0xac,0xe2,0xad,0xae,'b',0xaf,0xb0,0xb1,
  'a',0xb2,0xb3,0xb4,0xe3,'e',0xb6,0xb7,
  0xb8,0xb9,0xba,0xbb,0xbc,0xbd,'o',0xbe,
  'p','c',0xbf,'y',0xe4,'x',0xe5,0xc0,
  0xc1,0xe6,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7
};

__flash char Message_Trial[]=
"Ознакомительный режим";
__flash char Message_NeedMoney[]=
"Обратитесь к поставщику";

// Сообщения основного меню
__flash STR Message_MainMenu[]=
{
  (STR)"Фотон-А",
  (STR)"Просмотр архива событий",
  (STR)"Перезапуск системы",
  (STR)"Конфигурация ПУ",
  (STR)"Конфигурация БР-А",
  (STR)"Конфигурация извещателей",
  (STR)"Конфигурация зон",
  (STR)"Конфигурация ТК",
  (STR)"Конфигурация БРА/ПКП",
  (STR)"Конфигурация АПС",
  (STR)"Изменение адресов",
  (STR)"Отладка приборов"
};

__flash STR Message_ViewLog[]=
{
#ifdef SECURITY
  (STR)"Тревоги",
#else
(STR)"Пожары",
#endif
(STR)"Отказы",
(STR)"Предупреждения",
(STR)"Все",
(STR)"Текущие отключения"
};

__flash STR Message_Boot[]=
{
  (STR)"Горячий перезапуск",
  (STR)"Холодный перезапуск",
  (STR)"Проверка индикации",
  (STR)"Искать основной ПУ"
};

__flash STR Message_ConfigPU[]=
{
  (STR)"Установка даты и времени",
  (STR)"Конфигурация с ПК",
  (STR)"Настройка паролей",
  (STR)"Печать всей конфигурации",
  (STR)"Наличие доп. устройств",
  (STR)"Настройка вывода текста",
  (STR)"Сбор данных по нал. изв.",
  (STR)"Изменение адреса ПУ",
  (STR)"Реле ОТКАЗ",
#ifdef SECURITY
(STR)"Реле ТРЕВОГА 120С",
#else
(STR)"Реле ПОЖАР 120С",
#endif

(STR)"Реле ПОЖАР до сброса",
(STR)"Реле ПОЖАР до звук откл.",

(STR)"Звук включен",
(STR)"Звук отключен",

(STR)"Дублирование RS0/RS1",
(STR)"Независимые RS0/RS1",

(STR)"Одиночный ПУ",
(STR)"Дублирование ПУ",

(STR)"Основной ПУ по умолчанию",
(STR)"Дублирующий ПУ по умолчанию",

(STR)"СБРОС после ЗВУК ОТКЛ.",
(STR)"СБРОС всегда",

(STR)"ЗВУК ОТКЛ - посл. события",
(STR)"ЗВУК ОТКЛ - только звук",

(STR)"Тампер включен",
(STR)"Тампер отключен"
};

__flash STR Message_Debug[]=
{
  (STR)"Патчер EEPROM абонентов",
  (STR)"Чтение статуса изв-ля",
  (STR)"Чтение установок изв-ля",
  (STR)"Чтение токов ответа",
  (STR)"Очистка Flash-памяти",
  (STR)"Очистка архива событий",
  (STR)"Отладка МРВ",
  (STR)"Диагностика Фотон-П",
};

__flash STR Message_EditAdr[]=
{
  (STR)"Изменение адреса БР-А",
  (STR)"Изменение адреса изв-ля",
  (STR)"Установить шлейф изв-лей",
  (STR)"Снять шлейф изв-лей",
  (STR)"Изменение адреса АПС",
  (STR)"Изменение адреса БРА/ПКП",
};

// Типы извещателей 0-31
__flash char NameSensorType[][6]=
{
  "Нет  ","ИПР  ","ИПТ  ","ИПТД ",
  "ИПД  ","ИПЛ  ","ИПК  ","ИПП  ",
  "-----","-----","-----","-----",
  "БР-1 ","БС   ","-----","-----",
  "БТВ  ","БКА  ","-----","-----",
  "ЛПП  ","-----","-----","-----", //20,21,22,23
  "-----","-----","-----","-----",
  "-----","-----","-----","-----"
};

// Количество дней на начало месяца
__flash int DMonth[]={0,31,59,90,120,151,181,212,243,273,304,334,365};

//---------------------------------------------------------------
// Define bios variables
//---------------------------------------------------------------

tiny static char NeedDecTrialTime=1; //Флаг необходимости уменьшения количества рабочих часов, устанавливается раз в час
tiny static char DoorsStatus=0; // Статус тампера двери, 1 - Открыта

//tiny static char PU_IDmem; // Копия номера ПУ
tiny unsigned int PrinterOnLine=1000; // Состояние принтера

//char DPU_txcnt;

struct
{
  char CursorPos; //Позиция курсора
  unsigned int cursector; //Тек. проверяемый сектор
  unsigned int curseccrc; //Тек. контр. сумма сектора
  unsigned long date; //Время и дата для синхронизации
  char CON[81]; // Консоль (81 из за sprintf'a)
};
volatile char NeedSendScr;

// Данные для обработчика кнопок и клавиатуры

// Задержка на дребезг
#define KEY_DELAY -5
// Задержка до первого повтора
#define KEY_REPDEL 30
// Задержка на автоповтор
#define KEY_REPPER 24
// Коды клавиш
#define kREDRAW	0x1F
#define kSECRET 0x1E
#define kACK  0x1A
#define kLT   8
#define kRT   10
#define kUP   11
#define kDW   12
#define kESC   27
#define kINS  14
#define kDEL  15
#define kPGUP 4
#define kPGDW 3
#define kHOME 2
#define kEND  1

volatile tiny char LED=0; // Статус светодиодов, по битам, 1 - включен
volatile tiny char LEDflash=0; // Статус мигания светодиодов, по битам, 1 - мигает
tiny char LEDflashcnt=1; // Счетчик для мигания

volatile tiny char SndMode=0; // Тип звука, 0 - выключен, 1 - пожар, 2 - отказ, 3 - предупреждение, 4 - короткий сигнал
volatile tiny char COPY_PU_STAT=0;

// Биты светодиодов в порту
#ifdef PUA1
#define LEDfire   0x80
#define LEDmfire  0x40
#define LEDoff    0x20
#define LEDfail   0x04
#define LEDsndoff 0x10
#define LEDsysf   0x08
#else
#define LEDfire   0x01
#define LEDmfire  0x20
#define LEDoff    0x08
#define LEDfail   0x04
#define LEDsndoff 0x02
#define LEDsysf   0x10
#endif

char IsPreAtt;
char IsOffline;

volatile tiny char KEY=0x00; //Код нажатой клавиши, 0 - нет нажатия
tiny char OldKey=0x00; //Старый код клавиши
tiny signed char KeyCounter=KEY_DELAY; // Счетчик для задержек клавиатуры
tiny unsigned char ATKbitcount=11; // Счетчик принятых битов от внешней клавиатуры
static char ATKbuf[8]; // Буфер принятых данных от внешней клавиатуры
static char ATKwp=0; // Указатель записи в буфер принятых данных от внешней клавиатуры
static char ATKrp=0; // Указатель чтения из буфера принятых данных от внешней клавиатуры
tiny static char ATKstat=0; // Статус клавиш Shift, Alt, Ctrl, CapsLock

static unsigned int check_flash_sector_number=0xFFFF;

// Время в секундах после 01-01-2000 00:00, расположено в SRAM по абсолютному адресу
#define TimeDate (*(unsigned long*)(0xFFC))

tiny unsigned long Timer30sec=0xFFFFFFFF; //Таймер 30с
tiny unsigned long Fire120sec=0xFFFFFFFF; //Таймер 120с
tiny static unsigned int TotalFires; // Общее количество пожаров

tiny static char PWDmode=0; //Какой пароль совпал, 0 - не совпал, 2 - уровень 2, 3 - уровень 3

char EventFIFO[64]; // Стек событий, FIFO, события без времени, по 4 байта, всего 16 событий
char EventFIFOwp=0; // Указатель стека событий для записи
char EventFIFOrp=0; // Указатель стека событий для чтения


// Данные после разборщика события, подробнее см. протокол передачи сообщений прибора ПУ
tiny static char TemporaryEVENT; // Тип текущего обрабатываемого события
tiny static char TemporaryZONE0; // Зоны текущего события
tiny static char TemporaryZONE1;
tiny static char TemporaryZONE2;
tiny static char TemporaryZONE3;

static char IsSuprDZ; //Признак наличия зон в дист. режиме

//tiny static char TemporaryTYPE; // Тип извещателя текущего события

//==============================================================================================================
//static char SaveTIMSK; //Копия регистра TIMSK

volatile char ISMASTER=0; //<>0 если ПУ - мастер

struct
{
  struct TLogEvent DPU_MESS; //Пакет для передачи в дубл. ПУ
  char DPU_SEQ; //Номер пакета, изм. при получении подтверждения
};

volatile char DPUmess_ready; //Готово событие для дубл. ПУ
static int LogFileRP_DPU; //Указатель на событие для дубл. ПУ

volatile static char WarmBoot_cmd; //Команда для сброса 4E или 4F

volatile char RebootMode=0; //0 - норм. обмен, FF - стоп, иначе коммада сброса

// Буфер передачи сервисного пакета,
// первый байт - адрес БР-А, если он не 255 - запрос передачи сервисного пакета
char SysPPKP_TXB[8];
volatile char SysPPKP_txsize; //количество байт в буфере сервисного пакета
// Буфер приема сервисного пакета
char SysPPKP_RXB[8];
// количество принятых байт сервисного пакета,
// пока 0 - пакет еще не принят
// 255 - нет обмена с БР-А в сервисном режиме
volatile char SysPPKP_rxsize=255;

//volatile char NeedNoError=0; // Если 1 - не сообщать об отсутствии обмена с БР-А в сервисном режиме

// Таблица для расчета CRC
__flash static char CRC8tab[256]=
{
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};

// Данные по абонентам RS485, 0-63 БР-А, 64-95 - БРА, 96-127 - АПС
// Бит 0-2 - ошибки по RS0, 7 - полный отказ
// Бит 3-5 - ошибки по RS1, 7 - полный отказ
// Бит 6 - Номер подтверждения ACK
// Бит 7 - Включен/Выключен

volatile static char ABONS[128];

static int LogFileWP; // Указатель на стек событий в 45??021 для записи

volatile tiny static char CommunicateWithPC=0; // Флаг необходимости перехода в режим обмена с ПК (конфигурирование с ПК)

volatile static char DBLE_RS0; //Счетчик секунд до отказа обмена с дубл. ПУ по RS0
volatile static char DBLE_RS1; //Счетчик секунд до отказа обмена с дубл. ПУ по RS1
volatile static char DBLS; //Счетчик секунд до перехода в режим осн. прибора, также
//счетчик необходимости передачи в дубл. прибор

//-----------------------------------------
tiny static int LogFileRP_CAN;
tiny static char PUID;
// Ошибки CAN
// bit 0 - TX0BUSY
// bit 1 - TX1BUSY
// bit 2 - RX0ERR
// bit 3 - RX1ERR
tiny static char FlagsCAN;

tiny static char CANtimeout; //Таймаут для передачи/приема в/из ПУС
static char EPUS[64];

//-----------------------------------------
tiny static char NeedMoney=0; // Флаг окончания срока работы, 1 - Обратитесь к поставщику
//-----------------------------------------
//-----------------------------------------
// Переменные для работы с зонами
//-----------------------------------------
// Для каждой зоны по 2 байта, первый - таймер, второй - количество сработок
static char ZoneTF[512];
static char ZoneDelays[1024];
static unsigned int CurrentZoneCheck;
static tiny char mrv1relay; // Копия байта реле для МРВ1
static tiny char mrv2relay; // Копия байта реле для МРВ2

static tiny char TADEVoff; // Флаги отсутствия обмена с модулями (для ПУ-А)

static tiny char I2Cerror; // Флаг ошибки обмена по I2C
static tiny char TKevent; // Код события ТК

//==================================================
// Переменные для работы с макроязыком
//==================================================
tiny static char TemporaryBITN;
tiny static char TemporaryBITV;
static char BitFlags[32]; //Биты состояния 0-255
static char MacroTimers[32]; //Таймеры (16 шт), таймер + номер бита
static unsigned int VMPC[16]; //Указатели в макрокоде
static unsigned int VMBASE[16]; //Указатели в макрокоде


char IsLineSpace(char *p);
void FullPrintEvent(int LFRP, unsigned int n);

//===================================================================
// Разборщик сообщений системы, подробнее см. протокол передачи сообщений прибора ПУ
//===================================================================
#include "EventParser.c"

//===================================================================

// Описания для форварда
void WarmBoot(char cmd);
__monitor void Event2FIFO(char p,char s,char e,char e2);
void DoConfigPUpk(void);
void SendCursorPos(char pos);
void PUSconnect(char mode, char pus, char section);

//#pragma warnings=off
#pragma optimize=no_inline
void delay(unsigned int delay) // Задержка в delay микросекунд
{
  // delay-=5;
  while(--delay);
}
//#pragma warnings=on

void Max63(char *v) // Проверка значения в диапазоне 0..63 и его коррекция
{
  if (*v>63) *v=63;
}

char IsLineSpace(char *p) // Если строка состоит из пробелов, вернуть 1
{
  char c=24;
  do
  {
    if (*p++!=' ') return(0);
    if (!(--c)) return(1); //Все 24
  }
  while (*p);
  return(1);
}

//Чтение данных в буфер ABONS
void ConstructABONS()
{
  //char c;
  char i=0;
  do
  {
    ABONS[i]=PPKP_AREA[i]&0x80;
    i++;
  }
  while (i<64);
  do
  {
    ABONS[i]=UPS_AREA[i&0x1F]&0x80;
    i++;
  }
  while(i<96);
  do
  {
    ABONS[i]=BRA_AREA[i&0x1F]&0x80;
    i++;
  }
  while(i<128);
}

//---------------------------------
// BIOS Клавиатура, Принтер, ЖКИ
//---------------------------------
#include "KBDLCDLPT.c"
//=================================

// Преобразование времени и даты в количество секунд после 01-01-2000 00:00
unsigned long TimeDate2Long(char hrs, char min,char sec,char day,char mon,char yrs)
{
  unsigned long iday;
  iday=365*yrs+DMonth[mon-1]+(day - 1);
  iday=iday+(yrs>>2);
  if (mon>2||(yrs&3)>0)
    iday++;
  return(sec+60*(min+60*(hrs+24* iday)));
}

// Печать даты и времени по количеству секунд с 01-01-2000 00:00
void PrintTimeDate(char *p, unsigned long x)
{
  char sec,min,hrs,mon,yrs;
  unsigned int day,iday,day4,yrs4;

  sec=x%60;
  min=(x/60)%60;
  hrs=(x/3600)%24;
  iday=x/86400;
  yrs4=x/((4*365+1)*86400);
  day4=iday%(4*365+1);
  iday=(day4==(31+28));
  if (day4>=(31+28)) day4--;
  yrs=(yrs4<<2)+day4/365;
  day=day4%365;
  mon=0;
  while (DMonth[++mon]<=day);
  day-=DMonth[mon-1];
  if (iday) day++;
  day++;
  if (yrs>99) yrs=0;
  sprintf_P(p,(STR)"%02d:%02d:%02d %02d-%02d-%02d",hrs,min,sec,day,mon,yrs);
}

void LPT_PrintTimeDate(unsigned long x)
{
  char sec,min,hrs,mon,yrs;
  unsigned int day,iday,day4,yrs4;

  sec=x%60;
  min=(x/60)%60;
  hrs=(x/3600)%24;
  iday=x/86400;
  yrs4=x/((4*365+1)*86400);
  day4=iday%(4*365+1);
  iday=(day4==(31+28));
  if (day4>=(31+28)) day4--;
  yrs=(yrs4<<2)+day4/365;
  day=day4%365;
  mon=0;
  while (DMonth[++mon]<=day);
  day-=DMonth[mon-1];
  if (iday) day++;
  day++;
  if (yrs>99) yrs=0;
  printf_P((STR)"%02d:%02d:%02d %02d-%02d-%02d ",hrs,min,sec,day,mon,yrs);
}

//---------------------------------------------
// Инициализация оборудования
//---------------------------------------------

#include "SysInit.c"

//---------------------------------------------------------
// Работа с 45??021
//---------------------------------------------------------
#include "Flash.c"

//Проверка CRC флеша программ
#include "PGMCRC.c"

//---------------------------------------------------------
// Проверка наличия +24В
//---------------------------------------------------------
char CheckPowerDeath(void)
{
  unsigned long td;

#ifdef PUA1
  if (PINF&2) return(0); //Если +24В есть, просто возвращаемся
#else
  if (PIND&0x04) return(0);
#endif
  // Подготовка к переходу в режим микропотребления
  __disable_interrupt();
  __watchdog_reset();
  WDTCR=0x18;
  WDTCR=0x00;
  __watchdog_reset();
  MCUCR=0x38;
  UCSR0B=0;
  SPCR=0;
  ADCSR=0;
  ACSR=1<<ACD;
  TCCR2=0;
  TCCR1B=0;
  EIMSK=0;
  TIMSK=1;
#ifdef PUA1
  PORTB=0;
  DDRE=0;
  PORTE=0x00;
  PORTA=0x00;
  PORTC=0x00;
  PORTD=0x00;
#else
  PORTB=0;
  DDRE=0;
  PORTE=0x00;
  PORTA=0x00;
  PORTC=0x00;
  DDRD=~4;
  PORTD=0x00;
#endif
  //FlushLogFile(); // Записать в 45??021 незаписанные данные
  PORTB=0;
#ifdef PUA1
#else
  DDRB=~0x20;
#endif

  while(ASSR&7); // Включение прерываний от часов 1 раз за 8 секунд
  TCCR0=0x07;
  while(ASSR&7);
  TIFR=0xFF;
  td=TimeDate;
  __enable_interrupt();
  __sleep(); // Засыпаем на 8 секунд
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  td+=8; // Увеличиваем время
  __insert_opcode(0xF3FF);
  __enable_interrupt();
  do
  {
    __sleep(); // Засыпаем на 8 секунд
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    td+=8; // Увеличиваем время
    __insert_opcode(0xF3FF);
    __enable_interrupt();
  }
#ifdef PUA1
  while (!(PINF&4)); // Цикл, пока не появилось питание
#else
  while (!(PIND&0x04));
#endif
  TimeDate=td;
  __disable_interrupt();
  /* __watchdog_reset();
  WDTCR=0x08;
  WDTCR=0x18;
  WDTCR=0x0A; //Разрешаем сторожевой таймер
  __watchdog_reset();
  for(;;);*/
  asm("JMP 0\n");
  return(0);
}

//----------------------------------------------------
// Обработка событий
//----------------------------------------------------

// Посылает событие в стек, при переполнении генерирует соответствующее событие
__monitor void Event2FIFO(char p,char s,char e,char e2)
{
  char* p1;
  if (NeedMoney) return; // Если окончилось время работы, не реагировать на события
  p1=&EventFIFO[EventFIFOwp];
  // Запись в стек
  *p1++=p;
  *p1++=s;
  *p1++=e;
  *p1++=e2;
  EventFIFOwp=(EventFIFOwp+4)&0x3f; // Изменение указателя
  if (EventFIFOwp==EventFIFOrp) // Если переполнение, записываем соответствующее событие
  {
    p1=&EventFIFO[EventFIFOwp];
    *p1++=0xFF;
    *p1++=0xFF;
    *p1++=0x01;
    *p1=0x00;
    EventFIFOwp=(EventFIFOwp+4)&0x3f;
  }
}

char LEDCONV(char i)
{
  char c;
  c=0;
  if (i&0x01) c|=0x80;
  if (i&0x20) c|=0x40;
  if (i&0x08) c|=0x20;
  if (i&0x04) c|=0x04;
  if (i&0x02) c|=0x10;
  if (i&0x10) c|=0x08;
  return(c);
}

void CheckDoors(void)
{
  char ds=PIND&0x40;
  if (IS_TAMPER_DISABLED) return;
  if (DoorsStatus^ds)
  {
    DoorsStatus=ds;
    if (DoorsStatus)
    {
      Event2FIFO(0xFF,0xFF,22,0);
    }
  }
}

void CheckADevices(void)
{
  char c;
  char v;
  static char sec;
  static char oldsec;
  static char pus;

  pus++;
  c=pus>>2;
  if (!(pus&3))
  {
    if (EPUS[c]==0x80) PUSconnect(0,c,0);
  }
  if (oldsec!=(char)TimeDate)
  {
    oldsec=(char)TimeDate;
    sec=(sec+1)&7;
    switch (sec)
    {
    case 0:
      //mrv1
      if (MRV1_STAT&0x80)
      {
        for (c=0;c<16;c++)
        {
          v=ReadI2C(0x92,1);
          if (v!=mrv1relay) I2Cerror=1;
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&1)))
        {
          Event2FIFO(0xFF,0xFF,6,0);
          TADEVoff|=1;
        }
      }
      break;
    case 1:
      //mrv2
      if (MRV2_STAT&0x80)
      {
        for (c=0;c<16;c++)
        {
          v=ReadI2C(0x94,1);
          if (v!=mrv2relay) I2Cerror=1;
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&2)))
        {
          Event2FIFO(0xFF,0xFF,7,0);
          TADEVoff|=2;
        }
      }
      break;
    case 2:
      //tk
      break;
    case 3:
      //vst
      break;
    case 4:
      break;
    case 5:
      //vtk
      break;
    }
  }
}

void SendToTK(char data)
{
  char i;
  if (!(TK_STAT&0x80)) return;
  if (data)
  {
    for (i=0;i<16;i++)
    {
      WriteI2C(0xF2,1,data);
      if (!I2Cerror) break;
    }
    if (I2Cerror)
    {
      Event2FIFO(0xFF,0xFF,8,0);
    }
  }
}

// Обработка зоны
void ZoneJob(char TemporaryZONE,char is32)
{
  char c;

  is32&=32;

  if ((ZONEMF_AREA[TemporaryZONE]&0x80)==0) return; //Зона выключена!
  if (is32) goto L1; //Принудительное включение всех зон
  // Устанавливаем новый таймер для зоны
  ZoneTF[TemporaryZONE<<1]=ZONETO_AREA[TemporaryZONE];
  // Увеличиваем количество сработок в зоне, если зона не заблокирована
  if (ZoneTF[(TemporaryZONE<<1)+1]==0xFF) return; //В зоне блокирована автоматика
  ZoneTF[(TemporaryZONE<<1)+1]++;
  // Сравниваем с необходимым количеством
  c=(ZONEMF_AREA[TemporaryZONE]&3)+1;
  if (ZoneTF[(TemporaryZONE<<1)+1]>=c)
  {
    unsigned int z;
  L1:
    z=TemporaryZONE<<2;
    if (TemporaryZONE) Event2FIFO(0xFF,0xFF,31,TemporaryZONE);
    c=4;
    do
    {
      if (ZoneDelays[z]==0xFE) ZoneDelays[z]=ZONEDELAY_AREA[z];
      z++;
    }
    while(--c);
  }
}

// Необходимо обработать зону
void ZoneCheck(void)
{
  char i;
  unsigned int zp;
  char s0,s1,s2;
  unsigned int z;

  z=CurrentZoneCheck&1023;

  do
  {
    //  i=z;
    if (ZoneDelays[z]==0xFF) goto L1;
    z++;
    //  i++;
  }
  while(z&0x3F);
  CurrentZoneCheck=z;
  return;
L1:
  CurrentZoneCheck=z+1;
  // Получаем точку сработки
  zp=ZONEPOINT_AREA[z];
  if (zp==0xffff)
  {
    ZoneDelays[z]=0xFE;
    return;
  }
  switch (zp&0xC000)
  {
    // БР-А/Извещатель/Реле
  case 0x0000:
  case 0x4000:
    if (!ISMASTER) break;
    if (zp&0x1f8)
    {
      // Извещатель<>0, включаем реле на БКА
      while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
      __disable_interrupt();
      s0=SysPPKP_rxsize; // Сохраняем принятые данные
      s1=SysPPKP_RXB[0];
      s2=SysPPKP_RXB[1];
      // Подготовка данных для передачи комманды включения реле
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=zp>>9;
      SysPPKP_TXB[1]=0x8;
      SysPPKP_TXB[2]=(zp&0x1f8)>>3;
      SysPPKP_TXB[3]=1<<(zp&7);
      SysPPKP_txsize=4;
      __enable_interrupt();
      while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
      SysPPKP_rxsize=s0; // Восстанавливаем старые данные
      SysPPKP_RXB[0]=s1;
      SysPPKP_RXB[1]=s2;
    }
    else
    {
      // Извещатель=0, включаем реле на БР-А
      while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
      __disable_interrupt();
      s0=SysPPKP_rxsize; // Сохраняем принятые данные
      s1=SysPPKP_RXB[0];
      s2=SysPPKP_RXB[1];
      // Подготовка данных для передачи комманды включения реле в БР-А
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=zp>>9;
      SysPPKP_TXB[1]=0x48;
      SysPPKP_TXB[2]=0;
      SysPPKP_TXB[3]=1<<(zp&7);
      SysPPKP_txsize=4;
      __enable_interrupt();
      while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
      SysPPKP_rxsize=s0; // Восстанавливаем старые данные
      SysPPKP_RXB[0]=s1;
      SysPPKP_RXB[1]=s2;
    }
    ZoneDelays[z]=0xFE;
    break;
  case 0x8000:
    if (!ISMASTER) break;
    // Реле в ПУС
    if (z>=4) Event2FIFO(((zp>>8)&0x3F)+0x80,0,2,(char)zp);
    PUSconnect(1,(zp>>8)&0x3F,(char)zp);
    ZoneDelays[z]=0xFE;
    break;
  case 0xC000:
    if (zp&0x2000)
    {
      if (!ISMASTER) break;
      //Реле в БРА/ПКП
      //111aaaaaxxxxxxxx
      while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
      __disable_interrupt();
      s0=SysPPKP_rxsize; // Сохраняем принятые данные
      s1=SysPPKP_RXB[0];
      s2=SysPPKP_RXB[1];
      // Подготовка данных для передачи комманды включения реле в БРА
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=(zp>>8)&0x7F;
      SysPPKP_TXB[1]=0x48;
      SysPPKP_TXB[2]=0;
      SysPPKP_TXB[3]=(char)zp; //Номер реле
      SysPPKP_txsize=4;
      __enable_interrupt();
      while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
      SysPPKP_rxsize=s0; // Восстанавливаем старые данные
      SysPPKP_RXB[0]=s1;
      SysPPKP_RXB[1]=s2;
      ZoneDelays[z]=0xFE;
      break;
    }
    // point - mrv
    if (!(zp&8))
    {
      mrv1relay|=1<<(zp&7);
      if (MRV1_STAT&0x80)
      {
        for (i=0;i<16;i++)
        {
          WriteI2C(0x92,1,mrv1relay);
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&1)))
        {
          Event2FIFO(0xFF,0xFF,6,0);
          TADEVoff|=1;
        }
      }
    }
    else
    {
      mrv2relay|=1<<(zp&7);
      if (MRV2_STAT&0x80)
      {
        for (i=0;i<16;i++)
        {
          WriteI2C(0x94,1,mrv2relay);
          if (!I2Cerror) break;
        }
        if (I2Cerror&&(!(TADEVoff&2)))
        {
          Event2FIFO(0xFF,0xFF,7,0);
          TADEVoff|=2;
        }
      }
    }
    ZoneDelays[z]=0xFE;
    break;
  }
}

void TransmitToDPU(void)
{
  if (DPUmess_ready) return; //Не передано пред. событие
  if (LogFileRP_DPU!=LogFileWP)
  {
    ReadEvent(LogFileRP_DPU,&DPU_MESS); // Читаем его из 45??021
    LogFileRP_DPU=(LogFileRP_DPU+1)&0xFFF;
    DPUmess_ready=1;
  }
}

//--------------------------------------------------------------
// Извлечение события из стека,
// запись в архив 45??021, первичная обработка
// печать на принтер, передача в ДВП, ВСТ и т.д.
// Данная процедура вызывается постоянно, в т.ч. и при ожидании нажатия клавиш,
// при необходимости сюда можно вставлять фоновые задачи
//--------------------------------------------------------------
char FIFO2EventLog(void)
{
  struct TLogEvent ev; // Текущее событие
  union
  {
    char s[61]; // Буфер для текстовых строк
    struct TSensorConfig sc;
  };
  char c;
  void Event2CAN(void);
  void DoVirtualMachines(void);

  CheckPowerDeath(); // Проверяем наличие питания
  CheckADevices();
  if (ISMASTER)
  {
    Event2CAN();
    TransmitToDPU();
  }
  else
  {
    //Только, если сейчас режим slave
    WorkSecond(); // Если есть флаг таймера 1с, вызываем обработчик
    LogFileRP_CAN=LogFileWP; //Эти пакеты не надо передавать обратно при переключении
    LogFileRP_DPU=LogFileWP; //на мастера
  }
  ZoneCheck();
  DoVirtualMachines();

  COPY_PU_STAT=PU_STAT;
  if (COPY_PU_STAT&0x10) SetLEDflash(LEDsndoff); //Если звук отключен

#ifdef PUA1
#else
  CheckDoors();
#endif
  _CLI();
  c=COPY_PU_STAT;
  if ((Fire120sec<=TimeDate)&&(c&4)) // Если сработал таймер 120сек, включаем реле
  {
#ifdef PUA1
    PORTB|=0x40;
#else
    PORTD|=0x10;
#endif
    Fire120sec=0xFFFFFFFF;
  }
  _SEI();
L_SKIPEVENT:
  if (EventFIFOwp==EventFIFOrp) // Если событий в стеке нет, возврат
  {
    return(0);
  }
  else
  {
    memcpy(&ev,&EventFIFO[EventFIFOrp],4); // Копируем событие из стека в буфер
    _CLI();
    ev.Second=TimeDate; // запоминаем время события
    _SEI();
    EventFIFOrp=(EventFIFOrp+4)&0x3f; // Перемещаем указатель чтения
    //Пропускаем события об отказе линий питания, если не включены в конфигурацию
    if ((ev.PPKP&0xE0)==0x40) //Событие от АПС
    {
      if (ev.Sensor==4) //Отказ линии 1
      {
        if (!(UPS_AREA[ev.PPKP&0x1F]&1)) goto L_SKIPEVENT;
      }
      if (ev.Sensor==5) //Отказ линии 2
      {
        if (!(UPS_AREA[ev.PPKP&0x1F]&2)) goto L_SKIPEVENT;
      }
    }
    WriteEvent(&ev); // Записываем событие в 45??021, указатель записи меняется автоматически
    if (!ISMASTER)
    {
      if ((ev.Event==0xFD)||(ev.Event==0xFE))
      {
	//Включаем/выключаем БР-А/АПС/БРА в дубл. приборе
	if ((ev.Sensor&0xC0)==0xC0)
	{
	  if ((ev.PPKP&0xC0)==0x00) PPKP_AREA[ev.PPKP&0x3F]=ev.Event2;
	  if ((ev.PPKP&0xC0)==0x40) UPS_AREA[ev.PPKP&0x1F]=ev.Event2;
	  if ((ev.PPKP&0xC0)==0x60) BRA_AREA[ev.PPKP&0x1F]=ev.Event2;
	}
      }
      //Проверка вкл/выкл извещателя
      //Event2FIFO(ppkp,sensor|0xc0,ts.Type>>7,ts.Type); //Сообщение "Извещатель включен/выключен"
      if (ev.PPKP<0x40)
      {
	if ((ev.Sensor&0xC0)==0xC0)
	{
	  if (ev.Event<2)
	  {
	    ReadSensorConfig(ev.PPKP,ev.Sensor&0x3F,&sc);
	    sc.Type=ev.Event2&0x9F;
	    WriteSensorConfig(ev.PPKP,ev.Sensor&0x3F,&sc);
	  }
	}
      }
    }
    if (
        (ev.PPKP==255)&&
          (ev.Sensor==255))
    {
      if ((ev.Event>=34)&&(ev.Event<=36)) SetLEDflash(LEDsysf); //Системная ошибка
      if (ev.Event==4) //Событие - норма
      {
        void DoBoot(char cmd); //Выполняем сброс системы
        DoBoot(ev.Event2);
        check_flash_sector_number=0;
        KEY=0x81; //Выйти из просмотра событий
      }
      if (!ISMASTER)
      {
	if (ev.Event==0x26)
	{
	  //Отключение зоны в дубл. приборе
	  ZONEMF_AREA[ev.Event2]&=~0x80; //Выключаем зону
	}
	if (ev.Event==0x27)
	{
	  //Включение зоны в дубл. приборе
	  ZONEMF_AREA[ev.Event2]|=0x80; //Включаем зону
	}
      }
      if (ev.Event==37)
      {
        //Восстановление автоматики
        char ppkp=0;
        char z=ev.Event2;
        ZoneTF[(z<<1)+1]=0; //Разблокировка
        if (ISMASTER)
        {
          ClrScr();
          sprintf_P(CON,(STR)"Восстановление автоматики в зоне %03d",z);
          UpdateConsole();
          do
          {
            if (ABONS[ppkp]!=0) //Если БР-А в конфигурации и с ним есть обмен
            {
              char sensor=0;
              do
              {
                ReadSensorConfig(ppkp,sensor,&sc); //Читаем конфигурацию
                if ((sc.Type==(0x80+0x14))&&(sc.Zone1==z)) //Если тип извещателя ЛПП и он включен
                {
                  char s0,s1,s2;
                  while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
                  __disable_interrupt();
                  s0=SysPPKP_rxsize; // Сохраняем принятые данные
                  s1=SysPPKP_RXB[0];
                  s2=SysPPKP_RXB[1];
                  // Подготовка данных для передачи комманды выключения светодиода
                  SysPPKP_rxsize=0;
                  SysPPKP_TXB[0]=ppkp;
                  SysPPKP_TXB[1]=sensor;
                  SysPPKP_TXB[2]=0;
                  SysPPKP_txsize=3;
                  _SEI();
                  __enable_interrupt();
                  while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
                  SysPPKP_rxsize=s0; // Восстанавливаем старые данные
                  SysPPKP_RXB[0]=s1;
                  SysPPKP_RXB[1]=s2;
                }
                sensor++;
              }
              while(sensor<64);
            }
            ppkp++;
          }
          while(ppkp<64);
        }
        KEY=kREDRAW;
        IsSuprDZ=0;
      }
    }
    PrintEventType(&ev,s,1); // Разбираем событие
    c=TemporaryBITN;
    //  _CLI();
    if (TemporaryBITV)
      BitFlags[c>>3]|=(1<<(c&7));
    else
      BitFlags[c>>3]&=~(1<<(c&7));
    //  _SEI();
    // Разрешаем вывод на принтер и печатаем
    LPT_PrintTimeDate(ev.Second);
    putstr(s);
    PRINT_CRLF;
    SendToTK(TKevent);
    if (TemporaryEVENT&16)
    {
      //Блокировка автоматики в зоне
      char ppkp=0;
      ZoneTF[(TemporaryZONE0<<1)+1]=0xFF; //Блокируем зону
      if (ISMASTER)
      {
        ClrScr();
        sprintf_P(CON,(STR)"Отключение автоматики в зоне %03d",TemporaryZONE0);
        UpdateConsole();
        do
        {
          if (ABONS[ppkp]!=0) //Если БР-А в конфигурации и с ним есть обмен
          {
            char sensor=0;
            do
            {
              ReadSensorConfig(ppkp,sensor,&sc); //Читаем конфигурацию
              if ((sc.Type==(0x80+0x14))&&(sc.Zone1==TemporaryZONE0)) //Если тип извещателя ЛПП и он включен
              {
                char s0,s1,s2;
                while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
                __disable_interrupt();
                s0=SysPPKP_rxsize; // Сохраняем принятые данные
                s1=SysPPKP_RXB[0];
                s2=SysPPKP_RXB[1];
                // Подготовка данных для передачи комманды включения светодиода
                SysPPKP_rxsize=0;
                SysPPKP_TXB[0]=ppkp;
                SysPPKP_TXB[1]=sensor;
                SysPPKP_TXB[2]=1;
                SysPPKP_txsize=3;
                _SEI();
                __enable_interrupt();
                while (SysPPKP_rxsize==0); // Ожидаем окончание сервисного обмена
                SysPPKP_rxsize=s0; // Восстанавливаем старые данные
                SysPPKP_RXB[0]=s1;
                SysPPKP_RXB[1]=s2;
              }
              sensor++;
            }
            while(sensor<64);
          }
          ppkp++;
        }
        while(ppkp<64);
        KEY=kREDRAW;
      }
    }
    if (TemporaryEVENT&64)
    {
      //Тестирование зоны
      unsigned int z;
      z=TemporaryZONE0<<2;
      if (ZoneDelays[z]==0xFE) ZoneDelays[z]=ZONEDELAY_AREA[z]; //Первое реле в зоне - СЗУ
    }
    if (TemporaryEVENT&1) // Если текущее событие - пожар
    {
      TotalFires++; // Увеличиваем общее количество пожаров
      _CLI();
#ifdef PUA1
      PORTB|=0x10; // Включаем реле "Пожар"
#else
      PORTD|=0x20;
#endif
      if (Fire120sec==0xFFFFFFFF) Fire120sec=TimeDate+120; //Устанавливаем таймер 120сек
      _SEI();
      if (LED&LEDfire) // Если уже горел "Пожар"
      {
        SetLEDon(LEDmfire); // Включаем "Много пожаров"
        SndMode=1; // Звук - пожар
      }
      else // первый пожар
      {
        SetLEDon(LEDfire); // Включаем "Пожар"
        SndMode=1; // Звук - пожар
      }
      ZoneJob(TemporaryZONE0,TemporaryEVENT); //Обрабатываем зоны по данному извещателю
      ZoneJob(TemporaryZONE1,TemporaryEVENT);
      ZoneJob(TemporaryZONE2,TemporaryEVENT);
      ZoneJob(TemporaryZONE3,TemporaryEVENT);
    }
    if (TemporaryEVENT&2) // Если событие - отказ
    {
      if (!(PU_STAT&4))
      {
#ifdef PUA1
        PORTB&=~0x40;
#else
        PORTD&=~0x10;
#endif
      }
      SetLEDflash(LEDfail); // Включаем мигание "Отказ"
      if (SndMode!=1) SndMode=2; //Если звук не пожар, то звук - отказ
    }
    if (TemporaryEVENT&4) // Если событие - предупреждение
    {
      IsPreAtt=1;
      if (SndMode!=1) SndMode=3; // Если звук не пожар, то звук - предупреждение
    }
    if (TemporaryEVENT&8) // Если событие информационное
    {
      if (SndMode==0) SndMode=4; // Если звук отключен, то звук - одиночный сигнал
    }
    return(TemporaryEVENT&7); // Если есть пожары, отказы, предупр. - возвращаем истину
  }
}

#include "RS.c"


//----------------------------------------------------
// Просмотр событий по кнопке ЗВУК ОТКЛ
// Вызывается при нажатии кнопки ЗВУК ОТКЛ или при появлении пожаров и т.д.
//----------------------------------------------------
void ShowLog(void)
{
  char c;
  unsigned int i;
  char s[61]; // Буфера строк
  struct TLogEvent ev; // Текущее сообщение

  unsigned int FirstEv; //Указатель на первое приоритетное событие
  unsigned int LastEv; //Указатель на последнее приоритетное событие
  unsigned int EvCnt1; //Номер события в первой строке
  unsigned int EvCnt2; //Номер события во второй строке
  char mask=0; //Режим отображения - 0 - все, 1 - пожары, 2 - отказы, 4 - предупр.
  //Старший бит - полное отображение
#ifdef PUA1
  PORTA|=0x01; // Включаем подсветку
#else
  PORTB|=0x40;
#endif
  KEY=0; // Нет нажатия на клавиши

L_CHECK_PRI:
  mask=0x7F; //Все
  if (IsPreAtt) mask=4; //Есть предупреждения
  if (LEDflash&LEDfail) mask=2; //Если есть отказы
  if (LED&LEDfire) mask=1; //Если есть более приоритетные пожары
L_CHECK_PRI1:
  i=SearchLastNormal();
  EvCnt1=1;
  EvCnt2=0;
  c=1; //Первое не найдено
  for(;;)
  {
    ReadEvent(i,&ev); //Читаем событие
    if (ev.Second==0xFFFFFFFF) break; //Все нашли
    PrintEventType(&ev,s,0); // Разбираем событие без текстовых строк
    if (TemporaryEVENT&mask)
    {
      //Нашли подходящее событие
      EvCnt2++; //Счетчик подходящих событий
      LastEv=i; //Посл. событие
      if (c)
      {
        FirstEv=i; //Найдено первое событие
        c=0; //Больше не искать первое событие
      }
    }
    i++;
    i&=0xFFF;
  }
  //Проверяем на наличие заданных событий в архиве
  if (c&&mask)
  {
    //Если искали и не нашли ни одного события
    WarmBoot(0x4F); //Сброс системы
    Event2FIFO(0xff,0xff,5,0); // Событие "Архив поврежден"
    goto L_CHECK_PRI;
  }
  __disable_interrupt();
  Timer30sec=TimeDate+30;
  __enable_interrupt();
  do
  {
    ClrScr();
    if ((mask&0x80)||(FirstEv==LastEv))
    {
      //Принудительная печать одного события
      ReadEvent(FirstEv,&ev);
      PrintEventType(&ev,s,1);
      sprintf_P(CON,(STR)"%04d ",EvCnt1);
      memcpy(CON+5,s,35);
      PrintTimeDate(CON+40, ev.Second);
      memcpy(CON+59,s+36,21);
    }
    else
    {
      //Печатаем 2 события
      ReadEvent(FirstEv,&ev);
      PrintEventType(&ev,s,1);
      sprintf_P(CON,(STR)"%04d ",EvCnt1);
      memcpy(CON+5,s,35);
      ReadEvent(LastEv,&ev);
      PrintEventType(&ev,s,1);
      sprintf_P(CON+40,(STR)"%04d ",EvCnt2);
      memcpy(CON+45,s,35);
    }
    UpdateConsole();
    if (FIFO2EventLog())
    {
#ifdef PUA1
      PORTA|=0x01; // Включаем подсветку
#else
      PORTB|=0x40;
#endif
      goto L_CHECK_PRI; // Если опять пришло событие (приоритетное)
    }
    __disable_interrupt();
    if (Timer30sec<TimeDate) //Прошло 30сек
    {
      __enable_interrupt();
      if (!SndMode)
      {
#ifdef PUA1
        PORTA&=~0x01; //Выключили подсветку
#else
        PORTB&=~0x40;
#endif
      }
      goto L_CHECK_PRI;
    }
    __enable_interrupt();
    c=KEY; // Проверяем кнопки
    if (c==0x81) goto L_EXIT;
    if (c)
    {
      KEY=0;
      __disable_interrupt();
      Timer30sec=TimeDate+25;
      __enable_interrupt();
#ifdef PUA1
      PORTA|=0x01; // Включаем подсветку
#else
      PORTB|=0x40;
#endif
    } //Если нажали то переустанавливаем таймер 30 сек
    if (SndMode) // Если есть звук
    {
      if (c==kACK) // И нажали ЗВУК ОТКЛ.
      {
        Event2FIFO(0xFF,0xFF,32,0);
        if (!(COPY_PU_STAT&0x10)) SetLEDon(LEDsndoff); //Включаем светодиод ЗВУК ОТКЛ
        SndMode=0; //Выключаем звук
        if (COPY_PU_STAT&0x08)
        {
#ifdef PUA1
          PORTB&=~0x10; // Выключаем реле "Пожар"
#else
          PORTD&=~0x20;
#endif
        }
        if (COPY_PU_STAT&4)
        {
#ifdef PUA1
	  PORTB&=~0x40; //Выключаем реле "Пожар 120сек"
#else
	  PORTD&=~0x10;
#endif
        }
        Fire120sec=0xFFFFFFFF; //Сбрасываем таймер 120сек
      }
      else //Нет нажатия или нажали не ЗВУК ОТКЛ.
      {
        c=0;
        if ((SndMode!=4)&&(!(COPY_PU_STAT&0x10))) SetLEDoff(LEDsndoff); //Если звук - не одиночный сигнал, то выключили светодиод ЗВУК ОТКЛ.
      }
    }
    else //Звук выключен
    {
      switch(c)
      {
      case kRT:
        //Переключение режима показа
        mask&=0x7F;
        switch(mask)
        {
        case 0x7F:
          if (LED&LEDfire) {mask=1; goto L_CHECK_PRI1;}
        case 1:
          if (LEDflash&LEDfail) {mask=2; goto L_CHECK_PRI1;}
        case 2:
          if (IsPreAtt) {mask=4; goto L_CHECK_PRI1;}
        case 4:
          mask=0x7F;
          goto L_CHECK_PRI1;
        }
        break;
      case kLT: //Переключение режима вывода
        mask^=0x80;
        break;
      case kUP:
        LastEv=FirstEv; //Первое событие -> второе событие
        EvCnt2=EvCnt1;
        if (EvCnt1==1) break; //Только первое событие
        //Ищем предыдущее событие
        EvCnt1--;
        do
        {
          FirstEv--;
          FirstEv&=0xFFF;
          ReadEvent(FirstEv,&ev); //Читаем пред. событие
          if (ev.Second==0xFFFFFFFF) goto L_CHECK_PRI; //Ошибка
          PrintEventType(&ev,s,0); // Разбираем событие без текстовых строк
        }
        while(!(TemporaryEVENT&mask)); //Пока не совпало
        break;
      case kDW:
        FirstEv=LastEv; //Второе событие -> первое событие
        EvCnt1=EvCnt2;
        //Ищем сл. событие
        EvCnt2++;
        do
        {
          LastEv++;
          LastEv&=0xFFF;
          ReadEvent(LastEv,&ev); //Читаем сл. событие
          if (ev.Second==0xFFFFFFFF)
          {
            //Нет больше событий
            LastEv=FirstEv;
            EvCnt2=EvCnt1;
            break;
          }
          PrintEventType(&ev,s,0); // Разбираем событие без текстовых строк
        }
        while(!(TemporaryEVENT&mask)); //Пока не совпало
        break;
      }
    }
  }
  while (c!=kESC); //Цикл пока не нажали кнопку СБРОС
  WarmBoot(0x4E); //Если сброс - то теплый рестарт
L_EXIT:
  KEY=kREDRAW;
}

//--------------------------------------
// Если есть отключенные устройства, то зажигаем "Отключения"
//--------------------------------------
void IncOfflineCounter(void)
{
  SetLEDon(LEDoff);
  IsOffline=1;
}

//-------------------------------------------------------------------
// Ожидание нажатия на клавиши
// На входе - флаг, надо ли выводить дату и время (для основного меню)
// Постоянно вызывает процедуру FIFO2EventLog
//-------------------------------------------------------------------
char KBD(char mode)
{
  char c,total,on;
  unsigned long td;
  unsigned int tt;
  static char CurrentTestPPKP;
  static char ViewSuprDZ; //счетчик зон для индикации дист. пуска
  static char ViewSuprDZ_flag; //флаг для индикации пуска
  unsigned int fs;
  void TestLPT(void);
  void SearchOnSensors(char ppkp, char *tot_sens, char *on_sens); //Описание для форварда
  if (!mode) UpdateConsole();
  do
  {
    fs=check_flash_sector_number;
    if (fs<512)
    {
      if ((char)FlashSectorCRC(fs,257)) //С занесенной контрольной суммой
      {
        //Не совпала CRC, генерируем ошибку
        fs|=512;
        Event2FIFO(255,255,36,0);
      }
      else
      {
        fs=(fs+1)&0x1FF;
      }
    }
    check_flash_sector_number=fs;
    //Включение/выключение светодиода ОТКЛЮЧЕНИЯ
    if (!CurrentTestPPKP)
    {
      if (IsOffline==0) SetLEDoff(LEDoff);
      IsOffline=0; //Начало теста
    }
    if (ZONEMF_AREA[CurrentTestPPKP]&0x80)
    {}
    else
    {
      //Зона выключена
      unsigned int z=CurrentTestPPKP;
      z<<=2;
      if (ZONEPOINT_AREA[z++]!=0xFFFF) IncOfflineCounter();
      if (ZONEPOINT_AREA[z++]!=0xFFFF) IncOfflineCounter();
      if (ZONEPOINT_AREA[z++]!=0xFFFF) IncOfflineCounter();
      if (ZONEPOINT_AREA[z]!=0xFFFF) IncOfflineCounter();
    }
    if (CurrentTestPPKP<64) //Если не протестировали все БР-А
    {
      SearchOnSensors(CurrentTestPPKP,&total,&on); //Считаем сколько всего извещателей для текущего БР-А и сколько вкл.
      if (PPKP_AREA[CurrentTestPPKP]&0x80) //Если БР-А включен
      {
        if (total!=on) IncOfflineCounter(); //Если суммарное кол-во не совпадает с кол-вом вкл, зажигаем "ОТКАЗ"
      }
      else //БР-А выключен
      {
        if (total>0) IncOfflineCounter(); //а извещатели есть, зажигаем "ОТКАЗ"
      }
    }
    if ((CurrentTestPPKP&0xE0)==64)
    {
      //Проверяем АПС
      if ((UPS_AREA[CurrentTestPPKP&0x1F]&0xC0)==0x40) IncOfflineCounter();
    }
    if ((CurrentTestPPKP&0xE0)==96)
    {
      //Проверяем БРА/ПКП
      if ((BRA_AREA[CurrentTestPPKP&0x1F]&0xC0)==0x40) IncOfflineCounter();
    }
    CurrentTestPPKP++; //Переходим к следующему абоненту
    tt=TRIALTIME; //Считываем количество рабочих часов
    switch(tt)
    {
    case 0xFFFF: break; //Работать вечно
    case 0x0000: NeedMoney=1; break; //Время истекло
    default: if (NeedDecTrialTime) // Если прошел час
    {
      NeedDecTrialTime=0;
      tt--; //уменьшаем время
      TRIALTIME=tt; //и записываем его
    }
    }
    _CLI();
    td=TimeDate;
    _SEI();
    if ((td>Timer30sec)) //Если прошло 30 сек
#ifdef PUA1
      PORTA&=~0x01; //Выключили подсветку
    else PORTA|=0x01; //иначе включили подсветку
#else
    PORTB&=~0x40;
    else PORTB|=0x40;
#endif
    if (mode) // Если необходимо, печатаем дату и время
    {
      char z=ViewSuprDZ;
      PrintTimeDate(CON+40,td);
      if (!z) z=1;
      if (ZoneTF[(z<<1)+1]==0xFF)
      {
        //В зоне блокирована автоматика
        if (!IsSuprDZ)
        {
          IsSuprDZ=1;
          KEY=kREDRAW;
        }
        sprintf_P(CON+58,(STR)"Дист%03d",z); //Печатаем сообщение
        if ((char)TimeDate!=ViewSuprDZ_flag) goto LNDZ;
      }
      else
      {
      LNDZ:
        do
        {
          z++;
          if (ZoneTF[(z<<1)+1]==0xFF) break; //Найдена зона
        }
        while(z&0x3F);
        ViewSuprDZ_flag=(char)TimeDate;
      }
      ViewSuprDZ=z;
      UpdateConsole();
    }
    TestLPT();
    if (MCUCSR&(1<<WDRF))
    {
      Event2FIFO(255,255,35,0); //Сброс по WDT
      MCUCSR=0;
    }
    PGMCRC256();
    c=KEY;
    if (
        FIFO2EventLog() ||   // Если пришло событие
          ((c==kACK)&&(!IS_NOLOG_ACK)) || // Или нажали ЗВУК ОТКЛ.
            (td>Timer30sec && (LED&LEDfire || LEDflash&LEDfail) ) // Или прошло 30 сек и есть отказы или пожары
              )
    {
      c=KEY=0; //Нет нажатия
      ShowLog(); // Просматриваем события
    }
  }
  while(c==0); //Пока нет нажатия на клавишу
#ifdef PUA1
  PORTA|=0x01; //Включаем подсветку
#else
  PORTB|=0x40;
#endif
  Timer30sec=td+25; //Устанавливаем таймер 30 сек
  KEY=0; //Очищаем буфер
  if ((c==kESC)&&IS_ALLWAYS_RESET)
  {
    WarmBoot(0x4E);
    c=kREDRAW;
  }
  SendCmdLCD(0x0C); //Выключаем курсор
  CursorPos=80;
  return(c); //Возвращаем код нажатой клавиши
}

//--------------------------------------
// Макроязык
//--------------------------------------
#include "vm.c"

//--------------------------------------
// Процедуры редактирования данных
//--------------------------------------
#include "EditSelect.c"

//----------------------------------------------------------
// Редактирование конфигурации извещателей
//----------------------------------------------------------
#include "SensorConfig.c"

//--------------------------------------
// Конфигурация ПУ
//--------------------------------------
#include "ConfigPU.c"

//--------------------------------------
// Перезапуск системы
//--------------------------------------
#include "Boot.c"

//--------------------------------------------------
// Отладка
//--------------------------------------------------
#include "Debug.c"

//--------------------------------------------------
// Основное меню
//--------------------------------------------------

// Показывает сообщение о авторе и вводит код разблокировки
void ShowC(void)
{                             //012345678901234567890123
  static __flash char ecode[]="Код разблокировки %04X%02X";

  int tt;
  char pos=18;
  char crc=0;
  char c;

  tt=TRIALTIME; //Остаток времени
  do
  {
    ClrScr();
    //Показывать одно или другое сообщение на 4 сек
    if ((char)TimeDate&4)
    {
      sprintf_P(CON,(STR)"ПУ-А версия 4.0   ГНПП \"Меридиан\"");
      sprintf_P(CON+40,(STR)"Украина, Харьков +380(572)528026");
    }
    else
    {
      sprintf_P(CON,(STR)"Дата/время сборки: " __DATE__ " " __TIME__);
    }
    UpdateConsole();
    if (FIFO2EventLog()) {Timer30sec=0; break;} //Если есть события - выход
  } while (KEY==0); //Пока не нажали кнопку
  if ((KBD(0)==kSECRET) && (tt!=0xFFFF)) //Если нажали стрелку вправо и не вечная работа
  {
    ClrScr(); //Очистили экран
    tt=0;
    do
    {
      sprintf_P(CON,ecode,tt,crc); //Сообщение о коде разблокировки
      CursorPos=pos; //Устанавливаем курсор
      c=KBD(0); //Ожидаем код клавиши
      if ((c>=32 && c<128)||(c>=192)) //Если символьная клавиша
      {
        if (Edit_hi(c,pos,18,&tt)) {c=kRT;} //Пытаемся редактировать
        if (Edit_hb(c,pos,22,&crc)) {c=kRT;}
      }
      switch(c) //обрабатываем курсор
      {
      case kLT: //нажали "<<"
        switch (pos)
        {
        case 18: c=kESC; break; //Курсор уже левее всего
        default: pos--; //Перемещение курсора влево
        }
        break;
      case kRT:
        switch (pos)
        {
        case 23: break; //Курсор уже правее всего
        default: pos++; //Перемещение вправо
        }
        break;
      case kUP:
        Select_hi(pos,18,1,&tt); //Пытаемся редактировать (увеличение)
        Select_hb(pos,22,1,&crc);
        break;
      case kDW:
        Select_hi(pos,18,-1,&tt); //Пытаемся редактировать (уменьшение)
        Select_hb(pos,22,-1,&crc);
        break;
      }
    }
    while (c!=kESC); // Цикл пока не выход
    //Считаем СКС
    c=CRC8tab[(char)tt];
    c=CRC8tab[((char)(tt>>8))^c];
    if ((!CRC8tab[crc^c])&&tt) // Если все совпало
    {
      NeedMoney=0; // Обычный режим
      TRIALTIME=tt; //Записываем кол-во введенных часов
    }
  }
}

//----------------------
// Просмотр архива событий
//----------------------
#include "ViewLog.c"

//--------------------------
// Конфигурация БР-А
//--------------------------
#include "ConfigPPKP.c"


//---------------------------
// Конфигурация зон
//---------------------------
#include "Zone.c"
#include "ConfigTK.c"

//-------------------------------------
// Изменение адресов
//-------------------------------------
#include "EditAdr.c"


//-------------------------------------------------------
#include "MainCAN.c"

//-------------------------------------------------------
#include "ConfigBRA.c"
#include "ConfigUPS.c"

//-------------------------------------------------------
// Процедуры начального тестирования
//-------------------------------------------------------
void TestKBD(void)
{
  static __flash STR testm[]=
  {
    (STR)"Нажми <Звук откл.>",
    (STR)"Нажми <Влево>",
    (STR)"Нажми <Вправо>",
    (STR)"Нажми <Вверх>",
    (STR)"Нажми <Вниз>",
    (STR)"Нажми <Сброс>"
  };

  char tm;
  char c;
  tm=TEST_STAT;
  SetLEDflash(LEDfire|LEDmfire|LEDoff|LEDfail|LEDsndoff|LEDsysf);
  for(;;)
  {
    if (tm>5) return;
    ClrScr();
    sprintf_P(CON,testm[tm]);
    UpdateConsole();
    KEY=0;
    while((c=KEY)==0);
    switch(tm)
    {
    case 0: if (c!=kACK) tm=0xFF; break;
    case 1: if (c!=kLT) tm=0xFF; break;
    case 2: if (c!=kRT) tm=0xFF; break;
    case 3: if (c!=kUP) tm=0xFF; break;
    case 4: if (c!=kDW) tm=0xFF; break;
    case 5: if (c==kESC) {TEST_STAT=6; return; }
    default: tm=0xFF;
    }
    tm++;
  }
}

void TestLPT(void)
{
  static unsigned int LPTstep=128;
  //static char LPTbyte;
  char c;
  char s;
  char stb;
  if (TSTS!=18) return;

  s=(char)LPTstep;
  stb=LPTstep>>8;

#ifdef PUA1
  PORTC=~s; //Выводим данные
  if (stb) PORTA&=0x7F; else PORTA|=0x80;
  delay(100);
  s=(PINA&0x40)?0:1;
#else
  c=0;
  if (s&0x01) c|=0x40;
  if (s&0x02) c|=0x80;
  if (s&0x04) c|=0x02;
  if (s&0x08) c|=0x08;
  if (s&0x10) c|=0x20;
  if (s&0x20) c|=0x01;
  if (s&0x40) c|=0x04;
  if (s&0x80) c|=0x10;
  PORTC=~c;
  if (stb) PORTB&=0xEF; else PORTB|=0x10;
  delay(100);
  s=(PINB&0x20)?0:1;
#endif
  c=lpt_tab[LPTstep>>3]&(0x80>>(LPTstep&7))?1:0;
  //if (s) LPTbyte=(LPTbyte<<1)+1; else LPTbyte<<=1;

  memset(CON,' ',40);
  sprintf_P(CON,(PGM_P)"Тест принтера %03X=%d/%d",LPTstep,s,c);
  UpdateConsole();
  //if ((LPTstep&7)==7)
  //{
  // _EEPUT((BRVU_AREA+(LPTstep>>3)),LPTbyte);
  //}
  if (c^s)
  {
    LPTstep=128;
    return;
  }
  LPTstep++;
  if (LPTstep==256) LPTstep=256+128;
  if (LPTstep==512)
  {
    TSTS=19;
    TEST_STAT=19;
    KEY=1;
  }
}

//Восстановление автоматики в выбраной зоне
void AutoSupr(void)
{
  char z;
  char c=0;
  do
  {
    z=1;
    //Ищем зону
    do
    {
      if (ZoneTF[(z<<1)+1]==0xFF) break; //Найдена зона
    LNEXT:
      z++;
    }
    while(z);
    if (!z)
    {
      if (c)
      {
        c=0;
        continue;
      }
      else
        return;
    }
    ClrScr();
    sprintf_P(CON,(STR)"Зона %03d в дистанционном режиме",z);
    sprintf_P(CON+40,(STR)"<Вправо>-восстановить  Другая-следующая");
    c=KBD(0);
    switch(c)
    {
    case kLT: c=kESC; break;
    case kRT:
      //Восстанавливаем автоматику
      Event2FIFO(255,255,37,z); //Основная обработка в Event2FIFO
      return;
      //      break;
    default:
      goto LNEXT;
    }
  }
  while(c!=kESC);
}

//-------------------------------------------------------
// Основная задача
//-------------------------------------------------------
void main(void)
{
  unsigned long td;
  char c,i=0;
  unsigned int z;
  InitSystem(); //Инициализируем аппаратуру
  InitLCD(); //Инициализируем ЖКИ
  memcpy_P(CON,
           "Проверка памяти программ                ",40);
  memcpy_P(CON+40,MCUCSR&(1<<WDRF)?
           "Системная неисправность!!!              ":
             "                                        ",40);
  if (MCUCSR&(1<<WDRF))
  {
    PORTB|=0x80; //Включить звук...
#ifdef PUA1
    PORTD=LEDsysf;
#else
    PORTA=LEDsysf;
#endif
  }
  else
  {
#ifdef PUA1
    PORTD=0;
#else
    PORTA=0;
#endif
  }
  UpdateConsole();
  PGMCRC();
  ClrScr();
  PUID=PU_ID;
  COPY_PU_STAT=PU_STAT;
  SysPPKP_txsize=0; //Сброс флага сервисного обмена
  InitCAN(PUID);
  z=0;
  do
  {
    ZoneDelays[z]=0xFE;
    z++;
  }
  while (z!=1024);
  SearchTopEventLog(); //Ищем последнее событие в архиве
  LogFileRP_CAN=LogFileWP;
  LogFileRP_DPU=LogFileWP;
  ConstructABONS(); //Читаем данные по БР-А
  __watchdog_reset();
  WDTCR=0x08;
  WDTCR=0x18;
  WDTCR=0x0B; //Разрешаем сторожевой таймер
#ifdef PUA1
  if (!(PINA&0x40)) PrinterOnLine=0; //Проверяем подключенность принтера
#else
  if (!(PINB&0x20)) PrinterOnLine=0;
#endif
  InitRS();
  _SEI();
  printf_P(Message_MainMenu[i]); //Печатаем заголовок
  _CLI(); //Разрешаем обработку внешней клавиатуры
  DDRD&=0xFE;
  PORTD|=0x01;
  EIMSK=0x01;
  td=TimeDate;
  _SEI();
  putchar(' ');
  LPT_PrintTimeDate(td);
  PRINT_CRLF;
  PrintSeparator();
L_TEST1:
#ifdef PUA1
  PORTA|=0x01; //Включаем подсветку
#else
  PORTB|=0x40;
#endif
  TestKBD();
  TSTS=TEST_STAT;
  //WarmBoot(0x4F);
  Event2FIFO(255,255,0,0);
  Event2FIFO(255,255,4,0x4F);
  PrinterOnLine=1000; //Время до события "Принтер в режиме OffLine"
  for(;;)
  {
    ClrScr();
    // Если закончилось время, печатаем сообщение, иначе пункт меню
    if (NeedMoney) sprintf_P(CON,Message_NeedMoney); else
    {
      if (i)
      {
        sprintf_P(CON,Message_MainMenu[i]);
        sprintf_P(CON+66,(STR)"<Вправо>-войти");
      }
      else
      {
        if (TSTS<6) goto L_TEST1;
        if (TSTS>18)
        {
          if (IsSuprDZ&&ISMASTER) sprintf_P(CON+28,(STR)"<Вправо>-авт");
          sprintf_P(CON,Message_MainMenu[i]);
          sprintf_P(CON+69,(STR)"<Вниз>-меню");
          if (DOUBLE_PU)
          {
            if (ISMASTER)
            {
              sprintf_P(CON+8,(STR)"Основной");
            }
            else
            {
              sprintf_P(CON+8,(STR)"Дублирующий");
            }
          }
        }
        else
        {
          c=TSTDBG;
          sprintf_P(CON,testm1[TSTS-6],c);
        }
      }
    }
    c=KBD(1); //Ждем клавишу
    if (
        (TSTS>5)&&(TSTS<16) //Надо ждать нажатия на PC клав.
          &&((TSTS-6+'0')==c) //И нажата ожидаемая клавиша
            )
    {
      TSTS++; //Следующая проверка
      TEST_STAT=TSTS;
    }
    if (c>=0x11 && c<=0x19) //Если F1..F8
    {
      i=c-0x10; //Устанавливаем нужный пункт
      c=kRT; //и входим в него
    }
    if (NeedMoney) i=0; //если закончилось время, то не ходить в меню
    if ((!ISMASTER)&&(i>2)) i=0;
    switch(c)
    {
    case kLT:
      i=0;
      break;
    case kRT: //Нажали стрелку вниз
      switch(i) //Идем в пункт меню
      {
      case 0: if (IsSuprDZ&&ISMASTER) AutoSupr(); else ShowC(); break;
      case 1: ViewLog(); break;
      case 2: Boot(); break;
      case 3: ConfigPU(); break;
      case 4: ConfigPPKP(); break;
      case 5: ConfigSensor(); break;
      case 6: ConfigZone(); break;
      case 7: break;
      case 8: ConfigBRA(); break;
      case 9: ConfigUPS(); break;
      case 10: EditAdresses(); break;
      case 11: Debug(); break;
      } ClrScr(); break;
      //Выбор пункта меню
    case kUP: if (i>0) i--; if (i==7) i=6; break;
    case kDW: if (i<11) i++; if (i==7) i=8; break;
    }
    if ((!ISMASTER)&&(i>2)) i=0;
  }
}

