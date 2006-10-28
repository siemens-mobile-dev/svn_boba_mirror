#include "..\inc\cfg_items.h"
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_UINT,"UIN",0,0xFFFFFFFF};
__root const unsigned int UIN=0;

__root const CFG_HDR cfghdr1={CFG_STR_WIN1251,"Password",0,63};
__root const char PASS[64]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"History path",0,63};
__root const char HIST_PATH[64]="4:\\NatICQ_hist";

__root const CFG_HDR cfghdr2_0={CFG_CBOX,"Log status changes",0,2};
__root const int LOG_STATCH = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"No","Yes"};

__root const CFG_HDR cfghdr3={CFG_UINT,"Offline icon",0,32767};
__root const unsigned int ICON0=0x162;

__root const CFG_HDR cfghdr4={CFG_UINT,"Invisible icon",0,32767};
__root const unsigned int ICON1=0x232;

__root const CFG_HDR cfghdr5={CFG_UINT,"Away icon",0,32767};
__root const unsigned int ICON2=0x22F;

__root const CFG_HDR cfghdr6={CFG_UINT,"N/A icon",0,32767};
__root const unsigned int ICON3=0x17A;

__root const CFG_HDR cfghdr7={CFG_UINT,"Occupied icon",0,32767};
__root const unsigned int ICON4=0x195;

__root const CFG_HDR cfghdr8={CFG_UINT,"DND icon",0,32767};
__root const unsigned int ICON5=0x231;

__root const CFG_HDR cfghdr9={CFG_UINT,"FFC icon",0,32767};
__root const unsigned int ICON6=0x18C;

__root const CFG_HDR cfghdr10={CFG_UINT,"Online icon",0,32767};
__root const unsigned int ICON7=0x22C;

__root const CFG_HDR cfghdr11={CFG_UINT,"Message icon",0,32767};
__root const unsigned int ICON8=0x24C;

__root const CFG_HDR cfghdr12={CFG_UINT,"Unknown state icon",0,32767};
__root const unsigned int ICON9=0x185;
