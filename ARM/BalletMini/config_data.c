#include "..\inc\cfg_items.h"

__root const CFG_HDR cfghdr0={CFG_CBOX,"Picture Quality",0,4};
__root const int PIC_QUALITY=2;
__root const CFG_CBOX_ITEM cfgcbox0[]={"None","Text","Low","High"};

__root const CFG_HDR cfghdr01={CFG_STR_UTF8,"Home page",0,255};
__root const char homepage[256]="";

__root const CFG_HDR cfghdr1={CFG_STR_WIN1251,"Host",0,31};
__root const char OM_POST_HOST[32]="80.232.117.10";

__root const CFG_HDR cfghdr2={CFG_UINT,"Port",0,65535};
__root const unsigned int OM_POST_PORT=80;

__root const CFG_HDR cfghdr3={CFG_UINT,"Heap size(kb)",0,65535};
#ifndef  NEWSGOLD
__root const int java_heap_size=768;
#else
__root const int java_heap_size=3*1024;
#endif
