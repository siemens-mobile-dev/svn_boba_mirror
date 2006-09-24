//----------------------
// View log file
//----------------------
void VLOGmask(char msk)
{
  static __flash char messap[]="Архив событий на ";
  static __flash char messapn[]="%04d: ";
  static __flash char messapb[]="Остановлено!";
  static __flash char messprt[]="Печать архива... Любая клавиша - стоп";

  unsigned long td;
  char c;
  char s[61];
  union
  {
    struct TLogEvent ev;
    unsigned long le;
  };
  //char slmode;
  char prtmode;
  int direction;
  int lastp;
  int apn;
  int LogFileRP;

  //slmode=0;
  direction=-1;
  prtmode=0;

  LogFileRP=SearchLastNormal();
  lastp=LogFileRP;
  LogFileRP=LogFileWP&0xFFF;
  do
  {
    if (direction)
    {
      if (!prtmode)
      {
        ClrScr();
        sprintf_P(CON,(STR)"Поиск события... Ожидайте...");
        UpdateConsole();
      }
      do
      {
        LogFileRP=(LogFileRP+direction)&0xFFF;
        ReadEvent(LogFileRP,&ev);

        if ((ev.Second==0xFFFFFFFF)||(le==0x0002FFFF))
        {
          LogFileRP=lastp;
          if (prtmode)
          {
            PrintSeparator();
            PRINT_CRLF;
            PRINT_CRLF;
            prtmode=0;
          }
          break;
        }
        PrintEventType(&ev,s,0);
      }
      while (!(TemporaryEVENT&msk));
    }
    lastp=LogFileRP;
    ReadEvent(LogFileRP,&ev);
    PrintEventType(&ev,s,1);
    if (prtmode)
    {
      printf_P(messapn,apn);
      apn++;
      LPT_PrintTimeDate(ev.Second);
      putstr(s);
      PRINT_CRLF;
      if (EventFIFOwp!=EventFIFOrp)
      {
        printf_P(messapb);
        PRINT_CRLF;
        PrintSeparator();
        PRINT_CRLF;
        PRINT_CRLF;
        prtmode=0;
      }
      if (KEY)
      {
        KEY=0;
        printf_P(messapb);
        PRINT_CRLF;
        PrintSeparator();
        PRINT_CRLF;
        PRINT_CRLF;
        prtmode=0;
      }
    }
    else
    {
      direction=0;
      ClrScr();
      sprintf_P(CON,(STR)"%04d ",(LogFileWP-LogFileRP)&0xFFF);
      memcpy(CON+5,s,35);
      PrintTimeDate(CON+40, ev.Second);
      memcpy(CON+59,s+36,21);
      UpdateConsole();
      c=KBD(0);
      switch(c)
      {
      case kUP:
        direction=-1;
        break;
      case kDW:
        direction=1;
        break;
      case kLT:
        //slmode^=1;
        c=kESC;
        break;
      case kRT:
        apn=1;
        prtmode=1;
        direction=1;
        ClrScr();
        sprintf_P(CON,messprt);
        UpdateConsole();
        PRINT_CRLF;
        PRINT_CRLF;
        PrintSeparator();
        printf_P(messap);
        _CLI();
        td=TimeDate;
        _SEI();
        LPT_PrintTimeDate(td);
        if (msk==0x01) printf_P(Message_ViewLog[0]);
        if (msk==0x02) printf_P(Message_ViewLog[1]);
        if (msk==0x04) printf_P(Message_ViewLog[2]);
        if (msk==0xFF) printf_P(Message_ViewLog[3]);
        PRINT_CRLF;
        PrintSeparator();
        break;
      }
    }
  }
  while ((c!=kESC));
}



