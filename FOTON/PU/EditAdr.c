//---------------------------------------
// Меню "Изменение адресов"
//---------------------------------------
void EditPPKPAdr(void)
{
  static __flash char mess1[]="БР-А: %02d->%02d";
  static __flash char mess2[]="<Вверх>-Запись/Выкл   <Вверх>-Запись/Вкл";

  char ppkp=0;
  char new=0;
  char c;
  char pos=6;

  do
  {
    Max63(&ppkp);
    Max63(&new);
    ClrScr();
    sprintf_P(CON,mess1,ppkp,new);

    _CLI();
    //NeedNoError=1;
    SysPPKP_rxsize=0;
    SysPPKP_TXB[0]=new;
    SysPPKP_TXB[1]=0x43;
    SysPPKP_txsize=2;
    _SEI();
    while (SysPPKP_rxsize==0);

    if (SysPPKP_rxsize==255) sprintf_P(CON+40,mess2);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,6,&ppkp)) c=kRT;
      if (Edit_dc(c,pos,10,&new)) c=kRT;
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 6: c=kESC; break;
      case 10: pos=7; break;
      case 40: pos=11; break;
      case 62: pos=40; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 62: break;
      case 40: pos=62; break;
      case 11: if (SysPPKP_rxsize==255) pos=40; break;
      case 7: pos=10; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,6,1,&ppkp);
      Select_dc(pos,10,1,&new);
      if (pos==40||pos==62)
      {
        if (ABONS[ppkp]&0x80) ABONS[ppkp]=0; //Запрет обмена с БР-А, если был
        Event2FIFO(ppkp,0xc0,3,new);
        _CLI();
        SysPPKP_rxsize=0;
        SysPPKP_TXB[0]=ppkp;
        SysPPKP_TXB[1]=0x47;
        SysPPKP_TXB[2]=new;
        SysPPKP_txsize=3;
        _SEI();
        while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        if (SysPPKP_rxsize==2)
        {
          if (pos==62)
          {
            PPKP_AREA[new]|=0x80;
            ABONS[new]=0x80;
          }
          PPKP_AREA[ppkp]&=0x7F;
        }
        pos=6;
      }
      break;
    case kDW:
      Select_dc(pos,6,-1,&ppkp);
      Select_dc(pos,10,-1,&new);
      break;
    }
  }
  while (c!=kESC);
}

void PrintSensAdrResult(char code)
{
  //                              012345678901234567890123
  static __flash char messr0[]="Успешное изменение";
  static __flash char messr1[]="Извещатель не найден";
  static __flash char messr2[]="Более одного извещателя";
  static __flash char messr3[]="Занят конечный адрес";
  static __flash char messr4[]="Ошибка обмена";
  static __flash char messr5[]="Время ожидания истекло";
  static __flash char messre[]="Неизвестное событие %02X";
  static __flash char messr6[]="Отказ EEPROM";
  memset(CON+40,' ',40);
  switch (code)
  {
  case 0: sprintf_P(CON+40,messr0);
  break;
  case 1: sprintf_P(CON+40,messr1); break;
  case 2: sprintf_P(CON+40,messr2); break;
  case 3: sprintf_P(CON+40,messr3); break;
  case 4: sprintf_P(CON+40,messr4); break;
  case 5: sprintf_P(CON+40,messr5); break;
  case 6: sprintf_P(CON+40,messr6); break;
  default: sprintf_P(CON+40,messre,code); break;
  }
  UpdateConsole();
}

