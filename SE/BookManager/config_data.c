#include "..\\include\cfg_items.h"
#include "config_data.h"

#pragma diag_suppress=Pe177

__root const CFG_HDR cfghdr0={CFG_KEYCODE,"Start Button",0,0};
__root const int KeyActiv=0x1F;
__root const int KeyPressMode=3;


__root const CFG_HDR cfghdr1={CFG_LEVEL,"Tabs images",1,0};
  __root const CFG_HDR cfghdr1_1={CFG_STR_WIN1251,"1 tab active",0,39};
  __root const char tab1_act[40]="";
  
  __root const CFG_HDR cfghdr1_2={CFG_STR_WIN1251,"1 tab inactive",0,39};
  __root const char tab1_inact[40]="";

  __root const CFG_HDR cfghdr1_3={CFG_STR_WIN1251,"2 tab active",0,39};
  __root const char tab2_act[40]="";

  __root const CFG_HDR cfghdr1_4={CFG_STR_WIN1251,"2 tab inactive",0,39};
  __root const char tab2_inact[40]="";
__root const CFG_HDR cfghdr2={CFG_LEVEL,"",0,0};


__root const CFG_HDR cfghdr3={CFG_CHECKBOX,"Ignore Keylock",0,0};
__root const int Ignore_KeyLock=0;


__root const CFG_HDR cfghdr4={CFG_CHECKBOX,"Display elfs on 1 tab",0,0};
__root const int ElfInBookListEnabled=1;


__root const CFG_HDR cfghdr5={CFG_CHECKBOX,"Close when inactive",0,0};
__root const int UserInactivityEventEnabled=1;


__root const CFG_HDR cfghdr6={CFG_CBOX,"Name Books",0,2};
__root const int NameBookAsSession=0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"As book","As session"};


__root const CFG_HDR cfghdr7={CFG_CBOX,"Show after shortcut",0,2};
__root const int StandbyOnTop=0;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"Current book","Standby"};


__root const CFG_HDR cfghdr8={CFG_CBOX,"Active Tab",0,3};
__root const int FirstTab=0;
__root const CFG_CBOX_ITEM cfgcbox3[3]={"Last Active","Books","Elfs"};

