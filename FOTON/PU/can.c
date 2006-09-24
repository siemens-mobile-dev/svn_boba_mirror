#include "iom128.h"
#include "canll.h"
#include "can.h"

void WriteCAN(char adr, char val)
{
  EnaCAN;
  SPDR=CMD_WR;
  WaitSPI;
  SPDR=adr;
  WaitSPI;
  SPDR=val;
  WaitSPI;
  DisCAN;
}

char ReadCAN(char adr)
{
  EnaCAN;
  SPDR=CMD_RD;
  WaitSPI;
  SPDR=adr;
  WaitSPI;
  SPDR=0xFF;
  WaitSPI;
  DisCAN;
  return(SPDR);
}

void BitmCAN(char adr, char mask, char data)
{
  EnaCAN;
  SPDR=CMD_BM;
  WaitSPI;
  SPDR=adr;
  WaitSPI;
  SPDR=mask;
  WaitSPI;
  SPDR=data;
  WaitSPI;
  DisCAN;
}

char TransmitCAN(char nport,TCANTXBUF *tbuf)
{
  char *buf=(char *)tbuf;
  char port;
  char i;
  switch(nport)
  {
  case 0: port=TX0; break;
  case 1: port=TX1; break;
  case 2: port=TX2; break;
  default: return(ECAN_ILLEGALPORT);
  }
  if (ReadCAN(EFLG)&ECAN_TXBO) return(ECAN_BUSOFF);
  if (ReadCAN(port-TX0+TX0CTRL)&CAN_TXREQ) return(ECAN_TXBUSY);
  i=sizeof(TCANTXBUF);
  do
  {
    WriteCAN(port,*buf);
    port++;
    buf++;
  }
  while (--i);
  EnaCAN;
  SPDR=CMD_RTS|(1<<nport);
  WaitSPI;
  DisCAN;
  return(ECAN_OK);
}

char ReciveCAN(char port,TCANRXBUF *rbuf)
{
  char *buf=(char *)rbuf;
  char i;
  if (port>1) return(ECAN_ILLEGALPORT);
  i=1<<port;
  if (!(ReadCAN(CANINTF)&i)) return(ECAN_RXEMPTY);
  BitmCAN(CANINTF,i,0);
  switch(port)
  {
  case 0: port=RX0; break;
  case 1: port=RX1; break;
  default: return(ECAN_ILLEGALPORT);
  }
  i=sizeof(TCANRXBUF);
  do
  {
    *buf=ReadCAN(port);
    port++;
    buf++;
  }
  while (--i);
  return(ECAN_OK);
}

void InitCAN(char PUID)
{
  if (!PUID) return;
  EnaCAN;
  SPDR=CMD_RES;
  WaitSPI;
  DisCAN;
  WriteCAN(CNF1,0x40);
  WriteCAN(CNF2,0xF1);
  WriteCAN(CNF3,0x05);

  WriteCAN(RX0CTRL,0x20);
  WriteCAN(RX1CTRL,0x20);
  WriteCAN(FILTER0,PUID); WriteCAN(FILTER0+1,0x00);
  WriteCAN(FILTER1,PUID); WriteCAN(FILTER1+1,0x00);
  WriteCAN(FILTER2,PUID+0xC0); WriteCAN(FILTER2+1,0x00);
  WriteCAN(FILTER3,PUID+0xC0); WriteCAN(FILTER3+1,0x00);
  WriteCAN(FILTER4,PUID+0xC0); WriteCAN(FILTER4+1,0x00);
  WriteCAN(FILTER5,PUID+0xC0); WriteCAN(FILTER5+1,0x00);
  WriteCAN(MASK0,0xFF); WriteCAN(MASK0+1,0x00);
  WriteCAN(MASK1,0xFF); WriteCAN(MASK1+1,0x00);

  WriteCAN(CANCTRL,0x07);
}


