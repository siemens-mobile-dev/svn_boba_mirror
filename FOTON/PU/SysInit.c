//---------------------------------------------
// Инициализация аппаратуры
//---------------------------------------------
#ifdef PUA1
void InitSystem(void)
{
  MCUCR=0x38;

  DDRA=0xBF;
  PORTA=0x80;

  DDRB=0xF7;   //11110111;
  PORTB=0x2F;

  DDRC=0xFF;
  PORTC=0x00;

  DDRD=0xFD;
  PORTD=0x02;

  DDRE=0xFE;
  PORTE=0xFF;

  DDRF=0x00;
  PORTF=0x00;

  TCCR0=5;
  TCCR2=4;
  ASSR=8;
  TIMSK=0x40;  //0b01000001;
  EIMSK=0x00;
  SPCR=0x5C;
  UBRR=25;
}
#else
void InitSystem(void)
{
  MCUCR=0x38;

  DDRD=0xB9;
  PORTD=0xCA;

  DDRA=0xFF;
  PORTA=0x00;

  DDRC=0xFF;
  PORTC=0x00;

  DDRB=0xD7;   //11010111;
  PORTB=0x7F;

  DDRE=0xFE;
  PORTE=0xFF;

  TCCR0=5;
  TCCR2=4;
  ASSR=8;
  TIMSK=0x40;  //0b01000001;
  EIMSK=0x00;
  SPCR=0x5C;
  UBRR=25;
}
#endif
