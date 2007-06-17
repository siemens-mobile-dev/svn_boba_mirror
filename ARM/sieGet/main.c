#include "..\inc\swilib.h"

#include "main.h"

const char req[] = "GET /wiki/HTTP HTTP/1.1\n"
                   "Host: ru.wikipedia.org\n"
                   "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\n"
                   "Connection: close\n\n";

extern RECT Canvas;

int nrecv = 0;

SOCKDATAHANDLER mysock =
{
  -1,
  SOCK_UNDEF,
  sock_onCreate,
  sock_onDataRead,
  sock_onConnected,
  sock_onClose,
  sock_onRemoteClose,
  sock_onError
};

//------------------------------

RGBA MAIN_BG = {0, 0, 0, 100};
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

void onFocus()
{
  DisableIDLETMR();
}

int onStart(char *exename, char *fname)
{
  return 0;
}

void onCreate()
{
  SUBPROC((void *)SocketCreate, &mysock);
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
  char str[200];
  sprintf(str, "socket state=%d, received %d", mysock.state, nrecv);
  RenderString(&Canvas, str, FONT_SMALL, 0, color(MAIN_TEXT), 0);
}


int onKey(unsigned char keycode, int pressed)
{
  switch(pressed){
  case KEY_UP: break;
  case LONG_PRESS:
  case KEY_DOWN:
    switch(keycode){
    case RED_BUTTON:
      return 1;
    case RIGHT_BUTTON:
      return 1;
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

//--------------------------------------

void sock_onCreate(void* sockdata)
{
  SocketConnect(sockdata, IP_ADDR(66,230,200,100), 80);
  REDRAW();
}

void sock_onConnected(void* sockdata)
{
  SocketSend(sockdata, (char *)req, strlen(req));
  REDRAW();
}

void sock_onDataRead(void* sockdata)
{
  char buf[1025];
  int size = recv(((SOCKDATAHANDLER *)sockdata)->sock, buf, 1024, 0);
  nrecv+=size;
  unsigned int ul;
  int f=fopen("4:\\http.data",A_ReadWrite+A_Create+A_Append,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    fwrite(f,buf,size,&ul);;
    fclose(f,&ul);
  }
  REDRAW();
}

void sock_onClose(void* sockdata)
{
  SocketUnReg(sockdata);
}

void sock_onRemoteClose(void* sockdata)
{
  SocketClose(sockdata);
  REDRAW();
}

void sock_onError(void* sockdata, SOCK_ERROR err)
{

}

