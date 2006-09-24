// Константы для TIMER1
// Таймер 3.2мс
#define Time3200 (unsigned int)(-4*3200/8)
// Таймер 2.2мс
#define Time2200 (unsigned int)(-4*2200/8)
// Таймер 1.6мс
#define Time1600 (unsigned int)(-4*1600/8)
//#define Time1400 (unsigned int)(-4*1400/8)
// Таймер 1.3мс
//#define Time1300 (unsigned int)(-4*1300/8)
// Таймер 1мс
#define Time1000 (unsigned int)(-4*1000/8)
// Таймер 20мс
#define Time20ms (unsigned int)(-10000)
//#define Time20ms (unsigned int)(-4*20000/8)
// Таймер 40мс
#define Time40ms (unsigned int)(-20000)

//----------------------------------------------------
// Обмен с ППКП или дубл. прибором
//----------------------------------------------------

#ifdef PUA1
// Разрешение передачи по каналу ДВП
#define EnaTX1 PORTE&=~0x04
//  Запрет передачи по каналу ДВП
#define DisTX1 PORTE|=0x04
// Разрешение приема по каналу ДВП
#define EnaRX1 PORTE&=~0x08
// Запрет приема по каналу ДВП
#define DisRX1 PORTE|=0x08
// Макросы включения/выключения передачи/приема в канале обмена с ППКП
#define EnaTX0 PORTE&=~0x10
#define DisTX0 PORTE|=0x10
#define EnaRX0 PORTE&=~0x20
#define DisRX0 PORTE|=0x20
#define EnaTX2 PORTE&=~0x40
#define DisTX2 PORTE|=0x40
#define EnaRX2 PORTE&=~0x80
#define DisRX2 PORTE|=0x80
#else
#define EnaTX1 PORTE&=~0x10
#define DisTX1 PORTE|=0x10
#define EnaRX1 PORTE&=~0x20
#define DisRX1 PORTE|=0x20
#define EnaTX0 PORTE&=0xBF
#define DisTX0 PORTE|=0x40
#define EnaRX0 PORTE&=0x7F
#define DisRX0 PORTE|=0x80
#define EnaTX2 PORTE&=~0x04
#define DisTX2 PORTE|=0x04
#define EnaRX2 PORTE&=~0x08
#define DisRX2 PORTE|=0x08
#endif

#define    CONTEXT_SWT_vect SPM_RDY_vect

//char IN_CSWT;
#define IN_CSWT ACSR_ACIS1

jmp_buf main_task;
jmp_buf rs_task;
char rs_rstack[8];
char rs_cstack[64];

char rxbuf[16];