void EditSensorAdr(void)
{
  static __flash char mess1[]="БР-А/Изв-ль: %02d/%02d->%02d <Вверх>-Изменить";
  static __flash char mess3[]="%.24s";
  static __flash char mess_do[]="Изменение адреса...";

  struct TSensorConfig sc;
  static char ppkp=1;
  char sensor=0;
  char new=0;
  char c;
  char pos=13;
  char vflg=1;

  ClrScr();
  do
  {
    memset(CON,' ',40);
    Max63(&ppkp);
    Max63(&sensor);
    Max63(&new);
    if (pos>=19) ReadSensorConfig(ppkp,new,&sc);
    else ReadSensorConfig(ppkp,sensor,&sc);
    sprintf_P(CON,mess1,ppkp,sensor,new);
    if (vflg)
    {
      memset(CON+40,' ',40);
      sprintf_P(CON+40,mess3,sc.GeoPos);
    }
    vflg=1;
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,13,&ppkp)) c=kRT;
      if (Edit_dc(c,pos,16,&sensor)) c=kRT;
      if (Edit_dc(c,pos,20,&new)) c=kRT;
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 13: c=kESC; break;
      case 23: pos=21; break;
      case 20: pos=17; break;
      case 16: pos=14; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 23: break;
      case 21: pos=23; break;
      case 17: pos=20; break;
      case 14: pos=16; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,13,1,&ppkp);
      Select_dc(pos,16,1,&sensor);
      Select_dc(pos,20,1,&new);
      if (pos==23)
      {
        // Send command to change adr
        memset(CON+40,' ',40);
        sprintf_P(CON+40,mess_do);
        UpdateConsole();
        do
        {
          _CLI();
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=0x07;
          SysPPKP_TXB[2]=sensor;
          SysPPKP_TXB[3]=new;
          SysPPKP_txsize=4;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        }
        while (SysPPKP_rxsize==2);
        if (SysPPKP_rxsize!=6) break;
        //               if (Timer30sec==0 || SysPPKP_rxsize!=6) break;
        PrintSensAdrResult(SysPPKP_RXB[4]);
        if (!SysPPKP_RXB[4]) Event2FIFO(ppkp,sensor|0xc0,2,new);
        vflg=0;
      }
      break;
    case kDW:
      Select_dc(pos,13,-1,&ppkp);
      Select_dc(pos,16,-1,&sensor);
      Select_dc(pos,20,-1,&new);
      break;
    }
  }
  while (c!=kESC);
}

void ZeroToSens(void)
{
  static __flash char mess[]="БР-А: %02d <Вверх>-Установить";
  static __flash char mess1[]="Текущий извещатель %02d";
  char sensor;
  char ppkp=0;
  char c;
  char pos=6;
  struct TSensorConfig ts;

  do
  {
    Max63(&ppkp);
    ClrScr();
    sprintf_P(CON,mess,ppkp);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,6,&ppkp)) c=kRT;
    }
    switch(c)
    {
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
    case kUP:
      Select_dc(pos,6,1,&ppkp);
      if (pos==9)
      {
        RebootMode=0xFF; //Запрет опроса
        sensor=1;
        KEY=0;
        do
        {
          sprintf_P(CON+40,mess1,sensor);
          UpdateConsole();
          c=5;
          do
          {
            //Читаем статус адресного устройства
            do
            {
              //Подготовка пакета
              _CLI();
              SysPPKP_rxsize=0;
              SysPPKP_TXB[0]=ppkp;
              SysPPKP_TXB[1]=0;
              SysPPKP_TXB[2]=3;
              SysPPKP_txsize=3;
              _SEI();
              //Ожидание завершения обмена
              //Если пришло событие, приготовится к отображению
              while (SysPPKP_rxsize==0) if (FIFO2EventLog()) goto LEXIT;
            }
            while (SysPPKP_rxsize==2); //Цикл, пока не прочитается статус
            if (SysPPKP_rxsize==255) goto LEXIT;
            if (KEY!=0) goto LEXIT;
            if (!(SysPPKP_RXB[1]&0x1F)) c=5;
          }
          while(--c);
          c=SysPPKP_RXB[1]&0x1F;
        LLOOP:
          do
          {
            _CLI();
            SysPPKP_rxsize=0;
            SysPPKP_TXB[0]=ppkp;
            SysPPKP_TXB[1]=0x07;
            SysPPKP_TXB[2]=0;
            SysPPKP_TXB[3]=sensor;
            SysPPKP_txsize=4;
            _SEI();
            while (SysPPKP_rxsize==0) if (FIFO2EventLog()) goto LEXIT;
          }
          while (SysPPKP_rxsize==2);
          if (SysPPKP_rxsize!=6) goto LEXIT;
          if (SysPPKP_RXB[4]==3)
          {
            sensor++;
            if (sensor==64) break;
            sprintf_P(CON+40,mess1,sensor);
            UpdateConsole();
            goto LLOOP;
          }
          if (SysPPKP_RXB[4]) goto LERR;

          ReadSensorConfig(ppkp,sensor,&ts); //Читаем конфигурацию извещателя
          //Передача комманды "Включить/Выключить адресное устройство"
          _CLI();
          //NeedNoError=0;
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=0x48;
          SysPPKP_TXB[2]=sensor+0xC0;
          SysPPKP_TXB[3]=c|0x20; //включаем
          SysPPKP_txsize=4;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) goto LEXIT;
          ts.Type=c|0x80;
          WriteSensorConfig(ppkp,sensor,&ts); //Запись новой конфигурации в 45??021
          //               Event2FIFO(ppkp,sensor|0xc0,ts.Type>>7,0); //Сообщение "Извещатель включен/выключен"
          sensor++;
        }
        while(sensor!=64);
        ClrScr();
      }
      else break;
      goto LRST;
    LEXIT:
      Timer30sec=0;
      ClrScr();
      goto LRST;
    LERR:
      PrintSensAdrResult(SysPPKP_RXB[4]);
      SndMode=3;
      KBD(0);
      SndMode=0;
    LRST:
      //Подготовка пакета
      _CLI();
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=ppkp;
      SysPPKP_TXB[1]=0x4E;
      SysPPKP_txsize=2;
      _SEI();
      //Ожидание завершения обмена
      //Если пришло событие, приготовится к отображению
      while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
      __delay_cycles(200000);
      __delay_cycles(200000);
      __delay_cycles(200000);
      __delay_cycles(200000);
      RebootMode=0;
      break;
    case kDW:
      Select_dc(pos,6,-1,&ppkp);
      break;
    }
  }
  while (c!=kESC);
}

