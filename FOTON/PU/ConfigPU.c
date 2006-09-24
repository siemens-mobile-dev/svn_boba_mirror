//--------------------------------------
// Конфигурация ПУ
//--------------------------------------

//Конфигурация даты и времени
void ConfigPUtimedate(void)
{
  char h,m,s,dd,mm,yy; //часы, минуты, секунды, день, месяц, год
  char c;
  char pos=0;
  unsigned long t;
  _CLI();
  t=TimeDate; //взяли текущее время
  _SEI();
  do
  {
    memset(CON,' ',80);
    PrintTimeDate(CON,t); //Напечатали время
    sprintf_P(CON+40,Message_ConfigPU[0]);
    //Из буфера экрана значения собрали в переменные, содержащии часы, минуты, секунды, день, месяц, год
    h=((CON[0]-'0')*10)+CON[1]-'0';
    m=((CON[3]-'0')*10)+CON[4]-'0';
    s=((CON[6]-'0')*10)+CON[7]-'0';
    dd=((CON[9]-'0')*10)+CON[10]-'0';
    mm=((CON[12]-'0')*10)+CON[13]-'0';
    yy=((CON[15]-'0')*10)+CON[16]-'0';
    CursorPos=pos; //Вывели курсор
    c=KBD(0); //Ждем клавиши
    //Пытаемся редактировать
    if (Edit_dc(c,pos,0,&h)) c=kRT;
    if (Edit_dc(c,pos,3,&m)) c=kRT;
    if (Edit_dc(c,pos,6,&s)) c=kRT;
    if (Edit_dc(c,pos,9,&dd)) c=kRT;
    if (Edit_dc(c,pos,12,&mm)) c=kRT;
    if (Edit_dc(c,pos,15,&yy)) c=kRT;
    switch(c)
    {
      //Пытаемся перемещать курсор
    case kLT: if (pos==3||pos==6||pos==9||pos==12||pos==15) pos--;
    if (pos==0)
    {
      c=kESC;
      break;
    }
    if (pos>0) pos--;
    break;
    case kRT: if (pos==1||pos==4||pos==7||pos==10||pos==13) pos++;
    if (pos<16) pos++;
    break;
    //Пытаемся перебирать параметры
    case kUP:
      Select_dc(pos,0,1,&h);
      Select_dc(pos,3,1,&m);
      Select_dc(pos,6,1,&s);
      Select_dc(pos,9,1,&dd);
      Select_dc(pos,12,1,&mm);
      Select_dc(pos,15,1,&yy);
      break;
    case kDW:
      Select_dc(pos,0,-1,&h);
      Select_dc(pos,3,-1,&m);
      Select_dc(pos,6,-1,&s);
      Select_dc(pos,9,-1,&dd);
      Select_dc(pos,12,-1,&mm);
      Select_dc(pos,15,-1,&yy);
      break;
    }
    //Преобразуем часы, минуты, секунды, день, месяц, год в количество секунд
    t=TimeDate2Long(h,m,s,dd,mm,yy);
  }
  while (c!=kESC); //Цикл, пока не выход
  _CLI();
  TimeDate=t; //Установили новое значение времени
  _SEI();
}

//------------------------
//Конфигурация с ПК
//------------------------
#include "pupc.c"

