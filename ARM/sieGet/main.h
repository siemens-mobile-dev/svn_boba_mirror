/*
  Проект SieGet Downloader
                          */

#ifndef _MAIN_H_
#define _MAIN_H_

#define color(x) (char *)(&(x))
#define allpoints(a) a.x, a.y, a.x2, a.y2

typedef struct
{
  char r;
  char g;
  char b;
  char a;
} RGBA;

void RenderString(RECT *rc, char *str, int font, int t_attr, char *Pen, char *Brush);
void Log(char *str);

void onFocus();
void onStart(char *exename, char *fname);
void onCreate();
void onClose();
void onExit();
void onRedraw(void);

void onDNREvent(int DNR_ID);
void InitDNR();
void StopDNR();
void SendDNR(char *host);

void onSockEvent(int sock, int event);
void InitSocket();
void StopSocket();
void SendRequest(int ip);

void ResolveProc(void *_handler);
void SendRequest(int ip);
void RecvProc(int res, void *data, int size);

#endif
