#include "..\inc\swilib.h"
#include "dns.h"

typedef struct _DNRQ
{
  DNRHANDLER *handler;
  struct _DNRQ *next;
} DNRQ;

DNRQ *dnr_top = NULL;

DNRHANDLER *GetHandlerByID(int DNR_ID)
{
  DNRQ *tmp = dnr_top;
  while (tmp)
  {
    if (tmp->handler->DNR_ID==DNR_ID)
      return tmp->handler; // Нашли!
  }
  return NULL; // Не нашли...
}

void AddHandler(DNRHANDLER *handler)
{
  DNRQ *tmp;
  tmp = malloc(sizeof(DNRQ));
  tmp->handler = handler;
  tmp->next = dnr_top;
  dnr_top = tmp;
}

void DeleteHandler(DNRHANDLER *handler)
{
  if (!dnr_top)
    return;
  DNRQ *tmp;
  if (dnr_top->handler==handler)
  {
    tmp = dnr_top;
    dnr_top = dnr_top->next;
    mfree(tmp);
  }
  else
  {
    DNRQ *prev;
    prev = dnr_top;
    tmp = dnr_top->next;
    while (tmp)
    {
      if (tmp->handler==handler)
      {
        prev->next = tmp->next;
        mfree(tmp);
        break;
      }
      prev = tmp;
      tmp = tmp->next;
    }
  }
}

void SendDNR(DNRHANDLER *handler);
void AddDNR(DNRHANDLER *handler);
int onDNREvent(int DNR_ID);

void AddDNR(DNRHANDLER *handler)
{
  AddHandler(handler);
  SUBPROC((void *)SendDNR, handler);
}

void SendDNR(DNRHANDLER *handler)
{
  int ***res = NULL;
  int err;
  err = async_gethostbyname(handler->host, &res, &handler->DNR_ID);
  if (err)
  {
    if ((err==0xC9)||(err==0xD6))
    {
      if (handler->DNR_ID)
      {
        return; // Ждем готовности DNR
      }
    }
    else
    {
      handler->result = DNR_RESULT_ERROR;
      handler->value = err;
      SUBPROC((void *)(handler->CallbackProc), handler);
      DeleteHandler(handler); // Удаляем из списка
      return; // Получили ошибку
    }
  }
  if (res)
  {
    if (res[3])
    {
      handler->result = DNR_RESULT_OK;
      handler->value = res[3][0][0];
      SUBPROC((void *)(handler->CallbackProc), handler);
      DeleteHandler(handler); // Удаляем из списка
      // Получили адрес
    }
  }
  else
  {
    handler->DNR_TRIES--;
    if (!handler->DNR_TRIES)
    {
      handler->result = DNR_RESULT_OUT_OF_TRIES;
      handler->value = 0;
      SUBPROC((void *)(handler->CallbackProc), handler);
      DeleteHandler(handler); // Удаляем из списка
      // Истекло количество попыток
    }
  }
}

int onDNREvent(int DNR_ID)
{
  DNRHANDLER *handler = GetHandlerByID(DNR_ID);
  if (handler)
  {
    SUBPROC((void *)SendDNR, handler);
    return 0;
  }
  else
    return 1;
}
