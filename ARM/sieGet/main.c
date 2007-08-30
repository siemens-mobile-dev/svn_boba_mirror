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
const char host[] = "borman.pocto.net";
const char path[] = "/webstat";

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

char log[2048] = "";
char req[1024] = "";

char *recvbuf;
int recvbufsize;

HTTP_RESPONSE http_h;

void ResolveProc(void *_handler);

DNRHANDLER DNRH =
{
  0,
  3,
  "",
  ResolveProc
};

void ResolveProc(void *_handler)
{
  char tmp[100];
  DNRHANDLER *handler = (DNRHANDLER *)_handler;
  int ip = handler->value;
  switch (handler->result)
  {
  case DNR_RESULT_OK:
    sprintf(tmp, "IP addr for %s is %d.%d.%d.%d\n", handler->host, ip&0xff, (ip>>8)&0xff, (ip>>16)&0xff, (ip>>24)&0xff);
    break;
  case DNR_RESULT_ERROR:
    sprintf(tmp, "DNR ERROR for %s is %d\n", handler->host, handler->value);
    break;
  case DNR_RESULT_OUT_OF_TRIES:
    sprintf(tmp, "DNR OUT OF TRIES (%s)\n", handler->host);
    break;
  }
  strcat(log, tmp);
  REDRAW();
  if (handler->result==DNR_RESULT_OK)
    SendRequest(handler->value);
}

void SendRequest(int ip)
{
  sprintf(req, _req, path, host);
  GetDataByReq(req, ip, 80, RecvProc);
  char tmp[100];
  sprintf(tmp, "Sent request to %s...\n", host);
  strcat(log, tmp);
  REDRAW();
}

void RecvProc(int res, void *data, int size)
{
  char tmp[512];
  switch(res)
  {
  case RECV_RESULT_OK:
    sprintf(tmp, "received %d bytes\n", size);
    strcat(log, tmp);
    recvbuf = data;
    ParseHeaders(data, size, &http_h);
    sprintf(tmp, "Response: %d (%s)\n", http_h.resp_code, http_h.resp_msg);
    strcat(log, tmp);
    sprintf(tmp, "Server: %s\n", http_h.resp_headers.Server);
    strcat(log, tmp);
    break;
  case RECV_RESULT_ERROR:
    strcpy(tmp, "error receiving\n");
    strcat(log, tmp);
    break;
  }
  REDRAW();
}

void onFocus()
{
  DisableIDLETMR();
}

int onStart(char *exename, char *fname)
{
  UpdateCSMName("SieGET");
  return 0;
}

void onCreate()
{
  char tmp[100];
  sprintf(tmp, "Send DNR for %s...\n", host);
  strcat(log, tmp);
  REDRAW();
  DNRH.host = host;
  AddDNR(&DNRH);
}

void onClose()
{
  mfree(recvbuf);
  FreeHTTPMem(&http_h);
}

void onExit()
{
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



