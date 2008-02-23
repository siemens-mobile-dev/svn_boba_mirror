#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

u16 timer; // ID таймера
int lamp=0; // текуща€ €ркость лампы
int n=10; // счетчик циклов мигани€

void onTimer (u16 * timerID , LPARAM data)
{
  // если счетчик не дошел до нул€
  if(n--)
  {
    // перезапускаем таймер
    Timer_ReSet(&timer,500,onTimer,data);
    // переключаем лампу
    SetLampLevel(lamp^=0x10);
  }
  else
  {
    // убираем "заметки" на StandBy
    StatusIndication_ShowNotes(0x6FFFFFFF);
    // выключаем лампу
    SetLampLevel(0);
    // завершаем работу и выгружаем эльфа из пам€ти
    SUBPROC(elf_exit);
  }
}

int main (void)
{
  // выводим текст в "заметки" на StandBy
  StatusIndication_ShowNotes(STR("Hello World!"));
  // запускаем таймер на 0.5 сек.
  timer=Timer_Set(500,onTimer,0);
  return(0);
}