void SensToZero(void)
{
  static __flash char mess[]="БР-А: %02d <Вверх>-Снять";
  static __flash char mess1[]="Текущий извещатель %02d";
  char sensor;
  char ppkp=0;
  char c;
  char pos=6;
  struct TSensorConfig ts;

  do
  {
    ClrScr();
    Max63(&ppkp);
    sprintf_P(CON,mess,ppkp);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,6,&ppkp)) c=kRT;
    }
    switch(c)
    {
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
    case kUP:
      Select_dc(pos,6,1,&ppkp);
      if (pos==9)
      {
        RebootMode=0xFF; //Запрет опроса
        sensor=1;
        KEY=0;
        do
        {
          sprintf_P(CON+40,mess1,sensor);
          UpdateConsole();
          ReadSensorConfig(ppkp,sensor,&ts); //Читаем конфигурацию извещателя
          //Передача комманды "Включить/Выключить адресное устройство"
          _CLI();
          //NeedNoError=0;
          SysPPKP_rxsize=0;
          SysPPKP_TXB[0]=ppkp;
          SysPPKP_TXB[1]=0x48;
          SysPPKP_TXB[2]=sensor+0xC0;
          SysPPKP_TXB[3]=0; //выключаем
          SysPPKP_txsize=4;
          _SEI();
          while (SysPPKP_rxsize==0) if (FIFO2EventLog()) goto LEXIT;
          ts.Type=0;
          WriteSensorConfig(ppkp,sensor,&ts); //Запись новой конфигурации в 45??021
          //               Event2FIFO(ppkp,sensor|0xc0,ts.Type>>7,0); //Сообщение "Извещатель включен/выключен"

          c=3;
          do
          {
            //Читаем статус адресного устройства
            do
            {
              //Подготовка пакета
              _CLI();
              SysPPKP_rxsize=0;
              SysPPKP_TXB[0]=ppkp;
              SysPPKP_TXB[1]=0;
              SysPPKP_TXB[2]=3;
              SysPPKP_txsize=3;
              _SEI();
              //Ожидание завершения обмена
              //Если пришло событие, приготовится к отображению
              while (SysPPKP_rxsize==0) if (FIFO2EventLog()) goto LEXIT;
            }
            while (SysPPKP_rxsize==2); //Цикл, пока не прочитается статус
            if (SysPPKP_rxsize==255) goto LEXIT;
            if (SysPPKP_RXB[1]&0x1F) c=3;
          }
          while(--c);
        LLOOP:
          do
          {
            _CLI();
            SysPPKP_rxsize=0;
            SysPPKP_TXB[0]=ppkp;
            SysPPKP_TXB[1]=0x07;
            SysPPKP_TXB[2]=sensor;
            SysPPKP_TXB[3]=0;
            SysPPKP_txsize=4;
            _SEI();
            while (SysPPKP_rxsize==0) if (FIFO2EventLog()) goto LEXIT;
            if (KEY!=0) goto LEXIT;
          }
          while (SysPPKP_rxsize==2);
          if (SysPPKP_rxsize!=6) goto LEXIT;
          switch(SysPPKP_RXB[4])
          {
          case 0: sensor++;
          break;
          case 1: sensor++;
          sprintf_P(CON+40,mess1,sensor);
          UpdateConsole();
          if (sensor!=64) goto LLOOP;
          break;
          default: goto LERR;
          }
        }
        while(sensor!=64);
        ClrScr();
      }
      else break;
      goto LRST;
    LEXIT:
      Timer30sec=0;
      ClrScr();
      goto LRST;
    LERR:
      PrintSensAdrResult(SysPPKP_RXB[4]);
      SndMode=3;
      KBD(0);
      SndMode=0;
    LRST:
      //Подготовка пакета
      _CLI();
      SysPPKP_rxsize=0;
      SysPPKP_TXB[0]=ppkp;
      SysPPKP_TXB[1]=0x4E;
      SysPPKP_txsize=2;
      _SEI();
      //Ожидание завершения обмена
      //Если пришло событие, приготовится к отображению
      while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
      __delay_cycles(200000);
      __delay_cycles(200000);
      __delay_cycles(200000);
      __delay_cycles(200000);
      RebootMode=0;
      break;
    case kDW:
      Select_dc(pos,6,-1,&ppkp);
      break;
    }
  }
  while (c!=kESC);
}

