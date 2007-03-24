#include "..\inc\cfg_items.h"

// оординаты
//X
__root const CFG_HDR cfghdr0 = {CFG_UINT, "X Position", 0, 239};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgX = 0;

//Y
__root const CFG_HDR cfghdr1 = {CFG_UINT, "Y Position", 0, 319};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgY = 80;

//W
__root const CFG_HDR cfghdr2 = {CFG_UINT, "Width", 0, 240};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgW = 132;

//H
__root const CFG_HDR cfghdr3 = {CFG_UINT, "Height", 0, 320};
//тип беззнаковый инт, текст, мин. значение, макс значение
__root const unsigned int cfgH = 24;

__root const CFG_HDR cfghdr4 = {CFG_STR_WIN1251,"Mediaplayer CSM addr",1,8};
__root const char CSMADR[9]="A068A8D4";
