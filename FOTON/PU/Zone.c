//--------------------------------------------------

// Меню "Конфигурация зон"
//--------------------------------------------------
void ConfigZone(void)
{
  static __flash char mess1[]="Зона: %03d%c N:%03d T:%03d P:%01d";
  static __flash char mess_brvl[]="БР-А: %02d Изв: %02d Реле: %01d";
  //012345678901234567890123
  static __flash char mess_brvu[]="ПУС: %02d Направление: %03d";
  //9            //21
  static __flash char mess_bra[]= "БРА/ПКП: %02d Исп.цепь %03d";

  static __flash char mess_mrv0[]="МРВ1 Реле: %01d            ";
  static __flash char mess_mrv1[]="МРВ2 Реле: %01d            ";
  static __flash char mess_none[]="Нет исполнительного реле";
//  static __flash char mess_del[]= "Задержка %03d с"; //75-77

  unsigned int zonepoint=0xffff;
  char zonetimeout;
  char zoneminfire;

  char zone=1;
  char nzp=0;
  char c;
  char pos=6;
  char p,s,r;
  char writeflag;
  char pus,np;
  char bra;
  char zdelay;

  TestPWD(40);
  do
  {
    writeflag=0;
    nzp&=3;
    zonepoint=ZONEPOINT_AREA[(zone<<2)+nzp];
    zonetimeout=ZONETO_AREA[zone];
    zoneminfire=(ZONEMF_AREA[zone]&3)+1; //Только младшие 3 бита от 1 до 4
    zdelay=ZONEDELAY_AREA[(zone<<2)+nzp];
    ClrScr();
    sprintf_P(CON,mess1,zone,ZONEMF_AREA[zone]&0x80?'+':'-',zoneminfire,zonetimeout,nzp+1);
    if (zonepoint==0xffff)
    {
      sprintf_P(CON+40,mess_none);
    }
    else
    {
      switch (zonepoint>>14)
      {
      case 0:
      case 1:
        // brvl
        sprintf_P(CON+40,mess_brvl,zonepoint>>9,(zonepoint&0x1f8)>>3,zonepoint&7);
        break;
      case 2:
        // brvu
        sprintf_P(CON+40,mess_brvu,(zonepoint>>8)&0x3f,zonepoint&255);
        break;
      case 3:
        // mrv
        if (zonepoint&0x2000)
        {
          sprintf_P(CON+40,mess_bra,(zonepoint>>8)&0x1f,zonepoint&255);
          break;
        }
        zonepoint&=0xc00f;
        sprintf_P(CON+40,zonepoint&8?mess_mrv1:mess_mrv0,zonepoint&7);
        break;
      }
//    sprintf_P(CON+66,mess_del,zdelay);
    }
    r=zonepoint&7;
    p=zonepoint>>9;
    if (zonepoint>0x8000) p=0;
    s=(zonepoint&0x1f8)>>3;
    pus=(zonepoint>>8)&0x3f;
    np=(zonepoint)&0xff;
    bra=(zonepoint>>8)&0x1F;
    CursorPos=pos;
    c=KBD(0);
    if ((c>=32 && c<128)||(c>=192))
    {
      //  Write symbol
      if (Edit_db(c,pos,6,&zone)) c=kRT;
      if (Edit_db(c,pos,13,&zoneminfire))
        if (PWDmode==3) {ZONEMF_AREA[zone]=(ZONEMF_AREA[zone]&0xFC)+((zoneminfire-1)&3); c=kRT;}
      if (Edit_db(c,pos,19,&zonetimeout))
        if (PWDmode==3) {ZONETO_AREA[zone]=zonetimeout; c=kRT;}
      if (Edit_db(c,pos,75,&zdelay))
        if (PWDmode==3) {if (zdelay>=0xFE) zdelay=0;ZONEDELAY_AREA[(zone<<2)+nzp]=zdelay; c=kRT;}
      if (Edit_dc(c,pos,24,&nzp)) c=kRT;
      switch (zonepoint>>14)
      {
      case 0:
      case 1:
        if (Edit_dc(c,pos,30+16,&p)) {c=kRT;writeflag=1;};
        if (Edit_dc(c,pos,38+16,&s)) {c=kRT;writeflag=1;};
        if (Edit_dc(c,pos,46+16,&r)) {c=kRT;writeflag=1;};
        break;
      case 2:
        if (Edit_dc(c,pos,29+16,&pus)) {c=kRT;writeflag=1;};
        if (Edit_db(c,pos,45+16,&np)) {c=kRT;writeflag=1;};
        break;
      case 3:
        if (zonepoint&0x2000)
        {
          if (Edit_dc(c,pos,33+16,&bra)) {c=kRT;writeflag=1;};
          if (Edit_db(c,pos,45+16,&np)) {c=kRT;writeflag=1;};
          break;
        }
        if (Edit_dc(c,pos,34+16,&r)) {c=kRT;writeflag=1;};
        break;
      }
    }
    switch(c)
    {
    case kLT:
      if (pos>40&&zonepoint!=0xffff)
        switch (zonepoint>>14)
        {
        case 0:
        case 1:
          switch (pos)
          {
          case 30+16: pos=24+16; break;
          case 38+16: pos=31+16; break;
          case 47+16: pos=39+16; break;
          case 75: pos=47+16; break;
          default: pos--;
          }
          break;
        case 2:
          switch (pos)
          {
          case 29+16: pos=24+16; break;
          case 45+16: pos=30+16; break;
          case 75: pos=47+16; break;
          default: pos--;
          }
          break;
        case 3:
          if (zonepoint&0x2000)
          {
            switch (pos)
            {
            case 33+16: pos=24+16; break;
            case 45+16: pos=34+16; break;
            case 75: pos=47+16; break;
            default: pos--;
            }
            break;
          }
          switch (pos)
          {
          case 75: pos=35+16; break;
          case 35+16: pos=24+16; break;
          default: pos--; break;
          }
          break;
        }
      else
        switch (pos)
        {
        case 6: c=kESC; break;
        case 13: pos=9; break;
        case 19: pos=15; break;
        case 25: pos=21; break;
        case 40: pos=25; break;
        default: pos--;
        }
      break;
    case kRT:
      if (pos>=40&&zonepoint!=0xffff)
        switch (zonepoint>>14)
        {
        case 0:
        case 1:
          switch (pos)
          {
          case 24+16: pos=30+16; break;
          case 31+16: pos=38+16; break;
          case 39+16: pos=47+16; break;
          case 47+16: /*pos=75;*/ break;
          case 77: break;
          default: pos++;
          }
          break;
        case 2:
          switch (pos)
          {
          case 24+16: pos=29+16; break;
          case 30+16: pos=45+16; break;
          case 47+16: /*pos=75;*/ break;
          case 77: break;
          default: pos++;
          }
          break;
        case 3:
          if (zonepoint&0x2000)
          {
            switch (pos)
            {
            case 24+16: pos=33+16; break;
            case 34+16: pos=45+16; break;
            case 47+16: /*pos=75;*/ break;
            case 77: break;
            default: pos++;
            }
            break;
          }
          switch (pos)
          {
          case 35+16: /*pos=75;*/ break;
          case 24+16: pos=35+16;
          case 77: break;
          default: pos++; break;
          }
          break;
        }
      else
        switch (pos)
        {
        case 40: break;
        case 25: pos=40; break;
        case 21: pos=25; break;
        case 15: pos=19; break;
        case 9: pos=13; break;
        default: pos++;
        }
      break;
    case kUP:
      if (pos>39) writeflag=1;
      if (pos==40)
      {
        p=0;
        s=0;
        r=0;
        if (zonepoint==0xffff) {zonepoint=0x0000; break;}
        if (zonepoint<0x8000) {zonepoint=0x8000; break;}
        if (zonepoint<0xc000) {s=0;zonepoint=0xc000; break;}
        if (zonepoint<0xc008) {s=1;zonepoint=0xc000; break;}
        if (zonepoint<0xE000) {zonepoint=0xE000; break;}
        zonepoint=0xffff; break;
      }
      Select_db(pos,6,1,&zone);
      Select_db(pos,13,1,&zoneminfire);
      Select_db(pos,19,1,&zonetimeout);
      Select_dc(pos,24,1,&nzp);
      Select_db(pos,75,1,&zdelay);
      if ((pos==9)&&(PWDmode>=2))
      {
        ZONEMF_AREA[zone]|=0x80; //Включаем зону
	Event2FIFO(0xFF,0xFF,0x27,zone);
      }
      if (zdelay>=0xFE) zdelay=0;
      if (PWDmode==3)
      {
        if (pos==13||pos==14||pos==15)
          ZONEMF_AREA[zone]=(ZONEMF_AREA[zone]&0xFC)+((zoneminfire-1)&3);
        if (pos==19||pos==20||pos==21) ZONETO_AREA[zone]=zonetimeout;
        if (pos==75||pos==76||pos==77) ZONEDELAY_AREA[(zone<<2)+nzp]=zdelay;
      }
      switch (zonepoint>>14)
      {
      case 0:
      case 1:
        Select_dc(pos,30+16,1,&p);
        Select_dc(pos,38+16,1,&s);
        Select_dc(pos,46+16,1,&r);
        break;
      case 2:
        Select_dc(pos,29+16,1,&pus);
        Select_db(pos,45+16,1,&np);
        break;
      case 3:
        if (zonepoint&0x2000)
        {
          Select_dc(pos,33+16,1,&bra);
          Select_db(pos,45+16,1,&np);
          break;
        }
        Select_dc(pos,34+16,1,&r);
        break;
      }
      break;
    case kDW:
      if (pos>39) writeflag=1;
      Select_db(pos,6,-1,&zone);
      Select_db(pos,13,-1,&zoneminfire);
      Select_db(pos,19,-1,&zonetimeout);
      Select_dc(pos,24,-1,&nzp);
      Select_db(pos,75,-1,&zdelay);
      if ((pos==9)&&(PWDmode>=2))
      {
        ZONEMF_AREA[zone]&=0x7F; //Выключаем зону
	Event2FIFO(0xFF,0xFF,0x26,zone);
      }
      if (zdelay>=0xFE) zdelay=0;
      if (PWDmode==3)
      {
        if (pos==13||pos==14||pos==15)
          ZONEMF_AREA[zone]=(ZONEMF_AREA[zone]&0xFC)+((zoneminfire-1)&3);
        if (pos==19||pos==20||pos==21) ZONETO_AREA[zone]=zonetimeout;
        if (pos==75||pos==76||pos==77) ZONEDELAY_AREA[(zone<<2)+nzp]=zdelay;
      }
      switch (zonepoint>>14)
      {
      case 0:
      case 1:
        Select_dc(pos,30+16,-1,&p);
        Select_dc(pos,38+16,-1,&s);
        Select_dc(pos,46+16,-1,&r);
        break;
      case 2:
        Select_dc(pos,29+16,-1,&pus);
        Select_db(pos,45+16,-1,&np);
        break;
      case 3:
        if (zonepoint&0x2000)
        {
          Select_dc(pos,33+16,-1,&bra);
          Select_db(pos,45+16,-1,&np);
        }
        Select_dc(pos,34+16,-1,&r);
        break;
      }
      break;
    }
    pus&=63;
    p&=63;
    s&=63;
    r&=7;
    if (zonepoint!=0xffff)
    {
      if (zonepoint>=0xE000)
      {
        zonepoint=0xE000;
        zonepoint|=((unsigned int)bra<<8)|((unsigned int)np);
        goto L1;
      }
      if (zonepoint>0x8000) zonepoint&=0xc000; else zonepoint&=0x8000;
      if (zonepoint==0x8000)
      {
        zonepoint|=((unsigned int)pus<<8)|((unsigned int)np);
      }
      else
      {
        zonepoint|=((unsigned int)p<<9)|((unsigned int)s<<3)|(unsigned int)r;
      }
    }
  L1:
    if (writeflag)
    {
      if (PWDmode==3)
      {
        ZONEPOINT_AREA[(zone<<2)+nzp]=zonepoint;
      }
    }
  }
  while (c!=kESC);
}

