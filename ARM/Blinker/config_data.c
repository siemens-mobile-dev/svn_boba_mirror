#include "..\inc\cfg_items.h"

__root const CFG_HDR cfghdr0={CFG_CBOX,"Blink keyboard",0,2};
__root const int bl_key=1;
__root const CFG_CBOX_ITEM cfgcbox0[2]={"No","Yes"};

__root const CFG_HDR cfghdr1={CFG_CBOX,"Blink display",0,2};
__root const int bl_dis=1;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"No","Yes"};

__root const CFG_HDR cfghdr2={CFG_CBOX,"Use vibra",0,2};
__root const int use_vib=1;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"No","Yes"};

__root const CFG_HDR cfghdr3={CFG_CBOX,"Play sound",0,2};
__root const int play_snd=1;
__root const CFG_CBOX_ITEM cfgcbox3[2]={"No","Yes"};

__root const CFG_HDR cfghdr4={CFG_CBOX,"Only while locked",0,2};
__root const int only_loc=1;
__root const CFG_CBOX_ITEM cfgcbox4[2]={"No","Yes"};

__root const CFG_HDR cfghdr5={CFG_UINT,"Blink freq",0,1048};
__root const unsigned int blink_delay=64;

__root const CFG_HDR cfghdr6={CFG_UINT,"Check each",0,104800};
__root const unsigned int check_each=1310;

__root const CFG_HDR cfghdr7={CFG_UINT,"Blink light",0,100};
__root const unsigned int blink_light=50;

__root const CFG_HDR cfghdr8={CFG_UINT,"Vibra power",0,100};
__root const unsigned int vibra_pow=50;

__root const CFG_HDR cfghdr9={CFG_CBOX,"Check",0,3};
__root const int check=2;
__root const CFG_CBOX_ITEM cfgcbox5[3]={"Calls","SMS", "Both"};
