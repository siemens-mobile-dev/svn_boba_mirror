//---------------------------------------------------
//  Обмен с ПК для изменения конфигурации извещателей
//---------------------------------------------------

static char CRC8;

//Прием байта от ПК с контролем таймаута и перерасчетом CRC
char rxpc(void)
{
  char c;
  //Устанавливаем таймер
  _CLI();
  TIFR=4;
  TCNT1=Time3200;
  _SEI();
  while(!((USR&0x80)||(TIFR&4))); //Ждем или байт или таймаут
  c=UDR;
  CRC8=CRC8tab[CRC8^c]; //Перерасчитываем CRC
  return(c); //Возвращаем принятый байт
}

//Передача байта в ПК с перерасчетом СКС
void txpc(char c)
{
  while (!(USR&0x20)); //Ждем готовности
  CRC8=CRC8tab[CRC8^(UDR=c)]; //Передача
}

//Собственно обмен с ПК
void DoConfigPUpk(void)
{

  char __farflash *fp;
  char c;
  unsigned int page;
  char *p;

  EnaRX1; //Разрешаем прием
  ClrScr();
  sprintf_P(CON,(STR)"Обмен по RS2...");
  UpdateConsole();
  do
  {
    UCR=0x10; //Разрешаем приемник
    while(!((USR&0x80)||(KEY!=0))); //Первый байт принимается без таймаута
    c=UDR;
    CRC8=CRC8tab[c]; //Считаем CRC
    if (KEY) //Если нажали кнопку, готовимся к выходу
    {
    }
    else //Приняли первый байт
    {
      switch(c) //Выбор комманды
      {
      case 1: //Чтение сектора
        p=(char *)&page;
        *p++=rxpc(); //Младший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        *p=rxpc();   //Старший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        rxpc();     //CRC
        if (TIFR&4) break; //Если таймаут - выход
        if (CRC8) break; //Если не совпала CRC - выход
        //Включаем передачу
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //Ждем таймаута для правильного переключения
        EnaTX1;
        if (page<0xFE00)
        {
          //Читаем сектор из 45??021 в буфер 1
          WaitFlash;
          FlashOn;
          SPDR=0x53;
          WaitSPI;
          SPDR=page>>7;
          WaitSPI;
          SPDR=page<<1;
          WaitSPI;
          SPDR=0;
          WaitSPI;
          FlashOff;
          //Читаем из буфера 1
          WaitFlash;
          FlashOn;
          SPDR=FlashCMDbuf1read;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          SPDR=0x00;
          WaitSPI;
          PulseSCK;
          CRC8=0; //Начальное значение CRC
          c=0;
          //Передача сектора
          do
          {
            //Читаем очередной байт
            SPDR=0xFF;
            WaitSPI;
            txpc(SPDR); //Передаем в ПК
            c++;
          }
          while (c); //Цикл, пока не все 256 байт переданы
          FlashOff;
        }
        else
        {
#pragma diag_suppress=Pe1053
          fp=(char __farflash *) (((unsigned long)(page-0xFE00))<<8);
#pragma diag_default=Pe1053
          CRC8=0; //Начальное значение CRC
          c=0;
          //Передача сектора
          do
          {
            //Читаем очередной байт
            txpc(*fp++); //Передаем в ПК
            c++;
          }
          while (c); //Цикл, пока не все 256 байт переданы
        }
        txpc(CRC8); //Передаем CRC
        delay(2000);
        //Выключаем передачу
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      case 2: //Запись сектора
        p=(char *)&page;
        *p++=rxpc(); //Младший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        *p=rxpc(); //Старший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        //Запись в буфер 1
        WaitFlash;
        FlashOn;
        SPDR=0x84;
        WaitSPI;
        SPDR=0x00;
        WaitSPI;
        SPDR=0x00;
        WaitSPI;
        SPDR=0x00;
        WaitSPI;
        c=0;
        do
        {
          SPDR=rxpc(); //Принимаем очередной байт и записываем его в буфер 1
          if (TIFR&4) break; //Если таймаут - выход
          c++;
          WaitSPI; //Ожидаем окончания записи
        }
        while (c); //Цикл, пока не все 256 байт приняты
        FlashOff;
        if (TIFR&4) break; //Если вышли по таймауту - не записываем сектор
        rxpc(); //Приняли CRC
        if (TIFR&4) break; //Если таймаут - выход
        if (CRC8) break; //Если не совпала CRC - выход
        //Разрешаем передачу
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //Ждем таймаута для правильного переключения
        EnaTX1;
        AddFlashSectorCRC(); //Добавляем контр. сумму
        WaitFlash;
        //Запись сектора из буфера 1 в 45??021
        FlashOn;
        SPDR=0x83;
        WaitSPI;
        SPDR=page>>7;
        WaitSPI;
        SPDR=page<<1;
        WaitSPI;
        SPDR=0;
        WaitSPI;
        FlashOff;
        WaitFlash;
        //Запись произведена, передаем подтверждение
        CRC8=0;
        txpc(0);
        txpc(CRC8);
        delay(2000);
        //Запрещаем передачу
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      case 3: //Чтение байта из EEPROM
        p=(char *)&page;
        *p++=rxpc(); //Младший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        *p=rxpc();   //Старший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        rxpc();     //CRC
        if (TIFR&4) break; //Если таймаут - выход
        if (CRC8) break; //Если не совпала CRC - выход
        //Включаем передачу
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //Ждем таймаута для правильного переключения
        EnaTX1;
        CRC8=0; //Начальное значение CRC
        txpc(*((__eeprom char *)page)); //Передаем в ПК
        txpc(CRC8); //Передаем CRC
        delay(2000);
        //Выключаем передачу
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      case 4: //Запись байта из EEPROM
        p=(char *)&page;
        *p++=rxpc(); //Младший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        *p=rxpc();   //Старший байт номера сектора
        if (TIFR&4) break; //Если таймаут - выход
        c=rxpc();
        if (TIFR&4) break; //Если вышли по таймауту - не записываем байт
        rxpc(); //Приняли CRC
        if (TIFR&4) break; //Если таймаут - выход
        if (CRC8) break; //Если не совпала CRC - выход
        //Разрешаем передачу
        UCR=0x08;
        DisRX1;
        while(TIFR&4); //Ждем таймаута для правильного переключения
        EnaTX1;
        *((__eeprom char *)page)=c;
        CRC8=0;
        txpc(0);
        txpc(CRC8);
        delay(2000);
        //Запрещаем передачу
        UCR=0;
        DisTX1;
        EnaRX1;
        break;
      }
    }
  }
  while (KEY==0); //Цикл, пока не нажата любая клавиша
  KEY=0;
  //Запрещаем прием
  UCR=0;
  DisRX1;
  UCSR0B_TXEN0=1; //Разрешили передатчик
}

