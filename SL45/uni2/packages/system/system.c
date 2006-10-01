#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"

void SetFreq(void)
{
 char freq=GetUniVars()->system_vars.freq;
 switch (freq) {
  case 0: return;
  case 1: if ((*((int far * far *)(0x37*0x4000L+0x327A)))[6]!=IDLEDIALOG_ID) {
           if (!Is26MHz) Set26MHz(); 
          } else { 
           if (IdleKeyFlag) if (Is26MHz) Set13MHz(); 
           else if (!Is26MHz) Set26MHz(); 
          } 
          return;
  case 2: if (MP3_Started) if (!Is26MHz) Set26MHz(); return;
 }

}
