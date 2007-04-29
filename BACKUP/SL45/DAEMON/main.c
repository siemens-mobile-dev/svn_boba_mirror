#include "sl45.h"
#include "reg167.h"

typedef unsigned short WORD;

typedef huge int p_onkey(DIALOG_WSK_RAM *data, struct MSG far *Msg);

struct DIALOG_WSK far my_dia;
p_onkey * far old_onkey;

#define SMS_buffer ((char far *)(0xCA9B8))

static const char smslog_name[32]="A:\\SMS\\sms.log";

void wrlog(char far *s)
{
 int f=FileOpen((STR)smslog_name,_O_CREAT+_O_RDWR+_O_APPEND+_O_BINARY,_S_IREAD);
 if (f!=-1)
 {
  FileWrite(f,s,strlen(s));
  FileClose(f);
 }
 EX_heap_free_with_lock(s);
}

int hook_onKey(DIALOG_WSK_RAM *data, struct MSG far *Msg)
{
 unsigned int m=Msg->Msg;
 if ((m==0x32)&&(Msg->Param[0]!=3))
 {
  //Получено CMC сообщение, разбираем
  char far *s;
  char far *p=SMS_buffer;
  char far *ss;
  unsigned int i;
  char c;
  char c1;
  char c2;
  unsigned int is_udhl;
  p++; //Пропускаем тип
  c=*p++; //Пропускаем SCA
  p+=c;
  is_udhl=*p&0x40;
  if ((*p++)&3) goto LERR; //Неподходящий тип сообщения ;)
  s=EX_heap_malloc_with_lock(256);
  ss=s;
  i=*p++; //Длина номера в нибблах
  if (*p++==0x91) {*s='+'; s++;}
  m=0;
  c=0;
  while(m<i)
  {
   if (m&1) c1=c>>4; else c1=(c=(*p++))&0x0F;
   *s=c1+0x30; s++;
   m++;
  }
  *s=' '; s++;
  //PID
  p++;
  //Кодировка
  c1=*p++;
  //Время/дата
  m=6;
  do
  {
   c=*p++;
   *s=(c&0x0F)+'0'; s++;
   *s=(c>>4)+'0'; s++;
   switch(m)
   {
    case 6:
	case 5:
	 c='-';
	 break;
    case 3:
	case 2:
	 c=':';
	 break;
	default:
	 c=' ';
   }
   *s=c; s++;
  }
  while(--m);
  p++; //Пропускаем TimeZone
  m=*p++; //Длинна текста
  if (is_udhl)
  {
   //Есть доп. заголовок, вычитываем
   is_udhl=*p; //Длина поля данных заголовка
   c=p[1];
   if ((c==0)||(c==8))
   {
    //Длинная смс, добавляем номер
	if (c)
	{
	 c=p[6];
	 c2=p[5];
    }
	else
	{
	 c=p[5];
	 c2=p[4];
    }
	s+=xsprintf(s,(STR)"%u/%u ",c,c2);
   }
   is_udhl++; //Длина заголовка с учетом байта длинны
   if (c1==8)
   {
    //Для сообщения в юникоде
    p+=is_udhl; //Указатель на текст
    m-=is_udhl; //Убрать длинну
	is_udhl=0; //Все уже пропущено
   }
   else
   {
    //Для сообщения 7-бит
	is_udhl=((is_udhl*8)+6)/7; //Преобразуем в число пропускаемых септетов
   }
  }
  i=0;
  c2=0;
  if (c1==8) m>>=1;
  while(m)
  {
   switch(c1)
   {
   default:
    //7 бит
	c=0x80; //Символ
	do
	{
	 if (!i) {c2=*p++; i=8;}
	 c>>=1;
	 if (c2&1) c|=0x80;
	 c2>>=1; i--;
	}
	while(!(c&1));
	c>>=1;
	if (!c) c='@';
	if (c==2) c='$';
	break;
   case 8:
    //Юникод
	i=(*p++)<<8;
	i|=(*p++);
	if ((i>=0x410)&&(i<0x450)) i-=0x350;
	c=i;
	break;
   }
   if (!c) c='?';
   if ((c==0x0A)||(c==0x0D)) c=' ';
   if (is_udhl) //Если необходимо пропустить символы
   {
    is_udhl--; //Пропускаем
   }
   else
   {
    *s=c; s++;
   }
   m--;
  }
  *s=13; s++;
  *s=0;
/*  char far *ss=EX_heap_malloc_with_lock(176);
  memcpy(ss,SMS_buffer,176);*/
  FilesysICall_FP(wrlog,ss);
 }
 LERR:
 return old_onkey(data,Msg);
}

void main(void)
{
 DIALOG_WSK_RAM far *dp;
 AcquireGbsLock();
 dp=FindDialogWithSoftKey_by_ID(IDLEDIALOG_ID);
 memcpy(&my_dia,dp->constr,sizeof(my_dia));
 old_onkey=my_dia.onKeyPress;
 my_dia.onKeyPress=hook_onKey;
 dp->constr=&my_dia;
 FreeGbsLock();
}
