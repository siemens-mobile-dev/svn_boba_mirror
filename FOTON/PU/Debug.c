//--------------------------------------------------
// Меню "Отладка приборов"
//--------------------------------------------------
void DebugPPKP(void)
{
  static char ppkp=1;
  static char adr=0;
  char val;
  char st[8];
  char c;
  static char pos=9;
  char readflag;
  readflag=1;
  SysPPKP_rxsize=2;
  do
  {
    ClrScr();
    ppkp&=127;
    if (readflag)
    {
      _CLI();
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=ppkp;
      SysPPKP_TXB[1]=0x49;
      SysPPKP_TXB[2]=adr;
      SysPPKP_txsize=3;
      _SEI();
      while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
      if (SysPPKP_rxsize!=3)
      {
        if (pos>11) pos=11;
      }
      val=SysPPKP_RXB[1];
      readflag=0;
    }
    c=0;
    do
    {
      st[c]=(val&(0x80>>c))?'1':'0';
      c++;
    }
    while (c<8);
    sprintf_P(CON,"Абонент: %03d Adr: %02X  ",ppkp,adr);
    if (SysPPKP_rxsize==3)
    {
      sprintf_P(CON+22,"<Вверх>-Запись");
      sprintf_P(CON+40,"%02X %.8s %03d %c",val,st,val,((val>=32 && val<128)||(val>=192))?val:'.');
    }
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_db(c,pos,9,&ppkp)) {ppkp&=127; c=kRT;}
      if (Edit_hb(c,pos,18,&adr)) {readflag=1; c=kRT;}
      if (Edit_hb(c,pos,40,&val)) c=kRT;
      if (Edit_bb(c,pos,43,&val)) c=kRT;
      if (Edit_db(c,pos,52,&val)) c=kRT;
      if (pos==56) val=c;
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 9: c=kESC; break;
      case 18: pos=11; break;
      case 22: pos=19; break;
      case 40: pos=22; break;
      case 43: pos=41; break;
      case 52: pos=50; break;
      case 56: pos=54; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 56: break;
      case 54: pos=56; break;
      case 50: pos=52; break;
      case 41: pos=43; break;
      case 22: pos=40; break;
      case 19: pos=22; break;
      case 11: pos=18; readflag=1; break;
      default: pos++;
      }
      break;
    case kUP:
      if (pos==22)
      {
        _CLI();
        SysPPKP_rxsize=0;
        SysPPKP_TXB[0]=ppkp;
        SysPPKP_TXB[1]=0x48;
        SysPPKP_TXB[2]=adr;
        SysPPKP_TXB[3]=val;
        SysPPKP_txsize=4;
        _SEI();
        while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        readflag=1;
      }
      if (pos==18||pos==19) readflag=1;
      Select_db(pos,9,1,&ppkp);
      Select_hb(pos,18,1,&adr);
      Select_hb(pos,40,1,&val);
      Select_bb(pos,43,1,&val);
      Select_db(pos,52,1,&val);
      if (pos==56) val++;
      break;
    case kDW:
      if (pos==18||pos==19) readflag=1;
      Select_db(pos,9,-1,&ppkp);
      Select_hb(pos,18,-1,&adr);
      Select_hb(pos,40,-1,&val);
      Select_bb(pos,43,-1,&val);
      Select_db(pos,52,-1,&val);
      if (pos==56) val--;
      break;
    }
  }
  while (c!=kESC);
}

