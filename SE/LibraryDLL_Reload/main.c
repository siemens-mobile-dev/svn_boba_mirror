#include "..\\include\dir.h"
#include "..\\include\Types.h"
#include "..\\include\Lib_Clara_DLL.h"
#include "..\\include\dll.h"


void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}


int main (void)
{
  EP_DATA * epd = (EP_DATA *)get_envp(get_bid(current_process()),"elfpackdata");
  if (fstat(GetDir(DIR_DLL),L"LibraryDLL.dll",0)<0) MessageBox(EMPTY_SID,STR("LibraryDLL.dll is absent"), NOIMAGE, 1, 0, 0);
  else
  {
    if (epd->LibraryDLL) UnLoadDLL(epd->LibraryDLL);
    epd->LibraryDLL = LoadDLL(L"LibraryDLL.dll");
    if ((INVALID(epd->LibraryDLL)))
    {
      MessageBox(EMPTY_SID,STR("Load LibraryDLL.dll error"), NOIMAGE, 1, 0, 0);
      epd->LibraryDLL = 0;
    }
  }
  SUBPROC(elf_exit);
  return(0);
}
