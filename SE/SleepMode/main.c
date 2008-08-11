#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

#define ICONS_COUNT 20
#define FONTS_COUNT 20
#define WINDOWS_COUNT 4
#define DEFAULT_FONTS 0x55
#define color clBlack

typedef struct
{
  int top;
  int height;
}WND;

int missed_icons[ICONS_COUNT];
int fonts[FONTS_COUNT];
WND windows[WINDOWS_COUNT];
int Offset1=0x146;
int Offset2=0x14C;
int DISPLAY_WIDTH;

// пока так.. руки до констант не доходят))
int hook_ev=0x00000851; //UI_SLEEPMODE_ACTIVATED_EVENT

void elf_exit(void)
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}


const int Start=1900;
const int YEAR_0[]={0,31,60,91,121,152,182,213,244,274,305,335};
const int YEAR_1[]={366,397,425,456,486,517,547,578,609,639,670,700};
const int YEAR_2[]={731,762,790,821,851,882,912,943,974,1004,1035,1065};
const int YEAR_3[]={1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430};
wchar_t * days[][7]={L"Воскресенье",
L"Понедельник",
L"Вторник",
L"Среда",
L"Четверг",
L"Пятница",
L"Суббота"
};

int h2i(char * h)
{
  char c;
  int res=0;
  while((*h++)!='x');
  do
  {
    c=*h++;
    if (c>0x60) c-=0x20; else c-=0x30;
    if (c>9) c-=7;
    res<<=4;
    res|=c;
  }
  while(*h!=';');
  return(res);
}

// в проше не нашел..(
int GetDay(DATE *dt)
{
  int Y=dt->year;
  int HiYear = (Y/4)*4;
  int FYear = ((HiYear-Start)/4)*(366+(365*3));
  switch (Y - HiYear)
  {
  case 0: FYear = FYear + YEAR_0[dt->mon-1]; break;
  case 1: FYear = FYear + YEAR_1[dt->mon-1]; break;
  case 2: FYear = FYear + YEAR_2[dt->mon-1]; break;
  case 3: FYear = FYear + YEAR_3[dt->mon-1]; break;
  }
  return(Str2ID(days[0][(FYear+dt->day-1)%7],0,SID_ANY_LEN));
}


void  DrawScreenSaver(DISP_OBJ *dobj,int r1 ,int r2,int r3)
{
  DATETIME dt;
  int SIDtime;
  
  
  char * p=(char*)dobj;
  if ((!p[Offset1])||((p[Offset1]&&(!p[Offset2]))))
  {
    REQUEST_DATEANDTIME_GET(SYNC,&dt);

    SIDtime=Time2ID(&dt.time,2,0);
    SetFont(fonts[0]);
    DrawString(SIDtime,2,1,windows[0].top,DISPLAY_WIDTH,windows[0].top+windows[0].height,20,0x05,color ,color);
    TextFree(SIDtime);
    
    
    SIDtime=Date2ID(&dt.date,0,1);
    SetFont(fonts[1]);
    DrawString(SIDtime,2,1,windows[1].top,DISPLAY_WIDTH,windows[1].top+windows[1].height,20,0x05,color ,color);
    TextFree(SIDtime);
    
    
    SetFont(fonts[2]);
    SIDtime=GetDay(&dt.date);
    
    DrawString(SIDtime,2,1,windows[2].top,DISPLAY_WIDTH,windows[2].top+windows[2].height,20,0x05,color ,color);
    TextFree(SIDtime);
    
    int missed[20];
    int *p=missed;
    int m=*MissedEvents();
    int i;
    for (i=0;i<ICONS_COUNT;i++)
    {
      if (m&(1<<i))
      {
        *p++=missed_icons[i];
        if (i<2) *p++=0x78000020;
      }
    }
    
    SIDtime=Str2ID(&missed[0],5,p-missed);
    DrawString(SIDtime,2,1,windows[3].top,DISPLAY_WIDTH,windows[3].top+windows[3].height,40,0x05,color ,color);
    TextFree(SIDtime);
    
  }
}

// берем значения из ини-файла
void get_ini_key(void)
{
  int file;
  int i;
  char * buf;
  if ((file=_fopen(GetDir(DIR_INI),L"SleepMode.ini",0x001,0x180,0))>=0)
  {
    FSTAT _fstat;
    if (fstat(GetDir(DIR_INI),L"SleepMode.ini",&_fstat)==0)
    {
      buf=(char*)malloc(_fstat.fsize+1);
      fread(file,buf,_fstat.fsize);
      char * param;
      // иконки пропущенных
      for (i=0;i<ICONS_COUNT;i++)
      {
        char key[50];
        sprintf(key,"[ICON%.02d]",i);
        if (param=manifest_GetParam(buf,key,0))
        {
          missed_icons[i]=h2i(param)|0x78000000;
          mfree(param);
        }        
        else
        {
          missed_icons[i]=0x7800FFFF;
        }
      }

      // фонты
      for (i=0;i<FONTS_COUNT;i++)
      {
        char key[50];
        sprintf(key,"[FONT%.02d]",i);
        if (param=manifest_GetParam(buf,key,0))
        {
          fonts[i]=h2i(param);
          mfree(param);
        }        
        else
        {
          fonts[i]=DEFAULT_FONTS;
        }
      } 
      
      // окошки
      for (i=0;i<WINDOWS_COUNT;i++)
      {
        char key[50];
        sprintf(key,"[WIN%.02d.TOP]",i);
        if (param=manifest_GetParam(buf,key,0))
        {
          windows[i].top=h2i(param);
          mfree(param);
        }        
        sprintf(key,"[WIN%.02d.HEIGHT]",i);
        if (param=manifest_GetParam(buf,key,0))
        {
          windows[i].height=h2i(param);
          mfree(param);
        }        
        if (param=manifest_GetParam(buf,"[OFFSET1]",0))
        {
          Offset1=h2i(param);
          mfree(param);
        }  
        if (param=manifest_GetParam(buf,"[OFFSET2]",0))
        {
          Offset2=h2i(param);
          mfree(param);
        }        
        if (param=manifest_GetParam(buf,"[EVENT]",0))
        {
          hook_ev=h2i(param);
          mfree(param);
        }         
        
      }       
      mfree(buf);
    }
    fclose(file);
  }
}

int onSleepModeActivate (UI_MESSAGE*)
{
  // после создания SleepMode перечитываем ини и заменяем onRedraw
  BOOK * ScreenSaverBook = FindBook(isScreenSaverBook());
  if (ScreenSaverBook)
  {
    get_ini_key();
    GUI * g=(GUI*)ScreenSaverBook->gui;
    DISP_DESC_SetOnRedraw(DISP_OBJ_GetDESC(g->DISP_OBJ),DrawScreenSaver);
  }
  return(0);
}

// для выхода по "громкость вверх" при отладке...
int NewKey(int key, int r1 , int mode)
{
  if (mode==KBD_SHORT_PRESS)
  {
    if (key==0x11)
    {
      ModifyKeyHook(NewKey,0);
      ModifyUIHook(hook_ev,onSleepModeActivate,0);
      SUBPROC(elf_exit);
      return(0);
    }
  }
  return(0);
}

int main()
{
  DISPLAY_WIDTH=Display_GetWidth(0)-1;
  get_ini_key();
  ModifyUIHook(hook_ev,onSleepModeActivate,1);
  //ModifyKeyHook(NewKey,1);
  return 0;
}
