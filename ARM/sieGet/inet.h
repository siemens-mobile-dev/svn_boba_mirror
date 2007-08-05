/*
  Проект SieGet Downloader
                          */

//inetapi.h
//Высокоуровневые функции для получения потока данных целиком по одному запросу

#ifndef _INETAPI_H_
#define _INETAPI_H_

typedef void (*RecvCallbackProc)(int res, void *data, int size);

//Загружает данные, получаемые после посылки запроса.
//Возвращает, успешно ли завершилась операция.
//Адрес буфера и его размер возвращаются в переменных dat и size.
void GetDataByReq(char *req, int ip, short port, RecvCallbackProc rproc);

#endif
