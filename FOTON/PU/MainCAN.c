void DebugCAN(void)
{                          //   01 234   5   678
  /* static __flash char mess1[]="\200TX%02X%02X ";
  static __flash char mess2[]="%02X";
  static __flash char mess3[]="\230RxStat: %02X  ";
  static __flash char mess4[]="\230RX%02X%02X ";
  static __flash char mess5[]="\244TxStat: %02X  ";

  TCANTXBUF txb;
  TCANRXBUF rxb;

  char sid;
  char dlc;
  char i;

  char c;
  static char pos=0;

  ClrScr();
  do
  {
  c=ReciveCAN(0,&rxb);
  if (c==ECAN_OK)
  {
  printf_P(mess4,rxb.SID,(rxb.DLC&0xF)|(rxb.SIDlo&0x10));
  i=0;
  do
  {
  printf_P(mess2,rxb.D[i]);
  i++;
} while (i!=8);
} else printf_P(mess3,c);
  txb.SID=sid;
  txb.DLC=dlc;
  txb.SIDlo=0;
  printf_P(mess1,sid,dlc);
  i=0;
  do
  {
  printf_P(mess2,txb.D[i]);
  i++;
} while (i!=8);
  putchar(pos+128);
  CursorON();
  c=KBD(0);
  CursorOFF();
  if ((c>=32 && c<128)||(c>=192))
  {
  if (Edit_hb(c,pos,2,&sid)) c=kRT;
  if (Edit_hb(c,pos,4,&dlc)) c=kRT;
  i=0;
  do
  {
  if (Edit_hb(c,pos,i*2+8,&txb.D[i])) c=kRT;
  i++;
} while (i!=8);
}
  switch(c)
  {
case kLT:
  switch (pos)
  {
case 0: break;
case 2: pos=0; break;
case 8: pos=6; break;
  default: pos--;
}
  break;
case kRT:
  switch (pos)
  {
case 23: break;
case 6: pos=8; break;
case 0: pos=2; break;
  default: pos++;
}
  break;
case kSEL:
case kUP:
  if (pos==0)
  {
  printf_P(mess5,TransmitCAN(0,&txb));
}
  Select_hb(pos,2,1,&sid);
  Select_hb(pos,4,1,&dlc);
  i=0;
  do
  {
  Select_hb(pos,i*2+8,1,&txb.D[i]);
  i++;
} while (i!=8);
  break;
case kOK:
case kDW:
  if (pos==0)
  {
}
  Select_hb(pos,2,-1,&sid);
  Select_hb(pos,4,-1,&dlc);
  i=0;
  do
  {
  Select_hb(pos,i*2+8,-1,&txb.D[i]);
  i++;
} while (i!=8);
  break;
}
}
  while (c!=kESC);
  */}

void Event2CAN(void)
{
  TCANTXBUF txb;
  TCANRXBUF rxb;

  char c;
  struct TLogEvent ev; // Текущее сообщение

  if (!PUID) return;

  c=ReciveCAN(0,&rxb);
  if (c==ECAN_OK)
  {
    if (((rxb.DLC&0xF)|(rxb.SIDlo&0x10))==0x10)
    {
      if (LogFileRP_CAN!=LogFileWP)
      {
        ReadEvent(LogFileRP_CAN,&ev); // Читаем его из 45??021
        memcpy(txb.D,&ev,sizeof(ev));
        txb.DLC=sizeof(ev);
      }
      else
      {
        txb.DLC=0;
      }
      txb.SID=PUID;
      txb.SIDlo=0;
      txb.EID=0;
      c=TransmitCAN(0,&txb);
      if ((c==ECAN_OK)&&(LogFileRP_CAN!=LogFileWP)) LogFileRP_CAN=(LogFileRP_CAN+1)&0xFFF; //Берем следующее событие
      if ((c==ECAN_TXBUSY)&&(!(FlagsCAN&1)))
      {
        Event2FIFO(0xFF,0xFF,27,0);
        FlagsCAN|=1;
      }
    }
  }
  else
  {
    if ((c!=ECAN_RXEMPTY)&&(!(FlagsCAN&4)))
    {
      Event2FIFO(0xFF,0xFF,29,c);
      FlagsCAN|=4;
    }
  }
}

void PUSconnect(char mode, char pus, char section)
{
  TCANTXBUF txb;
  TCANRXBUF rxb;

  char c;

  if (!PUID) return;

  if (mode)  //Необходимо послать пуск
  {
    txb.DLC=2;
    txb.D[1]=section;
  }
  else //Просто опрос
  {
    txb.DLC=1;
  }
  txb.SID=pus+0x80;
  txb.D[0]=PUID;
  txb.SIDlo=0;
  txb.EID=0;
  c=TransmitCAN(1,&txb);
  if ((c==ECAN_TXBUSY)&&(!(FlagsCAN&2)))
  {
    Event2FIFO(0xFF,0xFF,28,0);
    FlagsCAN|=2;
  }
  else
  {
    CANtimeout=100; //1c
    do
    {
      c=ReciveCAN(1,&rxb);
      if (!CANtimeout)
      {
        if (!(EPUS[pus]&0x40)) Event2FIFO(pus+0x80,0,1,0);
        EPUS[pus]|=0x40;
        break;
      }
      if ((c!=ECAN_RXEMPTY)&&(c!=ECAN_OK))
      {
        Event2FIFO(0xFF,0xFF,30,c);
        FlagsCAN|=8;
        break;
      }
    }
    while (c!=ECAN_OK);
    if ((c==ECAN_OK)&&((rxb.DLC&0x0F)==1))
    {
      Event2FIFO(pus+0x80,0,3+rxb.D[0],section);
    }
  }
}
