#ifndef _DNS_H_
#define _DNS_H_

#define DNR_RESULT_OK 0
#define DNR_RESULT_ERROR 1
#define DNR_RESULT_OUT_OF_TRIES 2

typedef void (*DNRResultFunc)(void *handle);

typedef struct
{
  int DNR_ID;
  int DNR_TRIES;
  const char *host;
  DNRResultFunc CallbackProc; //Â HELPER
  int result;
  int value;
} DNRHANDLER;

void AddDNR(DNRHANDLER *handler);

#endif
