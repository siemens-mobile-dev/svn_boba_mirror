#include "..\inc\cfg_items.h"

__root const CFG_HDR cfghdr0={CFG_CBOX,"Picture Quality",0,4};
__root const int PIC_QUALITY=2;
__root const CFG_CBOX_ITEM cfgcbox0[]={"None","Text","Low","High"};

__root const CFG_HDR cfghdr1={CFG_UINT,"History depth",0,128};
__root const int HISTORY_DEPTH=20;

__root const CFG_HDR cfghdr2={CFG_UINT,"Java Heap value(kb)",0,16384};
#ifdef NEWSGOLD
#ifdef ELKA
__root const int JAVA_HEAP_SIZE=3072;
#else
__root const int JAVA_HEAP_SIZE=3072;
#endif
#else
__root const int JAVA_HEAP_SIZE=768;
#endif

//----------------------------------- Server -----------------------------------
__root const CFG_HDR cfghdr3={CFG_LEVEL,"Server",1,0};

__root const CFG_HDR cfghdr3_1={CFG_STR_WIN1251,"Host",0,31};
__root const char OM_POST_HOST[32]="80.232.117.10";

__root const CFG_HDR cfghdr3_2={CFG_UINT,"Port",0,65535};
__root const unsigned int OM_POST_PORT=80;

__root const CFG_HDR cfghdr3_end={CFG_LEVEL,"",0,0};

//----------------------------------- Colors -----------------------------------
__root const CFG_HDR cfghdr4={CFG_LEVEL,"Colors",1,0};

//__________________________________ Main GUI __________________________________
__root const CFG_HDR cfghdr4_00={CFG_LEVEL,"Main GUI",1,0};

__root const CFG_HDR cfghdr4_1={CFG_COLOR,"Background",0,0};
__root const char BG_COLOR[4]={0xFF,0xFF,0xFF,0x64};

__root const CFG_HDR cfghdr4_2={CFG_COLOR,"Statusbar",0,0};
__root const char ST_COLOR[4]={0xB0,0xB0,0xB0,0x32};

__root const CFG_HDR cfghdr4_3={CFG_COLOR,"Statusbar Frame",0,0};
__root const char ST_FRAME_COLOR[4]={0x35,0x35,0x35,0x50};

__root const CFG_HDR cfghdr4_4={CFG_COLOR,"Statusbar Text",0,0};
__root const char ST_TEXT_COLOR[4]={0x00,0x00,0x00,0x50};

__root const CFG_HDR cfghdr4_5={CFG_COLOR,"Stop Button",0,0};
__root const char STOP_COLOR[4]={0xFF,0x00,0x00,0x32};

__root const CFG_HDR cfghdr4_6={CFG_COLOR,"Stop Frame",0,0};
__root const char STOP_FRAME_COLOR[4]={0xFF,0x00,0x00,0x50};

__root const CFG_HDR cfghdr4_7={CFG_COLOR,"Stop Text",0,0};
__root const char STOP_TEXT_COLOR[4]={0x00,0x00,0x00,0x50};

__root const CFG_HDR cfghdr4_8={CFG_COLOR,"Progressbar",0,0};
__root const char PR_COLOR[4]={0x00,0x00,0xFF,0x32};

__root const CFG_HDR cfghdr4_9={CFG_COLOR,"Roll Line",0,0};
__root const char ROLL_COLOR[4]={0xFF,0x00,0x00,0x20};

__root const CFG_HDR cfghdr4_10={CFG_COLOR,"Cursor",0,0};
__root const char CUR_COLOR[4]={0x00,0x00,0xAA,0x64};

__root const CFG_HDR cfghdr4_11={CFG_COLOR,"Cursor Pic.",0,0};
__root const char CUR_PIC_COLOR[4]={0xBE,0x00,0x00,0x64};

__root const CFG_HDR cfghdr4_00end={CFG_LEVEL,"",0,0};

//______________________________ Menu GUI ______________________________________
__root const CFG_HDR cfghdr4_01={CFG_LEVEL,"Menu GUI",1,0};

__root const CFG_HDR cfghdr4_12={CFG_COLOR,"Menu BG 1",0,0};
__root const char M_BG1[4]={0x35,0x35,0x35,0x64};

__root const CFG_HDR cfghdr4_13={CFG_COLOR,"Menu BG 2",0,0};
__root const char M_BG2[4]={0xB0,0xB0,0xB0,0x64};

__root const CFG_HDR cfghdr4_14={CFG_COLOR,"Menu Cursor",0,0};
__root const char M_CURSOR[4]={0x00,0x94,0xFF,0x32};

__root const CFG_HDR cfghdr4_15={CFG_COLOR,"Menu Cursor Frame",0,0};
__root const char M_CURSOR_FRAME[4]={0x00,0x94,0xFF,0x32};

__root const CFG_HDR cfghdr4_16={CFG_COLOR,"Menu Text",0,0};
__root const char M_TEXT[4]={0x00,0x00,0x00,0x64};

__root const CFG_HDR cfghdr4_17={CFG_COLOR,"Statusbar",0,0};
__root const char MST_COLOR[4]={0xB0,0xB0,0xB0,0x64};

__root const CFG_HDR cfghdr4_18={CFG_COLOR,"Statusbar Frame",0,0};
__root const char MST_FRAME_COLOR[4]={0x35,0x35,0x35,0x50};

__root const CFG_HDR cfghdr4_19={CFG_COLOR,"Statusbar Text",0,0};
__root const char MST_TEXT_COLOR[4]={0x00,0x00,0x00,0x64};

__root const CFG_HDR cfghdr4_01end={CFG_LEVEL,"",0,0};

__root const CFG_HDR cfghdr4_end={CFG_LEVEL,"",0,0};

__root const CFG_HDR cfghdr5={CFG_STR_UTF8,"Default launch parameter",0,127};
__root const char DEFAULT_PARAM[128]="bookmarks";
