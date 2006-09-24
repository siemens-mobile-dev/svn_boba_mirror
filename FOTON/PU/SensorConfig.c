//----------------------------------------------------------
// Редактирование конфигурации извещателей
// Географическое положение, тип, и т.д.
//----------------------------------------------------------
//Пункт меню "Конфигурация извещателей"
void ConfigSensor(void)
{
  //0         1         2         3
  //0123456789012345678901234567890123456789
  //Адрес:00/00+ Z1:000 Z2:000 Z3:000 Z4:000
  //?Тип? ТекстТекстТекстТекстТ... м0000 к00
  //      ------ Нет текста ------
  static char ppkp=1,sensor=1,pos=6;
  char c;
  struct TSensorConfig ts;
  char oldt;

  ReadSensorConfig(ppkp,sensor,&ts); //Читаем конфигурацию извещателя
  oldt=ts.Type&0x9F;
  //Запрашиваем пароль
  TestPWD(40);
  do
  {
    if (pos<11)
    {
      ReadSensorConfig(ppkp,sensor,&ts); //Читаем конфигурацию извещателя
      oldt=ts.Type&0x9F;
    }
    if (!((ts.Type)&0x1F)) ts.Type=0; //Если тип "Нет" - выключаем его
    if ((PWDmode<3)&&pos>11) pos=11;
    Max63(&ppkp); //Номер ППКП 0..63
    Max63(&sensor); //Номер извещателя 0..63
    sprintf_P(CON,(STR)"Адрес:%02d/%02d%c Z1:%03d Z2:%03d Z3:%03d Z4:%03d",
              ppkp,sensor,ts.Type&128?'+':'-',
              ts.Zone1,ts.Zone2,ts.Zone3,ts.Zone4);
    sprintf_P(CON+40,NameSensorType[(ts.Type)&0x1F]);
    sprintf_P(CON+45,(STR)" %.24s м%04X к%02X",
              ts.GeoPos,ts.Led,ts.TCode);
    if (IsLineSpace(ts.GeoPos)) sprintf_P(CON+46,"------ Нет текста ------");
    CursorPos=pos;
    c=KBD(0); //Ждем клавишу
    if (c==kESC) goto L_MODIFY;
    if ((c>=32 && c<128)||(c>=192)) //Символьная клавиша, пытаемся редактировать
    {
      if (Edit_dc(c,pos,6,&ppkp)||Edit_dc(c,pos,9,&sensor))
      {
        ReadSensorConfig(ppkp,sensor,&ts); //Читаем конфигурацию извещателя
        oldt=ts.Type&0x9F;
        c=kRT;
      }
      if (Edit_db(c,pos,16,&ts.Zone1)) c=kRT;
      if (Edit_db(c,pos,23,&ts.Zone2)) c=kRT;
      if (Edit_db(c,pos,30,&ts.Zone3)) c=kRT;
      if (Edit_db(c,pos,37,&ts.Zone4)) c=kRT;
      if (Edit_hb(c,pos,78,&ts.TCode)) c=kRT;
      if (Edit_hi(c,pos,72,&ts.Led)) c=kRT;   // Write symbol
      if ((pos>=46)&&(pos<70)) //Географическая координата
      {
        // Write symbol to geopos
        ts.GeoPos[pos-46]=c;
        c=kRT;
      }
      if (pos==40||pos==11) c=kUP;
    }
    switch(c)
    {
      //Пытаемся двигать курсор влево
    case kLT:
      switch (pos)
      {
      case 6: c=kESC; break;
      case 9: pos=7; break;
      case 11:
      L_MODIFY:
        if (oldt!=(ts.Type&0x9F)) //Если параметр вкл/выкл изменился
        {
          oldt=ts.Type&0x9F;
          Event2FIFO(ppkp,sensor|0xc0,ts.Type>>7,ts.Type); //Сообщение "Извещатель включен/выключен"
          //Передача комманды "Включить/Выключить адресное устройство"
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=0x48;
          SysPPKP_TXB[2]=sensor+0xC0;
          SysPPKP_TXB[3]=(ts.Type&0x80?0x20:0x00)|(ts.Type&0x1f); //Включаем или выключаем
          SysPPKP_txsize=4;
          _SEI();
          //Ожидание завершения обмена
          //Если пришло событие, приготовится к отображению
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        //Передача комманды "Выключить светодиод на адресном устройстве"
        _CLI();
        //NeedNoError=1;
        SysPPKP_rxsize=0;
        SysPPKP_TXB[0]=ppkp;
        SysPPKP_TXB[1]=sensor;
        SysPPKP_TXB[2]=0;
        SysPPKP_txsize=3;
        _SEI();
        //Ожидание завершения обмена
        //Если пришло событие, приготовится к отображению
        while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        WriteSensorConfig(ppkp,sensor,&ts); //Запись новой конфигурации в 45??021
        pos=10;
        break;
      case 16: pos=11; break;
      case 23: pos=18; break;
      case 30: pos=25; break;
      case 37: pos=32; break;
      case 40: pos=39; break;
      case 46: pos=40; break;
      case 72: pos=69; break;
      case 78: pos=75; break;
      default: pos--;
      }
      break;
      //Пытаемся двигать курсор вправо
    case kRT:
      switch (pos)
      {
      case 10: //Если правее всего
        //Передаем комманду "Зажечь светодиод на адресном устройстве"
        _CLI();
        //NeedNoError=1;
        SysPPKP_rxsize=0;
        SysPPKP_TXB[0]=ppkp;
        SysPPKP_TXB[1]=sensor;
        SysPPKP_TXB[2]=1;
        SysPPKP_txsize=3;
        _SEI();
        //Ожидание завершения обмена
        //Если пришло событие, приготовится к отображению
        while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        pos=11;
        break;
      case 7: pos=9; break;
      case 11: pos=16; break;
      case 18: pos=23; break;
      case 25: pos=30; break;
      case 32: pos=37; break;
      case 39: pos=40; break;
      case 40: pos=46; break;
      case 69: pos=72; break;
      case 75: pos=78; break;
      case 79: break;
      default: pos++;
      }
      break;
      //Пытаемся перебирать параметры
    case kUP:
      if (pos==11) ts.Type|=0x80; //Включение
      if (pos==40) ts.Type=(ts.Type+1)&0x9F; //Изменение типа
      Select_dc(pos,6,1,&ppkp);
      Select_dc(pos,9,1,&sensor);
      Select_db(pos,16,1,&ts.Zone1);
      Select_db(pos,23,1,&ts.Zone2);
      Select_db(pos,30,1,&ts.Zone3);
      Select_db(pos,37,1,&ts.Zone4);
      Select_hi(pos,72,1,&ts.Led);
      Select_hb(pos,78,1,&ts.TCode);
      if ((pos>=46)&&(pos<70)) //Географическая координата
      {
        char cs=ts.GeoPos[pos-46];
        cs++; //Следующий код символа
        //Теперь корректируем в зависимости от режима ввода/вывода текста
        //Читаем конфигурацию ПУ
        switch (PU_STAT&0x03)
        {
        case 0: //Все символы
          if (cs>=128 && cs<192) cs=192;
          if (cs<32) cs=32;
          break;
        case 1: //Только большие буквы
          if (cs>=96 && cs<192) cs=192;
          if (cs>=224) cs=32;
          if (cs<32) cs=32;
          break;
        case 2: //Только русские буквы
          if (cs>=64 && cs<192) cs=192;
          if (cs<32) cs=32;
          break;
        case 3: //Только большие русские буквы
          if (cs>=64 && cs<192) cs=192;
          if (cs>=224) cs=32;
          if (cs<32) cs=32;
          break;
        }
        ts.GeoPos[pos-46]=cs; //Сохраняем новый код, только если уровень 2,3
      };
      break;
    case kDW:
      if (pos==11) ts.Type&=0x7F;
      if ((pos==40)&&((ts.Type&0x1F)>0)) ts.Type=(ts.Type-1)&0x9F; //Изменение типа
      Select_dc(pos,6,-1,&ppkp);
      Select_dc(pos,9,-1,&sensor);
      Select_db(pos,16,-1,&ts.Zone1);
      Select_db(pos,23,-1,&ts.Zone2);
      Select_db(pos,30,-1,&ts.Zone3);
      Select_db(pos,37,-1,&ts.Zone4);
      Select_hi(pos,72,-1,&ts.Led);
      Select_hb(pos,78,-1,&ts.TCode);
      if ((pos>=46)&&(pos<70)) //Географическая координата
      {
        char cs=ts.GeoPos[pos-46];
        cs--; //Предыдущий код символа
        //Теперь корректируем в зависимости от режима ввода/вывода текста
        //Читаем конфигурацию ПУ
        switch (PU_STAT&0x03)
        {
        case 0: //Все символы
          if (cs>=128 && cs<192) cs=127;
          if (cs<32) cs=255;
          break;
        case 1: //Только большие буквы
          if (cs>=96 && cs<192) cs=95;
          if (cs>=224) cs=223;
          if (cs<32) cs=223;
          break;
        case 2: //Только русские буквы
          if (cs>=64 && cs<192) cs=63;
          if (cs<32) cs=255;
          break;
        case 3: //Только большие русские буквы
          if (cs>=64 && cs<192) cs=63;
          if (cs>=224) cs=223;
          if (cs<32) cs=223;
          break;
        }
        ts.GeoPos[pos-46]=cs; //Сохраняем новый код, только если уровень 2,3
      }
      break;
    case kINS: if ((pos>=46)&&(pos<70)) //Вставка пробела в географическую координату
    {
      c=23;
      while (c!=(pos-46))
      {
        ts.GeoPos[c]=ts.GeoPos[c-1];
        c--;
      }
      ts.GeoPos[pos-46]=' ';
    }
    break;
    case kDEL: if ((pos>=46)&&(pos<70)) //Удаление символа под курсором в географической координате
    {
      c=pos-46;
      while (c!=23)
      {
        ts.GeoPos[c]=ts.GeoPos[c+1];
        c++;
      }
      ts.GeoPos[23]=' ';
    }
    break;
    }
  }
  while (c!=kESC); //Цикл, пока не выход
}

