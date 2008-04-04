#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

BATT  bat;
u16   ws[193];
u16   timer; // ID таймера
BOOK * HWBook;

void elf_exit(void)
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

int TerminateElf(void * ,BOOK* book)
{
  FreeBook(book);
  return(1);
}

int NewKey(int key, int r1 , int mode)
{
  if (key==KEY_POWER)
  {
    if(mode==KBD_SHORT_PRESS)
      {
        ModifyKeyHook(NewKey,0);
        Timer_Kill(&timer);
        StatusIndication_ShowNotes(0x6FFFFFFF);
        SUBPROC(elf_exit);
        return(0);
      }   
  }
  return(0);
}

typedef struct
{
  BOOK * book;
}MSG;

int ShowAuthorInfo(void *mess ,BOOK* book)
{
  MSG * msg = (MSG*)mess;
  MessageBox(0x6fFFFFFF,STR("SystemTemp\n\n(C) GPcH"),0, 1 ,5000,msg->book);
  return(1);
}

const PAGE_MSG HW_PageEvents[]@ "DYN_PAGE" ={
  ELF_TERMINATE_EVENT , TerminateElf,
  ELF_SHOW_INFO_EVENT  , ShowAuthorInfo,
  0,0
};

PAGE_DESC base_page ={"HW_BasePage",0,HW_PageEvents};

void onCloseHWBook(BOOK * book)
{
  if (book)
  {
    ModifyKeyHook(NewKey,0);
    Timer_Kill(&timer);
    StatusIndication_ShowNotes(0x6FFFFFFF);
    SUBPROC(elf_exit);
  }
}

BOOK * CreateHWBook()
{
  HWBook= new BOOK;
  CreateBook(HWBook,onCloseHWBook,&base_page,"SystemTemp",-1,0);
  return(HWBook);
}

void onTimer (u16 *timerID , LPARAM lparam)
{
  GetBattaryState((int const*)1 , &bat);
  snwprintf(ws,196,(u16*)L"Temp = %d C",bat.SystemTemperature);
  StatusIndication_ShowNotes(Str2ID(ws,0,SID_ANY_LEN));
  Timer_ReSet(&timer,1000,onTimer,lparam);
}

int main(u16 *exename,u16 *filename)
{
  CreateHWBook();
  ModifyKeyHook(NewKey,1);
  timer=Timer_Set(1000,onTimer,0);
  return(0);
}
