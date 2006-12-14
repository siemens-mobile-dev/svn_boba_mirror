#include "../inc/swilib.h"

#define DEFAULT_FOLDER "0:\\Zbin\\img\\"
#define CACHE_PNG 50

extern void kill_data(void *p, void (*func_p)(void *));
extern void(*OldOnClose)(void *);
extern void(*OldOnCreate)(void *);
extern IMGHDR* create_imghdr(const char* fname);

typedef struct
{
  char *pngname;
  IMGHDR * img;
}PNG_PICS;
  
typedef struct
{
  CSM_RAM csm;
  char dummy[0xC4-sizeof(CSM_RAM)];
  int f;
  unsigned int errno;
  PNG_PICS * png;
}IDLECSM;

#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}

int get_file_handler(void)
{
  IDLECSM *icsm=(IDLECSM *)FindCSMbyID(CSM_root()->idle_id);
  return (icsm->f);
}

void set_file_handler(int f)
{
  IDLECSM *icsm=(IDLECSM *)FindCSMbyID(CSM_root()->idle_id);
  icsm->f=f;
}

unsigned int* get_errno(void)
{
  IDLECSM *icsm=(IDLECSM *)FindCSMbyID(CSM_root()->idle_id);
  return (&(icsm->errno));
}

int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

int is_file_exist(const char* fname)
{
  unsigned int err;
  FSTATS fs;
  return (GetFileStats(fname,&fs,&err));
}
int get_number_of_png(PNG_PICS*p)
{
  int i=0;
  while (p[i].pngname&&i<CACHE_PNG)
  {
    i++;
  }
  return (i); 
}

void add_to_first(const char* fname,IMGHDR* img,PNG_PICS*p)   // Используется для добавления в начало списка новых пнг, с перемещением старых
{
  void * buf;
  int n;
  n=get_number_of_png(p);
  if (n==0) goto L_STORE;               // если до этого не было записей то просто пишем в первую
  if ((n+1)!=CACHE_PNG) goto L_MOVE;    // если были, но для одной записи места хватит
  if ((n+1)==CACHE_PNG)                 // для одной не хватит
  {
    mfree(p[n].img->bitmap);
    mfree(p[n].img);
    mfree(p[n].pngname);
    n--;
  }
L_MOVE:
  buf=malloc(n*sizeof(PNG_PICS));
  memcpy(buf,&(p[0]),n*sizeof(PNG_PICS));
  memcpy(&(p[1]),buf,n*sizeof(PNG_PICS));
  mfree(buf); 
L_STORE: 
  p[0].pngname=malloc(strlen(fname)+1);
  strcpy(p[0].pngname,fname);
  p[0].img=img;
  return;
}


// Используется для перемещения в начало списка старых пнг, с перемещением
// при перемещении нет мороки с вытеснением, так что тут все просто..
void move_to_first(int i,PNG_PICS*p)                   
{
  if (!i)  return;
  char* buf;
  buf=malloc((i+1)*sizeof(PNG_PICS));
  memcpy(buf,&(p[i]),sizeof(PNG_PICS));
  memcpy(buf+sizeof(PNG_PICS),&(p[0]),i*sizeof(PNG_PICS));
  memcpy(&(p[0]),buf,(i+1)*sizeof(PNG_PICS));
  mfree(buf);
}

// Ищем пнг в кеше
int find_png_in_cache(const char* fname,PNG_PICS*p)
{
  int i=0;
  while(p[i].pngname&&i<CACHE_PNG)
  {
    if (!strcmp_nocase(p[i].pngname,fname))  return i;
    else i++;
  }
  return -1;
}

// Пытаемся создать новую картинку
IMGHDR * try_to_create(const char* fname)
{
  IMGHDR * img;
  if (is_file_exist(fname)==-1) return (0);
  if (!(img=create_imghdr(fname))) return (0);
  return (img);
}



void MyIDLECSMonCreate(IDLECSM *icsm)
{
  void * buf=malloc(CACHE_PNG*sizeof(PNG_PICS));
  zeromem(buf,CACHE_PNG*sizeof(PNG_PICS));
  icsm->png=buf;
  kill_data(icsm,OldOnCreate);
  
}

void MyIDLECSMonClose(IDLECSM *icsm)
{
  mfree(icsm->png);  
  kill_data(icsm,OldOnClose);
}

IMGHDR* PatchGetPIT(unsigned int pic)
{
  IDLECSM *icsm;
  int i;
  IMGHDR * img;
  int idle_id=CSM_root()->idle_id;
  if (!idle_id) return 0;
  if (!(icsm=(IDLECSM *)FindCSMbyID(idle_id))) return 0;
  PNG_PICS*p;
  p=icsm->png;
  char fname[256];
  if ((pic>>28)==0xA)
  {
    strcpy(fname,(char*)pic);
  }
  else
  {
    sprintf(fname,DEFAULT_FOLDER"%u.png",pic);
  } 
  if ((i=find_png_in_cache(fname,p))==-1)
  {
    if (!(img=try_to_create(fname))) return 0;
    add_to_first(fname,img,p);
  }
  else
  {
    move_to_first(i,p);
  }
  return (p[0].img);   
}

//Патчи
#pragma diag_suppress=Pe177
__root static const int NEW_ONCREATE @ "PATCH_ONCREATE" = (int)MyIDLECSMonCreate;

__root static const int NEW_ONCLOSE @ "PATCH_ONCLOSE" = (int)MyIDLECSMonClose;

__root static const int NEW_CSMSIZE @ "PATCH_CSMSIZE" = sizeof(IDLECSM);
#pragma diag_default=Pe177