void ReadSensorStatus(void)
{
  static __flash char mess1[]="БР-А/Изв-ль: %02d/%02d <Вверх>-Чтение";
  static __flash char mess2[]="%.5s: %.8s %03d %03d ";
  static __flash char mess3[]="%.24s";
  struct TSensorConfig sc;
  static char ppkp=1;
  static char sensor=1;
  char sb[8];
  char st[5];
  char c;
  static char pos=13;

  SysPPKP_rxsize=0xFF;
  do
  {
    Max63(&ppkp);
    Max63(&sensor);
    memcpy_P(st,NameSensorType[SysPPKP_RXB[1]&0x1f],5);
    c=0;
    do
    {
      sb[c]=(SysPPKP_RXB[2]&(0x80>>c))?'1':'0';
      c++;
    }
    while (c<8);
    ReadSensorConfig(ppkp,sensor,&sc);
    ClrScr();
    sprintf_P(CON,mess1,ppkp,sensor);
    if (pos==19)
    {
      if (SysPPKP_rxsize!=6)
        sprintf_P(CON+40,(STR)"Не получены данные!");
      else
        sprintf_P(CON+40,mess2,st,sb,SysPPKP_RXB[3],SysPPKP_RXB[4]);
    }
    else sprintf_P(CON+40,mess3,sc.GeoPos);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,13,&ppkp)) {c=kRT; SysPPKP_rxsize=0xFF;}
      if (Edit_dc(c,pos,16,&sensor)) {c=kRT; SysPPKP_rxsize=0xFF;}
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 13: c=kESC; break;
      case 19: pos=17; break;
      case 16: pos=14; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 19: break;
      case 17: pos=19; break;
      case 14: pos=16; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,13,1,&ppkp);
      Select_dc(pos,16,1,&sensor);
      if (pos==19)
      {
        //Read status
        do
        {
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=sensor;
          SysPPKP_TXB[2]=3;
          SysPPKP_txsize=3;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        while (SysPPKP_rxsize==2);
      }
      else SysPPKP_rxsize=0xFF;
      break;
    case kDW:
      Select_dc(pos,13,-1,&ppkp);
      Select_dc(pos,16,-1,&sensor);
      if (pos!=19) SysPPKP_rxsize=0xFF;
      break;
    }
  }
  while (c!=kESC);
}

void ReadSensorSetup(void)
{
  static __flash char mess1[]="БР-А/Изв-ль: %02d/%02d <Вверх>-Чтение";
  static __flash char mess2[]="%03d: %.8s %03d %03d   ";
  static __flash char mess3[]="%.24s";
  struct TSensorConfig sc;
  static char ppkp=1;
  static char sensor=1;
  char sb[8];
  char c;
  static char pos=13;

  SysPPKP_rxsize=0xFF;
  do
  {
    Max63(&ppkp);
    Max63(&sensor);
    c=0;
    do
    {
      sb[c]=(SysPPKP_RXB[2]&(0x80>>c))?'1':'0';
      c++;
    }
    while (c<8);

    ReadSensorConfig(ppkp,sensor,&sc);
    ClrScr();
    sprintf_P(CON,mess1,ppkp,sensor);
    if (pos==19)
    {
      if (SysPPKP_rxsize!=6)
        sprintf_P(CON+40,(STR)"Не получены данные!");
      else
        sprintf_P(CON+40,mess2,SysPPKP_RXB[1],sb,SysPPKP_RXB[3],SysPPKP_RXB[4]);
    }
    else sprintf_P(CON+40,mess3,sc.GeoPos);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,13,&ppkp)) {c=kRT; SysPPKP_rxsize=0xFF;}
      if (Edit_dc(c,pos,16,&sensor)) {c=kRT; SysPPKP_rxsize=0xFF;}
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 13: c=kESC; break;
      case 16: pos=14; break;
      case 19: pos=17; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 19: break;
      case 17: pos=19; break;
      case 14: pos=16; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,13,1,&ppkp);
      Select_dc(pos,16,1,&sensor);
      if (pos==19)
      {
        //Read status
        do
        {
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=sensor;
          SysPPKP_TXB[2]=4;
          SysPPKP_txsize=3;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        while (SysPPKP_rxsize==2);
      } else SysPPKP_rxsize=0xFF;
      break;
    case kDW:
      Select_dc(pos,13,-1,&ppkp);
      Select_dc(pos,16,-1,&sensor);
      if (pos!=19) SysPPKP_rxsize=0xFF;
      break;
    }
  }
  while (c!=kESC);
}

void ReadSensorCurrents(void)
{
  static __flash char mess1[]="БР-А/Изв-ль: %02d/%02d <Вверх>-Чтение";
  static __flash char mess2[]="%.5s: Л:%03d A:%03d C:%03d";
  static __flash char mess3[]="%.24s";
  static char ppkp=1;
  static char sensor=1;
  struct TSensorConfig sc;
  char st[5];
  char c;
  static char pos=13;

  SysPPKP_rxsize=0xFF;
  do
  {
    Max63(&ppkp);
    Max63(&sensor);
    ReadSensorConfig(ppkp,sensor,&sc);
    memcpy_P(st,NameSensorType[sc.Type&0x1f],5);
    ClrScr();
    sprintf_P(CON,mess1,ppkp,sensor);
    if (pos==19)
    {
      if (SysPPKP_rxsize!=5)
        sprintf_P(CON+40,(STR)"Не получены данные!");
      else
        sprintf_P(CON+40,mess2,st,SysPPKP_RXB[1],SysPPKP_RXB[2],SysPPKP_RXB[3]);
    }
    else sprintf_P(CON+40,mess3,sc.GeoPos);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,13,&ppkp)) {c=kRT; SysPPKP_rxsize=0xFF;}
      if (Edit_dc(c,pos,16,&sensor)) {c=kRT; SysPPKP_rxsize=0xFF;}
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 13: c=kESC; break;
      case 16: pos=14; break;
      case 19: pos=17; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 19: break;
      case 17: pos=19; break;
      case 14: pos=16; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,13,1,&ppkp);
      Select_dc(pos,16,1,&sensor);
      if (pos==19)
      {
        //Read status
        do
        {
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=sensor;
          SysPPKP_TXB[2]=0x06;
          SysPPKP_txsize=3;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        while (SysPPKP_rxsize==2);
      } else SysPPKP_rxsize=0xFF;
      break;
    case kDW:
      Select_dc(pos,13,-1,&ppkp);
      Select_dc(pos,16,-1,&sensor);
      if (pos!=19) SysPPKP_rxsize=0xFF;
      break;
    }
  }
  while (c!=kESC);
}

