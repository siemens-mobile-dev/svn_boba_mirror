/*
  Проект SieGet Downloader
                          */

#include "..\inc\swilib.h"

#include "main.h"
#include "socket.h"
#include "inet.h"

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

int state = 0;
char *buf;
int bufsize;

void RecvProc(int res, void *data, int size)
{
  if (res)
  {
    state = 1;
    buf = data;
    bufsize = size;
  }
  else
    state = -1;
  REDRAW();
}

void RenderString(RECT *rc, char *str, int font, int t_attr, char *Pen, char *Brush)
{
  int len = strlen(str);
  WSHDR *ws = AllocWS(len);
  str_2ws(ws, str, len);
  DrawString(ws, rc->x, rc->y, rc->x2, rc->y2, font, t_attr, Pen, Brush);
  FreeWS(ws);
}

//------------------------------

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
  GetDataByReq((char *)req, IP_ADDR(81,19,70,3), 80, RecvProc);
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
  char *str;
  switch(state)
  {
  case 0:
    str = "waiting for result...";
    break;
  case 1:
    str = malloc(bufsize+1);
    memcpy(str, buf, bufsize);
    str[bufsize] = 0;
    break;
  case -1:
    str = "error";
    break;
  }
  RenderString(&Canvas, str, FONT_SMALL, 0, color(MAIN_TEXT), 0);
  if (state==1) mfree(str);
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



