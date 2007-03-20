#include "..\inc\cfg_items.h"
#include "TextInfo.h"
//Конфигурация

#ifdef NEWSGOLD
#define DEFAULT_DISK "4"
#else
#define DEFAULT_DISK "0"
#endif

__root const CFG_HDR cfghdr0 = {CFG_CBOX, "Net display", 0, 2};
__root const int NET_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox1[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr00={CFG_COORDINATES,"Net position",0,0};
__root const unsigned int NET_X=0;
__root const unsigned int NET_Y=20;

__root const CFG_HDR cfghdr1={CFG_COLOR,"Net color",0,0};
__root const char NET_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr2={CFG_UINT,"Net font",0,65535};
__root const unsigned int NET_FONT=7;

__root const CFG_HDR cfghdr21={CFG_STR_WIN1251,"Net string format",0,15};
__root const char NET_FMT[16]="%c%ddB";

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr01 = {CFG_CBOX, "Temperature display", 0, 2};
__root const int TEMP_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox2[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr3={CFG_COORDINATES,"Temperature position",0,0};
__root const unsigned int TEMP_X=36;
__root const unsigned int TEMP_Y=20;

__root const CFG_HDR cfghdr4={CFG_COLOR,"Temperature color",0,0};
__root const char TEMP_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr5={CFG_UINT,"Temperature font",0,65535};
__root const unsigned int TEMP_FONT=7;

__root const CFG_HDR cfghdr31={CFG_STR_WIN1251,"Temp string format",0,15};
__root const char TEMP_FMT[16]="%d,%d°C";

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr02 = {CFG_CBOX, "Voltage display", 0, 2};
__root const int VOLT_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox3[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr6={CFG_COORDINATES,"Voltage position",0,0};
__root const unsigned int VOLT_X=77;
__root const unsigned int VOLT_Y=20;

__root const CFG_HDR cfghdr7={CFG_COLOR,"Voltage color",0,0};
__root const char VOLT_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr8={CFG_UINT,"Voltage font",0,65535};
__root const unsigned int VOLT_FONT=7;

__root const CFG_HDR cfghdr41={CFG_STR_WIN1251,"Voltage string format",0,15};
__root const char VOLT_FMT[16]="%d,%02dV";

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr03 = {CFG_CBOX, "AccuCap display", 0, 2};
__root const int CAP_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox4[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr9={CFG_COORDINATES,"AccuCap position",0,0};
__root const unsigned int ACCU_X=110;
__root const unsigned int ACCU_Y=20;

__root const CFG_HDR cfghdr10={CFG_COLOR,"AccuCap color",0,0};
__root const char ACCU_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr11={CFG_UINT,"AccuCap font",0,65535};
__root const unsigned int ACCU_FONT=7;

__root const CFG_HDR cfghdr51={CFG_STR_WIN1251,"AccuCap string format",0,15};
__root const char CAP_FMT[16]="%02d%%";

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr04 = {CFG_CBOX, "GPRS Traff display", 0, 2};
__root const int GPRS_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox5[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr12={CFG_COORDINATES,"GPRS position",0,0};
__root const unsigned int GPRS_X=93;
__root const unsigned int GPRS_Y=145;

__root const CFG_HDR cfghdr13={CFG_COLOR,"GPRS color",0,0};
__root const char GPRS_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr14={CFG_UINT,"GPRS font",0,65535};
__root const unsigned int GPRS_FONT=7;

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr05 = {CFG_CBOX, "AvailRAM display", 0, 2};
__root const int RAM_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox6[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr15={CFG_COORDINATES,"AvailRam position",0,0};
__root const unsigned int RAM_X=51;
__root const unsigned int RAM_Y=145;

__root const CFG_HDR cfghdr16={CFG_COLOR,"AvailRam color",0,0};
__root const char RAM_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr17={CFG_UINT,"AvailRam font",0,65535};
__root const unsigned int RAM_FONT=7;

// -------------------------------------------------------------------------- //
__root const CFG_HDR cfghdr06 = {CFG_CBOX, "CPULoad display", 0, 2};
__root const int CPU_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox7[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr18={CFG_COORDINATES,"CPULoad position",0,0};
__root const unsigned int CPU_X=0;
__root const unsigned int CPU_Y=145;

__root const CFG_HDR cfghdr19={CFG_COLOR,"CPULoad color",0,0};
__root const char CPU_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr20={CFG_UINT,"CPULoad font",0,65535};
__root const unsigned int CPU_FONT=7;

__root const CFG_HDR cfghdr61={CFG_STR_WIN1251,"CPULoad string format",0,15};
__root const char CPU_FMT[16]="%02d%%";

//===========================================================================

__root const CFG_HDR cfghdr71={CFG_STR_WIN1251,"Bytes string format",0,15};
__root const char BYTES_FMT[16]="%u%s";

__root const CFG_HDR cfghdr81={CFG_STR_WIN1251,"Bytes sign",0,7};
__root const char BYTES_SG[8]="B";

__root const CFG_HDR cfghdr91={CFG_STR_WIN1251,"KiloBytes sign",0,7};
__root const char KBYTES_SG[8]="KB";

__root const CFG_HDR cfghdr22={CFG_UINT,"Refresh period, 1/10 sec",0,600};
__root const unsigned int REFRESH=20;
