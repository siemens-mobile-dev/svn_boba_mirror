#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"

__root const CFG_HDR cfghdr0 = {CFG_CBOX, "Show in", 0, 3};
__root const int cfgShowIn = 0;
__root const CFG_CBOX_ITEM cfgcbox1[3] = {"Both", "Lock", "Unlock"};

__root const CFG_HDR cfghdr1={CFG_RECT, "Position Rect", 0, 0};
__root const RECT cfgRect = {2, 67, 22, 101};

__root const CFG_HDR cfghdr1_1 = {CFG_CBOX, "Orientation", 0, 2};
__root const int cfgOrient = 1;
__root const CFG_CBOX_ITEM cfgcbox2[2] = {"Horizontal", "Vertical"};

__root const CFG_HDR cfghdr4 = {CFG_UINT, "Interval", 0, 10};
__root const int cfgInterval = 3;

__root const CFG_HDR cfghdr5={CFG_COLOR,"Border Color",0,0};
__root const char cfgBorderCol[4]={0,0,0,100};

__root const CFG_HDR cfghdr6={CFG_COLOR,"Back Color",0,0};
__root const char cfgBGCol[4]={0xFF,0xFF,0xFF,100};

__root const CFG_HDR cfghdr7={CFG_COLOR,"Disk 0: Fill Color",0,0};
__root const char cfg0FillCol[4]={0xFF,0xFF,0,100};

__root const CFG_HDR cfghdr8={CFG_COLOR,"Disk 4: Fill Color",0,0};
__root const char cfg4FillCol[4]={0,0xFF,0xFF,100};

