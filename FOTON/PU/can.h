//
// CAN buffers structures
//
typedef unsigned int WORD;
typedef unsigned long DWORD;

typedef struct
{
  char SID;
  char SIDlo;
  WORD EID;
  char DLC;
  char D[8];
} TCANTXBUF;

typedef struct
{
  char SID;
  char SIDlo;
  WORD EID;
  char DLC;
  char D[8];
} TCANRXBUF;

//
// error codes
//
#define ECAN_OK 0
#define ECAN_ILLEGALPORT 1
#define ECAN_BUSOFF 2
#define ECAN_TXBUSY 3
#define ECAN_RXEMPTY 4

#define ECAN_RX1OVR 0x80
#define ECAN_RX0OVR 0x40
#define ECAN_TXBO   0x20
#define ECAN_TXEP   0x10
#define ECAN_RXEP   0x08
#define ECAN_TXWAR  0x04
#define ECAN_RXWAR  0x02
#define ECAN_EWARN  0x01

#define CAN_TXREQ   0x08

void InitCAN(char PUID);
char ReciveCAN(char port,TCANRXBUF *rbuf);
char TransmitCAN(char nport,TCANTXBUF *tbuf);


