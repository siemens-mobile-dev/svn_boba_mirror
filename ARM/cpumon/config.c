#include "..\inc\cfg_items.h"
#include "..\inc\swilib.h"

//Координаты
__root const CFG_HDR cfghdr0 = {CFG_RECT, "XYXY Position", 0, 239};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const RECT position = {65,108,131,153};

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

__root const CFG_HDR cfghdr4_1 = {CFG_COLOR, "Color (104 MHz)", 0, 0};
__root const char cfgLoad104[4] = {255,0,0,100};

__root const CFG_HDR cfghdr4_4 = {CFG_COLOR, "Color (52 MHz)", 0, 0};
__root const char cfgLoad52[4] = {255,0,0,100};

