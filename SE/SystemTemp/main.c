#include "..\\include\Lib_Clara_DLL.h"
#include "..\\include\Dir.h"

BATT  bat;
wchar_t   ws[193];

void elf_exit(void)
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

int main(u16 *exename,u16 *filename)
{
  GetBattaryState(NULL , &bat);
  snwprintf(ws,193,L"Temp = %d C",bat.SystemTemperature);
  MessageBox(EMPTY_SID,Str2ID(ws,0,SID_ANY_LEN), NOIMAGE, 1, 5000 , 0);
  SUBPROC(elf_exit);
  return(0);
}