void EditUPSAdr(void)
{
  static __flash char mess1[]="АПС:  %02d->%02d";
  static __flash char mess2[]="<Вверх>-Запись/Выкл   <Вверх>-Запись/Вкл";

  char ppkp=0;
  char new=0;
  char c;
  char pos=6;

  do
  {
    ppkp&=31;
    new&=31;
    ClrScr();
    sprintf_P(CON,mess1,ppkp,new);

    _CLI();
    //NeedNoError=1;
    SysPPKP_rxsize=0;
    SysPPKP_TXB[0]=new+0x40;
    SysPPKP_TXB[1]=0x43;
    SysPPKP_txsize=2;
    _SEI();
    while (SysPPKP_rxsize==0);

    if (SysPPKP_rxsize==255) sprintf_P(CON+40,mess2);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,6,&ppkp)) c=kRT;
      if (Edit_dc(c,pos,10,&new)) c=kRT;
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 6: c=kESC; break;
      case 10: pos=7; break;
      case 40: pos=11; break;
      case 62: pos=40; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 62: break;
      case 40: pos=62; break;
      case 11: if (SysPPKP_rxsize==255) pos=40; break;
      case 7: pos=10; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,6,1,&ppkp);
      Select_dc(pos,10,1,&new);
      if (pos==40||pos==62)
      {
        if (ABONS[ppkp+0x40]&0x80) ABONS[ppkp+0x40]=0; //Запрет обмена с БР-А, если был
        Event2FIFO(ppkp+0x40,0xc0,3,new);
        _CLI();
        SysPPKP_rxsize=0;
        SysPPKP_TXB[0]=ppkp+0x40;
        SysPPKP_TXB[1]=0x47;
        SysPPKP_TXB[2]=new+0x40;
        SysPPKP_txsize=3;
        _SEI();
        while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        if (SysPPKP_rxsize==2)
        {
          if (pos==62)
          {
            UPS_AREA[new]|=0xC0;
            ABONS[new+0x40]=0x80;
          }
          UPS_AREA[ppkp]&=0x3F;
        }
        pos=6;
      }
      break;
    case kDW:
      Select_dc(pos,6,-1,&ppkp);
      Select_dc(pos,10,-1,&new);
      break;
    }
  }
  while (c!=kESC);
}

