//Полный подсчет за один раз
void PGMCRC(void)
{
  unsigned long p;
  char h12=0;
  char h0=0;
  char h1;
  char hl5;
  char hh5;

  p=0;
  do
  {
    h1=h12; 				//F  E  D  C  B  A  9  8
    h12=__swap_nibbles(h12);		//B  A  9  8  F  E  D  C
    hh5=h12;				//B  A  9  8  F  E  D  C
    h12^=h1;				//FB EA D9 C8 BF AE 9D 8C
    h12&=0xF0;				//FB EA D9 C8 _  _  _  _
    hh5&=0x0F;				//_  _  _  _  F  E  D  C
    h1^=hh5;				//F  E  D  C  FB EA D9 C8
    hl5=h12;				//FB EA D9 C8 _  _  _  _
    {
      unsigned int i=(hh5<<8)|hl5;	//_  _  _  _  F  E  D  C  FB EA D9 C8 _  _  _  _
      i<<=1;
      hh5=i>>8;			//_  _  _  F  E  D  C  FB
      hl5=(char)i;			//                        EA D9 C8 _  _  _  _  _
    }
    h1^=hl5; //crclo		//                                FEA ED9 DC8 C   FB  EA  D9  C8
    h12^=hh5; //crchi		//FB  EA  D9  FC8 E   D   C   FB

    h12^=h0;
#pragma diag_suppress=Pe1053
    h0=h1^(p<0x1FFFE?(*(char __farflash *)p):0);
#pragma diag_default=Pe1053
    p++;
#ifdef PUA1
    if (!((char)p))
    {
      if (PORTA&8) PORTA&=~8; else PORTA|=8;
    }
#endif
  }
  while(p<0x20000);
#pragma diag_suppress=Pe1053
  if(((h12<<8)|h0)!=*(unsigned int __farflash *)0x1FFFE)
  {
    //Ошибка контрольной суммы
    __disable_interrupt();
    __watchdog_reset();
    WDTCR=0x08;
    WDTCR=0x18;
    WDTCR=0x0A; //Разрешаем сторожевой таймер
    __watchdog_reset();
    for(;;);
  }
#pragma diag_default=Pe1053
}

//Проверка по блокам 256 байт
void PGMCRC256(void)
{
  unsigned long p;
  static unsigned long curp=0;
  static char sh12=0;
  static char sh0=0;

  char h12=sh12;
  char h0=sh0;
  char h1;
  char hl5;
  char hh5;

  p=curp;
  do
  {
    h1=h12; 				//F  E  D  C  B  A  9  8
    h12=__swap_nibbles(h12);		//B  A  9  8  F  E  D  C
    hh5=h12;				//B  A  9  8  F  E  D  C
    h12^=h1;				//FB EA D9 C8 BF AE 9D 8C
    h12&=0xF0;				//FB EA D9 C8 _  _  _  _
    hh5&=0x0F;				//_  _  _  _  F  E  D  C
    h1^=hh5;				//F  E  D  C  FB EA D9 C8
    hl5=h12;				//FB EA D9 C8 _  _  _  _
    {
      unsigned int i=(hh5<<8)|hl5;	//_  _  _  _  F  E  D  C  FB EA D9 C8 _  _  _  _
      i<<=1;
      hh5=i>>8;			//_  _  _  F  E  D  C  FB
      hl5=(char)i;			//                        EA D9 C8 _  _  _  _  _
    }
    h1^=hl5; //crclo		//                                FEA ED9 DC8 C   FB  EA  D9  C8
    h12^=hh5; //crchi		//FB  EA  D9  FC8 E   D   C   FB

    h12^=h0;
#pragma diag_suppress=Pe1053
    h0=h1^(p<0x1FFFE?(*(char __farflash *)p):0);
#pragma diag_default=Pe1053
    p++;
  }
  while((char)p);
  if (p&0x20000)
  {
#pragma diag_suppress=Pe1053
    if(((h12<<8)|h0)!=*(unsigned int __farflash *)0x1FFFE)
    {
      //Ошибка контрольной суммы
      __disable_interrupt();
      for(;;);
    }
#pragma diag_default=Pe1053
    p=0;
    sh12=0;
    sh0=0;
  }
  else
  {
    sh12=h12;
    sh0=h0;
  }
  curp=p;
}
