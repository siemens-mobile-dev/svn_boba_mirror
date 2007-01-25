#include "..\inc\cfg_items.h"
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_UINT,"UIN",0,0xFFFFFFFF};
__root const unsigned int UIN=0;

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,8};
__root const char PASS[9]="";

__root const CFG_HDR cfghdr1_1={CFG_STR_WIN1251,"Host",0,127};
__root const char NATICQ_HOST[128]="cbsie.dyndns.info";

__root const CFG_HDR cfghdr1_2={CFG_UINT,"Port",0,65535};
__root const unsigned int NATICQ_PORT=5050;

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"History path",0,63};
__root const char HIST_PATH[64]="4:\\ZBin\\NatICQ\\history";

__root const CFG_HDR cfghdr2_0={CFG_CBOX,"Log status changes",0,2};
__root const int LOG_STATCH = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"No","Yes"};

__root const CFG_HDR cfghdr3={CFG_STR_UTF8,"Offline icon",0,63};
__root const char ICON0[64]="4:\\ZBin\\NatICQ\\img\\2.png";

__root const CFG_HDR cfghdr4={CFG_STR_UTF8,"Invisible icon",0,63};
__root const char ICON1[64]="4:\\ZBin\\NatICQ\\img\\8.png";

__root const CFG_HDR cfghdr5={CFG_STR_UTF8,"Away icon",0,63};
__root const char ICON2[64]="4:\\ZBin\\NatICQ\\img\\3.png";

__root const CFG_HDR cfghdr6={CFG_STR_UTF8,"N/A icon",0,63};
__root const char ICON3[64]="4:\\ZBin\\NatICQ\\img\\4.png";

__root const CFG_HDR cfghdr7={CFG_STR_UTF8,"Occupied icon",0,63};
__root const char ICON4[64]="4:\\ZBin\\NatICQ\\img\\5.png";

__root const CFG_HDR cfghdr8={CFG_STR_UTF8,"DND icon",0,63};
__root const char ICON5[64]="4:\\ZBin\\NatICQ\\img\\6.png";

__root const CFG_HDR cfghdr9={CFG_STR_UTF8,"FFC icon",0,63};
__root const char ICON6[64]="4:\\ZBin\\NatICQ\\img\\7.png";

__root const CFG_HDR cfghdr10={CFG_STR_UTF8,"Online icon",0,63};
__root const char ICON7[64]="4:\\ZBin\\NatICQ\\img\\1.png";

__root const CFG_HDR cfghdr11={CFG_STR_UTF8,"Message icon",0,63};
__root const char ICON8[64]="4:\\ZBin\\NatICQ\\img\\9.png";

__root const CFG_HDR cfghdr12={CFG_STR_UTF8,"Unknown state icon",0,63};
__root const char ICON9[64]="4:\\ZBin\\NatICQ\\img\\10.png";

__root const CFG_HDR cfghdr13={CFG_UINT,"Idle icon X-pos",0,1000};
__root const unsigned int IDLEICON_X=0;

__root const CFG_HDR cfghdr14={CFG_UINT,"Idle icon Y-pos",0,1000};
__root const unsigned int IDLEICON_Y=50;

