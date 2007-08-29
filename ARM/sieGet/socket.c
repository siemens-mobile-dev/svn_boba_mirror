/*
  Проект SieGet Downloader
                          */

#include "..\inc\swilib.h"
#include "socket.h"

//Макрос чтобы писать меньше было, вызов обработчика ошибки
#define SocketError(x, y) SUBPROC((void *)(x->onError), x, y)


typedef struct _SOCKQ
{
  SOCKDATAHANDLER *data;
  struct _SOCKQ *next;
} SOCKQ;

typedef struct _DATAQ
{
  void *data;
  int size;
  struct _DATAQ *next;
  struct _DATAQ *prev;
} DATAQ;

DATAQ *sendq = NULL;

SOCKQ *sock_top = NULL;

//Получить класс зарегистрированного сокета
SOCKDATAHANDLER *GetSocketHandle(int sock)
{
  SOCKQ *tmp_sock = sock_top;
  while(tmp_sock)
  {
    if (tmp_sock->data->sock==sock)
      return tmp_sock->data;
    tmp_sock = tmp_sock->next;
  }
  return 0;
}

//Проверить процесс (сокеты только в хелпере)
int CheckCepId()
{
  if (GBS_GetCurCepid()==MMI_CEPID) return 1;
  return 0;
}

//Соединить сокет по ip и порту
void SocketConnect(SOCKDATAHANDLER *sock, int ip, short port)
{
  if (CheckCepId())
  {
    SocketError(sock, SOCK_ERROR_INVALID_CEPID);
    return;
  }
  if(sock->sock<0)
  {
    SocketError(sock, SOCK_ERROR_INVALID_SOCKET);
    return;
  }

  SOCK_ADDR sa;
  sa.family=1;
  sa.port=htons(port);
  sa.ip=ip;
  if (connect(sock->sock,&sa,sizeof(sa))==-1)
  {
    SocketError(sock, SOCK_ERROR_CONNECTING);
    return;
  }
  sock->state = SOCK_CREATED;
}

//Отправить данные
void SocketSend(SOCKDATAHANDLER *sock, const char *data, int size)
{
  if (CheckCepId())
  {
    SocketError(sock, SOCK_ERROR_INVALID_CEPID);
    return;
  }

  int send_res = send(sock->sock, data, size, 0);
  if (send_res==-1)
  {
    SocketError(sock, SOCK_ERROR_SENDING);
    return;
  }
  /*
  if (send_res<size) // Если не весь блок передан, надо передавать через очередь. Пока оставлю на потом.
  {
  }
  */

}

//Закрыть сокет
void SocketClose(SOCKDATAHANDLER *sock)
{
  if (CheckCepId())
  {
    SocketError(sock, SOCK_ERROR_INVALID_CEPID);
    return;
  }
  if(sock->sock<0)
  {
    SocketError(sock, SOCK_ERROR_INVALID_SOCKET);
    return;
  }

  shutdown(sock->sock,2);
  closesocket(sock->sock);

  sock->state = SOCK_UNDEF;
}

//Удалить сокет из обработки
void SocketUnReg(SOCKDATAHANDLER *sock)
{
  SOCKQ *tmp_sock = sock_top;
  if (sock_top->data==sock)
  {
    sock_top = sock_top->next;
    mfree(tmp_sock);
  }
  else
  {
    SOCKQ *prev_sock = sock_top;
    tmp_sock = sock_top->next;
    while (tmp_sock)
    {
      if (tmp_sock->data==sock)
      {
        prev_sock->next = tmp_sock->next;
        mfree(tmp_sock);
      }
      prev_sock = tmp_sock;
      tmp_sock = tmp_sock->next;
    }
  }
}

//Создать сокет и поместить его в очередь обработки
void SocketCreate(SOCKDATAHANDLER *sock)
{
  if (CheckCepId())
  {
    SocketError(sock, SOCK_ERROR_INVALID_CEPID);
    return;
  }

  sock->sock = socket(1,1,0);
  if(sock->sock<0)
  {
    SocketError(sock, SOCK_ERROR_CREATING);
    return;
  }

  sock->state = SOCK_CREATED;

  SOCKQ *tmp_sock = malloc(sizeof(SOCKQ)); //Создали сокет, добавляем его в список
  tmp_sock->data = sock;
  tmp_sock->next = sock_top;
  sock_top = tmp_sock;

  SUBPROC((void *)(sock->onCreate), sock);
}

//---------------------------------------------------------------
//Обработка сообщений, связанных с сокетом
int onSockEvent(int sock, int event)
{
  SOCKDATAHANDLER *hndl;
  if (hndl = GetSocketHandle(sock))
  {
    //Если наш сокет
    switch(event)
    {
    case ENIP_SOCK_CONNECTED: //Соединение через сокет установлено
      SUBPROC((void *)(hndl->onConnected), hndl);
      break;

    case ENIP_SOCK_DATA_READ: //Готовность данных к получению
      SUBPROC((void *)(hndl->onDataRead), hndl);
      break;

    case ENIP_SOCK_REMOTE_CLOSED: //Соединение разорвано сервером
      SUBPROC((void *)(hndl->onRemoteClose), hndl);
      break;

    case ENIP_SOCK_CLOSED: //Соединение разрвано клиентом
      SUBPROC((void*)(hndl->onClose), hndl);
      break;

    case ENIP_BUFFER_FREE: //Буфер отпраки пуст
      //To be implemented...
      break;

    case ENIP_BUFFER_FREE1: //Буфер отпраки пуст (в чем разница? - хз)
      //To be implemented...
      break;
    }

    return 0; //Сокет наш, мы его обработали
  }
  return 1; //Сокет чужой, пропускаем сообщение дальше по очереди
}