char TestSure(char xpos)
{
  char c;
  char pos=9;
  //012345678901234567890123
  static __flash char mess[]="Уверены? Нет! Да!";
  memset(CON+xpos,' ',40);
  sprintf_P(CON,mess);
  do
  {
    CursorPos=pos+xpos;
    c=KBD(0);
    switch(c)
    {
    case kLT:
      pos=9;
      break;
    case kRT:
      pos=14;
      break;
    case kUP:
    case kDW:
      return(pos==9?0:1);
    }
  }
  while (c!=kESC);
  return(0);
}

void ClearFlash(void)
{
  static __flash char Message_ClearFlash[]="Очистка Flash-памяти...";
  int page;
  char i,j;

  memset(CON+66,32,80-66);
  if (!TestSure(0)) return;
  page=0;
  ClrScr();
  sprintf_P(CON,Message_ClearFlash);
  UpdateConsole();
  WaitFlash;
  // Construct free config in buffer
  // _CLI();
  FlashOn;
  SPDR=0x84;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  SPDR=0x00;
  WaitSPI;
  for (j=0;j<8;j++)
  {
    for (i=0;i<24;i++)
    {
      SPDR=' ';
      WaitSPI;
    }
    for (i=0;i<8;i++)
    {
      SPDR=0;
      WaitSPI;
    }
  }
  FlashOff;
  AddFlashSectorCRC();
  // _SEI();
  // Write to flash
  do
  {
    WaitFlash;
    //  _CLI();
    FlashOn;
    SPDR=0x83;
    WaitSPI;
    SPDR=page>>8;
    WaitSPI;
    SPDR=page;
    WaitSPI;
    SPDR=0;
    WaitSPI;
    FlashOff;
    //  _SEI();
    page+=2;
  }
  while (page<1024);
  Event2FIFO(0xff,0xff,3,0);
}

void ClearLogFile(void)
{
  void WarmBoot(char cmd);
  memset(CON+66,32,80-66);
  if (!TestSure(0)) return;
  Event2FIFO(0xff,0xff,2,0);
  WarmBoot_cmd=0x4E;
  if (ISMASTER) Event2FIFO(0xff,0xff,4,0x4E); //Сообщение "Норма"
  //WarmBoot(0x4E);
}

void DebugMRV(void)
{
  static __flash char mess1[]="МРВ%01d: %.8s %.8s ";
  char mrv=0;
  char vr;
  char vw=0;
  char s1[8];
  char s2[8];
  char c;
  static char pos=0;
  do
  {
    for (c=0;c<16;c++)
    {
      WriteI2C(0x92+(mrv<<1),1,vw);
      if (!I2Cerror) break;
    }
    if (!I2Cerror)
    {
      for (c=0;c<16;c++)
      {
        vr=ReadI2C(0x92+(mrv<<1),1);
        if (!I2Cerror) break;
      }
    }
    c=0;
    do
    {
      s1[c]=(vw&(0x80>>c))?'1':'0';
      s2[c]=(vr&(0x80>>c))?'1':'0';
      c++;
    }
    while (c<8);
    ClrScr();
    sprintf_P(CON,mess1,mrv+1,s1,s2);
    sprintf_P(CON+40,I2Cerror?"Ошибка обмена":"Нет ошибки обмена");
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_bb(c,pos,6,&vw)) {c=kRT;}
      if (pos==0) mrv^=1;
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 0: c=kESC; break;
      case 6: pos=0; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 13: break;
      case 0: pos=6; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_bb(pos,6,1,&vw);
      if (pos==0) mrv^=1;
      break;
    case kDW:
      Select_bb(pos,6,-1,&vw);
      if (pos==0) mrv^=1;
      break;
    }
  }
  while (c!=kESC);
}

