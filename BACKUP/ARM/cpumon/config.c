#include "..\inc\cfg_items.h"

//Координаты
//X
__root const CFG_HDR cfghdr0 = {CFG_UINT, "X Position", 0, 239};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgX = 65;

//Y
__root const CFG_HDR cfghdr1 = {CFG_UINT, "Y Position", 0, 319};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgY = 108;

//W
__root const CFG_HDR cfghdr2 = {CFG_UINT, "Width", 0, 240};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgW = 66;

//H
__root const CFG_HDR cfghdr3 = {CFG_UINT, "Height", 0, 320};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgH = 45;


//KeyLock
__root const CFG_HDR cfghdr7 = {CFG_CBOX, "Show In", 0, 3};
__root const int cfgShowIn = 2;
__root const CFG_CBOX_ITEM cfgcbox1[3] = {"Lock", "Unlock", "Both"};

//© string
__root const CFG_HDR cfghdr8 = {CFG_CBOX, "Show copyright text at startup", 0, 2};
__root const int cfgStTxt = 0;
__root const CFG_CBOX_ITEM cfgcbox2[2] = {"No", "Yes"};

//Update Time
__root const CFG_HDR cfghdr9 = {CFG_UINT, "Update Time (1/10 sec)", 0, 50};
__root const unsigned int cfgUpTime = 10;

//Colours...

__root const CFG_HDR cfghdr4_1 = {CFG_UINT, "Red Value (104 MHz)", 0, 31};
__root const unsigned int cfgLoadR104 = 24;

__root const CFG_HDR cfghdr4_2 = {CFG_UINT, "Green Value (104 MHz)", 0, 63};
__root const unsigned int cfgLoadG104 = 0;

__root const CFG_HDR cfghdr4_3 = {CFG_UINT, "Blue Value (104 MHz)", 0, 31};
__root const unsigned int cfgLoadB104 = 0;

__root const CFG_HDR cfghdr4_4 = {CFG_UINT, "Red Value (52 MHz)", 0, 31};
__root const unsigned int cfgLoadR52 = 24;

__root const CFG_HDR cfghdr4_5 = {CFG_UINT, "Green Value (52 MHz)", 0, 63};
__root const unsigned int cfgLoadG52 = 0;

__root const CFG_HDR cfghdr4_6 = {CFG_UINT, "Blue Value (52 MHz)", 0, 31};
__root const unsigned int cfgLoadB52 = 0;
