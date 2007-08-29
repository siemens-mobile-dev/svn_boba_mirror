/*
Проект SieGet Downloader
*/
#include "..\inc\swilib.h"
#include "socket.h"
#include "inet.h"

typedef struct _SOCKHQ
{
  SOCKDATAHANDLER handler;
  struct _SOCKHQ *next;
  struct _SOCKHQ *prev;
} SOCKHQ;

typedef struct
{
  int ip;
  short port;
  const char *req;
  SOCKHQ *entry;
  char *buf;
  int bufsize;
  int recvsize;
  RecvCallbackProc rproc;
} CONNDATA;

void inet_onCreate(void* sockdata);
void inet_onConnected(void* sockdata);
void inet_onDataRead(void* sockdata);
void inet_onClose(void* sockdata);
void inet_onRemoteClose(void* sockdata);
void inet_onError(void* sockdata, SOCK_ERROR err);

SOCKDATAHANDLER stdhandle =
{
  -1,
  SOCK_UNDEF,
  inet_onCreate,
  inet_onDataRead,
  inet_onConnected,
  inet_onClose,
  inet_onRemoteClose,
  inet_onError
};

SOCKHQ *sock_q = NULL;

void GetDataByReq(const char *req, int ip, short port, RecvCallbackProc rproc)
{
  SOCKHQ *tmp_sock = malloc(sizeof(SOCKHQ));
  CONNDATA *tmp_cndata = malloc(sizeof(CONNDATA));

  tmp_cndata->req = req;
  tmp_cndata->ip = ip;
  tmp_cndata->port = port;
  tmp_cndata->rproc = rproc;
  tmp_cndata->entry = tmp_sock;

  tmp_sock->handler = stdhandle;
  tmp_sock->handler.data = tmp_cndata;

  tmp_sock->next = sock_q;
  if (sock_q) sock_q->prev = tmp_sock;
  tmp_sock->prev = NULL;
  SUBPROC((void *)SocketCreate, &(tmp_sock->handler));
}

void DisposeHandler(SOCKDATAHANDLER *sockdata)
{
  CONNDATA *cndata = sockdata->data;
  SOCKHQ *tmp_sock = cndata->entry;
  if (tmp_sock->next) tmp_sock->next->prev = tmp_sock->prev;
  if (tmp_sock->prev) tmp_sock->prev->next = tmp_sock->next;
  else
    sock_q = tmp_sock->next;
  mfree(cndata);
  mfree(tmp_sock);
}

void inet_onCreate(void* sockdata)
{
  SOCKDATAHANDLER *sd = sockdata;
  CONNDATA *cndata = sd->data;
  cndata->buf = malloc(1024);
  cndata->bufsize = 1024;
  cndata->recvsize = 0;
  SocketConnect(sockdata, cndata->ip, cndata->port);
}

void inet_onConnected(void* sockdata)
{
  SOCKDATAHANDLER *sd = sockdata;
  CONNDATA *cndata = sd->data;
  SocketSend(sockdata, cndata->req, strlen(cndata->req));
}

void inet_onDataRead(void* sockdata)
{
  SOCKDATAHANDLER *sd = sockdata;
  CONNDATA *cndata = sd->data;
  void *tmpbuf = malloc(1024);
  int recvsize = recv(sd->sock, tmpbuf, 1024, 0);
  if (cndata->recvsize+recvsize>cndata->bufsize-4)
  {
    cndata->buf = realloc(cndata->buf, cndata->bufsize+1024);
    cndata->bufsize += 1024;
  }
  memcpy(cndata->buf+cndata->recvsize, tmpbuf, recvsize);
  cndata->recvsize += recvsize;
}

void inet_onClose(void* sockdata)
{
  SOCKDATAHANDLER *sd = sockdata;
  CONNDATA *cndata = sd->data;
  SocketUnReg(sockdata);
  cndata->rproc(1, cndata->buf, cndata->recvsize);
  DisposeHandler(sockdata);
}

void inet_onRemoteClose(void* sockdata)
{
  SocketClose(sockdata);
}

void inet_onError(void* sockdata, SOCK_ERROR err)
{
  SOCKDATAHANDLER *sd = sockdata;
  CONNDATA *cndata = sd->data;
  SocketUnReg(sockdata);
  cndata->rproc(0, 0, 0);
  DisposeHandler(sockdata);
}

