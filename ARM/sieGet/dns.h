#ifndef _DNS_H_
#define _DNS_H_

#define DNR_RESULT_OK 0
#define DNR_RESULT_ERROR 1
#define DNR_RESULT_OUT_OF_TRIES 2

class DNR;
class DNRHandler;

class DNR
{
public:
  DNR(DNRHandler *handler);
  void Start(const char *host, int tries);
  virtual void onResolve(int result, int value) = 0; //Â HELPER
private:
  int DNR_ID;
  int DNR_TRIES;
  const char *host;
  DNRHandler *handler;

  friend class DNRHandler;
};

class DNRHandler
{
public:
  void Reg(DNR *dnr);
  void onDNREvent(int DNR_ID);
  DNRHandler();
  ~DNRHandler();
private:
  void SendDNR(DNR *dnr);
  void Add(DNR *dnr);
  DNR *GetDNR(int DNR_ID);
  void Delete(DNR *dnr);
  struct DNRQ
  {
    DNR *dnr;
    DNRQ *next;
  };
  DNRQ *queue;
};

#endif
