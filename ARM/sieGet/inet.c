/*
Проект SieGet Downloader
*/
#include "..\inc\swilib.h"
#include "socket.h"
#include "inet.h"

void HttpGet::Start(const char *req, int ip, short port)
{
  this->req = req;
  this->ip = ip;
  this->port = port;
  this->buf = new char[1024];
  this->buf_size = 1024;
  this->recvsize = 0;
  this->Connect(this->ip, this->port);
}

void HttpGet::onCreate()
{
}

void HttpGet::onConnected()
{
  this->Send(this->req, strlen(this->req));
}

void HttpGet::onDataRead()
{
  char *tmpbuf = new char[1024];
  int recvsize = Recv(tmpbuf, 1024);
  if (this->recvsize+recvsize>this->buf_size-4)
  {
    this->buf = (char *)realloc(this->buf, this->buf_size+1024);
    this->buf_size += 1024;
  }
  memcpy(this->buf+this->recvsize, tmpbuf, recvsize);
  this->recvsize += recvsize;
}

void HttpGet::onClose()
{
  this->body = this->buf;
  this->body_size = this->buf_size;
  this->onFinish(RECV_RESULT_OK);
}

void HttpGet::onRemoteClose()
{
  this->Close();
}

void HttpGet::onError(SOCK_ERROR err)
{
  this->onFinish(RECV_RESULT_ERROR);
}

HttpGet::HttpGet(SocketHandler *handler)
  :Socket(handler)
{

}

HttpGet::~HttpGet()
{

}


