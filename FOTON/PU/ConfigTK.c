void ConfigTKtable(void)
{
  static __flash char mess1[]="Событие: %02d Пульт: %02X <Вверх> - Запись";
  //9 - event
  //19 - send
  //22 - save

  //Событие: 00 Пульт: 00 Зп
  //012345678901234567890123
  char event=0;
  char send;
  static char pos=9;
  char readflag=1;
  char c;
  do
  {
    ClrScr();
    c=0xFF;
    do
    {
      c++;
    } while (EventsTab[c].mask);
    if (event>c) event=c;
    if (readflag) send=TKtable[event];
    readflag=0;
    sprintf_P(CON,mess1,event,send);
    c=0;
    sprintf_P(CON+24,EventsTab[event].message,
              c,c,c,c
                );
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,9,&event)) {c=kRT ;readflag=1; };
      if (Edit_hb(c,pos,19,&send)) c=kRT;
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 9:  c=kESC; break;
      case 19: pos=10; break;
      case 22: pos=20; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 22: break;
      case 20: pos=22; break;
      case 10: pos=19; break;
      default: pos++;
      }
      break;
    case kUP:
      if (pos==22&&PWDmode==3)
      {
        TKtable[event]=send; readflag=1;
      }
      if (pos==9 || pos==10) readflag=1;
      Select_dc(pos,9,1,&event);
      Select_hb(pos,19,1,&send);
      break;
    case kDW:
      if (pos==9 || pos==10) readflag=1;
      Select_dc(pos,9,-1,&event);
      Select_hb(pos,19,-1,&send);
      break;
    }
  }
  while (c!=kESC);
}

void ConfigTK(void)
{                             //0123456789012345  67890123
  /* static __flash char mess1[]="\200Конф. TK Adr: %02X  Запись\230%02X %.8s %03d %c   Коды";
  //6,7 - ppkp
  //14,15 - adr
  //18 - write
  //24,25 - val hex
  //27-34 - val bin
  //36-38 - val dec
  //40 - val char
  //44 - codes

  //ППКП: xx Adr: xx  Write?
  //012345678901234567890123
  //xx xxxxxxxx xxx x
  //456789012345678901234567
  static char adr=0;
  char val;
  char st[8];
  char c;
  static char pos=14;
  char i;
  char readflag;
  readflag=1;
  TestPWD(40);
  do
  {
  if (readflag)
  {
  for (i=0;i<16;i++)
  {
  val=ReadI2C(0xF2,adr);
  if (!I2Cerror) break;
}
  if (I2Cerror)
  {
  Event2FIFO(0xFF,0xFF,8,0);
}
  readflag=0;
}
  c=0;
  do
  {
  st[c]=(val&(0x80>>c))?'1':'0';
  c++;
}
  while (c<8);
  printf_P(mess1,adr,val,st,val,((val>=32 && val<128)||(val>=192))?val:'.');
  putchar(pos+128);
  CursorON();
  c=KBD(0);
  CursorOFF();
  if ((c>=32 && c<128)||(c>=192))
  {
  if (Edit_hb(c,pos,14,&adr)) {readflag=1; c=kRT;}
  if (Edit_hb(c,pos,24,&val)) c=kRT;
  if (Edit_bb(c,pos,27,&val)) c=kRT;
  if (Edit_db(c,pos,36,&val)) c=kRT;
  if (pos==40) val=c;
}
  switch(c)
  {
case kLT:
  switch (pos)
  {
case 14: readflag=1; break;
case 18: pos=15; break;
case 24: pos=18; break;
case 27: pos=25; break;
case 36: pos=34; break;
case 40: pos=38; break;
case 44: pos=38; break;
  default: pos--;
}
  break;
case kRT:
  switch (pos)
  {
case 44: break;
case 40: pos=44; break;
case 38: pos=40; break;
case 34: pos=36; break;
case 25: pos=27; break;
case 18: pos=24; break;
case 15: pos=18; break;
  default: pos++;
}
  break;
case kSEL:
case kUP:
  if (pos==18&&PWDmode==3)
  {
  for (i=0;i<16;i++)
  {
  WriteI2C(0xF2,adr,val);
  if (!I2Cerror) break;
}
  if (I2Cerror)
  {
  Event2FIFO(0xFF,0xFF,8,0);
}
  readflag=1;
}
  if (pos==14||pos==15) readflag=1;
  Select_hb(pos,14,1,&adr);
  Select_hb(pos,24,1,&val);
  Select_bb(pos,27,1,&val);
  Select_db(pos,36,1,&val);
  if (pos==40) val++;
  if (pos==44) ConfigTKtable();
  break;
case kOK:
case kDW:
  if (pos==14||pos==15) readflag=1;
  Select_hb(pos,14,-1,&adr);
  Select_hb(pos,24,-1,&val);
  Select_bb(pos,27,-1,&val);
  Select_db(pos,36,-1,&val);
  if (pos==40) val--;
  if (pos==44) ConfigTKtable();
  break;
}
}
  while (c!=kESC);
  */}

