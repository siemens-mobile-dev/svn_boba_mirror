#include "..\inc\swilib.h"
#include "urlstack.h"
#include "file_works.h"

static const int DMonth[]={0,31,59,90,120,151,181,212,243,273,304,334,365};

typedef struct
{
  unsigned long id;
  unsigned int  view_line;
  unsigned int  pos_cur_ref;
}tPageStack;

tPageStack PageSTACK[PageSTACK_SIZE];

static int stack_top;

static char * pagename(int n)
{
  char *s=malloc(300);
  char * omscache_path = getSymbolicPath("$omscache\\");
  sprintf(s,"%s%d.oms", omscache_path, PageSTACK[n].id);
  mfree(omscache_path);
  return s;
}

char* getCurrPageName()
{
  return pagename(stack_top-1);
}

void* getPageParams(void)
{
  return &(PageSTACK[stack_top]);
}

void setPageParams(unsigned int view_line, unsigned int pos_cur_ref)
{
  PageSTACK[stack_top-1].view_line = view_line;
  PageSTACK[stack_top-1].pos_cur_ref = pos_cur_ref;
}

unsigned int getViewLine(void* data)
{
  return ((tPageStack*)(data))->view_line;
}
unsigned int getPosCurRef(void* data)
{
  return ((tPageStack*)(data))->pos_cur_ref;
}

static void killpage(int n)
{
  unsigned int err;
  char *fn=pagename(n);
  unlink(fn,&err);
  PageSTACK[n].id=0;
  PageSTACK[n].view_line=0;
  mfree(fn);
}

char *PushPageToStack(void)
{
  unsigned long iday;
  TTime tt;
  TDate dd;
  int i;
  GetDateTime(&dd,&tt);
  dd.year=dd.year%100;
  iday=365*dd.year+DMonth[dd.month-1]+(dd.day - 1);
  iday=iday+(dd.year>>2);
  if (dd.month>2||(dd.year&3)>0)
    iday++;
  iday=(tt.sec+60*(tt.min+60*(tt.hour+24* iday)));
  
  if (stack_top==PageSTACK_SIZE)
  {
    //Убиваем лишнее
    killpage(0);
    memcpy(PageSTACK,PageSTACK+1,(PageSTACK_SIZE-1)*sizeof(tPageStack));
    PageSTACK[--stack_top].id=0;
  }
  i=stack_top;
  //Убираем возможность пойти вперед ;)
  while(i!=PageSTACK_SIZE)
  {
    if (PageSTACK[i].id!=0)
    {
      killpage(i);
    }
    i++;
  }
  PageSTACK[stack_top].id=iday;
  PageSTACK[stack_top].view_line=0;
  return pagename(stack_top++);
}

char *PopPageFromStack(void)
{
  if (!stack_top) return NULL;
  return pagename(--stack_top);
}

int CheckPageStackTop(void)
{
  unsigned long retval = PageSTACK[stack_top].id;
  /*if (retval)
  {
    unsigned int err;
    FSTATS stat;
    char* toppagename = pagename(stack_top);
    if (GetFileStats(toppagename,&stat,&err)==-1) 
      retval = 0;
    mfree(toppagename);
  }*/
  return retval;
}

char *ForwardPageFromStack(void)
{
  if (stack_top==PageSTACK_SIZE) return NULL;
  if (!PageSTACK[stack_top].id) return NULL;
  return pagename(++stack_top);
}

void FreePageStack(void)
{
  int i=0;
  while(i!=PageSTACK_SIZE)
  {
    if (PageSTACK[i].id!=0)
    {
      killpage(i);
    }
    i++;
  }  
}

void UpPageStack(void)
{
  if (stack_top==PageSTACK_SIZE) return;
  stack_top++;
}

void InitUrlStack(void)
{
  stack_top = 0;
  zeromem(PageSTACK, (PageSTACK_SIZE)*sizeof(tPageStack));
}
