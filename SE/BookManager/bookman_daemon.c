#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\cfg_items.h"
#include "conf_loader.h"
#include "config_data.h"


void Daemon_onClose(BOOK * book)
{
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


const PAGE_MSG Daemon_Base_PageEvents[] @ "DYN_PAGE"  =
{
  ELF_RECONFIG_EVENT,ReconfigElf,
  NIL_EVENT_TAG,0
};

const PAGE_DESC Daemon_Base_Page = {"Daemon_Base_Page",0,Daemon_Base_PageEvents};


int isBookmanDaemonBook(BOOK * book)
{
  if(!strcmp(book->xbook->name,"BookManagerDaemon")) return(1);
  return(0);
}

void DestroyDaemon(void)
{
  BOOK * Daemon_Book;
  if (Daemon_Book=FindBook(isBookmanDaemonBook)) FreeBook(Daemon_Book);
}

void CreateDaemon(void)
{
  BOOK * Daemon_Book;
  if (Daemon_Book=FindBook(isBookmanDaemonBook)) FreeBook(Daemon_Book);
  Daemon_Book=new BOOK;
  CreateBook(Daemon_Book,Daemon_onClose,&Daemon_Base_Page,"BookManagerDaemon",-1,0);
}
