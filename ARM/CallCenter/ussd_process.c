#include "../inc/swilib.h"
#include "ussd_process.h"
#include "../inc/xtask_ipc.h"

//IPC
//const char ipc_my_name[32]="CallCenter";
//const char ipc_xtask_name[]=IPC_XTASK_NAME;
//IPC_REQ gipc;

int CASH_SIZE=0;
#define IMSI_DATA_BYTE_LEN  (9)

extern long  strtol (const char *nptr,char **endptr,int base);
char cashfname[128];

extern const char CASHREQNUM[];
extern const int ENA_CASHTRACE;
extern const char cashTEMP_PATH[];
extern const char cashLOG_FILE[];

int MaxCASH[MAX_CASH_SIZE];
int CurrentCASH[MAX_CASH_SIZE];

static GBSTMR ussd_tmr;

static volatile int ussdreq_sended;

static void WriteLog(int dummy, char *text)
{
  unsigned int ul;
  if (!text) return;
  int f=fopen(cashLOG_FILE,A_ReadWrite+A_Create+A_Append+A_BIN,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    fwrite(f,text,strlen(text),&ul);
    fclose(f,&ul);
  }
  mfree(text);
}

void SaveCash(void)
{
  int f;
  unsigned int ul; 
  unsigned char attrib;
  if (*cashfname)
  {
    if ((f=fopen(cashfname,A_ReadWrite+A_Create+A_BIN+A_Truncate,P_WRITE,&ul))!=-1)
    {
      fwrite(f,CurrentCASH,sizeof(CurrentCASH),&ul);      //by BoBa 4.07.07
      fwrite(f,MaxCASH,sizeof(MaxCASH),&ul);
      fclose(f,&ul);
      GetFileAttrib(cashfname,&attrib,&ul);
      attrib|=FA_HIDDEN;
      SetFileAttrib(cashfname,attrib,&ul);
    }
  }
}

/*
static void CheckPay(void)
{
  int i=0;
  int f=0;
  do
  {
    if (CurrentCASH[i]>MaxCASH[i])
    {
      MaxCASH[i]=CurrentCASH[i];
      f=1;
    }
    i++;
  }
  while(i<CASH_SIZE);
  if (f) SaveCash();
}
*/

extern const char CashPat0[];
extern const char CashPat1[];
extern const char CashPat2[];
extern const char CashPat3[];

//static 
const char * const patterns[MAX_CASH_SIZE]=
{
  CashPat0,
  CashPat1,
  CashPat2,
  CashPat3
};

static void FindCash(const char *s)
{
  int n=0; //Номер
  const char *pat;
  int i;
  int f=0;
  char *ep;
  //do
  while(n<CASH_SIZE)
  {
    pat=patterns[n];
    if (!*pat) break; //Больше паттернов нет
    if (!(s=strstr(s,pat))) break; //Что-то поломалось
    s+=strlen(pat);
    i=strtol(s,&ep,10)*100;
    s=ep;
    if ((*s=='.')||(*s==','))
    {
      if ((s[1]>='0')&&(s[1]<='9'))
      {
        s++;
        i+=strtol(s,&ep,10);
        s=ep;
      }
    }
    if (i>CurrentCASH[n]){     //by BoBa 4.07.07
      MaxCASH[n]=i;
      f=1;
    }
    CurrentCASH[n]=i;
    n++;
  }
  //while(n<CASH_SIZE);  Может быть ни одного пока идет поиск сети
  if (f) SaveCash();
}

static void ussd_timeout(void)
{
  ussdreq_sended=0;
}

int ProcessUSSD(CSM_RAM* data, GBS_USSD_MSG *msg)
{
  WSHDR *ws;
  int len;
  char *s;

#define ussdreqgui_id (((int *)data)[DISPLACE_OF_USSDREQGUI_ID/4])
  
  if (!ENA_CASHTRACE) return 0;
  if (!ussdreq_sended) return 0;
  EndUSSDtimer();
  ws=AllocWS(256);
  len=msg->pkt_length;
  if (len>240) len=240;
  GSMTXT_Decode(ws,msg->pkt,len,msg->encoding_type,(void*(*)(int))malloc_adr(),(void(*)(void))mfree_adr());
  if ((len=ws->wsbody[0]))
  {
    zeromem(s=malloc(len+3),len+3);
    len=0;
    while(len<ws->wsbody[0])
    {
      int c=ws->wsbody[len+1];
      if (c<32) c='?';
      if ((c>=0x410)&&(c<0x450))
	c-=0x350;
      else if (c>=0x80) c='?';
      s[len++]=c;
    }
    FindCash(s);
    s[len++]=13;
    s[len++]=10;
    SUBPROC((void *)WriteLog,0,s);
  }
  FreeWS(ws);
  GeneralFunc_flag1(ussdreqgui_id,0);
  ussdreqgui_id=0;
  return 1;
}

static void ussd_send(void)
{
//  gipc.name_to=ipc_xtask_name;
//  gipc.name_from=ipc_my_name;
//  gipc.data=0;
//  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_XTASK_IDLE,&gipc);
  if (IsGuiOnTop(((int *)FindCSMbyID(CSM_root()->idle_id))[DISPLACE_OF_IDLEGUI_ID/4]))
  {
    ussdreq_sended=1;
    MakeVoiceCall(CASHREQNUM,0x10,0x2FFF);
    GBS_StartTimerProc(&ussd_tmr,216*30,ussd_timeout);
  }
  else
  {
    SendCashReq();
  }
}

void SendCashReq(void)
{
  if (!ENA_CASHTRACE) return;
  if (ussdreq_sended) return; //Ужо летим ;)
  GBS_StartTimerProc(&ussd_tmr,216*3,ussd_send);
}

void EndUSSDtimer(void)
{
  GBS_DelTimer(&ussd_tmr);
  ussdreq_sended=0;
}

#pragma inline
int hex2alpha(int c)
{
  return (c<=9)?c+'0':c-0xA+'A';  
}

void hex2str(char *hex, char *str, int hexlen)
{
  unsigned int c, len;
  len=0;
  while(len<hexlen)
  {
    c=*hex++;
    *str++=hex2alpha(c>>4);
    *str++=hex2alpha(c&0xF);
    len++;
  }  
  *str=0;
}

void LoadCash(void)
{
  unsigned int ul;
  int s=0;
  char imsi[IMSI_DATA_BYTE_LEN*2+1];
 
  CASH_SIZE=0;
  
  extern char cur_imsi[];
  hex2str(cur_imsi,imsi,IMSI_DATA_BYTE_LEN);
  sprintf(cashfname,"%s\\CallCenter_cash_%s.tmp",cashTEMP_PATH,imsi);
  int f=fopen(cashfname,A_ReadOnly+A_BIN,P_READ,&ul);
  if (f!=-1)
  {
    s=fread(f,CurrentCASH,sizeof(CurrentCASH),&ul);     //by BoBa 4.07.07
    s=fread(f,MaxCASH,sizeof(MaxCASH),&ul);
    fclose(f,&ul);
  }
  if (s!=sizeof(MaxCASH))
  {
    memcpy(MaxCASH,CurrentCASH,sizeof(MaxCASH));
    SaveCash();
  }                    
  while ((CASH_SIZE<MAX_CASH_SIZE)&&(*patterns[CASH_SIZE])) {
   CASH_SIZE++;
  }
}  
