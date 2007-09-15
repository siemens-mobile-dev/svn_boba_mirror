#include "..\inc\cfg_items.h"
#include "lang.h"
//Конфигурация

#ifdef NEWSGOLD
#define DEFAULT_DISK_CFG "4"
#else
#define DEFAULT_DISK_CFG "0"
#endif

////////////////////////////////////////////////////////////////////////////////
__root const CFG_HDR cfghdr_CONNECT_LEVEL={CFG_LEVEL,LG_SETUPCONECT,1,0};

__root const CFG_HDR cfghdr0={CFG_UTF8_STRING,LG_LOGIN,0,31};
__root const char USERNAME[32]="";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,LG_PASSWORD,0,31};
__root const char PASSWORD[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,LG_SERVER,0,127};
__root const char JABBER_SERVER[128]="jabber.ru";

__root const CFG_HDR cfghdr3={CFG_STR_WIN1251,LG_HOST,0,127};
__root const char JABBER_HOST[128]="jabber.ru";

__root const CFG_HDR cfghdr3_1={CFG_CBOX,LG_HOSTTYPE,0,2};
__root const int IS_IP = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={LG_HOSTNAME,LG_IPADRESS};

__root const CFG_HDR cfghdr4={CFG_UINT,LG_PORT,0,65535};
__root const unsigned int JABBER_PORT=5222;

__root const CFG_HDR cfghdr5={CFG_CBOX,LG_AUTHTYPE,0,2};
__root const int USE_SASL = 0;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"Simple","SASL"};

__root const CFG_HDR cfghdr6={CFG_CBOX,LG_ZLIBUSE,0,2};
__root const int USE_ZLIB = 0;
__root const CFG_CBOX_ITEM cfgcbox3[2]={LG_NO,LG_YES};

__root const CFG_HDR cfghdr_CONNECT_LEVEL_END={CFG_LEVEL,"",0,0};

////////////////////////////////////////////////////////////////////////////////

__root const CFG_HDR cfghdr7={CFG_CBOX,LG_HISTORY,0,2};
__root const int WRITE_HISTORY = 0;
__root const CFG_CBOX_ITEM cfgcbox1435[2]={LG_DISABLED,LG_ENABLED};

__root const CFG_HDR cfghdr8={CFG_CBOX,LG_MUCHIST,0,2};
__root const int WRITE_MUC_HISTORY = 0;
__root const CFG_CBOX_ITEM cfgcbox36546[2]={LG_DISABLED,LG_ENABLED};

__root const CFG_HDR cfghdr9={CFG_UTF8_STRING,LG_MUCDEF,0,63};
__root const char DEFAULT_MUC[64]="siepatchdb@conference.jabber.ru";

__root const CFG_HDR cfghdr10={CFG_UTF8_STRING,LG_MUCDEFNIK,0,63};
__root const char DEFAULT_MUC_NICK[64]="";

__root const CFG_HDR cfghdr11={CFG_COORDINATES,LG_IDLEICON,0,0};
__root const unsigned int IDLE_ICON_X=0;
__root const unsigned int IDLE_ICON_Y=50;

__root const CFG_HDR cfghdr17657={CFG_CHECKBOX,LG_POPUPDIS,0,2};
__root const int DISPLAY_POPUPS=1;

__root const CFG_HDR cfghdr12={CFG_CHECKBOX,LG_COMPOSEV,0,2};
__root const int COMPOSING_EVENTS=1;

__root const CFG_HDR cfghdr15345={CFG_CHECKBOX,LG_DELIVREP,0,2};
__root const int DELIVERY_EVENTS=1;

__root const CFG_HDR cfghdr7_3={CFG_CBOX,"Time Zone",0,24};
__root const int MY_DEF_ZONE = 2;
__root const CFG_CBOX_ITEM cfgcbox7_1[24]={
  "+0:00",
  "+1:00",
  "+2:00",
  "+3:00",
  "+4:00",
  "+5:00",
  "+6:00",
  "+7:00",
  "+8:00",
  "+9:00",
  "+10:00",
  "+11:00",
  "+12:00",
  "-1:00",
  "-2:00",
  "-3:00",
  "-4:00",
  "-5:00",
  "-6:00",
  "-7:00",
  "-8:00",
  "-9:00",
  "-10:00",
  "-11:00"
  };

// Настройка звуков
////////////////////////////////////////////////////////////////////////////////
__root const CFG_HDR cfghdr_SOUNDS_SETUP={CFG_LEVEL,LG_SETUPSND,1,0};

//__root const CFG_HDR cfghdr13={CFG_CHECKBOX,LG_SNDSTATE,0,2};
//__root const int Default_Sounds_State=1;