#pragma vector=USART0_RXC_vect
__interrupt __raw void rs_rx(void)
{
  UCSR0B_RXCIE0=0;
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=USART0_UDRE_vect
__interrupt __raw void rs_tx(void)
{
  UCSR0B_UDRIE0=0;  //Запретили прерывания
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=USART0_TXC_vect
__interrupt __raw void rs_txc(void)
{
  //UCSR0A=1<<TXC0; //Сбросили флаг
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=TIMER1_OVF_vect
__interrupt __raw void rs_timeout(void)
{
  UCSR0B_RXCIE0=0; //Запретили прерывания от приемника
  if (!IN_CSWT)
  {
    IN_CSWT=1;
    __enable_interrupt();
    asm("call 0x88\n");
    __disable_interrupt();
    IN_CSWT=0;
  }
}

#pragma vector=CONTEXT_SWT_vect
__interrupt void CNTX_SWT(void)
{
  if (!setjmp(main_task)) //Запомнили контекст осн. задачи
  {
    longjmp(rs_task,1); //Перешли в контекст RS_TRX
  }
}

void wait_int()
{
  if (!setjmp(rs_task)) //Запомнили контекст RS_TASK
  {
    longjmp(main_task,1); //Перешли в контекст осн. задачи
  }
}

//txsize - количество байт без контрольной суммы
__x char DO_RS(char *out, char txsize, char n_rs, char CRC8RX)
{
  char CRC8;
  char c;
  UCSR0A=1<<TXC0; //Сбросили флаг от предыдущего прерывания
  CRC8=CRC8tab[UDR0=(*out++)]; //ПУ всегда начинает с нуля
  while(PINE_Bit1); //Ждем начала стартового бита
  if (n_rs) EnaTX2; else EnaTX0; //Разрешаем передатчик
  while(--txsize)
  {
    CRC8=CRC8tab[CRC8^(UDR0=(*out++))];
    UCSR0B_UDRIE0=1; //Разрешили прерывания от UDRIE
    wait_int(); //Ждем освобождения буфера
  }
  UDR0=CRC8;
  UCSR0B_TXCIE0=1; //Разрешили прерывание от TXC
  wait_int(); //Байт контрольной суммы - последний байт
  UCSR0B_TXCIE0=0; //Запретили прерывания от TXC
  DisTX2; //Выключили каналы передачи
  DisTX0;
  if (n_rs) EnaRX2; else EnaRX0; //Разрешили приемник (пока только на драйвере)
  TCNT1=Time1000;
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //Разрешили прерывание от таймера
  wait_int(); //Ждем 1 мс пока переходные процессы
  UCSR0B_RXEN0=1; //Разрешили приемник
  //txsize здесь равен 0
  CRC8RX=CRC8tab[CRC8RX]; //Нач. значение
  out=rxbuf;
  TCNT1=Time2200; //Время ожидания первого байта
  for(;;)
  {
    UCSR0B_RXCIE0=1; //Разрешили прерывания от приемника
    wait_int();
    if (!UCSR0A_RXC0)
    {
      //Выход по таймауту
      if (CRC8RX) txsize=0; //Не срослась контрольная сумма
      break; //Выходим из цикла
    }
    //Приняли байт
    if (UCSR0A_FE0||UCSR0A_DOR0)
    {
      txsize=0xFF; //Ошибки
    }
    c=UDR0;
    if (txsize!=sizeof(rxbuf))
    {
      CRC8RX=CRC8tab[CRC8RX^(*out=c)]; // Читаем принятый байт в буфер приема и перерасчет CRC
      txsize++;
      out++;
    }
    else
    {
      if (txsize==sizeof(rxbuf))
      {
        txsize=0xFF; //Слишком много байт
        break;
      }
    }
    TCNT1=Time1600;
  }
  UCSR0B_RXEN0=0; //Запретили приемник
  TIMSK_TOIE1=0; //Запретили таймер
  DisRX0;
  DisRX2;
  if (txsize==0xFF) txsize=0;
  return(txsize); //Количество принятых байт, если 0 - нет ответа
}

//Посыл байтов 0x55 для включения канала
void Do55(char n_rs, char c)
{
  char txsize=10;
  while(c)
  {
    __watchdog_reset();
    TCNT1=Time40ms;
    TIFR=1<<(TOV1);
    TIMSK_TOIE1=1; //Разрешили прерывание от таймера
    wait_int(); //Ждем 40 мс пока переходные процессы
    TIMSK_TOIE1=0; //Запретили таймер
    c--;
  }
  __watchdog_reset();
  UCSR0A=1<<TXC0; //Сбросили флаг от предыдущего прерывания
  UDR0=0x55;
  while(PINE_Bit1); //Ждем начала стартового бита
  if (n_rs) EnaTX2; else EnaTX0; //Разрешаем передатчик
  while(--txsize)
  {
    UDR0=0x55;
    UCSR0B_UDRIE0=1; //Разрешили прерывания от UDRIE
    wait_int(); //Ждем освобождения буфера
  }
  UDR0=0x55;
  UCSR0B_TXCIE0=1; //Разрешили прерывание от TXC
  wait_int(); //последний байт
  UCSR0B_TXCIE0=0; //Запретили прерывания от TXC
  DisTX2; //Выключили каналы передачи
  DisTX0;
  TCNT1=Time3200;
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //Разрешили прерывание от таймера
  wait_int(); //Ждем 3.2 мс пока переходные процессы
  TIMSK_TOIE1=0; //Запретили таймер
}

//Ожидание пакетов в режиме slave
char SLAVE_RS(char n_rs)
{
  char *out;
  char txsize=0;
  char CRC8RX=0;
  char c;

  if (n_rs)
  {
    DisRX0;
    EnaRX2;
  }
  else
  {
    DisRX2;
    EnaRX0; //Разрешили приемник (пока только на драйвере)
  }
  out=rxbuf;
  TCNT1=Time40ms; //Время ожидания первого байта
  TIFR=1<<(TOV1);
  TIMSK_TOIE1=1; //Разрешили прерывание от таймера
  UCSR0B_RXEN0=1; //Разрешили приемник
  for(;;)
  {
    UCSR0B_RXCIE0=1; //Разрешили прерывания от приемника
    wait_int();
    if (!UCSR0A_RXC0)
    {
      //Выход по таймауту
      if (!txsize) txsize=0xFE; //Нет ни одного байта!
      if (CRC8RX) txsize=0; //Не срослась контрольная сумма
      break; //Выходим из цикла
    }
    //Приняли байт
    if (UCSR0A_FE0||UCSR0A_DOR0) //Если ошибки
    {
      txsize=0xFF; //Ошибки
    }
    c=UDR0;
    if (txsize<sizeof(rxbuf))
    {
      CRC8RX=CRC8tab[CRC8RX^(*out=c)]; // Читаем принятый байт в буфер приема и перерасчет CRC
      txsize++;
      out++;
    }
    else
    {
      if (txsize==sizeof(rxbuf))
      {
        txsize=0xFF; //Слишком много байт
        break;
      }
    }
    TCNT1=Time1600;
  }
  TIMSK_TOIE1=0; //Запретили таймер
  if (txsize==0xFF) txsize=0;
  return(txsize);
}

__x void SLAVE_TX(char *out, char txsize, char n_rs)
{
  char CRC8;
  UCSR0A=1<<TXC0; //Сбросили флаг от предыдущего прерывания
  CRC8=CRC8tab[UDR0=(*out++)]; //ПУ всегда начинает с нуля
  while(PINE_Bit1); //Ждем начала стартового бита
  if (n_rs) EnaTX2; else EnaTX0; //Разрешаем передатчик
  while(--txsize)
  {
    CRC8=CRC8tab[CRC8^(UDR0=(*out++))];
    UCSR0B_UDRIE0=1; //Разрешили прерывания от UDRIE
    wait_int(); //Ждем освобождения буфера
  }
  UDR0=CRC8;
  UCSR0B_TXCIE0=1; //Разрешили прерывание от TXC
  wait_int(); //Байт контрольной суммы - последний байт
  UCSR0B_TXCIE0=0; //Запретили прерывания от TXC
  DisTX2; //Выключили каналы передачи
  DisTX0;
}

char isFreeEv(void)
{
  char free=(EventFIFOrp-EventFIFOwp)&0x3f; // Вычисляем количество свободного места в стеке событий
  if (free==8 || free==4)  // Если в стеке свободно менее 3 событий
  {
    return(0);
  }
  return(1);
}

void check5(void)
{
  //Для начала проверяем 5 кнопок
#ifdef PUA1
#else
  char tled,ckey;
#endif
  __disable_interrupt();
#ifdef PUA1
  if (!(PINF&0xF8)) for (;;); // Если нажаты все пять кнопок - вечный цикл, по сторожевому таймеру - сброс
#else
  tled=PORTA;
  PORTA=0xFF;
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  DDRA=0x00;
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  ckey=PINA;
  PORTA=tled;
  DDRA=0xFF;
  if (!(ckey&0x3E)) for (;;);
#endif
  __enable_interrupt();
}

__task __noreturn RS_TRX(void)
{
  struct
  {
    char PPKP;
    char Cmd;
  };

  static struct
  {
    unsigned long FF;
    unsigned long Second;
    unsigned char SEQ;
  }DPU_TPKT={0xFFFFFFFF,0,0};

  char CurRS=0;
  char abon;
  char c;
  char ca;
  char syserr;
  char sysrs;
//  char dblerr;

  unsigned int tstcount;

  abon=0;
//  dblerr=0;
  if (DOUBLE_PU)
  {
    char slave_seq;
    if (!IS_DOUBLER_DEFAULT) DBLS=40; else DBLS=80; //Если мастер по умолчанию, выходим в два раза быстрее
  L_RECONNECT:
    PPKP=0; //Является флагом передачи сброса
    slave_seq=0;
    do
    {
      check5();
      if ((RebootMode==0x4E)||(RebootMode==0x4F)) RebootMode=0; //Сброс завершен
      //Проверяем, нет ли на интерфейсе дубл. ПУ
      CurRS^=1; //Другой интерфейс
      do
      {
        __watchdog_reset();
        c=SLAVE_RS(CurRS);
        DBG1=c;
        if (c==10)
        {
          //наш пакет
          DBLS=80;
          ca=1; //Принято
          if (*((long *)rxbuf)!=-1)
          {
            if (slave_seq==rxbuf[8])
            {
              //SEQ совпал с ожидаемым
              if (isFreeEv())
              {
                //Есть место в стеке событий
                Event2FIFO(rxbuf[0],rxbuf[1],rxbuf[2],rxbuf[3]);
              }
              else
              {
                ca=2; //Не принято
              }
            }
          }
          else
          {
            //Тестовый пакет
            __disable_interrupt();
            TimeDate=*((unsigned long *)(rxbuf+4)); //Для синхронизации тек. времени
            __enable_interrupt();
          }
          if (slave_seq==rxbuf[8])
          {
            //Пред. подтверждение получено, значит можно удалить данные передачи
            PPKP=0;
          }
          slave_seq=rxbuf[8]^1; //Новый ожидаемый номер
          c=WarmBoot_cmd;
          if ((c==0x4E)||(c==0x4F))
          {
            //Надо послать сброс
            PPKP=c; //Данные передачи
            WarmBoot_cmd=0; //Сброс запроса передачи
          }
          UCSR0B_RXEN0=0; //Запретили приемник
          DisRX0;
          DisRX2;
          SLAVE_TX(&PPKP,ca,CurRS);
        }
      }
      while(c!=0xFE); //Пока принимаем пакеты (хоть какие нибудь)
    }
    while(DBLS);
  }
  ISMASTER=1; //Теперь режим - мастер
  KEY=kREDRAW; //Обновить ЖКИ
//  dblerr=0;
  if (DOUBLE_PU)
  {
    DBLE_RS0=32;
    DBLE_RS1=32;
  }
  else
  {
    DBLE_RS0=0;
    DBLE_RS1=0;
  }
  //Обмен, когда ПУ - мастер
  tstcount=0; //Счетчик для тестирования
  for(;;)
  {
    check5();
    if (CommunicateWithPC==1)
    {
      CommunicateWithPC=2;
      wait_int();
    }
    if (TSTS!=16) tstcount=0;
    __watchdog_reset();
    if (!ISMASTER)
    {
      DBLS=100;
      DBLE_RS0=0;
      DBLE_RS1=0;
      goto L_RECONNECT; //Если пытаются перейти обратно в slave
    }
    WorkSecond(); // Если есть флаг таймера 1с, вызываем обработчик
    __watchdog_reset();
    if (!abon) //Если абонент - 0
    {
      CurRS^=1; //Изменяем номер интрфейса
      Do55(CurRS,2);
    }
    if (SysPPKP_txsize)
    {
      //Надо послать сервисный пакет
      syserr=16;
      c=ABONS[SysPPKP_TXB[0]];
      if ((c&7)>((c>>3)&7)) sysrs=0;
      Do55(sysrs,2);
      do
      {
        if (syserr==8)
        {
          sysrs^=1;
          Do55(sysrs,2);
        }
        c=DO_RS(SysPPKP_TXB,SysPPKP_txsize,sysrs,SysPPKP_TXB[0]);
        __watchdog_reset();
        if (c) break;
      }
      while(--syserr);
      SysPPKP_txsize=0; //Закончили обмен
      if (!c)
      {
        SysPPKP_rxsize=0xFF; //Ошибка обмена
      }
      else
      {
        if (c<8) memcpy(SysPPKP_RXB,rxbuf,c); //Копируем принятые данные
        SysPPKP_rxsize=c;
      }
      Do55(CurRS,2); //Восстанавливаем тек. RS
    }
    if ((RebootMode==0xFF)||(!isFreeEv()))
    {
      //Не выполняем опрос (для сброса или когда нет места для сообщений)
      TCNT1=Time1000;
      TIFR=1<<(TOV1);
      TIMSK_TOIE1=1; //Разрешили прерывание от таймера
      wait_int(); //Ждем 1 мс пока переходные процессы
      TIMSK_TOIE1=0; //Запретили таймер
      abon=0;
      continue; //Повторить цикл с начала
    }
    if ((RebootMode==0x4E)||(RebootMode==0x4F))
    {
      //Посыл сброса в приборы
//      dblerr=0;
      Cmd=RebootMode;
      syserr=16;
      do
      {
        abon=0;
        ca=0; //Флаг, что есть неответившие
        CurRS^=1;
        Do55(CurRS,2);
        do
        {
          if ((ABONS[abon]&0xC0)==0x80)
          {
            //Этот абонент еще не ответил
            PPKP=abon;
            if (DO_RS(&PPKP,2,CurRS,abon)==2)
              ABONS[abon]|=0x40; //Прошел сброс
            else
              ca=1; //Есть неответившие
            __watchdog_reset();
          }
          abon++;
        }
        while(abon<128); //По всем абонентам
        if (ca==0) break; //Все ответили
      }
      while(--syserr);
      if (DOUBLE_PU)
      {
        DBLE_RS0=32;
        DBLE_RS1=32;
      }
      RebootMode=0; //Готов
      abon=0;
      continue; //C начала
    }
    ca=ABONS[abon];
    if (ca&0x80)
    {
      PPKP=abon|((ca&0x40)<<1); //Номер абонента и ACK
      c=DO_RS(&PPKP,1,CurRS,abon);
      ca=ABONS[abon];
      if (!(ca&0x80)) goto L1; //Уже выключили
      if ((rxbuf[0]&0x7F)!=abon) c=0; //Не тот абонент ответил, отказ!
      switch(c)
      {
      case 4:
        //Есть событие
        //Записываем событие в стек
        Event2FIFO(abon,rxbuf[1],rxbuf[2],0);
      case 2:
        //Нет события
        ca^=0x40; //Изменяем ACK
        if (CurRS)
        {
          //Сброс ошибки для RS1
          if ((ca&0x38)!=0x38) ca&=0xC7; //Если не 7, сброс
          if (INDEPENDED_RS)
          {
            //Если независимые интерфейсы, сброс ошибки и в другом RS
            if ((ca&0x07)!=0x07) ca&=0xF8; //Если не 7, сброс
          }
        }
        else
        {
          //Сброс ошибки для RS0
          if ((ca&0x07)!=0x07) ca&=0xF8; //Если не 7, сброс
          if (INDEPENDED_RS)
          {
            //Если независимые интерфейсы, сброс ошибки и в другом RS
            if ((ca&0x38)!=0x38) ca&=0xC7; //Если не 7, сброс
          }
        }
        if (c==4)
        {
          ABONS[abon]=ca;
          continue;
        }
        tstcount++; //ППКП ответил, увеличиваем счетчик тестов
        break;
      default:
        //Ошибка
        tstcount=0;
        if (CurRS)
        {
          //Ошибка в RS1
          if ((ca&0x38)!=0x38)
          {
            //Не заквитирована
            ca+=8;
            if ((ca&0x38)==0x38) Event2FIFO(abon,0xC0,0xFF,1);
          }
        }
        else
        {
          //Ошибка в RS0
          if ((ca&0x07)!=0x07)
          {
            //Не заквитирована
            ca+=1;
            if ((ca&0x07)==0x07) Event2FIFO(abon,0xC0,0xFF,0);
          }
        }
        if ((ca&0x3F)==0x3F)
        {
          //Полный отказ, выключаем из конфигурации
          ca=0;
        }
        break;
      }
      ABONS[abon]=ca;
    }
    else
    {
      //Абонент выключен, сбрасываем его
      PPKP=abon;
      Cmd=0x4F; //Холодный сброс
      DO_RS(&PPKP,2,CurRS,CRC8tab[abon]);
    }
    //Следующий абонент
  L1:
    abon++;
    //Проверяем, не надо ли передавать в дубл. прибор
    if (DOUBLE_PU&&(abon>=128))
    {
      if (DPUmess_ready)
      {
        //Передаем пакет
      L_FASTSEND:
        c=DO_RS((char *)(&DPU_MESS),9,CurRS,0); //Вместе с SEQ
      }
      else
      {
        //Передаем пакет с временем
        __disable_interrupt();
        DPU_TPKT.Second=TimeDate;
        __enable_interrupt();
        DPU_TPKT.SEQ=DPU_SEQ;
        c=DO_RS((char *)(&DPU_TPKT),9,CurRS,0); //Вместе с SEQ
      }
      if ((c==2)||(c==3))
      {
        //Успешная передача пакета
        if (c==2) DPUmess_ready=0; //Принято
        DPU_SEQ^=1; //Сл. номер пакета
        if (INDEPENDED_RS||(CurRS==0)) if (DBLE_RS0) DBLE_RS0=32;
        if (INDEPENDED_RS||(CurRS==1)) if (DBLE_RS1) DBLE_RS1=32;
        c=rxbuf[0];
        if ((c==0x4E)||(c==0x4F))
        {
          //Прислали сброс
          Event2FIFO(0xff,0xff,4,c); //Сообщение "Норма", вызывает сброс
        }
        //Пришел ответ, ждем ~40мс
        Do55(CurRS,0);
        Do55(CurRS,0);
        Do55(CurRS,0);
        Do55(CurRS,0);
        if (DPUmess_ready) goto L_FASTSEND;
      }
    }
    if (abon>=128)
    {
      abon=0;
      if (TSTS==16)
      {
#ifdef PUA1
        if ((char)tstcount>8)
#else
          if ((char)tstcount>0)
#endif
          {
            tstcount=(tstcount&0xFF00)+0x100;
          }
          else tstcount=0;
        if (tstcount>0x4000)
        {
          TSTS=17; //Следующий тест
        }
        TSTDBG=tstcount>>8;
        KEY=kREDRAW; //Перерисовка
      }
    }
  }
}

void InitRS(void)
{
  __disable_interrupt();
  DDRE=0xFE;
  PORTE=0xFF;
  UBRR0L=25;
  UCSR0B_TXEN0=1; //Разрешили передатчик
  TCCR1B=2;
  ((unsigned int *)rs_task)[10]=((unsigned int)rs_rstack)+7; //SP
  ((unsigned int *)rs_task)[8]=((unsigned int)rs_cstack)+64; //Y
  ((unsigned int *)rs_task)[9]=(unsigned int)RS_TRX; //Адрес перехода
  if (!setjmp(main_task)) longjmp(rs_task,1); //Переходим в RS_TRX
  __enable_interrupt();
}
