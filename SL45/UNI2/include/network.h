extern unsigned int NETMON_RX;
extern unsigned int NETMON_mA;
extern unsigned int NETMON_VB;
extern unsigned int NETMON_LS;
extern unsigned int NETMON_TB;
extern char NETMON_C1;
extern char NETMON_C2;


extern unsigned int NETMON_CI;
extern unsigned int NETMON_LAC;

struct CELLDATA
{
 unsigned int CH;
 unsigned int field2;
 unsigned int field4;
 unsigned int field6;
 unsigned int field8;
 unsigned char fieldA;
 unsigned char fieldB;
 unsigned int fieldC;
 unsigned int fieldE;
 unsigned int field10;
 unsigned int field12;
 unsigned int field14;
 unsigned int LAC;
 unsigned int CI;
};

extern struct CELLDATA far *CELL6_PNT[];
extern struct CELLDATA far *CURCELL_PNT;

extern struct
{
 unsigned int CH;
 unsigned char RXLVL;
 unsigned char C1;
 unsigned char C2;
 unsigned char N;
 unsigned char B;
 unsigned char dummy;
} NETMON7_TAB[7];

// 1 - 900
// 2 - 1800
// 3 - auto
extern void SetBAND(unsigned int band);
// Возвращает уровень приема в канале ch
extern unsigned int GetRXLVLbyCH(unsigned int ch);
