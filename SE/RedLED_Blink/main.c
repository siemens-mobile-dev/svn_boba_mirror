#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

u16 timer;

BATT * bat_struct;

void onTimer(u16 unk,void *);
void BeginTimer(u16 unk,void *);

typedef struct
{
  BOOK * book;
}MSG;

BOOK * RLBook;

int TerminateElf(void * ,BOOK * book)
{
  RedLED_Off(0);
  FreeBook(book);
  return(1);
}

int ShowAuthorInfo(void *mess ,BOOK* book)
{
  MSG * msg = (MSG*)mess;
  MessageBox(0x6fFFFFFF,STR("Blinked Red LED\n(c) IronMaster"),0, 1 ,5000,msg->book);
  return(1);
}

int BeginTimer_event(void *,BOOK * RLBook)
{
  BeginTimer(0,RLBook);
  return(1);
}


void BeginTimer(u16 unk,void * RLBook)
{
  BATT bat_struct;
  GetBattaryState(SYNC,&bat_struct);
  if ((bat_struct.ChargingState!=7)&&(bat_struct.ChargingState!=8))
  {
    RedLED_On(0);
    Timer_ReSet(&timer,500,onTimer,RLBook);
  }
  else
  {
    timer=0;
  }
}

const PAGE_MSG HW_PageEvents[]@ "DYN_PAGE" ={
  ELF_TERMINATE_EVENT , TerminateElf,
  ELF_SHOW_INFO_EVENT  , ShowAuthorInfo,
  BATTERY_CHARGING_STATE_EVENT_TAG, BeginTimer_event,
  NIL_EVENT_TAG,0
};

PAGE_DESC base_page ={"RL_BasePage",0,HW_PageEvents};


void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}


void onTimer (u16 unk,void * RLBook)
{
  RedLED_Off(0);
  Timer_ReSet(&timer,500,BeginTimer,RLBook);
}

void onCloseRLBook(BOOK * RLBook)
{
  if (timer)
  {
    Timer_Kill(&timer);
  }
  SUBPROC(elf_exit);
}

BOOK * CreateRLBook()
{
  RLBook=new(BOOK);
  CreateBook(RLBook,onCloseRLBook,&base_page,"Blinked Red LED",-1,0);
  return(RLBook);
}


int myFind(BOOK* book)
{
   if(!strcmp(book->xbook->name,"Blinked Red LED")) return(1);
   return(0);
}


int main (void)
{
  if (FindBook(myFind))
  {
    MessageBox(0x6fFFFFFF,STR("Already runed"),0, 1 ,5000,0);
    SUBPROC(elf_exit);
  }
  else
  {
    CreateRLBook();
    BeginTimer(0,RLBook);
    return(0);
  }
}
