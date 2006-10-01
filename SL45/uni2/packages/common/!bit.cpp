#ifndef _BOOL_DEF
typedef int     bool;
#define false   0
#define False   false
#define FALSE   false
#define true    1
#define True    true
#define TRUE    true
#define _BOOL_DEF
#endif


bool GetBit(int index)
{ 
 return (bool)( (Buf[(index>>3)] )&( 0x01<<(index%8) ) );
}

//Установка i-го бита
void SetBit(int index, bool Value)
{
 int i;
 BYTE bitno;
 i=(index>>3);
 bitno=(0x01<<(index%8));
 if (Value)
  if(!(Buf[i]&(bitno)))
   Buf[i]=Buf[i]|(bitno);
  else
   if((Buf[i]&(bitno)))
    Buf[i]^=(bitno);
}
//---------------------------------------------------------------------------
