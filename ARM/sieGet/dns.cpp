#include "..\inc\swilib.h"
#include "dns.h"

DNR *DNRHandler::GetDNR(int DNR_ID)
{
  DNRQ *tmp = queue;
  while (tmp)
  {
    if (tmp->dnr->DNR_ID==DNR_ID)
      return tmp->dnr; // Нашли!
  }
  return NULL; // Не нашли...
}

void DNRHandler::Add(DNR *dnr)
{
  DNRQ *tmp = new DNRQ;
  tmp->dnr = dnr;
  tmp->next = queue;
  queue = tmp;
}

void DNRHandler::Delete(DNR *dnr)
{
  if (!queue)
    return;
  DNRQ *tmp;
  if (queue->dnr==dnr)
  {
    tmp = queue;
    queue = queue->next;
    delete tmp;
  }
  else
  {
    DNRQ *prev;
    prev = queue;
    tmp = queue->next;
    while (tmp)
    {
      if (tmp->dnr==dnr)
      {
        prev->next = tmp->next;
        delete tmp;
        break;
      }
      prev = tmp;
      tmp = tmp->next;
    }
  }
}

void DNRHandler::Reg(DNR *dnr)
{
  dnr->DNR_ID = 0;
  Add(dnr);
  SendDNR(dnr);
}

void DNRHandler::SendDNR(DNR *dnr)
{
  int ***res = NULL;
  int err;
  err = async_gethostbyname(dnr->host, &res, &dnr->DNR_ID);
  if (err)
  {
    if ((err==0xC9)||(err==0xD6))
    {
      if (dnr->DNR_ID)
      {
        return; // Ждем готовности DNR
      }
    }
    else
    {
      dnr->onResolve(DNR_RESULT_ERROR, err);
      Delete(dnr); // Удаляем из списка
      return; // Получили ошибку
    }
  }
  if (res)
  {
    if (res[3])
    {
      dnr->onResolve(DNR_RESULT_OK, res[3][0][0]);
      Delete(dnr); // Удаляем из списка
      // Получили адрес
    }
  }
  else
  {
    dnr->DNR_TRIES--;
    if (!dnr->DNR_TRIES)
    {
      dnr->onResolve(DNR_RESULT_OUT_OF_TRIES, 0);
      Delete(dnr); // Удаляем из списка
      // Истекло количество попыток
    }
  }
}

void DNRHandler::onDNREvent(int DNR_ID)
{
  DNR *dnr = GetDNR(DNR_ID);
  if (dnr)
  {
    SendDNR(dnr);
  }
}

DNRHandler::DNRHandler()
{
  queue = NULL;
}

DNRHandler::~DNRHandler()
{
  DNRQ *tmp;
  while (tmp = queue)
  {
    queue = queue->next;
    delete tmp;
  }
}

//---------------------------------------------------------------


DNR::DNR(DNRHandler *handler)
{
  this->handler = handler;
}

void DNR::Start(const char *_host, int _tries)
{
  host = _host;
  DNR_TRIES = _tries;
  handler->Reg(this);
}
