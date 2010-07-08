#include "..\include\Lib_Clara_DLL.h"
#include "..\include\Dir.h"
#include "..\include\cfg_items.h"
#include "bookman_daemon.h"
#include "conf_loader.h"
#include "config_data.h"


void Daemon_onClose( BOOK* book )
{
}


static int ReconfigElf( void* mess, BOOK* book )
{
  RECONFIG_EVENT_DATA* reconf = ( RECONFIG_EVENT_DATA* )mess;
  
  if ( !wstrcmpi( reconf->path, successed_config_path ) && !wstrcmpi( reconf->name, successed_config_name ) )
  {
    InitConfig();
    return 1;
  }
  return 0;
}


const PAGE_MSG Daemon_Base_PageEvents[] @ "DYN_PAGE" =
{
  ELF_RECONFIG_EVENT, ReconfigElf,
  NIL_EVENT_TAG, 0
};

const PAGE_DESC Daemon_Base_Page = { "Daemon_Base_Page", 0, Daemon_Base_PageEvents };


int isBookmanDaemonBook( BOOK* book )
{
  return 0 == strcmp( book->xbook->name, "BookManagerDaemon" );
}

void DestroyDaemon( void )
{
  BOOK* Daemon_Book = FindBook( isBookmanDaemonBook );
  
  if ( Daemon_Book )
    FreeBook( Daemon_Book );
}

void CreateDaemon( void )
{
  DaemonBook* Daemon_Book = (DaemonBook*) FindBook( isBookmanDaemonBook );
  
  if ( Daemon_Book )
    FreeBook( Daemon_Book );
  
  Daemon_Book = new DaemonBook;
  Daemon_Book->ActiveTAB = 0;
  CreateBook( Daemon_Book, Daemon_onClose, &Daemon_Base_Page, "BookManagerDaemon", -1, 0 );
}
