#include "..\inc\cfg_items.h"
#include "..\inc\swilib.h"

//Координаты
#ifndef ELKA
__root const CFG_HDR cfghdr0 = {CFG_RECT, "XYXY Position", 0, 239};
__root const RECT position = {65,108,131,153};
#else
__root const CFG_HDR cfghdr0 = {CFG_RECT, "XYXY Position", 0, 239};
__root const RECT position = {136,224,239,280};
#endif

//KeyLock
__root const CFG_HDR cfghdr7 = {CFG_CBOX, "Show In", 0, 3};
__root const int cfgShowIn = 1;
__root const CFG_CBOX_ITEM cfgcbox1[3] = {"Lock", "Unlock", "Both"};

//Update Time
__root const CFG_HDR cfghdr9 = {CFG_INT, "Update Time (1/10 sec)", 0, 50};
__root const int cfgUpTime = 10;

//Рендер
__root const CFG_HDR cfghdr123={CFG_LEVEL,"Render",1,0};
  __root const CFG_HDR cfghdr200 = {CFG_CBOX, "Bmp Render", 0, 2};
  __root const int cfgRender = 0;
  __root const CFG_CBOX_ITEM cfgcbox200[2] = {"by BoBa", "by Sinclair"};

#if NEWSGOLD || X75
  __root const CFG_HDR cfghdr201 = {CFG_CBOX, "Render Bits", 0, 3};
  __root const int cfgRenderBit = 2;
  __root const CFG_CBOX_ITEM cfgcbox201[3] = {"8bit", "16bit", "24bit"};
#else  
  __root const CFG_HDR cfghdr201 = {CFG_CBOX, "Render Bits", 0, 2};
  __root const int cfgRenderBit = 1;
  __root const CFG_CBOX_ITEM cfgcbox201[2] = {"8bit", "16bit"};
#endif

  __root const CFG_HDR cfghdr202 = {CFG_CBOX, "Text Render", 0, 2};
  __root const int cfgTextRender = 0;
  __root const CFG_CBOX_ITEM cfgcbox202[2] = {"Over bmp", "Under bmp"};
__root const CFG_HDR cfghdr124={CFG_LEVEL,"",0,0};

//Sensors...
//CPU
__root const CFG_HDR cfghdr1={CFG_LEVEL,"CPU Graph",1,0};
  __root const CFG_HDR cfghdr5 = {CFG_CBOX, "Type Graph", 0, 4};
  __root const int cfgLoadType = 3;
  __root const CFG_CBOX_ITEM cfgcbox3[4] = {"None", "Doted", "Line", "Solid"};

  __root const CFG_HDR cfghdr11 = {CFG_INT, "Update Time Divisor", 1, 500};
  __root const int cfgCpuDiv = 1;

  __root const CFG_HDR cfghdr4_1 = {CFG_COLOR, "Color 208 MHz", 0, 0};
  __root const char cfgLoad208[4] = {0,0xff,0,100};

  __root const CFG_HDR cfghdr4_2 = {CFG_COLOR, "Color 104 MHz", 0, 0};
  __root const char cfgLoad104[4] = {0,0xd0,0,100};

  __root const CFG_HDR cfghdr4_4 = {CFG_COLOR, "Color 52 MHz", 0, 0};
  __root const char cfgLoad52[4] = {0,0xa0,0,100};
__root const CFG_HDR cfghdr3={CFG_LEVEL,"",0,0};

//RAM
__root const CFG_HDR cfghdr2={CFG_LEVEL,"RAM Graph",1,0};
  __root const CFG_HDR cfghdr6 = {CFG_CBOX, "Type Graph", 0, 4};
  __root const int cfgRamType = 1;
  __root const CFG_CBOX_ITEM cfgcbox4[4] = {"None", "Doted", "Line", "Solid"};

  __root const CFG_HDR cfghdr10 = {CFG_INT, "Update Time Divisor", 1, 500};
  __root const int cfgRamDiv = 10;

  __root const CFG_HDR cfghdr4_3 = {CFG_COLOR, "Color", 0, 0};
  __root const char cfgFreeRAM[4] = {0,0,0xff,100};
__root const CFG_HDR cfghdr4={CFG_LEVEL,"",0,0};

