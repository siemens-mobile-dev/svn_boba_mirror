
#include "include.h"
#include "string_util.h"
#include "log.h"

Log *Log::Active = NULL;

extern "C"
{
  void _C_LOG(char *str)
  {
    if (Log::Active)
      Log::Active->PrintLn(str);
  }
}
Log::Log()
{
  log_start = new LogLine;
  log_start->index = 0;
  log_start->str = AllocWS(256);
  ascii2ws(log_start->str, "--- LOG START ---");
  log_start->next = log_start;
  log_start->prev = log_start;
  Active = this;
}

Log::~Log()
{
  LogLine *tmp = log_start;
  while (tmp->next!=tmp)
  {
    tmp->next->prev = tmp->prev;
    tmp->prev->next = tmp->next;
    FreeWS(tmp->str);
    LogLine *tmp2 = tmp;
    tmp = tmp->next;
    delete tmp2;
  }
  FreeWS(tmp->str);
  delete tmp;
}

void Log::_add_line(WSHDR *ws)
{
  LogLine *tmp = new LogLine;
  tmp->str = ws;
  GetDateTime(&tmp->date, &tmp->time);
  LockSched();
  tmp->index = log_start->prev->index+1;
  tmp->prev = log_start->prev;
  log_start->prev->next = tmp;
  tmp->next = log_start;
  log_start->prev = tmp;
  UnlockSched();
}

void Log::PrintLn(char *str)
{
  WSHDR *ws = AllocWS(strlen(str)+4);
  ascii2ws(ws, str);
  _add_line(ws);
}
