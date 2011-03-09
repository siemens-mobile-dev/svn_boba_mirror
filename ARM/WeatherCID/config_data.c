#include "..\inc\cfg_items.h"
#include "..\inc\swilib.h"

#ifdef NEWSGOLD
#define DEFAULT_DISK "4"
#else
#define DEFAULT_DISK "0"
#endif

__root const CFG_HDR cfghdr1={CFG_STR_UTF8,"Icon path",0,127};
__root const char ICON_PATH[128]=DEFAULT_DISK ":\\ZBin\\Weather\\";

__root const CFG_HDR cfghdr2={CFG_STR_UTF8,"Data path",0,127};
__root const char DATA_PATH[128]=DEFAULT_DISK ":\\ZBin\\var\\";

__root const CFG_HDR cfghdr3={CFG_UINT,"Font size ",0,100};
#ifdef NEWSGOLD
#ifdef ELKA
__root const unsigned int FONT_SIZE=8; //ELKA
#else
__root const unsigned int FONT_SIZE=11; //NEWSGOLD
#endif
#else
__root const unsigned int FONT_SIZE=7; //SGOLD
#endif

__root const CFG_HDR cfghdr66={CFG_CHECKBOX,"Show City",0,0};
__root const int SHOW_CITY = 1;

__root const CFG_HDR cfghdr61={CFG_CHECKBOX,"Show Temperature",0,0};
__root const int SHOW_TEMP = 1;

__root const CFG_HDR cfghdr62={CFG_CHECKBOX,"Show Pressure",0,0};
__root const int SHOW_PRESSURE = 1;

__root const CFG_HDR cfghdr63={CFG_CHECKBOX,"Show Wind",0,0};
__root const int SHOW_WIND = 1;

__root const CFG_HDR cfghdr64={CFG_CHECKBOX,"Show Rewlet",0,0};
__root const int SHOW_REWLET = 1;

__root const CFG_HDR cfghdr65={CFG_CHECKBOX,"Show Pic",0,0};
__root const int SHOW_PIC = 1;

__root const CFG_HDR cfghdr6={CFG_COORDINATES,"Picture pos",0,0};
__root const unsigned int PICT_X=10;
__root const unsigned int PICT_Y=57;

__root const CFG_HDR cfghdr45 = {CFG_RECT, "Text Area", 0, 0};
__root const RECT TEXTRECT = {0,98,65,162};

