#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

u16 timer; // ID таймера
int lamp=0; // текуща€ €ркость лампы
BOOK * HWBook;

int TerminateElf(void * ,BOOK* book)
{
  FreeBook(book);
  return(1);
}

typedef struct
{
  BOOK * book;
}MSG;

int ShowAuthorInfo(void *mess ,BOOK* book)
{
  MSG * msg = (MSG*)mess;
  MessageBox(EMPTY_TEXTID,STR("Hello Wordl!\n\nExample elf.."), NOIMAGE, 1, 5000,msg->book);
  return(1);
}

const PAGE_MSG HW_PageEvents[]@ "DYN_PAGE" ={
  ELF_TERMINATE_EVENT , TerminateElf,
  ELF_SHOW_INFO_EVENT  , ShowAuthorInfo,
  0,0
};

PAGE_DESC base_page ={"HW_BasePage",0,HW_PageEvents};


void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}



void onTimer (u16 unk , LPARAM)
{
  {
    // перезапускаем таймер
    Timer_ReSet(&timer,500,onTimer,0);
    // переключаем лампу
    SetLampLevel(lamp^=0x10);
  }
}

void onCloseHWBook(BOOK * book)
{
  if (book)
  {
    Timer_Kill(&timer);
    StatusIndication_ShowNotes(EMPTY_TEXTID);
    SetLampLevel(0);
    SUBPROC(elf_exit);
  }
}

BOOK * CreateHWBook()
{
  HWBook= new BOOK;
  CreateBook(HWBook,onCloseHWBook,&base_page,"Example",-1,0);
  return(HWBook);
}




int main (void)
{
  CreateHWBook();
  // выводим текст в "заметки" на StandBy
  StatusIndication_ShowNotes(STR("Hello World!"));
  // запускаем таймер на 0.5 сек.
  timer=Timer_Set(500,onTimer,0);
  return(0);
}
