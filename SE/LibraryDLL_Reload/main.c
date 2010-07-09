#include "..\\include\dir.h"
#include "..\\include\Types.h"
#include "..\\include\Lib_Clara.h"
#include "..\\include\dll.h"


typedef struct
{
  int num;
  void * func;
}LIBPATCH_ELEM;


void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}


int main (void)
{
  EP_DATA * epd = (EP_DATA *)get_envp(get_bid(current_process()),"elfpackdata");
  void * old_lib = epd->LibraryDLL;
  if (fstat(GetDir(DIR_DLL),L"LibraryDLL.dll",0)<0) MessageBox(EMPTY_SID,STR("LibraryDLL.dll is absent"), NOIMAGE, 1, 0, 0);
  else
  {
    void * new_lib = LoadDLL(L"LibraryDLL.dll");
    if ((INVALID(new_lib)))
    {
      MessageBox(EMPTY_SID,STR("Load LibraryDLL.dll error"), NOIMAGE, 1, 0, 0);
      epd->LibraryDLL = 0;
    }
    else
    {
      LIBPATCH_ELEM * lp = (LIBPATCH_ELEM*)((char*)new_lib + 0x4);
      while (lp->num!=-1)
      {
        if (lp->func) epd->LibraryCache[lp->num]=lp->func;
        lp++;
      }
      if (old_lib) UnLoadDLL(old_lib);
      epd->LibraryDLL = new_lib;
    }
  }
  SUBPROC(elf_exit);
  return(0);
}
