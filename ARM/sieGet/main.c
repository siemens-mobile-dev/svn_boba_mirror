/*
  Проект SieGet Downloader
                          */

#include "..\inc\swilib.h"

#include "main.h"
#include "socket.h"
#include "dns.h"

const char req[] = "HEAD / HTTP/1.1\r\n"
                   "Host: www.r0.ru\r\n"
                   "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
                   "Connection: close\r\n\r\n";

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
  str_2ws(ws, str, len);
  DrawString(ws, rc->x, rc->y, rc->x2, rc->y2, font, t_attr, Pen, Brush);
  FreeWS(ws);
}

//------------------------------

char log[1024] = "";

void ResolveProc(void *_handler);

DNRHANDLER DNRH =
{
  0,
  3,
  "r0.ru",
  ResolveProc
};

void ResolveProc(void *_handler)
{
  char tmp[100];
  DNRHANDLER *handler = (DNRHANDLER *)_handler;
  switch (handler->result)
  {
  case DNR_RESULT_OK:
    sprintf(tmp, "DNR OK\r\nIP addr for %s is %08X\r\n", handler->host, handler->value);
    break;
  case DNR_RESULT_ERROR:
    sprintf(tmp, "DNR ERROR for %s is %d\r\n", handler->host, handler->value);
    break;
  case DNR_RESULT_OUT_OF_TRIES:
    sprintf(tmp, "DNR ERROR for %s: [OUT OF TRIES]\r\n", handler->host);
    break;
  }
  strcat(log, tmp);
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
  AddDNR(&DNRH);
}

void onClose()
{
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



