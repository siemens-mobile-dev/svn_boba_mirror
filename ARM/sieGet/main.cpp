/*
Проект SieGet Downloader
*/

#include "..\inc\swilib.h"

#include "main.h"
#include "socket.h"
#include "dns.h"
#include "inet.h"
#include "http.h"

const char _req[] = "HEAD %s HTTP/1.1\r\n"
"Host: %s\r\n"
"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
"Connection: close\r\n"
"\r\n";
const char host[] = "r0.ru";
const char path[] = "/";

extern RECT Canvas;
extern void UpdateCSMName(char *new_name);

//------------------------------

RGBA MAIN_BG =   {  0,   0,   0, 100};
RGBA MAIN_TEXT = {255, 255, 255, 100};

//-----------------------------

void RenderString(RECT *rc, char *str, int font, int t_attr, char *Pen, char *Brush)
{
  int len = strlen(str);
  WSHDR *ws = AllocWS(len);
  utf8_2ws(ws, str, len);
  DrawString(ws, rc->x, rc->y, rc->x2, rc->y2, font, t_attr, Pen, Brush);
  FreeWS(ws);
}

char log[4096] = "";
void Log(char *str)
{
  LockSched();
  strcat(log, str);
  int f;
  unsigned int ul;
  if ((f=fopen("4:\\ZBin\\var\\SieGet.log", A_ReadWrite+A_Create+A_Truncate, P_READ+P_WRITE, &ul))!=-1)
  {
    fwrite(f, log, strlen(log), &ul);
    fclose(f, &ul);
  }
  UnlockSched();
  REDRAW();
}

char req[1024] = "";

char *recvbuf;
int recvbufsize;

//---------------------------------------------------------------
// Обработка сообщений, связанных с сокетом

SocketHandler *MainSocketHandler;

void onSockEvent(int sock, int event)
{
  if (MainSocketHandler)
    MainSocketHandler->onSockEvent(sock, event);
}

void InitSocket()
{
  if (!MainSocketHandler)
    MainSocketHandler = new SocketHandler;
}

void StopSocket()
{
  if (MainSocketHandler)
    delete MainSocketHandler;
}
//---------------------------------------------------------------
// Обработка сообщений DNR

DNRHandler *MainDNRHandler;

void onDNREvent(int DNR_ID)
{
  if (MainDNRHandler)
    MainDNRHandler->onDNREvent(DNR_ID);
}

void InitDNR()
{
  if (!MainDNRHandler)
    MainDNRHandler = new DNRHandler;
}

void StopDNR()
{
  if (MainDNRHandler)
    delete MainDNRHandler;
}
//---------------------------------------------------------------
// Класс обработки DNR
class MyDNR: public DNR
{
public:
  virtual void onResolve(int result, int value);
  MyDNR(DNRHandler *handler): DNR(handler) {};
};

void MyDNR::onResolve(int result, int value)
{
  char tmp[100];
  switch (result)
  {
  case DNR_RESULT_OK:
    sprintf(tmp, "IP: %d.%d.%d.%d\n", value&0xff, (value>>8)&0xff, (value>>16)&0xff, (value>>24)&0xff);
    break;
  case DNR_RESULT_ERROR:
    sprintf(tmp, "DNR ERROR: %d\n", value);
    break;
  case DNR_RESULT_OUT_OF_TRIES:
    sprintf(tmp, "DNR OUT OF TRIES\n");
    break;
  }
  Log(tmp);
  REDRAW();
  if (result==DNR_RESULT_OK)
    SendRequest(value);
}
//---------------------------------------------------------------

// Класс сокета
class MySocket: public HttpGet
{
public:
  virtual void onFinish(int result);
  virtual void onEvent(int event, int data);
  MySocket(SocketHandler *handler): HttpGet(handler) {};
};

void MySocket::onFinish(int result)
{
  char tmp[512];
  HTTP_Response *http_resp = new HTTP_Response();
  switch(result)
  {
  case RECV_RESULT_OK:
    sprintf(tmp, "received %d bytes\n", body_size);
    strcat(log, tmp);
    recvbufsize = body_size+1;
    recvbuf = new char[recvbufsize];
    memcpy(recvbuf, body, body_size);
    recvbuf[body_size] = 0;
    Log("-=HTTP stream dump start=-\n");
    Log(recvbuf);
    Log("-=HTTP stream dump end=-\n");
    http_resp->Parse(recvbuf, recvbufsize);
    sprintf(tmp, "response: %d(%s)\n", http_resp->resp_code, http_resp->resp_msg);
    Log(tmp);
    char *srv = http_resp->headers->GetValue("Server");
    if (srv)
    {
      sprintf(tmp, "Server: %s\n", srv);
      Log(tmp);
    }
    break;
  case RECV_RESULT_ERROR:
    strcpy(tmp, "error receiving\n");
    Log(tmp);
    break;
  }
  REDRAW();
  delete http_resp;
}

void MySocket::onEvent(int event, int data)
{
  char tmp[512];
  sprintf(tmp, "!: %d(%d)", event, data);
  Log(tmp);
}

//---------------------------------------------------------------
// Объекты

MyDNR *dnr;
MySocket *sock;

void SendRequest(int ip)
{
  if (!sock)
    return;
  sprintf(req, _req, path, host);
  sock->Start(req, ip, 80);
  char tmp[100];
  sprintf(tmp, "Sent request to %s...\n", host);
  Log(tmp);
  REDRAW();
}

//---------------------------------------------------------------


void onFocus()
{
  DisableIDLETMR();
}

void onStartHelper()
{
  sock->Create();
}
void onStart(char *exename, char *fname)
{
  UpdateCSMName("SieGET");
  InitSocket();
  InitDNR();
  sock = new MySocket(MainSocketHandler);
  dnr = new MyDNR(MainDNRHandler);
  SUBPROC((void *)onStartHelper);
  return;
}

void onCreateHelper()
{
  dnr->Start(host, 3);
}
void onCreate()
{
  char tmp[100];
  sprintf(tmp, "Send DNR for %s...\n", host);
  Log(tmp);
  REDRAW();
  SUBPROC((void *)onCreateHelper);
}

void onClose()
{
  mfree(recvbuf);
}

void onExit()
{
  StopSocket();
  StopDNR();
  delete sock;
  delete dnr;
}

void onRedraw(void)
{
  DrawRectangle(allpoints(Canvas), 0, 0, color(MAIN_BG));
  RenderString(&Canvas, log, FONT_SMALL, 0, color(MAIN_TEXT), 0);
}


int onKey(unsigned char keycode, int pressed)
{
  switch(pressed){
  case KEY_UP: break;
  case LONG_PRESS:
  case KEY_DOWN:
    switch(keycode){
    case RIGHT_SOFT:
      return 1;
    case RED_BUTTON:
      return 1;
    case RIGHT_BUTTON:
      break;
    case LEFT_BUTTON:
      break;
    case ENTER_BUTTON:
      break;
    case UP_BUTTON:
      break;
    case DOWN_BUTTON:
      break;
    case '#':
      break;
    case '*':
      break;
    default: return 0;
    }
  }
  return 0;
}