__root const CFG_HDR cfghdr14={CFG_UINT,LG_SNDVOL,0,6};
__root const unsigned int sndVolume=6;

__root const CFG_HDR cfghdr15={CFG_STR_UTF8,LG_SNDMSGSND,0,63};
__root const char sndMsgSend[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Sounds\\send.wav";

__root const CFG_HDR cfghdr16={CFG_STR_UTF8,LG_SNDMSGMUC,0,63};
__root const char sndConf[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Sounds\\conf.wav";

__root const CFG_HDR cfghdr17={CFG_STR_UTF8,LG_SNDMSGPM,0,63};
__root const char sndPM[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Sounds\\message.wav";

__root const CFG_HDR cfghdr_SOUNDS_SETUP_END={CFG_LEVEL,"",0,0};

////////////////////////////// Пути ////////////////////////////////////////////

__root const CFG_HDR cfghdr_PATHS_LEVEL={CFG_LEVEL,LG_SETUPPATH,1,0};

__root const CFG_HDR cfghdr18={CFG_STR_UTF8,LG_PATHHIST,0,127};
__root const char HIST_PATH[128]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\history\\";

__root const CFG_HDR cfghdr19={CFG_STR_UTF8,LG_PATHPIC,0,127};
__root const char PATH_TO_PIC[128]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\img\\";

__root const CFG_HDR cfghdr20={CFG_STR_UTF8,LG_PATHCMD,0,63};
__root const char COMMANDS_PATH[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Templates\\commands.txt";

__root const CFG_HDR cfghdr21={CFG_STR_UTF8,LG_PATHMESG,0,63};
__root const char MESSAGES_PATH[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Templates\\Messages.txt";

__root const CFG_HDR cfghdr22={CFG_STR_UTF8,LG_PATHKNOWF,0,63};
__root const char KNOW_FEATURES_PATH[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Templates\\known_features.txt";

__root const CFG_HDR cfghdr23={CFG_STR_UTF8,LG_PATHSMILECFG,0,63};
__root const char SMILE_FILE[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\smiles\\smiles.cfg";

__root const CFG_HDR cfghdr233={CFG_STR_UTF8,LG_PATHSMILEDIR,0,63};
__root const char SMILE_PATH[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\smiles";

__root const CFG_HDR cfghdr24={CFG_STR_WIN1251,LG_DEFUALTDISK,0,127};
__root const char DEFAULT_DISC[128]=DEFAULT_DISK_CFG;

__root const CFG_HDR cfghdr_PATHS_LEVEL_END={CFG_LEVEL,"",0,0};

//////////////////Default text in status///////////////////////////

__root const CFG_HDR cfghdr_DEFTEXTSTAT_LEVEL={CFG_LEVEL,LG_TEXTSTATUS,1,0};

__root const CFG_HDR cfghdr25={CFG_STR_WIN1251,LG_STONLINE,0,255};
__root const char DEFTEX_ONLINE[256]="В сети";

__root const CFG_HDR cfghdr26={CFG_STR_WIN1251,LG_STCHAT,0,255};
__root const char DEFTEX_CHAT[256]="Готов общатся";

__root const CFG_HDR cfghdr27={CFG_STR_WIN1251,LG_STAWAY,0,255};
__root const char DEFTEX_AWAY[256]="";

__root const CFG_HDR cfghdr28={CFG_STR_WIN1251,LG_STXA,0,255};
__root const char DEFTEX_XA[256]="";

__root const CFG_HDR cfghdr29={CFG_STR_WIN1251,LG_STDND,0,255};
__root const char DEFTEX_DND[256]="Занят";

__root const CFG_HDR cfghdr30={CFG_STR_WIN1251,LG_STINVIS,0,255};
__root const char DEFTEX_INVISIBLE[256]="";

__root const CFG_HDR cfghdr_DEFTEXTSTAT_LEVEL_END={CFG_LEVEL,"",0,0};


//////////////////Default states///////////////////////////

__root const CFG_HDR cfghdr_DEFSTATES_LEVEL={CFG_LEVEL,LG_DEFSTATES,1,0};

__root const CFG_HDR cfghdr31={CFG_CHECKBOX,LG_MSOUND,0,2};
__root const int DEF_SOUND_STATE=1;

__root const CFG_HDR cfghdr32={CFG_CHECKBOX,LG_MVIBRA,0,2};
__root const int DEF_VIBRA_STATE=1;

__root const CFG_HDR cfghdr33={CFG_CHECKBOX,LG_MOFFLINE,0,2};
__root const int DEF_SHOW_OFFLINE=1;

__root const CFG_HDR cfghdr_DEFSTATES_LEVEL_END={CFG_LEVEL,"",0,0};
// EOL, EOF
