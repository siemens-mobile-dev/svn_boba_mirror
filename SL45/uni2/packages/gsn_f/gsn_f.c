#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"

//mamaich flasher, see in projects
static const unsigned char far Code[]=
#include "flasher\boot.h"
                                                                   
void Flasher(void)
{
 int far*Ptr=(int far*)0x15100;
 int far*Ptr1=(int far*)Code;
 int i;
 for(i=0; i<sizeof(Code)/2+1; i++)
 {
  *Ptr=*Ptr1;
  Ptr++; Ptr1++;
 }
}

void GSN_F(void)  
{        
 char far *R;
 R=GetATCmdLine();
 if (R[0]==':' && R[1]=='F') {
  SendStringToHost("\r\n");
  Flasher();
//  doIDLE();
  Set13MHz();
  #pragma asm
  calls 0D2h,0C122h ;jjPrint
  jmps  001h,05100h ;flashing mode
  #pragma endasm
 } else {
  #pragma asm
  extp  #2C3h,#2
  mov   r4, 3100h
  mov   r5, 3102h
  calls 0C7h,0EE88h ;__icall
  #pragma endasm
 }
}