void EditBRAAdr(void)
{
  static __flash char mess1[]="БРА/ПКП: %02d->%02d";
  static __flash char mess2[]="<Вверх>-Запись/Выкл   <Вверх>-Запись/Вкл";

  char ppkp=0;
  char new=0;
  char c;
  char pos=9;

  do
  {
    ppkp&=31;
    new&=31;
    ClrScr();
    sprintf_P(CON,mess1,ppkp,new);

    _CLI();
    //NeedNoError=1;
    SysPPKP_rxsize=0;
    SysPPKP_TXB[0]=new+0x60;
    SysPPKP_TXB[1]=0x43;
    SysPPKP_txsize=2;
    _SEI();
    while (SysPPKP_rxsize==0);

    if (SysPPKP_rxsize==255) sprintf_P(CON+40,mess2);
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      if (Edit_dc(c,pos,9,&ppkp)) c=kRT;
      if (Edit_dc(c,pos,13,&new)) c=kRT;
    }
    switch(c)
    {
    case kLT:
      switch (pos)
      {
      case 9: c=kESC; break;
      case 13: pos=10; break;
      case 40: pos=14; break;
      case 62: pos=40; break;
      default: pos--;
      }
      break;
    case kRT:
      switch (pos)
      {
      case 62: break;
      case 40: pos=62; break;
      case 14: if (SysPPKP_rxsize==255) pos=40; break;
      case 10: pos=13; break;
      default: pos++;
      }
      break;
    case kUP:
      Select_dc(pos,9,1,&ppkp);
      Select_dc(pos,13,1,&new);
      if (pos==40||pos==62)
      {
        if (ABONS[ppkp+0x60]&0x80) ABONS[ppkp+0x60]=0; //Запрет обмена с БР-А, если был
        Event2FIFO(ppkp+0x60,0xc0,3,new);
        _CLI();
        SysPPKP_rxsize=0;
        SysPPKP_TXB[0]=ppkp+0x60;
        SysPPKP_TXB[1]=0x47;
        SysPPKP_TXB[2]=new+0x60;
        SysPPKP_txsize=3;
        _SEI();
        while (SysPPKP_rxsize==0) if (FIFO2EventLog()) Timer30sec=0;
        if (SysPPKP_rxsize==2)
        {
          if (pos==62)
          {
            BRA_AREA[new]|=0xC0;
            ABONS[new+0x60]=0x80;
          }
          BRA_AREA[ppkp]&=0x3F;
        }
        pos=9;
      }
      break;
    case kDW:
      Select_dc(pos,9,-1,&ppkp);
      Select_dc(pos,13,-1,&new);
      break;
    }
  }
  while (c!=kESC);
}

void EditAdresses(void)
{
  char c,i=0;
  TestPWD(40);
  do
  {
    ClrScr();
    sprintf_P(CON,Message_MainMenu[10]);
    sprintf_P(CON+40,Message_EditAdr[i]);
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
      case 0: if (PWDmode==3) EditPPKPAdr(); break;
      case 1: if (PWDmode>=2) EditSensorAdr(); break;
      case 2: if (PWDmode==3) ZeroToSens(); break;
      case 3: if (PWDmode==3) SensToZero(); break;
      case 4: if (PWDmode==3) EditUPSAdr(); break;
      case 5: if (PWDmode==3) EditBRAAdr(); break;
      } ClrScr(); break;
    case kUP: if (i>0) i--; break;
    case kDW: if (i<5) i++; break;
    }
  } while (c!=kLT);
}

