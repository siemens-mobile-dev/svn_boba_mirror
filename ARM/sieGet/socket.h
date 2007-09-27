/*
  Проект SieGet Downloader
                          */

//socketapi.h
//Высокоуровневый интерфейс обработки сокетов

#ifndef _SOCKETAPI_H_
#define _SOCKETAPI_H_

typedef enum
{
  SOCK_UNDEF,
  SOCK_CREATED,
  SOCK_CONNECTED
} SOCK_STATE;

typedef enum
{
  SOCK_ERROR_CREATING,
  SOCK_ERROR_CONNECTING,
  SOCK_ERROR_SENDING,
  SOCK_ERROR_CLOSING,
  SOCK_ERROR_INVALID_SOCKET,
  SOCK_ERROR_INVALID_CEPID
} SOCK_ERROR;

class Socket;
class SocketHandler;

// Класс сокета. Одноразовый.
class Socket
{
public:
  // Интерфейс сокета, должен быть реализован потомками
  virtual void onCreate() = 0;
  virtual void onDataRead() = 0;
  virtual void onConnected() = 0;
  virtual void onClose() = 0;
  virtual void onRemoteClose() = 0;
  virtual void onError(SOCK_ERROR err) = 0;

  //------------------------------

  //Создать сокет
  Socket(SocketHandler *handler);

  void Create();

  //Соединить сокет по ip и порту
  //ip должен иметь порядок байтов сети (htonl)
  void Connect(int ip, short port);

  //Отправить данные
  void Send(const char *data, int size);

  //Получить данные
  int Recv(char *data, int size);

  //Закрыть сокет
  void Close();

  SOCK_STATE GetState() const;

  ~Socket();

private:
  int id;
  char *senq_p;
  int sendq_l;
  SOCK_STATE state;
  SocketHandler *handler;

  friend class SocketHandler;
};

class SocketHandler
{
public:
  void Reg(Socket *sock);
  void UnReg(Socket *sock);
  void onSockEvent(int sock, int event);
  SocketHandler();
  ~SocketHandler();
private:
  struct SocketQ
  {
    Socket *sock;
    SocketQ *next;
  };
  SocketQ *queue;
  Socket *GetSocket(int sock);
};

#endif
