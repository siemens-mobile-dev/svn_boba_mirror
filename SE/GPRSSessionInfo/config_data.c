#include "..\\include\cfg_items.h"
#include "config_data.h"
#include "..\\include\Colors.h"

#pragma diag_suppress=Pe177

__root const CFG_HDR cfghdr1={CFG_INT,"Cost for package",0,0xFFFFF};
__root const int cfgCost=68;

__root const CFG_HDR cfghdr2={CFG_INT,"Package size (KB)",0,0xFFFFF};
__root const int cfgLength=10000;

__root const CFG_HDR cfghdr3={CFG_STR_WIN1251,"Currency name",0,10};
__root const char cfgCurrency[11]="рублей";

__root const CFG_HDR cfghdr4={CFG_INT,"Min package size (KB)",0,0xFFFFF};
__root const int cfgMinLength=100;

__root const CFG_HDR cfghdr5={CFG_INT,"Not counted traf (KB)",0,0xFFFFF};
__root const int cfgNotCountedTraf=5;

__root const CFG_HDR cfghdr6={CFG_INT,"Show timeout (sec)",0,0xFFFFF};
__root const int cfgTimeout=10;