//Настройка паролей
void ConfigPassword(void)
{
  static __flash char mess1[]="Код доступа У%01d: %04X%04X";
  // static flash char mess2[]="Код доступа У2: XXXXXXXX";
  char c;
  int pass[4];

  char pos=16;

  pass[0]=PWDlevel2[0];
  pass[1]=PWDlevel2[1];
  pass[2]=PWDlevel3[0];
  pass[3]=PWDlevel3[1];
  do
  {
    ClrScr();
    sprintf_P(CON,mess1,2,pass[0],pass[1]); //Печать
    if (PWDmode==3)
    {
      sprintf_P(CON+40,mess1,3,pass[2],pass[3]); //Печать
    }
    CursorPos=pos; //Установка курсора
    c=KBD(0); //Ожидание клавиши
    if ((c>=32 && c<128)||(c>=192)) //Символьная клавиша
    {
      if (Edit_hi(c,pos,16,&pass[0])) c=kRT; //Пытаемся редактировать
      if (Edit_hi(c,pos,20,&pass[1])) c=kRT;
      if (Edit_hi(c,pos,16+40,&pass[2])) c=kRT; //Пытаемся редактировать
      if (Edit_hi(c,pos,20+40,&pass[3])) c=kRT;
    }
    switch(c)
    {
    case kLT: //Стрелка влево
      switch (pos)
      {
      case 16: c=kESC; break; //Левее некуда
      case 16+40: pos=23; break;
      default: pos--; //Курсор влево
      }
      break;
    case kRT: //Стрелка вправо
      switch (pos)
      {
      case 23+40: break; //Правее некуда
      case 23: if (PWDmode==3) pos=16+40; break;
      default: pos++; //Курсор вправо
      }
      break;
    case kUP: //Увеличение
      Select_hi(pos,16,1,&pass[0]); //Пытаемся увеличивать
      Select_hi(pos,20,1,&pass[1]);
      Select_hi(pos,16+40,1,&pass[2]); //Пытаемся увеличивать
      Select_hi(pos,20+40,1,&pass[3]);
      break;
    case kDW: //Уменьшение
      Select_hi(pos,16,-1,&pass[0]); //Пытаемся уменьшать
      Select_hi(pos,20,-1,&pass[1]);
      Select_hi(pos,16+40,-1,&pass[2]); //Пытаемся уменьшать
      Select_hi(pos,20+40,-1,&pass[3]);
      break;
    }
  }
  while (c!=kESC); //Пока не выход
  PWDlevel2[0]=pass[0];
  PWDlevel2[1]=pass[1];
  PWDlevel3[0]=pass[2];
  PWDlevel3[1]=pass[3];
}

//Печать всей конфигурации
void PrintSetup(void)
{
  static __flash char LPT_AllConfig1[]="Конфигураци\377 ПУ-А номер %d на ";
  static __flash char LPT_AllConfig2[]="БР-А номер %d\n";
  static __flash char LPT_AllConfig3[]="Извещатель Географическое положение Состо\377ние Тип   Зона 1-4 м    т\n";
  //                          "xx         xxxxxxxxxxxxxxxxxxxxxxxx x         xxxxx xx   xxxx xxxx xxxx\n";
  static __flash char LPT_AllConfig4[]="%02d         %.24s %c         %.5s %02d %02d %02d %04X %02X\n";
  static __flash char LPT_AllConfig5[]="Всего извещателей в луче %d, включено %d\n";
  static __flash char LPT_AllConfig6[]="Всего извещателей в системе %d, включено %d\n";
  char ppkp,sensor;
  char st[5];
  struct TSensorConfig sc;
  char ppkp_tots,ppkp_ons;
  int sys_tots,sys_ons;
  unsigned long td;

  sys_tots=sys_ons=0; //Обнуление счетчиков извещателей по системе
  //Печатаем шапку
  // PrinterOn;
  PrintSeparator();
  printf_P(LPT_AllConfig1,PU_ID);
  _CLI();
  td=TimeDate;
  _SEI();
  LPT_PrintTimeDate(td);
  PRINT_CRLF;
  PrintSeparator();
  //Цикл по всем ППКП
  for (ppkp=0;ppkp<64;ppkp++)
  {
    if (PPKP_AREA[ppkp]&0x80) //Если ППКП включен
    {
      ppkp_tots=ppkp_ons=0; //Обнуление счетчиков извещателей по ППКП
      printf_P(LPT_AllConfig2,ppkp); //Печатаем шапку по ППКП
      printf_P(LPT_AllConfig3);
      //Цикл по всем извещателям
      for (sensor=0;sensor<64;sensor++)
      {
        ReadSensorConfig(ppkp,sensor,&sc); //Читаем конфигурацию извещателей
        if (sc.Type&0x1f) //Если тип извещателя не "Нет"
        {
          //Печатаем конфигурацию
          memcpy_P(st,NameSensorType[sc.Type&127],5);
          printf_P(LPT_AllConfig4,
                   sensor,
                   sc.GeoPos,
                   sc.Type>127?'+':'-',
                   st,
                   sc.Zone1,
                   sc.Zone2,
                   sc.Zone3,
                   sc.Zone4,
                   sc.Led,
                   sc.TCode
                     );
          ppkp_tots++;sys_tots++; //Увеличиваем счетчик всего по системе и ППКП
          if (sc.Type&0x80) //Если извещатель включен
          {ppkp_ons++;sys_ons++;} //Увеличиваем счетчик включенных по системе и ППКП
        }
      }
      //Печатаем подвал по ППКП
      printf_P(LPT_AllConfig5,ppkp_tots,ppkp_ons);
      PrintSeparator();
    }
  }
  //Печатаем подвал по системе
  printf_P(LPT_AllConfig6,sys_tots,sys_ons);
  // PrinterOff;
}

