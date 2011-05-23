#include "..\\include\cfg_items.h"
#include "config_data.h"

#pragma diag_suppress=Pe177


CFGVAR CFG_HDR cfghdr1={CFG_LEVEL,"Fonts",1,0};
  CFGVAR CFG_HDR cfghdr1_1={CFG_FONT,"Clock Font",0,0};
  CFGVAR int font1=0x55;

  CFGVAR CFG_HDR cfghdr1_2={CFG_FONT,"Date Font",0,0};
  CFGVAR int font2=0x55;

  CFGVAR CFG_HDR cfghdr1_3={CFG_FONT,"WeekDay Font",0,0};
  CFGVAR int font3=0x55;
CFGVAR CFG_HDR cfghdr2={CFG_LEVEL,"",0,0};



CFGVAR CFG_HDR cfghdr3={CFG_LEVEL,"Windows",1,0};
  CFGVAR CFG_HDR cfghdr3_1={CFG_COORDINATES,"Clock",0,0};
  CFGVAR int x0=0x1;
  CFGVAR int y0=0x50;

  CFGVAR CFG_HDR cfghdr3_2={CFG_COORDINATES,"Date",0,0};
  CFGVAR int x1=0x1;
  CFGVAR int y1=0xB5;

  CFGVAR CFG_HDR cfghdr3_3={CFG_COORDINATES,"WeekDay",0,0};
  CFGVAR int x2=0x1;
  CFGVAR int y2=0xDD;

  CFGVAR CFG_HDR cfghdr3_4={CFG_COORDINATES,"Icons",0,0};
  CFGVAR int x3=0x1;
  CFGVAR int y3=0x1;
CFGVAR CFG_HDR cfghdr4={CFG_LEVEL,"",0,0};



CFGVAR CFG_HDR cfghdr5={CFG_LEVEL,"Default Date and Time",1,0};
  CFGVAR CFG_HDR cfghdr5_1={CFG_DATE,"Date",0,0};
  CFGVAR DATE default_date={2000,01,01};

  CFGVAR CFG_HDR cfghdr5_2={CFG_TIME,"Time",0,0};
  CFGVAR TIME default_time={05,00,00};
CFGVAR CFG_HDR cfghdr6={CFG_LEVEL,"",0,0};



CFGVAR CFG_HDR cfghdr7={CFG_CHECKBOX,"Show seconds",0,0};
CFGVAR int ShowSeconds=0;
