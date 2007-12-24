/*
  Проект SieGet Downloader
                          */

//inet.h
//Высокоуровневые функции для получения потока данных целиком по одному запросу

#ifndef _INETAPI_H_
#define _INETAPI_H_

#include "include.h"
#include "mem.h"
#include "socket.h"
#include "http.h"

#define RECV_RESULT_ERROR 0
#define RECV_RESULT_OK 1

enum HTTP_STATE
{
  HTTP_IDLE,
  HTTP_HEADER,
  HTTP_STREAM
};

class HttpAbstract: public Socket
{
public:
  virtual void onCreate();
  virtual void onDataRead();
  virtual void onConnected();
  virtual void onClose();
  virtual void onRemoteClose();
  virtual void onError(SOCK_ERROR err);

  virtual void onHTTPConnect() = 0;
  virtual void onHTTPHeaders() = 0;
  virtual void onHTTPData(char *data, int size) = 0;
  virtual void onHTTPFinish() = 0;

  void Start(const char *req, int ip, short port);
  void doConnect();

  HttpAbstract();
  ~HttpAbstract();

  HTTP_Response *Headers;
  HTTP_STATE http_state;

private:
  int ip;
  short port;
  char *req;

  Buffer *recvbuf;
};

class HttpHead: public HttpAbstract
{
  virtual void onHTTPConnect() {};
  virtual void onHTTPHeaders();
  virtual void onHTTPData(char *data, int size) {};
  virtual void onHTTPFinish() {}; // Требует переопределения
};


#endif
