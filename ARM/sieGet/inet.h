/*
  Проект SieGet Downloader
                          */

//inet.h
//Высокоуровневые функции для получения потока данных целиком по одному запросу

#ifndef _INET_H_
#define _INET_H_

#include "include.h"
#include "mem.h"
#include "socket.h"
#include "http.h"
#include "dns.h"
#include "url.h"
#include "log.h"

enum HTTP_STATE
{
  HTTP_IDLE,    // Простой
  HTTP_RESOLVE, // Получение IP
  HTTP_HEADER,  // Получение заголовка
  HTTP_STREAM,  // Получение данных
};

enum DOWNLOAD_STATE
{
  DOWNLOAD_WAITING,   // Ожидание
  DOWNLOAD_ERROR,     // Ошибка
  DOWNLOAD_CONNECT,   // Соединение
  DOWNLOAD_GET_INFO,  // Получение заголовка
  DOWNLOAD_DATA,      // Получение файла
  DOWNLOAD_COMPLETE,  // Успешно завершено
  DOWNLOAD_STOPPED    // Остановлено
};

#define ACTIVE_DOWNLOAD_STATE(x) ((x) == DOWNLOAD_WAITING      \
                                  || (x) == DOWNLOAD_CONNECT   \
                                  || (x) == DOWNLOAD_GET_INFO  \
                                  || (x) == DOWNLOAD_DATA)

class HttpAbstract;

class HttpDNR: public DNR
{
public:
  virtual void onResolve(DNR_RESULT_MSG result_msg, int value);
  void Bind(HttpAbstract * sock);
  HttpDNR();
private:
  HttpAbstract * Callback;
};

class HttpAbstract: public Socket
{
public:
  // Создание сокета
  virtual void onCreate();
  // Получение данных
  virtual void onDataRead();
  // Соединение
  virtual void onConnected();
  // Закрытие сокета
  virtual void onClose();
  virtual void onRemoteClose();
  // Обработчик ошибок
  virtual void onError(SOCK_ERROR err);

  // Вызывается при соединении
  virtual void onHTTPConnect() = 0;
  // Обработка заголовка HTTP
  virtual int onHTTPHeaders() = 0;
  // Обработка остальных данных (сохранение в файл)
  virtual void onHTTPData(char * data, int size) = 0;
  // Переадресация
  virtual void onHTTPRedirect() = 0;
  // Остановка
  virtual void onHTTPStopped()=0;
  // Окончание закачки
  virtual void onHTTPFinish()=0;
  
  void onResolve(DNR_RESULT_MSG result_msg, int value);
  // Соединение по IP
  void Start(int ip, short port);
  // Соединение по имени хоста
  void Start(const char * host, short port);
  
  void doConnect();
  
  HttpAbstract();
  ~HttpAbstract();
  
  // Запрос
  HTTP_Request * HTTPRequest;
  // Ответ
  HTTP_Response * HTTPResponse;
  // Состояние HTTP
  HTTP_STATE http_state;
  // Состояние закачки
  DOWNLOAD_STATE download_state;
  // Буфер запроса
  Buffer * req_buf;
private:
  // IP
  int ip;
  // Порт
  short port;
  // Буфер ответа
  Buffer * recv_buf;
  HttpDNR * Resolver;
};

class Download: public HttpAbstract
{
  virtual void onHTTPConnect() {};
  virtual int onHTTPHeaders();
  virtual void onHTTPData(char *data, int size);
  virtual void onHTTPRedirect();
  virtual void onHTTPStopped();
  virtual void onHTTPFinish();
public:
  Download();
  ~Download();

  void StartDownload();
  void StopDownload();
  
  char * url;             // URL
  char * file_name;       // Имя файла
  char * full_file_name;  // Полное имя файла
  char * file_path;       // Папка сохранения
  int file_size;          // Полный размер файла
  int file_loaded_size;   // Размер закачанной части
};

void _start_download(Download * download);

class DownloadHandler
{
public:
  DownloadHandler();
  ~DownloadHandler();
  
  // Добавить закачку
  void CreateDownload(Download * download);
  //Удалить закачку
  void DeleteDownload(Download * download);
  //Получить класс закачки по ID сокета
  Download * GetDownloadbyID(int socket_id);
  //Получить класс закачки по ее позиции в списке
  Download * GetDownloadbyN(int n);
  // Получить число закачек с определенным статусом
  int GetNumOfDownloadsByState(DOWNLOAD_STATE state);
  //Получить закачку по статусу
  Download * GetDownloadByState(DOWNLOAD_STATE state);

  int IsAnyDownloadActive();
  // Получить число всех закачек в очереди
  int GetNumOfDownloads();
  
  static DownloadHandler * Top;
  
  void SaveQueue();
  void LoadQueue();

private:
  // Собственно очередь сокетов
  struct DownloadQ
  {
    Download * download;
    DownloadQ * next;
  };
  DownloadQ * queue;
};

void _load_queue(DownloadHandler * dh);
void _save_queue(DownloadHandler * dh);

#endif
