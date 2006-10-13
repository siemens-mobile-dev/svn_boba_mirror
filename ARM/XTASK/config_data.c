#include "..\inc\cfg_items.h"
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_STR_UTF8,"CSM list file",3,127};
__root const char csmlist_fname[128]="4:\\ZBin\\etc\\CSMlist.txt";

__root const CFG_HDR cfghdr1={CFG_CBOX,"Enable hello message",0,2};
__root const int ENA_HELLO_MSG=1;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"No","Yes"};

__root const CFG_HDR cfghdr2={CFG_CBOX,"Enable long press exit",0,2};
__root const int ENA_LONG_PRESS=1;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"No","Yes"};

__root const CFG_HDR cfghdr3={CFG_CBOX,"Red button mode",0,3};
__root const int RED_BUT_MODE=2;
__root const CFG_CBOX_ITEM cfgcbox3[3]={"As in FW","Classic","Hide all tasks"};
