#include "..\inc\cfg_items.h"
#include "TextInfo.h"
//Конфигурация

#ifdef NEWSGOLD
#define DEFAULT_DISK "4"
#else
#define DEFAULT_DISK "0"
#endif

__root const CFG_HDR cfghdr0={CFG_COORDINATES,"NetPosition",0,0};
__root const unsigned int NET_X=0;
__root const unsigned int NET_Y=20;

__root const CFG_HDR cfghdr1={CFG_COLOR,"NetColor",0,0};
__root const char NET_COLORS[4]={255,0,0,100};

__root const CFG_HDR cfghdr2={CFG_UINT,"NetFont",0,65535};
__root const unsigned int NET_FONT=7;

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr3={CFG_COORDINATES,"TemperaturePosition",0,0};
__root const unsigned int TEMP_X=36;
__root const unsigned int TEMP_Y=20;

__root const CFG_HDR cfghdr4={CFG_COLOR,"TemperatureColor",0,0};
__root const char TEMP_COLORS[4]={255,0,0,100};

__root const CFG_HDR cfghdr5={CFG_UINT,"TemperatureFont",0,65535};
__root const unsigned int TEMP_FONT=7;

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr6={CFG_COORDINATES,"VoltagePosition",0,0};
__root const unsigned int VOLT_X=77;
__root const unsigned int VOLT_Y=20;

__root const CFG_HDR cfghdr7={CFG_COLOR,"VoltageColor",0,0};
__root const char VOLT_COLORS[4]={255,0,0,100};

__root const CFG_HDR cfghdr8={CFG_UINT,"VoltageFont",0,65535};
__root const unsigned int VOLT_FONT=7;
// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr9={CFG_COORDINATES,"AccuCapPosition",0,0};
__root const unsigned int ACCU_X=110;
__root const unsigned int ACCU_Y=20;

__root const CFG_HDR cfghdr10={CFG_COLOR,"AccuCapColor",0,0};
__root const char ACCU_COLORS[4]={255,0,0,100};

__root const CFG_HDR cfghdr11={CFG_UINT,"AccuCapFont",0,65535};
__root const unsigned int ACCU_FONT=7;

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr12={CFG_COORDINATES,"GPRSPosition",0,0};
__root const unsigned int GPRS_X=93;
__root const unsigned int GPRS_Y=145;

__root const CFG_HDR cfghdr13={CFG_COLOR,"GPRSColor",0,0};
__root const char GPRS_COLORS[4]={255,0,0,100};

__root const CFG_HDR cfghdr14={CFG_UINT,"GPRSFont",0,65535};
__root const unsigned int GPRS_FONT=7;

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr15={CFG_COORDINATES,"AvailRamPosition",0,0};
__root const unsigned int RAM_X=51;
__root const unsigned int RAM_Y=145;

__root const CFG_HDR cfghdr16={CFG_COLOR,"AvailRamColor",0,0};
__root const char RAM_COLORS[4]={255,0,0,100};

__root const CFG_HDR cfghdr17={CFG_UINT,"AvailRamFont",0,65535};
__root const unsigned int RAM_FONT=7;

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr18={CFG_COORDINATES,"CPULoadPosition",0,0};
__root const unsigned int CPU_X=0;
__root const unsigned int CPU_Y=145;

__root const CFG_HDR cfghdr19={CFG_COLOR,"CPULoadColor",0,0};
__root const char CPU_COLORS[4]={255,0,0,100};

__root const CFG_HDR cfghdr20={CFG_UINT,"CPULoadFont",0,65535};
__root const unsigned int CPU_FONT=7;

