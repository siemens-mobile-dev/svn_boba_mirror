#include "..\inc\swilib.h"
#include "inet.h"

extern char *strtok(char *s1, const char *s2);

GBSTMR reconnect_tmr;

#define TMR_SECOND 216

int is_gprs_online=0;

int is_handshaked=0;

int DNR_ID=0;
int DNR_TRIES=3;

int connect_state=0;

int sock=-1;

int sendq_l=0; //Длинна очереди для send
char *sendq_p=NULL; //указатель очереди

int recvq_l=0;
char *recvq_p=NULL;

const char HANDSHAKE_HOST[]="post.audioscrobbler.com";
const int HANDSHAKE_PORT=80;
char POST_HOST[256]="";
unsigned short POST_PORT=0;

void do_reconnect(void)
{
  void create_connect(void);
  if (is_gprs_online)
  {
    DNR_TRIES=3;
    SUBPROC((void*)create_connect);
  }
}

void create_connect(void)
{
  int ***p_res=NULL;
  void do_reconnect(void);
  const char *hostname;
  unsigned int ip;
  int err;

  SOCK_ADDR sa;
  //Устанавливаем соединение
  connect_state = 0;
  GBS_DelTimer(&reconnect_tmr);
  if (!IsGPRSEnabled())
  {
    is_gprs_online=0;
    return;
  }
  DNR_ID=0;
  *socklasterr()=0;
  hostname=is_handshaked?POST_HOST:HANDSHAKE_HOST;
  ip=str2ip(hostname);
  if (ip!=0xFFFFFFFF)  
  {
    sa.ip=ip;
    goto L_CONNECT;
  }  
  err=async_gethostbyname(hostname,&p_res,&DNR_ID); //03461351 3<70<19<81
  if (err)
  {
    if ((err==0xC9)||(err==0xD6))
    {
      if (DNR_ID)
      {
	return; //Ждем готовности DNR
      }
    }
    else
    {
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
      return;
    }
  }
  if (p_res)
  {
    if (p_res[3])
    {
      DNR_TRIES=0;
      sa.ip=p_res[3][0][0];
    L_CONNECT:
      sock=socket(1,1,0);
      if (sock!=-1)
      {
	sa.family=1;
	sa.port=htons(is_handshaked?POST_PORT:HANDSHAKE_PORT);
	if (connect(sock,&sa,sizeof(sa))!=-1)
	{
	  connect_state=1;
	}
	else
	{
	  closesocket(sock);
	  sock=-1;
	  GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
	}
      }
      else
      {
	//Не осилили создания сокета, закрываем GPRS-сессию
	GPRS_OnOff(0,1);
      }
    }	
  }
  else
  {
    DNR_TRIES--;
  }
}

void ClearSendQ(void)
{
  mfree(sendq_p);
  sendq_p=NULL;
  sendq_l=NULL;
}

void ClearRecvQ(void)
{
  mfree(recvq_p);
  recvq_p=NULL;
  recvq_l=NULL;
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

//Буферизированая посылка в сокет, c последующим освобождением указателя
void bsend(int len, void *p)
{
  int i;
  int j;
  if (connect_state<2)
  {
    mfree(p);
    return;
  }
  if (p)
  {
    //Проверяем, не надо ли добавить в очередь
    if (sendq_p)
    {
      //Есть очередь, добавляем в нее
      memcpy((sendq_p=realloc(sendq_p,sendq_l+len))+sendq_l,p,len);
      mfree(p);
      sendq_l+=len;
      return;
    }
    sendq_p=p;
    sendq_l=len;
  }
  //Отправляем уже существующее в очереди
  while((i=sendq_l)!=0)
  {
    if (i>0x400) i=0x400;
    j=send(sock,sendq_p,i,0);
    if (j<0)
    {
      j=*socklasterr();
      if ((j==0xC9)||(j==0xD6))
      {
	return; //Видимо, надо ждать сообщения ENIP_BUFFER_FREE
      }
      else
      {
	//Ошибка
	LockSched();
	ShowMSG(1,(int)"Send error!");
	UnlockSched();
	end_socket();
	return;
      }
    }
    memcpy(sendq_p,sendq_p+j,sendq_l-=j); //Удалили переданное
    if (j<i)
    {
      //Передали меньше чем заказывали
      return; //Ждем сообщения ENIP_BUFFER_FREE1
    }
  }
  mfree(sendq_p);
  sendq_p=NULL;
}

void get_answer(void)
{
  char rb[1024];
  int i=recv(sock,rb,sizeof(rb),0);
  if (i<0)
  {
    end_socket();
    return;
  }
  if (i==0) return;
  memcpy((recvq_p=realloc(recvq_p,recvq_l+i+1))+recvq_l,rb,i);
  recvq_l+=i;
}

int ParseSocketMsg(GBS_MSG *msg)
{
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    switch((int)msg->data0)
    {
    case LMAN_DISCONNECT_IND:
      is_gprs_online=0;
      return(1);
    case LMAN_CONNECT_CNF:
      is_gprs_online=1;
      GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*10,do_reconnect);
      return(1);
    case ENIP_DNR_HOST_BY_NAME:
      if ((int)msg->data1==DNR_ID)
      {
	if (DNR_TRIES) SUBPROC((void *)create_connect);
      }
      return(1);
    }
    if ((int)msg->data1==sock)
    {
      //Если наш сокет
//      if ((((unsigned int)msg->data0)>>28)==0xA)
//      {
//	//Принят пакет
//	ProcessPacket((TPKT *)msg->data0);
//	return(0);
//      }
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
	if (connect_state==1)
	{
	  //Соединение установленно, посылаем пакет login
	}
	else
	{
	}
	break;
      case ENIP_SOCK_DATA_READ:
	if (connect_state>=2)
	{
	  //Если посылали send
	  SUBPROC((void *)get_answer);
	  //REDRAW();
	}
	else
	{
	}
	break;
      case ENIP_BUFFER_FREE:
	if (!sendq_p)
	{
	  ShowMSG(1,(int)"Illegal ENIP_BUFFER_FREE!");
	  SUBPROC((void *)end_socket);
	}
	else
	{
	  //Досылаем очередь
	  SUBPROC((void *)bsend,0,0);
	}
	break;
      case ENIP_BUFFER_FREE1:
	if (!sendq_p)
	{
	  ShowMSG(1,(int)"Illegal ENIP_BUFFER_FREE1!");
	  SUBPROC((void *)end_socket);
	}
	else
	{
	  //Досылаем очередь
	  SUBPROC((void *)bsend,0,0);
	}
	break;
      case ENIP_SOCK_REMOTE_CLOSED:
	//Закрыт со стороны сервера
	if (connect_state)
	  SUBPROC((void *)end_socket);
	break;
      case ENIP_SOCK_CLOSED:
	connect_state=0;
	sock=-1;
	SUBPROC((void *)ClearSendQ);
	SUBPROC((void *)ClearRecvQ);
	GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
	break;
      }
    }
  }
  return(1);
}