//Net
__root const CFG_HDR cfghdr12={CFG_LEVEL,"Net Graph",1,0};
  __root const CFG_HDR cfghdr13 = {CFG_CBOX, "Type Graph", 0, 4};
  __root const int cfgNetType = 2;
  __root const CFG_CBOX_ITEM cfgcbox5[4] = {"None", "Doted", "Line", "Solid"};

  __root const CFG_HDR cfghdr14 = {CFG_INT, "Update Time Divisor", 1, 500};
  __root const int cfgNetDiv = 2;

  __root const CFG_HDR cfghdr151 = {CFG_COLOR, "Color 900Mhz", 0, 0};
  __root const char cfgNet900[4] = {0xff,0x70,0,100};

  __root const CFG_HDR cfghdr152 = {CFG_COLOR, "Color e900Mhz", 0, 0};
  __root const char cfgNete900[4] = {0xff,0xa0,0,100};

  __root const CFG_HDR cfghdr153 = {CFG_COLOR, "Color 1800Mhz", 0, 0};
  __root const char cfgNet1800[4] = {0xff,0xff,0,100};

__root const CFG_HDR cfghdr16={CFG_LEVEL,"",0,0};

//Disk
__root const CFG_HDR cfghdr17={CFG_LEVEL,"Disk Graph",1,0};
  __root const CFG_HDR cfghdr18 = {CFG_CBOX, "Type Graph", 0, 4};
  __root const int cfgDiskType = 3;
  __root const CFG_CBOX_ITEM cfgcbox6[4] = {"None", "Doted", "Line", "Solid"};

  __root const CFG_HDR cfghdr19 = {CFG_INT, "Update Time Divisor", 1, 5000};
  __root const int cfgDiskDiv = 1000;

  __root const CFG_HDR cfghdr20 = {CFG_COLOR, "Color", 0, 0};
  __root const char cfgDisk[4] = {0,0xaa,0xaa,100};
__root const CFG_HDR cfghdr21={CFG_LEVEL,"",0,0};

//GPRS
__root const CFG_HDR cfghdr22={CFG_LEVEL,"GPRS Graph",1,0};
  __root const CFG_HDR cfghdr23 = {CFG_CBOX, "Type Graph", 0, 4};
  __root const int cfgGPRSType = 3;
  __root const CFG_CBOX_ITEM cfgcbox27[4] = {"None", "Doted", "Line", "Solid"};

  __root const CFG_HDR cfghdr24 = {CFG_INT, "Update Time Divisor", 1, 500};
  __root const int cfgGPRSDiv = 5;

  __root const CFG_HDR cfghdr25 = {CFG_COLOR, "Color", 0, 0};
  __root const char cfgGPRS[4] = {0xff,0,0xff,100};
__root const CFG_HDR cfghdr26={CFG_LEVEL,"",0,0};

//Bat Capacity
__root const CFG_HDR cfghdr32={CFG_LEVEL,"Bat Capacity",1,0};
  __root const CFG_HDR cfghdr33 = {CFG_CBOX, "Type Graph", 0, 2};
  __root const int cfgCapaType = 1;
  __root const CFG_CBOX_ITEM cfgcbox37[2] = {"None", "Numeric"};

  __root const CFG_HDR cfghdr38 = {CFG_CBOX, "Corner", 0, 4};
  __root const int cfgCapaCorner = 1;
  __root const CFG_CBOX_ITEM cfgcbox39[4] = {"upper left", "upper right", "lower left", "lower right"};

  __root const CFG_HDR cfghdr34 = {CFG_INT, "Update Time Divisor", 1, 500};
  __root const int cfgCapaDiv = 100;

  __root const CFG_HDR cfghdr35 = {CFG_INT, "Font", 1, 20};
  __root const int cfgCapaFont = FONT_NUMERIC_XSMALL;

  __root const CFG_HDR cfghdr36 = {CFG_COLOR, "Color", 0, 0};
  __root const char cfgCapa[4] = {0xff,0,0,100};
__root const CFG_HDR cfghdr37={CFG_LEVEL,"",0,0};

//Temperature
__root const CFG_HDR cfghdr42={CFG_LEVEL,"Temperature",1,0};
  __root const CFG_HDR cfghdr43 = {CFG_CBOX, "Type Graph", 0, 2};
  __root const int cfgTempType = 1;
  __root const CFG_CBOX_ITEM cfgcbox47[2] = {"None", "Numeric"};

  __root const CFG_HDR cfghdr48 = {CFG_CBOX, "Corner", 0, 4};
  __root const int cfgTempCorner = 0;
  __root const CFG_CBOX_ITEM cfgcbox49[4] = {"upper left", "upper right", "lower left", "lower right"};

  __root const CFG_HDR cfghdr44 = {CFG_INT, "Update Time Divisor", 1, 500};
  __root const int cfgTempDiv = 100;

  __root const CFG_HDR cfghdr45 = {CFG_INT, "Font", 1, 20};
  __root const int cfgTempFont = FONT_NUMERIC_XSMALL;

  __root const CFG_HDR cfghdr46 = {CFG_COLOR, "Color", 0, 0};
  __root const char cfgTemp[4] = {0,0,0xff,100};
__root const CFG_HDR cfghdr47={CFG_LEVEL,"",0,0};
