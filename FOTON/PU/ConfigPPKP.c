//--------------------------
// Подпрограммы пункта меню "Конфигурация БР-А"
//--------------------------

// Процедура подсчета общего количества извещателей
// и количества включенных извещателей для необходимого БР-А.
// Данная процедура используется для режима поиска
// отключенных устройств (в модуле main) и выдачи информации
// о количестве извещателей в шлейфе в режиме "Сбор данных по наличию извещателей"
// (в модуле ConfigPU.c)
// На входе получает номер БР-А и две ссылки на переменные
// tot_sens - общее количество извещателей
// on_sens - количество включенных извещателей
void SearchOnSensors(char ppkp, char *tot_sens, char *on_sens)
{
  char i;
  char c;
  int page;
  page=(ppkp<<4); //Вычисляем адрес в 45??021
  *tot_sens=*on_sens=0; //Начальные значения счетчиков
  for (i=0;i<64;i++) //Цикл по всем извещателям
  {
    if (!(i&7)) //Если начало сектора
    {
      WaitFlash; //Читаем необходимый сектор
      FlashOn;
      SPDR=0x53;
      WaitSPI;
      SPDR=page>>8;
      WaitSPI;
      SPDR=page;
      WaitSPI;
      SPDR=0;
      WaitSPI;
      FlashOff;
      WaitFlash;
      page+=2; //Следующий сектор
    }
    FlashOn; //Читаем данные из буфера
    SPDR=FlashCMDbuf1read;
    WaitSPI;
    SPDR=0x00;
    WaitSPI;
    SPDR=0x00;
    WaitSPI;
    SPDR=(i<<5)+25; //Берем сразу байт, ответственный за тип извещателя
    WaitSPI;
    SPDR=0x00;
    WaitSPI;
    PulseSCK;
    SPDR=0xff;
    WaitSPI;
    c=SPDR;
    FlashOff;
    if (c&0x1f) //Если тип не "Нет"
    {
      (*tot_sens)++; //Увеличиваем общее количество
      if (c&0x80) (*on_sens)++; //Если извещатель включен, увеличиваем количество включенных
    }
  }
}

//
// Конфигурация данных в EEPROM БР-А
//
void ConfigPPKP_EEPROM(char ppkp)
{
  static __flash char mess1[]="Конфигурация БР-А %02d   <Вправо>-изменить";
  struct TPPKPEEPROM_Tab __flash *tpt=&PPKPEEPROM_Tab[1];
  char c;
  char val;

  do
  {
    ClrScr();
    sprintf_P(CON,mess1,ppkp);
    _CLI();
    SysPPKP_rxsize=0;
    SysPPKP_TXB[0]=ppkp;
    SysPPKP_TXB[1]=0x49;
    SysPPKP_TXB[2]=tpt->adr;
    SysPPKP_txsize=3;
    _SEI();
    while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
    if (SysPPKP_rxsize==0xFF) return;
    val=SysPPKP_RXB[1];
    sprintf_P(CON+40,val&tpt->mask?EventPEText(mess1):EventPEText(mess0));
    c=KBD(0); //Ждем клавиши
    switch(c)
    {
    case kUP:
      if (tpt[-1].adr!=0) tpt--;
      break;
    case kDW:
      if (tpt[1].adr!=0) tpt++;
      break;
    case kRT:
      val^=tpt->mask;
      _CLI();
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=ppkp;
      SysPPKP_TXB[1]=0x48;
      SysPPKP_TXB[2]=tpt->adr;
      SysPPKP_TXB[3]=val;
      SysPPKP_txsize=4;
      _SEI();
      while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
      if (SysPPKP_rxsize==2)
      {
        //FC, Sensor=C0+pnt, Event2 - val & mask
        Event2FIFO(ppkp,0xC0+(tpt-PPKPEEPROM_Tab),0xFC,val&tpt->mask);
      }
      break;
    }
  } while (c!=kLT); //Цикл, пока не выход
}

