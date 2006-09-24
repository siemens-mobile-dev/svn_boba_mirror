//--------------------------------------
// Перезапуск системы
//--------------------------------------

void DoBoot(char cmd)
{
  char i;
  unsigned int zp;
  unsigned int zone;
  static __flash char mess[]="Сброс системы: ...";
  //static __flash char mess2[]="DBG1: %02X";

  RebootMode=0xFF; //Запрет обмена с БР-А

  ClrScr();
  sprintf_P(CON,mess);
  UpdateConsole();
  i=0;
  do
  {
    VMPC[i]=0;
    MacroTimers[i]=0;
    i++;
  }
  while(i<16);
  ConstructVMPC();
  i=0;
  do
  {
    BitFlags[i]=0;
    i++;
  }
  while(i<32);
  ConstructABONS(); //Прочитать данные по БР-А в ОЗУ со сбросом флагов отказа обмена с БР-А
  TotalFires=0; //Общее количество пожаров = 0
  SndMode=0; //Выключить звук
  IsPreAtt=0;
  LED=0; //Выключить светодиоды
  LEDflash=0; //Выключить мигание
  IsSuprDZ=0; //Нет зон в дист. режиме
  FlagsCAN=0;
  //Очистили таймеры и количество сработок для всех зон
  i=0;
  do
  {
    _CLI();
    ZoneTF[i<<1]=0; //Очистили таймер
    ZoneTF[(i<<1)+1]=0; //Очистили количество сработок
    _SEI();
    i++; //Следующая зона
  }
  while (i!=0); //Цикл, пока не перебрали все зоны
  _CLI();
#ifdef PUA1
  PORTB&=~0x10; //Выключили реле "Пожар"
  PORTB&=~0x40; //Выключили реле "Пожар 120с"
#else
  PORTD&=0xCF;
#endif
  if (!(PU_STAT&4))
  {
#ifdef PUA1
        PORTB|=0x40; //Выключаем реле "Отказ"
#else
        PORTD|=0x10;
#endif
  }
  Fire120sec=0xFFFFFFFF; //Сбросили таймер 120с
  _SEI();
  mrv1relay=0;
  mrv2relay=0;
  for (i=0;i<16;i++)
  {
    WriteI2C(0x92,1,0);
    WriteI2C(0x94,1,0);
  }
  TADEVoff=0; //Сбросили флаги отказа обмена с МРВ и ТК
  // Подготовка массива EPUS по данным зон
  zone=0;
  do
  {
    zp=ZONEPOINT_AREA[zone++];
    if ((zp&0xC000)==0x8000)
    {
      EPUS[(zp>>8)&0x3F]=0x80;
    }
  }
  while (zone<(256*4));
  zone=0;
  do
  {
    ZoneDelays[zone]=0xFE;
    zone++;
  }
  while (zone<1024);
  //--------------------------------
  //Передать всем БР-А комманду рестарта
  RebootMode=cmd;
  while(RebootMode) //Ждем окончания сброса
  {
    //   ClrScr();
    //   sprintf_P(CON,mess2,DBG1);
    //   UpdateConsole();
  }
}

//Собственно перезапуск
//На входе получает комманду, которая передается в БР-А
void WarmBoot(char cmd)
{
  ClrScr();
  sprintf_P(CON,(STR)"Введите код для сброса системы!");
  if (XTestPWD(40)<2) return; //Неверный код
  WarmBoot_cmd=cmd;
  if (ISMASTER) Event2FIFO(0xff,0xff,4,cmd); //Сообщение "Норма"
}

//Полная проверка системы
void FullCheck(void)
{
  unsigned long td;
  char s1;
  char s2;
  char s3;
  _CLI();
  td=TimeDate+2; //Таймер 2 сек
  s1=LEDflash;
  s2=LED;
  s3=SndMode;
  for(;;)
  {
    _SEI();
    SndMode=1; //Включили звук
    SetLEDon(LEDfire|LEDmfire|LEDfail|LEDoff|LEDsndoff|LEDsysf); //Включили все светодиоды
    _CLI();
    if (td<TimeDate) break; //Выход, если прошло 2с
  }
  LEDflash=s1;
  LED=s2;
  SndMode=s3;
  _SEI();
  //WarmBoot(0x4E); //Теплый перезапуск
}

//Подменю "Перезапуск системы"
void Boot(void)
{
  char c,i=0;
  do
  {
    //Печатаем меню
    ClrScr();
    sprintf_P(CON,Message_MainMenu[2]);
    sprintf_P(CON+40,Message_Boot[i]);
    sprintf_P(CON+66,(STR)"<Вправо>-войти");
    c=KBD(0); //Ждем клавиши
    if (c>=0x11 && c<=0x14) //Оббработка функциональных клавиш
    {
      i=c-0x11;
      c=kRT;
    }
    switch(c)
    {
    case kRT:
      switch(i)
      {
      case 0: WarmBoot(0x4E); break; //Перезапуск с коммандой "Горячий перезапуск БР-А"
      case 1: WarmBoot(0x4F); break; //Перезапуск с коммандой "Холодный перезапуск БР-А"
      case 2: FullCheck(); break;
      case 3: if (DOUBLE_PU) ISMASTER=0; break;
      } ClrScr(); break;
    case kUP: if (i>0) i--; break;
    case kDW: if (i<3) i++; break;
    }
  } while (c!=kLT); //Цикл, пока не выход
}
