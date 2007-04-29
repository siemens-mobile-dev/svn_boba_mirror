#include "..\inc\cfg_items.h"
#include "TextInfo.h"
//Конфигурация

__root const CFG_HDR cfghdr0={CFG_UINT,"Refresh period, 1/10 sec",0,600};
__root const unsigned int REFRESH=20;

__root const CFG_HDR cfghdr_m11={CFG_LEVEL,"Net setup",1,0};

__root const CFG_HDR cfghdr1_1 = {CFG_CBOX, "Net display", 0, 2};
__root const int NET_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox1[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr1_2={CFG_COORDINATES,"Net position",0,0};
__root const unsigned int NET_X=0;
__root const unsigned int NET_Y=20;

__root const CFG_HDR cfghdr1_3={CFG_COLOR,"Net color",0,0};
__root const char NET_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr1_4={CFG_UINT,"Net font",0,65535};
__root const unsigned int NET_FONT=7;

__root const CFG_HDR cfghdr1_5={CFG_STR_WIN1251,"Net string format",0,15};
__root const char NET_FMT[16]="%c%ddB";

__root const CFG_HDR cfghdr_m10={CFG_LEVEL,"",0,0};

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr_m21={CFG_LEVEL,"Temperature setup",1,0};

__root const CFG_HDR cfghdr2_1 = {CFG_CBOX, "Temperature display", 0, 2};
__root const int TEMP_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox2[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr2_2={CFG_COORDINATES,"Temperature position",0,0};
__root const unsigned int TEMP_X=36;
__root const unsigned int TEMP_Y=20;

__root const CFG_HDR cfghdr2_3={CFG_COLOR,"Temperature color",0,0};
__root const char TEMP_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr2_4={CFG_UINT,"Temperature font",0,65535};
__root const unsigned int TEMP_FONT=7;

__root const CFG_HDR cfghdr2_5={CFG_STR_WIN1251,"Temp string format",0,15};
__root const char TEMP_FMT[16]="%d,%d°C";

__root const CFG_HDR cfghdr_m20={CFG_LEVEL,"",0,0};

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr_m31={CFG_LEVEL,"Voltage setup",1,0};

__root const CFG_HDR cfghdr3_1 = {CFG_CBOX, "Voltage display", 0, 2};
__root const int VOLT_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox3[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr3_2={CFG_COORDINATES,"Voltage position",0,0};
__root const unsigned int VOLT_X=77;
__root const unsigned int VOLT_Y=20;

__root const CFG_HDR cfghdr3_3={CFG_COLOR,"Voltage color",0,0};
__root const char VOLT_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr3_4={CFG_UINT,"Voltage font",0,65535};
__root const unsigned int VOLT_FONT=7;

__root const CFG_HDR cfghdr3_5={CFG_STR_WIN1251,"Voltage string format",0,15};
__root const char VOLT_FMT[16]="%d,%02dV";

__root const CFG_HDR cfghdr_m30={CFG_LEVEL,"",0,0};

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr_m41={CFG_LEVEL,"AccuCap setup",1,0};

__root const CFG_HDR cfghdr4_1 = {CFG_CBOX, "AccuCap display", 0, 2};
__root const int CAP_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox4[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr4_2={CFG_COORDINATES,"AccuCap position",0,0};
__root const unsigned int ACCU_X=110;
__root const unsigned int ACCU_Y=20;

__root const CFG_HDR cfghdr4_3={CFG_COLOR,"AccuCap color",0,0};
__root const char ACCU_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr4_4={CFG_UINT,"AccuCap font",0,65535};
__root const unsigned int ACCU_FONT=7;

__root const CFG_HDR cfghdr4_5={CFG_STR_WIN1251,"AccuCap string format",0,15};
__root const char CAP_FMT[16]="%02d%%";

__root const CFG_HDR cfghdr_m40={CFG_LEVEL,"",0,0};

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr_m51={CFG_LEVEL,"GPRS Traff setup",1,0};

__root const CFG_HDR cfghdr5_1 = {CFG_CBOX, "GPRS Traff display", 0, 2};
__root const int GPRS_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox5[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr5_2={CFG_COORDINATES,"GPRS position",0,0};
__root const unsigned int GPRS_X=93;
__root const unsigned int GPRS_Y=145;

__root const CFG_HDR cfghdr5_3={CFG_COLOR,"GPRS color",0,0};
__root const char GPRS_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr5_4={CFG_UINT,"GPRS font",0,65535};
__root const unsigned int GPRS_FONT=7;

__root const CFG_HDR cfghdr_m50={CFG_LEVEL,"",0,0};

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr_m61={CFG_LEVEL,"AvailRAM setup",1,0};

__root const CFG_HDR cfghdr6_1 = {CFG_CBOX, "AvailRAM display", 0, 2};
__root const int RAM_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox6[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr6_2={CFG_COORDINATES,"AvailRam position",0,0};
__root const unsigned int RAM_X=51;
__root const unsigned int RAM_Y=145;

__root const CFG_HDR cfghdr6_3={CFG_COLOR,"AvailRam color",0,0};
__root const char RAM_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr6_4={CFG_UINT,"AvailRam font",0,65535};
__root const unsigned int RAM_FONT=7;

__root const CFG_HDR cfghdr_m60={CFG_LEVEL,"",0,0};

// -------------------------------------------------------------------------- //

__root const CFG_HDR cfghdr_m71={CFG_LEVEL,"CPULoad setup",1,0};

__root const CFG_HDR cfghdr7_1 = {CFG_CBOX, "CPULoad display", 0, 2};
__root const int CPU_ENA = 1;
__root const CFG_CBOX_ITEM cfgcbox7[2] = {"No", "Yes"};

__root const CFG_HDR cfghdr7_2={CFG_COORDINATES,"CPULoad position",0,0};
__root const unsigned int CPU_X=0;
__root const unsigned int CPU_Y=145;

__root const CFG_HDR cfghdr7_3={CFG_COLOR,"CPULoad color",0,0};
__root const char CPU_COLORS[4]={255,255,255,100};

__root const CFG_HDR cfghdr7_4={CFG_UINT,"CPULoad font",0,65535};
__root const unsigned int CPU_FONT=7;

__root const CFG_HDR cfghdr7_5={CFG_STR_WIN1251,"CPULoad string format",0,15};
__root const char CPU_FMT[16]="%02d%%";

__root const CFG_HDR cfghdr_m70={CFG_LEVEL,"",0,0};

//===========================================================================

__root const CFG_HDR cfghdr_m81={CFG_LEVEL,"Traff string format setup",1,0};

__root const CFG_HDR cfghdr8_1={CFG_STR_WIN1251,"Bytes string format",0,15};
__root const char BYTES_FMT[16]="%u%s";

__root const CFG_HDR cfghdr8_2={CFG_STR_WIN1251,"Bytes sign",0,7};
__root const char BYTES_SG[8]="B";

__root const CFG_HDR cfghdr8_3={CFG_STR_WIN1251,"KiloBytes sign",0,7};
__root const char KBYTES_SG[8]="KB";

__root const CFG_HDR cfghdr_m80={CFG_LEVEL,"",0,0};
