#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\cfg_items.h"
#include "config_data.h"
#include "conf_loader.h"

void (*OldonRedraw)(DISP_OBJ *,int r1,int r2,int r3);

#define ICONS_COUNT 8
#define color clBlack


#define KEYLOCK _T("KEYLOCK_SLEEPMODE_ICN")
#define SILENT _T("SILENT_SLEEPMODE_ICN")
#define CALLS_MISSED _T("CALLS_MISSED_SLEEPMODE_ICN")
#define SMS _T("SMS_SLEEPMODE_ICN")
#define EMAIL _T("MSG_UI_EMAIL_MESSAGE_SLEEPMODE_ICN")
#define MMS _T("MMS_NEW_MESSAGE_SLEEPMODE_ICN")
#define APPOINTMENT _T("APPOINTMENT_REMINDER_SLEEPMODE_ICN")
#define TASK _T("TASK_REMINDER_SLEEPMODE_ICN")

#define MON _T("CALE_MONDAY_TXT")
#define TUE _T("CALE_TUESDAY_TXT")
#define WEN _T("CALE_WEDNESDAY_TXT")
#define THU _T("CALE_THURSDAY_TXT")
#define FRI _T("CALE_FRIDAY_TXT")
#define SAT _T("CALE_SATURDAY_TXT")
#define SUN _T("CALE_SUNDAY_TXT")


wchar_t * icons[ICONS_COUNT]=
{
  KEYLOCK,
  SILENT,
  CALLS_MISSED,
  SMS,
  EMAIL,
  MMS,
  APPOINTMENT,
  TASK
};


wchar_t * days_str[7]=
{
  MON,
  TUE,
  WEN,
  THU,
  FRI,
  SAT,
  SUN
};

typedef struct
{
  BOOK book;
  GUI * SleepMode;
}ScrSavBook;


typedef struct
{
  BOOK * book;
}MSG;

BOOK * RLBook;

int missed_icons[ICONS_COUNT];
int days[7];
//3210
//int Offset1=0x21C;
//int Offset2=0x216;
//3150
//int Offset1=0x1D0;
//int Offset2=0x1CA;
//2020
int Offset1=0x14C;
int DISPLAY_WIDTH;


/*
const int Start=1900;
const int YEAR_0[]={0,31,60,91,121,152,182,213,244,274,305,335};
const int YEAR_1[]={366,397,425,456,486,517,547,578,609,639,670,700};
const int YEAR_2[]={731,762,790,821,851,882,912,943,974,1004,1035,1065};
const int YEAR_3[]={1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430};
*/

/*
wchar_t * days[7]={L"Воскресенье",
L"Понедельник",
L"Вторник",
L"Среда",
L"Четверг",
L"Пятница",
L"Суббота"
};
*/

/*
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
*/

void  DrawScreenSaver(DISP_OBJ *dobj,int r1 ,int r2,int r3)
{
  DATETIME dt;
  int SIDtime;
  char weekday;
  
  char * p=(char*)dobj;
  if ( p[Offset1] ) OldonRedraw(dobj,r1,r2,r3);
  else
  {
    REQUEST_DATEANDTIME_GET(SYNC,&dt);

    SIDtime=Time2ID(&dt.time,2,0);
    SetFont(font1);
    DrawString(SIDtime,2,1,y0,DISPLAY_WIDTH,y0+GetImageHeight(30),20,0x05,color ,color);
    TextFree(SIDtime);
    
    
    SIDtime=Date2ID(&dt.date,0,1);
    SetFont(font2);
    DrawString(SIDtime,2,1,y1,DISPLAY_WIDTH,y1+GetImageHeight(30),20,0x05,color ,color);
    TextFree(SIDtime);
    
    
    SetFont(font3);
    DATE_GetWeekDay(&dt.date,&weekday);
    SIDtime=days[weekday];
    
    DrawString(SIDtime,2,1,y2,DISPLAY_WIDTH,y2+GetImageHeight(30),20,0x05,color ,color);
    TextFree(SIDtime);
    
    int missed[ICONS_COUNT];
    int *p=missed;
    int m=*MissedEvents();
    int i;
    for (i=0;i<ICONS_COUNT;i++)
    {
      if (m&(1<<i))
      {
        *p++=missed_icons[i];
      }
    }

    
    int x = 0;
    for (i=0;i<(p-missed);i++)
    {
      x = x + GetImageWidth(missed[i]);
    }
    
    x = (DISPLAY_WIDTH - (x + (i-1)*10) )/2;
    for (i=0;i<(p-missed);i++)
    {
      GC_PutChar(get_DisplayGC(),x,y3,0,0,missed[i]);
      x = x + 10 + GetImageWidth(missed[i]);
    }
  }
}