//Настройка вывода текста
void ConfigTextPrint(void)
{
  //                            012345678901234567890123
  static __flash char mess0[]="Большие/маленькие буквы <Вверх>-изменить";
  static __flash char mess1[]="Только большие буквы    <Вверх>-изменить";
  static __flash char mess2[]="Англ./русские буквы     <Вверх>-изменить";
  static __flash char mess3[]="Только русские буквы    <Вверх>-изменить";
  char c;
  char pos=0;
  do
  {
    ClrScr();
    //Читаем конфигурацию ПУ
    c=PU_STAT;
    //Печатаем текущую конфигурацию
    sprintf_P(CON,c&1?mess1:mess0);
    sprintf_P(CON+40,c&2?mess3:mess2);
    //Устанавливаем курсор
    CursorPos=pos;
    c=KBD(0); //Ждем клавишу
    switch(c)
    {
      //Пытаемся перемещать курсор
    case kLT:
      if (pos==0)
      {
        c=kESC;
      }
      if (pos==40) pos=0;
      break;
    case kRT:
      if (pos==0) pos=40;
      break;
      //Пытаемся перебирать параметры
    case kUP:
      switch (pos)
      {
      case 0:
        PU_STAT|=1;
        break;
      case 40:
        PU_STAT|=2;
        break;
      }
      break;
    case kDW:
      switch (pos)
      {
      case 0:
        PU_STAT&=0xFE;
        break;
      case 40:
        PU_STAT&=0xFD;
        break;
      }
      break;
    }
  }
  while (c!=kESC); //Цикл, пока не выход
}

