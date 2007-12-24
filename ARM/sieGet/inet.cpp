/*
  Проект SieGet Downloader
                          */
#include "include.h"
#include "socket.h"
#include "inet.h"
#include "mem.h"
#include "log.h"

//Проверить процесс (сокеты только в хелпере)
inline int CheckCepId()
{
  if (GBS_GetCurCepid()==MMI_CEPID) return 1;
  return 0;
}

HttpAbstract::HttpAbstract()
{
  ip = 0;
  port = 0;
  recvbuf = NULL;
  req = NULL;

  http_state = HTTP_IDLE;
  Headers = NULL;
}

HttpAbstract::~HttpAbstract()
{
  if (req)
    delete req;
  if (Headers)
    delete Headers;
  if (recvbuf)
    delete recvbuf;
}

void HttpAbstract::Start(const char *_req, int _ip, short _port)
{
  DEBUG("HttpAbstract::Start()");

  if (req)
    delete req;
  if (Headers)
  {
    delete Headers;
  }
  if (recvbuf)
  {
    delete recvbuf;
  }

  req = new char[strlen(_req)+1];
  Headers = new HTTP_Response();
  recvbuf = new Buffer();
  strcpy(req, _req);
  ip = _ip;
  port = _port;
  doConnect();
}

void _do_connect(HttpAbstract *obj)
{
  obj->doConnect();
}

void HttpAbstract::doConnect()
{
  if (CheckCepId())
    SUBPROC((void *)_do_connect, this);
  else
    Create();
}

void HttpAbstract::onCreate()
{
  Connect(ip, port);
}

void HttpAbstract::onConnected()
{
  Send(req, strlen(req));
  delete req;
  req = NULL;

  http_state = HTTP_HEADER;
  onHTTPConnect();
}

void HttpAbstract::onDataRead()
{
  char tmpbuf[1024];
  int nrecv = Recv(tmpbuf, 1024);
  if (http_state==HTTP_HEADER)
  {
    recvbuf->Write(tmpbuf, nrecv);
    int hsize = Headers->Parse(recvbuf->data, recvbuf->size);
    if (hsize)
    {
      onHTTPHeaders();
      if (hsize<recvbuf->size)
      {
        onHTTPData(recvbuf->data+hsize, recvbuf->size-hsize);
      }
      http_state = HTTP_STREAM;
    }
  }
  else
  {
    onHTTPData(tmpbuf, nrecv);
  }
}

void HttpAbstract::onClose()
{
  onHTTPFinish();
  http_state = HTTP_IDLE;
}

void HttpAbstract::onRemoteClose()
{
  Close();
}

void HttpAbstract::onError(SOCK_ERROR err)
{
  DEBUG("HttpAbstract::onError()");
}

//---------------------------------------------------------------

void HttpHead::onHTTPHeaders()
{
  DEBUG("HttpHead::onHTTPHeaders()");
  char tmp[512];
  sprintf(tmp, "Got response %d", Headers->resp_code);
  Log::Active->PrintLn(tmp);
  if (Headers->resp_msg)
    sprintf(tmp, "Reason: %s", Headers->resp_msg);
  else
    sprintf(tmp, "Reason: %s", "[No reason specified]");
  Log::Active->PrintLn(tmp);
  Close();
}

