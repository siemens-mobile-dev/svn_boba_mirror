#define color(x) (char *)(&(x))
#define allpoints(a) a.x, a.y, a.x2, a.y2

#include "netutils.h"

typedef struct
{
  char r;
  char g;
  char b;
  char a;
} RGBA;

void onFocus();
void RenderString(RECT *rc, char *str, int font, int t_attr, char *Pen, char *Brush);
int onStart(char *exename, char *fname);
void onCreate();
void onClose();
void onExit();
void onRedraw(void);

void sock_onCreate(void* sockdata);
void sock_onConnected(void* sockdata);
void sock_onDataRead(void* sockdata);
void sock_onClose(void* sockdata);
void sock_onRemoteClose(void* sockdata);
void sock_onError(void* sockdata, SOCK_ERROR err);