int onSleepModeActivate (void * ,BOOK * book)
{
  // после создания SleepMode заменяем onRedraw
  BOOK * ScreenSaverBook = FindBook(isScreenSaverBook());
  if (ScreenSaverBook)
  {
    DISP_OBJ * g=GUIObj_GetDISPObj(((ScrSavBook*)ScreenSaverBook)->SleepMode);
    OldonRedraw=DISP_OBJ_GetOnRedraw(g);
    DISP_DESC_SetOnRedraw(DISP_OBJ_GetDESC(g),DrawScreenSaver);
  }
  return(0);
}


int TerminateElf(void * ,BOOK * book)
{
  FreeBook(book);
  return(1);
}


int ShowAuthorInfo(void *mess ,BOOK* book)
{
  MSG * msg = (MSG*)mess;
  MessageBox(EMPTY_SID,STR("SleepMode v1.1\n\n(c) Hussein"), NOIMAGE, 1, 5000,msg->book);
  return(1);
}


static int ReconfigElf(void *mess ,BOOK *book)
{
  RECONFIG_EVENT_DATA *reconf=(RECONFIG_EVENT_DATA *)mess;
  int result=0;
  if (wstrcmpi(reconf->path,successed_config_path)==0 && wstrcmpi(reconf->name,successed_config_name)==0)
  {
    InitConfig();
    result=1;
  }
  return(result);
}


const PAGE_MSG SM_PageEvents[]@ "DYN_PAGE" ={
  ELF_TERMINATE_EVENT, TerminateElf,
  ELF_SHOW_INFO_EVENT, ShowAuthorInfo,
  UI_SLEEPMODE_ACTIVATED_EVENT_TAG, onSleepModeActivate,
  ELF_RECONFIG_EVENT, ReconfigElf,
  NIL_EVENT_TAG,0
};

PAGE_DESC base_page ={"SM_BasePage",0,SM_PageEvents};


void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}


void onCloseSMBook(BOOK * SMBook)
{
  SUBPROC(elf_exit);
}


BOOK * CreateSleepModeBook()
{
  int i;
  int icon_id;
  BOOK * SM_Book=new(BOOK);

  // иконки пропущенных
  for (i=0;i<ICONS_COUNT;i++)
  {
    iconidname2id(icons[i],SID_ANY_LEN,&icon_id);
    missed_icons[i]=icon_id;
  }
  
  //Дни недели
  for (i=0;i<7;i++)
  {
    textidname2id(days_str[i],SID_ANY_LEN,&icon_id);
    days[i]=icon_id;
  }
  
  int platform=GetChipID()&CHIPID_MASK;
  
  if (platform==CHIPID_DB3150) Offset1=0x1D0;
  if (platform==CHIPID_DB3200||platform==CHIPID_DB3210) Offset1=0x21C;
  CreateBook(SM_Book,onCloseSMBook,&base_page,"SleepMode",-1,0);
  return(SM_Book);
}


int myFind(BOOK* book)
{
   if(!strcmp(book->xbook->name,"SleepMode")) return(1);
   return(0);
}


int main()
{
  if (FindBook(myFind))
  {
    MessageBox(EMPTY_SID,STR("Already runed"), NOIMAGE, 1, 5000,0);
    SUBPROC(elf_exit);
  }
  else
  {
    DISPLAY_WIDTH=Display_GetWidth(0)-1;
    InitConfig();
    CreateSleepModeBook();
    return(0);
  }
}
