//-----------------------------------
// Доступ к EEPROM процессора
//-----------------------------------
#include "ina90.h"
#include "pgmspace.h"
#include "iom103.h"

char ReadEEPROM(unsigned int adr) //Чтение байта
{
 while (EECR & 0x02);
 EEAR = adr;
 EECR = 0x01;
 return(EEDR); 
}

void WriteEEPROM(unsigned int adr, unsigned char val) //Запись байта
{
 unsigned char i;
 while (EECR & 0x02);
 EEAR = adr;
 EEDR = val;
 i=SREG;
 _CLI();
 EECR = 0x04;
 EECR = 0x02;
 SREG=i; 
}