//Просмотр текущих отключений
void ViewOffline(void)
{
  void SearchOnSensors(char ppkp, char *tot_sens, char *on_sens); //Описание для форварда
  static __flash char m1[]="Отключено:  <Влево>-Назад  другая-след.";
  static __flash char m2[]="БР-А %02d";
  static __flash char m3[]="БР-А/Изв: %02d/%02d ";
  static __flash char m4[]="Зона %03d";
  static __flash char m5[]="АПС %02d";
  static __flash char m6[]="БРА %02d";
  static __flash char mwait[]="Поиск отключения...";

  struct TSensorConfig ts;
  unsigned int pos=0xFFFF;
  unsigned int oldpos=0xFFFF;
  char total;
  char on;

  char c;
  do
  {
    ClrScr();
    sprintf_P(CON,mwait);
    UpdateConsole();
    for(;;)
    {
      if (FIFO2EventLog())
      {
        Timer30sec=0;
        c=KBD(0);
      }
      if (pos<4096)
      {
        //Откл. извещатели
        c=pos>>6;
        if ((pos&63)==0)
        {
          //Проверяем весь ППКП
          SearchOnSensors(c,&total,&on); //Считаем сколько всего извещателей для текущего ППКП и сколько вкл.
          if (PPKP_AREA[c]&0x80)
          {
            //Если ППКП включен
            if (total==on)
            {
              //Нет отключений
              pos+=64; //Сл. ППКП
              continue;
            }
          }
          else
          {
            //Если ППКП выключен
            if (total)
            {
              //Есть извещатели - отключение!
              sprintf_P(CON+40,m2,c);
              pos+=64; //Пропуск всего шлейфа
              break; //Печатаем
            }
            pos+=64; //Пропуск всего шлейфа
            continue;
          }
        }
        //Проверяем по извещателям
        ReadSensorConfig(c,pos&63,&ts); //Читаем конфигурацию извещателя
        if ((char)((ts.Type&0x9F)-1)<127)
        {
          //Отключен извещатель
          sprintf_P(CON+40,m3,c,pos&63);
          memcpy(CON+56,&ts.GeoPos,24);
          pos++;
          break;
        }
        pos++;
        continue;
      }
      if (pos<(4096+256))
      {
        unsigned int z=pos-4096;
        c=z;
        //Откл. зон
        if (ZONEMF_AREA[z]&0x80)
        {
          //Зона включена
          pos++;
          continue;
        }
        z<<=2;
        if (ZONEPOINT_AREA[z+1]!=0xFFFF) goto LZZ;
        if (ZONEPOINT_AREA[z+2]!=0xFFFF) goto LZZ;
        if (ZONEPOINT_AREA[z+3]!=0xFFFF) goto LZZ;
        if (ZONEPOINT_AREA[z]!=0xFFFF)
        {
          LZZ:
          //Есть реле -> зона отключена!
          sprintf_P(CON+40,m4,c);
          pos++;
          break;
        }
        pos++;
        continue;
      }
      if (pos<(4096+256+32))
      {
        //Откл. АПС
        c=pos-(4096+256);
        if ((UPS_AREA[c]&0xC0)==0x40)
        {
          //Откл. АПС
          sprintf_P(CON+40,m5,c);
          pos++;
          break;
        }
        pos++;
        continue;
      }
      if (pos<(4096+256+64))
      {
        //Откл. БРА
        c=pos-(4096+256+32);
        if ((BRA_AREA[c]&0xC0)==0x40)
        {
          //Откл. BRA
          sprintf_P(CON+40,m6,c);
          pos++;
          break;
        }
      }
      pos++; //Сл. позиция
      if (pos>=(4096+256+64))
      {
        //Пройден весь список, ничего не нашли
        if (oldpos==0xFFFF) return; //Нет вообще отключений
        pos=0; //С начала
      }
    }
    oldpos=pos;
    sprintf_P(CON,m1);
    UpdateConsole();
    c=KBD(0);
    if (c==kLT) c=kESC;
  }
  while ((c!=kESC));
}


void ViewLog(void)
{
  static __flash char totmess[]="%04d";
  union
  {
    struct TLogEvent ev;
    unsigned long le;
  };
  char s[61];
  char c,i=0;
  int x;
  int LogFileRP;
  do
  {
    ClrScr();
    sprintf_P(CON,Message_MainMenu[1]);
    sprintf_P(CON+40,Message_ViewLog[i]);
    UpdateConsole();
    switch(i)
    {
    case 0: c=0x01; break;
    case 1: c=0x02; break;
    case 2: c=0x04; break;
    case 3: c=0xFF; break;
    case 4: goto L1; //Просмотр отключений
    }
    x=0;
    LogFileRP=(LogFileWP-1)&0xFFF;
    do
    {
      ReadEvent(LogFileRP,&ev);
      if ((ev.Second==0xFFFFFFFF)||(le==0x0002FFFF)) break;
      PrintEventType(&ev,s,0);
      if (TemporaryEVENT&c) x++;
      LogFileRP=(LogFileRP-1)&0xFFF;
    }
    while ((ev.Second!=0xFFFFFFFF)&&(le!=0x0002FFFF)&&(KEY==0)&&(!FIFO2EventLog()));
    if ((ev.Second==0xFFFFFFFF)||(le==0x0002FFFF)) sprintf_P(CON+76,totmess,x);
  L1:
    c=KBD(0);
    if (c>=0x11 && c<=0x15)
    {
      i=c-0x11;
      c=kRT;
    }
    switch(c)
    {
    case kRT:
      switch(i)
      {
      case 0: VLOGmask(0x01); break;
      case 1: VLOGmask(0x02); break;
      case 2: VLOGmask(0x04); break;
      case 3: VLOGmask(0xFF); break;
      case 4: ViewOffline(); break;
      } ClrScr(); break;
    case kUP: if (i>0) i--; break;
    case kDW: if (i<4) i++; break;
    }
  } while (c!=kLT);
}

