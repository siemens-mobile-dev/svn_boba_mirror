#include "sl45.h"

extern unsigned long curline;

extern unsigned char far dstk[];
extern unsigned char far ustk[];

extern unsigned int FL_loader(int fin, unsigned int p);
extern unsigned int FL_saver(int fs, unsigned int p);

unsigned int ConvertSimple(int fin,int fs)
{
 unsigned int s;
 unsigned int sym;
 unsigned int d=0;
 unsigned int i=0;
 unsigned int eolsymb=0;

 s=FL_loader(fin,0xFFFF);
 while((sym=dstk[s++]))
 {
   if (s>=8192) s=FL_loader(fin,s);
   switch(sym)
   {
    case 0x0D:
    case 0x0A:
     if (eolsymb==sym)
     {
	  goto LEOL;
     }
     if (eolsymb) break; //Пропустили второе окончание строки
     eolsymb=sym; //Определили конец строки
	 LEOL:
     curline++;
	 i=0; //Начало строки
	 ustk[d++]=0;
	 break;
	case 0x01:
    case 0x09:
     sym=' '; //Пока преобразовали табуляцию в пробел
    default:
	 ustk[d++]=sym;
     i++;
     if (i>255)
     {
       goto LEOL; //Слишком длинная строка
	 }
	 break;
   }
   if (d>=8192) d=FL_saver(fs,d);
 }
 if (i)
 {
   //Незавершенная строка
   ustk[d++]=0;
   curline++;
 }
 if (d>=8192) d=FL_saver(fs,d);
 return(d);
}





