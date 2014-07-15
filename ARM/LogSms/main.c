#include "..\inc\swilib.h"
#include "conf_loader.h"

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

static void RereadSettings()
{
  InitConfig();
}

static void FreeMem()
{
}

static char *UnpackNumber(char *s,const char **pp, unsigned int i){
  const char *p=*pp;
  unsigned int m=0;
  char c=0;
  char c1;
  while(m<i){
    if (m&1) c1=c>>4; else c1=(c=*p++)&0x0F;
    *s=c1+0x30; s++;
    m++;
  }
  *pp=p;
  return s;
}

static char Get7bitChar(const char **pp,unsigned int *ii,int *cc2){
  const char *p=*pp;
  unsigned i=*ii;
  int c2=*cc2;
  int c=0x80; //Символ
  do{
    if (!i) {c2=*p++; i=8;}
    c>>=1;
    if (c2&1) c|=0x80;
    c2>>=1; i--;
  }while(!(c&1));
  c>>=1;
  if (!c) c='@';
  if (c==2) c='$';
  *pp=p;
  *ii=i;
  *cc2=c2;
  return c;
}

static char *Unpack7bit(char *s,const char **pp, unsigned int i){
  unsigned int ii=0;
  int c2=0;
  while(i){
    *s=Get7bitChar(pp,&ii,&c2);
    s++;
    i--;
  }
  return s;
}

static char *UnpackSMS(const PDU *sms)
{
  char *s;
  const char *p=sms->data;
  char *ss=NULL;
  unsigned int i;
  unsigned int m;
  int c;
  int c1;
  int c2;
  unsigned int is_udhl;
  if ((*p++)!=0x11) return NULL;
  if ((*p++)!=0x11) return NULL;
  if (!(*p++)) return NULL;
  c=*p++; //Пропускаем SCA
  p+=c;
  is_udhl=*p&0x40;
  if ((*p++)&3) goto LERR; //Неподходящий тип сообщения ;)
  s=malloc(256);
  ss=s;
  i=*p++; //Длина номера в нибблах
  if (*p==0x91) {*s='+'; s++;}
  if ((*p++&0xD0)==0xD0){
    s=Unpack7bit(s,&p,(i+1)/2);
  }else{
    s=UnpackNumber(s,&p,i);
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
      s+=sprintf(s,"%u/%u ",c,c2);
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
      c=Get7bitChar(&p,&i,&c2);
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
LERR:
  return ss;
}

static void WriteLog(int dummy, char *text)
{
  unsigned int ul;
  extern const char LOGFILE[];
  if (!text) return;
  int f=fopen(LOGFILE,A_ReadWrite+A_Create+A_Append+A_BIN,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    fwrite(f,text,strlen(text),&ul);
    fclose(f,&ul);
  }
  mfree(text);
}

static int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  //Пришло сообщение
  if ((msg->msg&0xFF00)==0x6400) return 0;
  if ((msg->msg == MSG_SMS_RX)&&(msg->submess==0x26)&&(((((int)msg->data1)>>16)&0xFF)==0x20))
  {
    char *text=UnpackSMS(IncommingPDU());
    if (text) SUBPROC((void *)WriteLog,0,text);
  }else
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    FreeMem();
    RereadSettings();
  }
  return(old_icsm_onMessage(data, msg));
}

static void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

static void ElfKiller(void){
  extern void kill_data(void *p, void (*func_p)(void *));
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *data){
  SUBPROC((void *)ElfKiller);
}

static void maincsm_oncreate(CSM_RAM *data){
}

static unsigned short maincsm_name_body[140];
const int minus11=-11;

static const struct{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM ={
  {
    MyIDLECSM_onMessage,
    maincsm_oncreate,
#ifdef NEWSGOLD
    0,
    0,
    0,
    0,
#endif
    maincsm_onclose,
    sizeof(CSM_RAM),
    1,
    &minus11
  },{
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};

int main(void)
{
  RereadSettings();
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onMessage=MyIDLECSM_onMessage;
  icsmd.onClose=MyIDLECSM_onClose;
  icsm->constr=&icsmd;
  UnlockSched();
  return 0;
  
/*  RereadSettings();
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"LogSMS");
  CSM_RAM main_csm;
  LockSched();
  CSMROOT *csmr=CSM_root();
  CSM_RAM *save_cmpc=csmr->csm_q->current_msg_processing_csm;
  csmr->csm_q->current_msg_processing_csm=csmr->csm_q->csm.first;
  CreateCSM(&MAINCSM.maincsm,&main_csm,0);
  csmr->csm_q->current_msg_processing_csm=save_cmpc;
  UnlockSched();
  return 0;*/
}