//Запуск задачи обмена с ПК
void ConfigPUpk(void)
{
  void ConstructVMPC(void);
  CommunicateWithPC=1; //Устанавливаем флаг необходимости обмена с ПК
  // delay(10000);
  while (CommunicateWithPC!=2); //Ожидаем флага освобождения
  //Запрещаем сторожевой таймер
  _CLI();
  __watchdog_reset();
  WDTCR=0x18;
  WDTCR=0x10;
  __watchdog_reset();
  _SEI();
  DoConfigPUpk(); //Собственно связь с ПК
  //Разрешаем сторожевой таймер
  _CLI();
  __watchdog_reset();
  WDTCR=0x08;
  WDTCR=0x18;
  WDTCR=0x0B; //Разрешаем сторожевой таймер
  __watchdog_reset();
  TCNT1=Time1000;
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //Разрешили прерывание от таймера
  CommunicateWithPC=0;
  _SEI();
  if (TSTS==17)
  {
    unsigned long crcflash=0;
    unsigned int sector=0;
    //Заводское тестирование - стчитаем контр. сумму всех секторов
    ClrScr();
    sprintf_P(CON,(PGM_P)"Подсчет CRC...");
    UpdateConsole();
    do
    {
      crcflash+=FlashSectorCRC(sector,256); //Без занесенной контрольной суммы
      sector++;
    }
    while(sector<1024);
    //                   012345678901234567890123
    if (crcflash==0x22de66e)
    {
      TSTS=18;
      TEST_STAT=18;
    }
  }
  ConstructVMPC();
  Event2FIFO(0xff,0xff,4,0x4F);
}