void DebugFP(void)
{
  static __flash char mess1[]="БР-А/Изв-ль: %02d/%02d <Вверх>-Чтение/Авт";
  static __flash char mess2[]="%.5s: H:%03d Ф:%03d ";
  static __flash char messok[]="Норма";
  static __flash char messdust[]="Загрязнение";
  static __flash char messfail[]="Отказ УФ";

  struct TSensorConfig sc;
  static char ppkp=1;
  static char sensor=1;
  char st[5];
  char c;
  char pos=13;
  char results[64];
  char mode=0;

  SysPPKP_RXB[1]=0;
  do
  {
    Max63(&ppkp);
    Max63(&sensor);
    memcpy_P(st,NameSensorType[SysPPKP_RXB[1]&0x1f],5);
    ReadSensorConfig(ppkp,sensor,&sc);
    ClrScr();
    sprintf_P(CON,mess1,ppkp,sensor);
    if ((SysPPKP_RXB[1]&0x1f)!=4) goto L1;
    if (pos==27)
    {
      sprintf_P(CON+40,mess2,st,SysPPKP_RXB[3],SysPPKP_RXB[4]);
      if ((signed char)(SysPPKP_RXB[3]-SysPPKP_RXB[4])<=0)
      {
        sprintf_P(CON+60,messfail);
        results[sensor]=4;
      }
      else
      {
        if (SysPPKP_RXB[4]<10)
        {
          sprintf_P(CON+60,messdust);
          results[sensor]=8;
        }
        else sprintf_P(CON+60,messok);
      }
    }
    else
  L1:
    memcpy(CON+40,sc.GeoPos,24);
    UpdateConsole();
    if (mode)
    {
      c=kUP;
      goto L2;
    }
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,13,&ppkp)) c=kRT;
      if (Edit_dc(c,pos,16,&sensor)) c=kRT;
    }
  L2:
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 13: c=kESC; break;
      case 34: pos=27; break;
      case 27: pos=17; break;
      case 16: pos=14; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 34: break;
      case 27: pos=34; break;
      case 17: pos=27; break;
      case 14: pos=16; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,13,1,&ppkp);
      Select_dc(pos,16,1,&sensor);
      if (pos==27)
      {
        //Read status
        do
        {
          if (mode)
          {
            sensor++;
            if (sensor>=64)
            {
              sensor=0;
              c=0;
              mode=0;
              do
              {
                if (results[c]) Event2FIFO(ppkp,c,results[c],0);
                if (FIFO2EventLog()) Timer30sec=1;
                c++;
              }
              while(c<64);
              break;
            }
          }
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=sensor;
          SysPPKP_TXB[2]=9;
          SysPPKP_txsize=3;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        while (SysPPKP_rxsize==2);
        if (!Timer30sec) mode=0;
      }
      if (pos==34)
      {
        c=0;
        do
        {
          results[c]=0;
          c++;
        }
        while(c<64);
        pos=27;
        mode=1;
        sensor=0;
        SysPPKP_RXB[1]=0;
      }
      break;
    case kDW:
      Select_dc(pos,13,-1,&ppkp);
      Select_dc(pos,16,-1,&sensor);
      break;
    }
  }
  while (c!=kESC);
}

void Debug(void)
{
  // void DebugCAN(void);
  char c,i=0;
  TestPWD(40);
  do
  {
    ClrScr();
    sprintf_P(CON,Message_MainMenu[11]);
    sprintf_P(CON+40,Message_Debug[i]);
    sprintf_P(CON+66,(STR)"<Вправо>-войти");
    c=KBD(0);
    if (c>=0x11 && c<=0x16)
    {
      i=c-0x11;
      c=kRT;
    }
    switch(c)
    {
    case kRT:
      switch(i)
      {
      case 0: if (PWDmode==3) DebugPPKP(); break;
      case 1: ReadSensorStatus(); break;
      case 2: ReadSensorSetup(); break;
      case 3: ReadSensorCurrents(); break;
      case 4: if (PWDmode==3) ClearFlash();
      break;
      case 5: if (PWDmode==3) ClearLogFile();
      break;
      case 6: if (PWDmode==3) DebugMRV(); break;
      case 7: DebugFP(); //DebugCAN();

      } ClrScr(); break;
    case kUP: if (i>0) i--; break;
    case kDW: if (i<7) i++; break;
    }
  }
  while (c!=kLT);
}

