#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "main.h"
#include "calc_object.h"

const PAGE_MSG bk_msglst_base[] @ "DYN_PAGE"  = 
{
  ELF_TERMINATE_EVENT,     NULL,
  NIL_EVENT_TAG,           NULL
};

const PAGE_MSG bk_msglst_main[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    MainPageOnCreate,
  PAGE_EXIT_EVENT_TAG,     MainPageOnClose,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_base = {"SCalka_Base_Page",0,bk_msglst_base};
const PAGE_DESC bk_main = {"SCalka_Main_Page",0,bk_msglst_main};

#pragma segment="ELFBEGIN"
void elf_exit(void){
  kill_data(__segment_begin("ELFBEGIN"), (void(*)(void*))mfree_adr());
}



// при закрытии книги
static void onMyBookClose(BOOK * book)
{
  SUBPROC(elf_exit);
}

int isScalkaBook(BOOK * struc)
{
  return(struc->onClose==(void*)onMyBookClose);
}

void CloseMyBook(BOOK * bk, void *)
{
  FreeBook(bk);
};

static int MainPageOnCreate(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  GUI * si=(GUI *)CreateCalkGUI(bk);
  ShowWindow(si);
  mbk->si=si;
  return (1);
}

static int MainPageOnClose(void *, BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->si);
  return (1);
}

int main()
{
  MyBOOK * myBook=new MyBOOK;
  memset(myBook,0,sizeof(MyBOOK));
  if (!CreateBook(myBook,onMyBookClose,&bk_base,"SCalka",-1,0))
  {
    delete myBook;
    SUBPROC(elf_exit);
    return (0);    
  }
  BookObj_GotoPage((BOOK *)myBook,&bk_main);
  return 0;
}