//Сбор данных по наличию извещателей
void SearchSensors(void)
{
  void SearchOnSensors(char ppkp, char *tot_sens, char *on_sens); //Описание для форварда
  static __flash char mess[]="БР-А: %02d <Вверх>-Поиск       Сейчас: %02d";
  //                             012345678901234567890123
  static char ppkp=0;
  struct TSensorConfig sc;
  char sensor;
  char pos=6;
  char c,tot_sens,on_sens,i;
  do
  {
    ClrScr();
    SearchOnSensors(ppkp,&tot_sens,&on_sens); //Считаем количество извещателей в текущем шлейфе (по конфигурации)
    sprintf_P(CON,mess,ppkp,tot_sens); //Печатаем его
    CursorPos=pos; //Выводим курсор
    c=KBD(0); //Ждем клавишу
    if ((c>=32 && c<128)||(c>=192)) //Символьная клавиша, пытаемся редактировать данные
    {
      if (Edit_dc(c,pos,6,&ppkp)) c=kRT;
    }
    switch(c)
    {
      //Пытаемся перемещать курсор
    case kLT:
      switch (pos)
      {
      case 6: c=kESC; break;
      case 9: pos=7; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 9: break;
      case 7: pos=9; break;
      default: pos++;
      }
      break;
      //Попытка перебора
    case kUP:
      Select_dc(pos,6,1,&ppkp); //Перебор ППКП
      if (pos==9) //Если нажатие в позиции 9 ("Поиск")
      {
        Event2FIFO(0xFF,0xFF,20,0); //Сообщение "Старт поиска извещателей"
        sensor=0;
        do
        {
          ReadSensorConfig(ppkp,sensor,&sc); //Читаем текущую конфигурацию
          //Печатаем "ПУ: ???"
          c=sc.Type;
          sprintf_P(CON+40,(STR)"%02d ПУ: ",sensor);
          sprintf_P(CON+47,NameSensorType[c&0x1F]);
          i=2;
          do
          {
            //Читаем статус адресного устройства
            do
            {
              //Подготовка пакета
              _CLI();
              //NeedNoError=0;
              SysPPKP_rxsize=0;
              SysPPKP_TXB[0]=ppkp;
              SysPPKP_TXB[1]=sensor;
              SysPPKP_TXB[2]=3;
              SysPPKP_txsize=3;
              _SEI();
              //Ожидание завершения обмена
              //Если пришло событие, приготовится к отображению
              while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
            }
            while (SysPPKP_rxsize==2); //Цикл, пока не прочитается статус
            if (SysPPKP_rxsize==255) break;
            if (SysPPKP_RXB[1]&0x1F) break;
          }
          while(--i);
          //Печатаем "Шл: ???"
          sprintf_P(CON+53,(STR)"Шлейф: ");
          sprintf_P(CON+60,NameSensorType[SysPPKP_RXB[1]&0x1F]);
          if (SysPPKP_rxsize!=255) //Если нормальный обмен с ППКП
          {
            if ((!(sc.Type&0x1F))&&(SysPPKP_RXB[1]&0x1F)) tot_sens++;
            if ((sc.Type&0x1F)&&(!(SysPPKP_RXB[1]&0x1F))) tot_sens--;
            sprintf_P(CON,mess,ppkp,tot_sens); //Печатаем его
            sc.Type=SysPPKP_RXB[1]&0x1F; //Сохраняем тип извещателя
            if (sc.Type) sc.Type|=0x80; //Если тип не "Нет", то включаем его
            //Подготовка пакета "Включение/Выключение адресного устройства"
            _CLI();
            SysPPKP_rxsize=0;
            SysPPKP_TXB[0]=ppkp;
            SysPPKP_TXB[1]=0x48;
            SysPPKP_TXB[2]=sensor+0xC0;
            SysPPKP_TXB[3]=(sc.Type&0x80?0x20:0x00)|(sc.Type&0x1f); //Включаем или выключаем
            SysPPKP_txsize=4;
            _SEI();
            //Ожидание завершения обмена
            //Если пришло событие, приготовится к отображению
            while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
            WriteSensorConfig(ppkp,sensor,&sc); //Записываем конфигурацию извещателя
            //Если тип и параметр вкл/выкл извещателя изменился
            if (c!=sc.Type) Event2FIFO(ppkp,sensor|0xc0,sc.Type>>7,sc.Type); //Сообщение "Извещатель вкл/выкл"
          }
          UpdateConsole();
          sensor+=1; //Следующий извещатель
        }
        //Цикл, пока не все извещатели или не нажали клавишу или не пришло событие
        while (sensor<64 && KEY==0 && Timer30sec!=0);
        if (sensor>=64) //Если прошли все извещатели посылаем "Холодный сброс ППКП"
        {
          //Подготовка пакета
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=0x4F;
          SysPPKP_txsize=2;
          _SEI();
          //Ожидание завершения обмена
          //Если пришло событие, приготовится к отображению
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        Event2FIFO(0xFF,0xFF,21,0); //Сообщение "Конец поиска извещателей"
        ClrScr();
      }
      break;
      //Пытаемся перебирать параметры
    case kDW:
      Select_dc(pos,6,-1,&ppkp);
      break;
    }
  }
  while (c!=kESC); //Цикл, пока не выход
}

//Наличие доп. устройств
void AddDevice(void)
{
  static __flash char messon[]="Вкл";
  static __flash char messoff[]="Выкл";
  char c;
  char pos=5;
  do
  {
    //Печатаем состояния устройств
    ClrScr();
    sprintf_P(CON+0 ,(STR)"МРВ1:");
    sprintf_P(CON+5 ,MRV1_STAT&0x80?messon:messoff);
    sprintf_P(CON+10 ,(STR)"МРВ2:");
    sprintf_P(CON+15 ,MRV2_STAT&0x80?messon:messoff);
    sprintf_P(CON+20 ,(STR)"ТК:");
    sprintf_P(CON+25 ,TK_STAT&0x80?messon:messoff);
    sprintf_P(CON+30 ,(STR)"ДВП:");
    sprintf_P(CON+35 ,DVP_STAT&0x80?messon:messoff);
    sprintf_P(CON+40 ,(STR)"ВСТ:");
    sprintf_P(CON+45 ,VST_STAT&0x80?messon:messoff);
    sprintf_P(CON+50 ,(STR)"ВТК:");
    sprintf_P(CON+55 ,VTK_STAT&0x80?messon:messoff);
    sprintf_P(CON+60 ,(STR)"ПК:");
    sprintf_P(CON+65 ,VPK_STAT&0x80?messon:messoff);
    sprintf_P(CON+70 ,(STR)"АПС:");
    sprintf_P(CON+75 ,messoff);
    CursorPos=pos; //Выводим курсор
    c=KBD(0); //Ожидаем клавишу
    switch(c)
    {
      //Пытаемся передвинуть курсор влево
    case kLT:
      switch(pos)
      {
      case 5:  c=kESC; break;
      default: pos-=10; break;
      }
      break;
      //Пытаемся передвинуть курсор вправо
    case kRT:
      switch(pos)
      {
      case 75:  break;
      default: pos+=10; break;
      }
      break;
      //Пытаемся перебирать данные
    case kUP:
      switch(pos)
      {
      case 5:
        MRV1_STAT|=0x80;
        break;
      case 15:
        MRV2_STAT|=0x80;
        break;
      case 25:
        TK_STAT|=0x80;
        break;
      case 35:
        DVP_STAT|=0x80;
        break;
      case 45:
        VST_STAT|=0x80;
        break;
      case 55:
        VTK_STAT|=0x80;
        break;
      case 65:
        VPK_STAT|=0x80;
        break;
      case 75:
        //PU_STAT|=0x80;
        break;
      };
      break;
    case kDW:
      switch(pos)
      {
      case 5:
        MRV1_STAT&=0x7F;
        break;
      case 15:
        MRV2_STAT&=0x7F;
        break;
      case 25:
        TK_STAT&=0x7F;
        break;
      case 35:
        DVP_STAT&=0x7F;
        break;
      case 45:
        VST_STAT&=0x7F;
        break;
      case 55:
        VTK_STAT&=0x7F;
        break;
      case 65:
        VPK_STAT&=0x7F;
        break;
      case 75:
        //PU_STAT&=0x7F;
        break;
      };
      break;
    }
  }
  while (c!=kESC); //Цикл, пока не выход
}

void ConfigPUID(void)
{                            //012345678901234567890123
  static __flash char mess[]="Текущий адрес ПУ - %02d";
  char id;
  char pos=19;
  char c;
  id=PU_ID;
  do
  {
    id&=31;
    ClrScr();
    sprintf_P(CON,mess,id);
    CursorPos=pos; //Перемещаем курсор
    c=KBD(0); //Ждем клавиши
    if ((c>=32 && c<128)||(c>=192)) //Если символьная клавиша
    {
      if (Edit_dc(c,pos,19,&id)) { c=kRT;} //Пытаемся редактировать
    }
    switch(c)
    {
      //Пытаемся перемещать курсор влево
    case kLT:
      switch (pos)
      {
      case 19: c=kESC; break;
      default: pos--;
      }
      break;
      // Пытаемся перемещать курсор вправо
    case kRT:
      switch (pos)
      {
      case 20: break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,19,1,&id); //Пытаемся перебирать
      break;
    case kDW:
      Select_dc(pos,19,-1,&id);//Пытаемся перебирать
      break;
    }
  } while (c!=kESC); //Цикл, пока не выход
  PU_ID=id;
  PUID=id;
}

//Подменю конфигурация ПУ
void ConfigPU(void)
{
  char c,i=0;
  TestPWD(40); //Проверяем пароль
  do
  {
    //Печатаем меню
    ClrScr();
    sprintf_P(CON,Message_MainMenu[3]);
    c=PU_STAT;
    if (i>=8)
    {
      char x;
      x=(i<<1)-8;
      switch(x)
      {
      case 8: if (c&0x04) x++; break;
      case 10: if (c&0x08) x++; break;
      case 12: if (c&0x10) x++; break;
      case 14: if (c&0x20) x++; break;
      case 16: if (c&0x40) x++; break;
      case 18: if (c&0x80) x++; break;
      case 20: if (IS_ALLWAYS_RESET) x++; break;
      case 22: if (IS_NOLOG_ACK) x++; break;
      case 24: if (IS_TAMPER_DISABLED) x++; break;
      }
      sprintf_P(CON+40,Message_ConfigPU[x]);
      sprintf_P(CON+63,(STR)"<Вправо>-изменить");
    }
    else
    {
      sprintf_P(CON+40,Message_ConfigPU[i]);
      sprintf_P(CON+66,(STR)"<Вправо>-войти");
    }
    c=KBD(0);
    if (c>=0x11 && c<=0x18) //Обрабатываем функциональные клавиши
    {
      i=c-0x11;
      c=kRT;
    }
    switch(c)
    {
    case kRT:
      //В зависимости от пункта меню проверяем уровень доступа и, если хватает, вызываем процедуру
      switch(i)
      {
      case 0: if (PWDmode>=2)
      {
        Event2FIFO(0xFF,0xFF,17,0); //Сообщение "Начало изменения даты/времени"
        ConfigPUtimedate();
        Event2FIFO(0xFF,0xFF,18,0); //Сообщение "Конец изменения даты/времени"
      }
      break;
      case 1: if (PWDmode==3) ConfigPUpk(); break;
      case 2: if (PWDmode>=2) ConfigPassword(); break;
      case 3: PrintSetup(); break;
      case 4: if (PWDmode==3) AddDevice(); break;
      case 5: ConfigTextPrint(); break;
      case 6: if (PWDmode==3) SearchSensors(); break;
      case 7: if (PWDmode==3) ConfigPUID(); break;
      case 8: if (PWDmode!=3) break;
      PU_STAT^=0x04;
      break;
      case 9: if (PWDmode!=3) break;
      PU_STAT^=0x08;
      break;
      case 10: if (PWDmode!=3) break;
      PU_STAT^=0x10;
      break;
      case 11: if (PWDmode!=3) break;
      PU_STAT^=0x20;
      break;
      case 12: if (PWDmode!=3) break;
      PU_STAT^=0x40;
      break;
      case 13: if (PWDmode!=3) break;
      PU_STAT^=0x80;
      break;
      case 14: if (PWDmode!=3) break;
      PU_STAT2^=0x01;
      break;
      case 15: if (PWDmode!=3) break;
      PU_STAT2^=0x02;
      break;
      case 16: if (PWDmode!=3) break;
      PU_STAT2^=0x04;
      break;
      } ClrScr(); break;
    case kUP: if (i>0) i--; break;
    case kDW: if (i<16) i++; break;
    }
  } while (c!=kLT); //Цикл, пока не выход
}

