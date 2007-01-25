#ifndef _HISTORY_H_
  #define _HISTORY_H_

#include "SieJC.h"

void Add2History(CLIST *CListEx, char *header, char *message);
void LogStatusChange(CLIST *CListEx);

void Log(char* module, char* logstr);

#endif