// Собственно пункт основного меню "Конфигурация БР-А"
//
void ConfigPPKP(void)
{                                     //012345678901234567890123
  static __flash char Message_EPPKP[]="БР-А: %02d                 Интерфейс RS%01d";
  static __flash char messOFF[]=               "Выключен";
  static __flash char  messON[]=               "Включен %01X.%01X";
  static __flash char  messON1[]=              "Включен";

  static __flash char mess3[]="Извещателей: %02d из %02d";
  static char ppkp;
  static char pos=6;
  char c,tot_sens,on_sens;
  TestPWD(40); //Проверяем пароль
  do
  {
    ClrScr();
    ppkp&=63;
    SearchOnSensors(ppkp,&tot_sens,&on_sens); //Для печати сообщения о количестве извещателей
    sprintf_P(CON,Message_EPPKP,ppkp,PPKP_AREA[ppkp]&0x20?1:0); //Печатаем сообщение
    if (ABONS[ppkp]&0x80) //Если БР-А включен, получаем номер версии ПО
    {
      _CLI();
      //NeedNoError=1;
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=ppkp;
      SysPPKP_TXB[1]=0x49;
      SysPPKP_TXB[2]=0;
      SysPPKP_txsize=3;
      _SEI();
      while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
    }
    else SysPPKP_rxsize=0xFF;
    //Печатаем остальное
    if (SysPPKP_rxsize!=0xFF)
    {
      sprintf_P(CON+9,(PPKP_AREA[ppkp]&0x80)?messON:messOFF,SysPPKP_RXB[1]>>4,SysPPKP_RXB[1]&0x0F);
    }
    else
    {
      sprintf_P(CON+9,(PPKP_AREA[ppkp]&0x80)?messON1:messOFF);
    }
    sprintf_P(CON+40,mess3,on_sens,tot_sens);
    CursorPos=pos; //Перемещаем курсор
    c=KBD(0); //Ждем клавиши
    if ((c>=32 && c<128)||(c>=192)) //Если символьная клавиша
    {
      if (Edit_dc(c,pos,6,&ppkp)) { c=kRT;} //Пытаемся редактировать номер БР-А
    }
    switch(c)
    {
      //Пытаемся перемещать курсор влево
    case kLT:
      switch (pos)
      {
      case 6: c=kESC; break;
      case 9: pos=7; break;
      case 25: pos=9; break;
      default: pos--;
      }
      break;
      // Пытаемся перемещать курсор вправо
    case kRT:
      switch (pos)
      {
      case 25: if ((PWDmode==3)&&(SysPPKP_rxsize!=0xFF)) ConfigPPKP_EEPROM(ppkp); break;
      case 9: pos=25; break;
      case 7: pos=9; break;
      default: pos++;
      }
      break;
    case kUP:
      if (pos==9&&PWDmode>=2) //Если в нужной позиции и нужный уровень, включаем БР-А
      {
        PPKP_AREA[ppkp]|=0x80;
        ABONS[ppkp]=0x80; //Включаем
        Event2FIFO(ppkp,0xC0,0xFE,PPKP_AREA[ppkp]);
      }
      if (pos==25&&PWDmode==3)//Если в нужной позиции и нужный уровень, изменяем номер интерфейса
      {
        PPKP_AREA[ppkp]|=0x20;
      }
      Select_dc(pos,6,1,&ppkp); //Пытаемся перебирать номер БР-А
      break;
    case kDW:
      if (pos==9&&PWDmode>=2) //Если в нужной позиции и нужный уровень, выключаем БР-А
      {
        PPKP_AREA[ppkp]&=0x7F;
        ABONS[ppkp]=0; //Выключаем
        Event2FIFO(ppkp,0xC0,0xFD,PPKP_AREA[ppkp]);
      }
      if (pos==25&&PWDmode==3)//Если в нужной позиции и нужный уровень, изменяем номер интерфейса
      {
        PPKP_AREA[ppkp]&=~0x20;
      }
      Select_dc(pos,6,-1,&ppkp);//Пытаемся перебирать номер БР-А
      break;
    }
  } while (c!=kESC); //Цикл, пока не выход
}

