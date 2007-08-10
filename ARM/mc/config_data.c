#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "lng.h"

__root const CFG_HDR cfghdr0={CFG_CBOX,psz_showhidden,0,2};
__root const int SHOW_HIDDEN = 0;
__root const CFG_CBOX_ITEM cfgcbox0[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr11={CFG_CBOX,psz_showsysdrv,0,2};
__root const int SHOW_SYSDRV = 0;
__root const CFG_CBOX_ITEM cfgcbox11[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr10={CFG_CBOX,psz_showmmc,0,2};
__root const int SHOW_MMC = 1;
__root const CFG_CBOX_ITEM cfgcbox10[]={psz_no,psz_yes};


__root const CFG_HDR cfghdr1={CFG_CBOX,psz_savestate,0,2};
__root const int SAVE_PATH = 1;
__root const CFG_CBOX_ITEM cfgcbox1[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr2={CFG_CBOX,psz_confirmexit,0,2};
__root const int CONFIRM_EXIT = 1;
__root const CFG_CBOX_ITEM cfgcbox2[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr3={CFG_CBOX,psz_backexit,0,2};
__root const int BACK_EXIT = 1;
__root const CFG_CBOX_ITEM cfgcbox3[]={psz_no,psz_yes};


__root const CFG_HDR cfghdr4={CFG_UINT,psz_scspeed,0,2000};
__root const int SCROLL_TEXT_SPEED = 100;

__root const CFG_HDR cfghdr12={CFG_UINT,psz_scwait,0,200};
__root const int SCROLL_TEXT_WAIT = 10;

__root const CFG_HDR cfghdr9={CFG_CBOX,psz_sconchk,0,2};
__root const int SC_ON_CHK = 1;
__root const CFG_CBOX_ITEM cfgcbox9[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr5={CFG_CBOX,psz_loadicons,0,2};
__root const int LOAD_ICONS = 1;
__root const CFG_CBOX_ITEM cfgcbox5[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr6={CFG_CBOX,psz_loadkeys,0,2};
__root const int LOAD_KEYS = 1;
__root const CFG_CBOX_ITEM cfgcbox6[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr7={CFG_CBOX,psz_loadmui,0,2};
__root const int LOAD_MUI = 1;
__root const CFG_CBOX_ITEM cfgcbox7[]={psz_no,psz_yes};

__root const CFG_HDR cfghdr8={CFG_CBOX,psz_loadcs,0,2};
__root const int LOAD_CS = 1;
__root const CFG_CBOX_ITEM cfgcbox8[]={psz_no,psz_yes};

