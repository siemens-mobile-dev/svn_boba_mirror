#include "..\inc\cfg_items.h"
#include "..\inc\swilib.h"
//Конфигурация

#ifdef NEWSGOLD
#define DEFAULT_DISK 4
#else
#define DEFAULT_DISK 0
#endif

__root CFG_HDR cfghdr0={CFG_CBOX,"ZBin disk",0,5};
__root unsigned int DEFAULT_DISK_N=DEFAULT_DISK;
__root CFG_CBOX_ITEM cfgcbox1_1[5]={"0","1","2","3","4"};

__root CFG_HDR cfghdr1={CFG_UINT,"Alpha-chan threshold",0,255};
__root unsigned int ALPHA_THRESHOLD=128;

__root CFG_HDR cfghdr2={CFG_UINT,"PNG cache size",20,200};
__root unsigned int CACHE_PNG=50;

__root CFG_HDR cfghdr3={CFG_CBOX,"Default bit-depth",0,2};
__root unsigned int DEFAULT_COLOR = 1;
__root CFG_CBOX_ITEM cfgcbox3_1[2]={"8 bits","16 bits"};
