#include "..\include\Lib_Clara.h"
#include "..\include\dir.h"
#include "revision.h"
#include "book.h"
#include "Standby.h"
#include "conf_loader.h"
#include "config_data.h"
#include "Externals.h"
#include "string.h"
#include "structs.h"
#include "decoder.h"

u16 StartupTimer;

int StatusSize=18;
int SoftSize=26;

wchar_t extFolder[512];
wchar_t intFolder[512];
int YSize;

extern MUSIC nowmusic;

wchar_t *checkfile(wchar_t *name)
{
  FSTAT fst;
  if (fstat(extFolder,name,&fst)==0)
  {
    return extFolder;
  }
  else if (fstat(intFolder,name,&fst)==0)
  {
    return intFolder;
  }
  return extFolder;
};

void abc2ABC(char *ws)
{
  // Нормально работает только с английскими буквами и цифрами
  while (*ws) 
  {
    if (*ws>0x39)
    {
      *ws&=~0x20; 
    }
    *ws++;
  }
};

void GetSize()
{
  // Получаем размер статуса и софтов
  int a=Display_GetHeight(0);
  switch (a)
  {
  case 320:
    StatusSize=24;
    SoftSize=30;
    break;
  case 220:
    StatusSize=18;
    SoftSize=26;
    break;
  case 160:
    StatusSize=14;
    SoftSize=18;
    break;
  default:
    StatusSize=18;
    SoftSize=26;
    break;
  }
};

int Startup()
{
  // Функция запуска кавера
  Lang_Initialize();
  Anim_Init();
  CustomLbls();
  CreateCABook();
  Images_Load();
  GetSize();
  Standby_SetOnRedraw();
  if (AllowAnim==true)
  {
    locTime=AnimUpdateTime;
  }
  else
  {
    locTime=DefUpdateTime;
  }
  nowmusic.meta.HasImage=false;
  return 0;
};

void Startup_OnTimer(u16 timer,LPARAM lparam)
{
  if (StartupMode==1)
  {
    Startup();
    Timer_Kill(&StartupTimer);
  }
  else if (StartupMode==2)
  {
    if (FindBook(isAudioPlayerBook()))
    {
      Startup();
      Timer_Kill(&StartupTimer);
    }
    else
    {
      Timer_ReSet(&StartupTimer,1000,Startup_OnTimer,0);
    }
  }
};

int main()
{   
  if (FindBook(isCoverArtBook))
  {
    // Уже запущен кавер
    MessageBox(SID_NULL,Str2ID(lngRunned,0,SID_ANY_LEN),0,1,0,0);
    SUBPROC(CA_Kill);
  }
  else
  {
    wstrcpy(extFolder,GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG));
    wstrcat(extFolder,L"/CoverArt/");
    wstrcpy(intFolder,GetDir(MEM_INTERNAL+DIR_ELFS_CONFIG));
    wstrcat(intFolder,L"/CoverArt/");
    YSize=Display_GetHeight(0);
    InitConfig();
    switch (StartupMode)
    {
      case 0:
        Startup();
        break;
      case 1:
        StartupTimer=Timer_Set(60000, Startup_OnTimer, 0);
        break;
      case 2:
        StartupTimer=Timer_Set(1000, Startup_OnTimer, 0);
        break;
    }
  }
  return 0;
};
