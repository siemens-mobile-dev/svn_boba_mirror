#include "..\inc\cfg_items.h"
//Конфигурация

#ifdef NEWSGOLD
#define DEFAULT_DISK_CFG "4"
#else
#define DEFAULT_DISK_CFG "0"
#endif

////////////////////////////////////////////////////////////////////////////////
__root const CFG_HDR cfghdr_CONNECT_LEVEL={CFG_LEVEL,"Connection setup",1,0};

__root const CFG_HDR cfghdr0={CFG_UTF8_STRING,"Login",0,31};
__root const char USERNAME[32]="";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char PASSWORD[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"Server",0,127};
__root const char JABBER_SERVER[128]="jabber.ru";

__root const CFG_HDR cfghdr3={CFG_STR_WIN1251,"Host",0,127};
__root const char JABBER_HOST[128]="jabber.ru";

__root const CFG_HDR cfghdr3_1={CFG_CBOX,"Host type",0,2};
__root const int IS_IP = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"Host name","IP address"};

__root const CFG_HDR cfghdr4={CFG_UINT,"Port",0,65535};
__root const unsigned int JABBER_PORT=5222;

__root const CFG_HDR cfghdr5={CFG_CBOX,"Auth type",0,2};
__root const int USE_SASL = 0;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"Simple","SASL"};

__root const CFG_HDR cfghdr6={CFG_CBOX,"ZLib usage",0,2};
__root const int USE_ZLIB = 0;
__root const CFG_CBOX_ITEM cfgcbox3[2]={"No","Yes"};

__root const CFG_HDR cfghdr_CONNECT_LEVEL_END={CFG_LEVEL,"",0,0};

////////////////////////////////////////////////////////////////////////////////

__root const CFG_HDR cfghdr7={CFG_CBOX,"History",0,2};
__root const int WRITE_HISTORY = 0;
__root const CFG_CBOX_ITEM cfgcbox1435[2]={"Disabled","Enabled"};

__root const CFG_HDR cfghdr8={CFG_CBOX,"MUC history",0,2};
__root const int WRITE_MUC_HISTORY = 0;
__root const CFG_CBOX_ITEM cfgcbox36546[2]={"Disabled","Enabled"};

__root const CFG_HDR cfghdr9={CFG_UTF8_STRING,"Default MUC:",0,63};
__root const char DEFAULT_MUC[64]="siepatchdb@conference.jabber.ru";

__root const CFG_HDR cfghdr10={CFG_UTF8_STRING,"Default MUC nick:",0,63};
__root const char DEFAULT_MUC_NICK[64]="";

__root const CFG_HDR cfghdr11={CFG_COORDINATES,"Idle icon position",0,0};
__root const unsigned int IDLE_ICON_X=0;
__root const unsigned int IDLE_ICON_Y=50;

__root const CFG_HDR cfghdr17657={CFG_CHECKBOX,"Display pop-ups",0,2};
__root const int DISPLAY_POPUPS=1;

__root const CFG_HDR cfghdr12={CFG_CHECKBOX,"Composing Events",0,2};
__root const int COMPOSING_EVENTS=1;

__root const CFG_HDR cfghdr15345={CFG_CHECKBOX,"Delivery Report if required",0,2};
__root const int DELIVERY_EVENTS=1;
// Настройка звуков
////////////////////////////////////////////////////////////////////////////////
__root const CFG_HDR cfghdr_SOUNDS_SETUP={CFG_LEVEL,"Sounds setup",1,0};

__root const CFG_HDR cfghdr13={CFG_CHECKBOX,"Default Sounds State",0,2};
__root const int Default_Sounds_State=1;

__root const CFG_HDR cfghdr14={CFG_UINT,"sndVolume",0,6};
__root const unsigned int sndVolume=6;

__root const CFG_HDR cfghdr15={CFG_STR_UTF8,"Sound Msg Send",0,63};
__root const char sndMsgSend[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Sounds\\send.wav";

__root const CFG_HDR cfghdr16={CFG_STR_UTF8,"Sound Msg Conf",0,63};
__root const char sndConf[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Sounds\\conf.wav";

__root const CFG_HDR cfghdr17={CFG_STR_UTF8,"Sound Msg PM",0,63};
__root const char sndPM[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Sounds\\message.wav";
////////////////////////////////////////////////////////////////////////////////
__root const CFG_HDR cfghdr_SOUNDS_SETUP_END={CFG_LEVEL,"",0,0};

////////////////////////////// Пути ////////////////////////////////////////////

__root const CFG_HDR cfghdr_PATHS_LEVEL={CFG_LEVEL,"Paths setup",1,0};

__root const CFG_HDR cfghdr18={CFG_STR_UTF8,"History path",0,127};
__root const char HIST_PATH[128]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\history\\";

__root const CFG_HDR cfghdr19={CFG_STR_UTF8,"Pictures path",0,127};
__root const char PATH_TO_PIC[128]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\img\\";

__root const CFG_HDR cfghdr20={CFG_STR_UTF8,"Commands path",0,63};
__root const char COMMANDS_PATH[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Templates\\commands.txt";

__root const CFG_HDR cfghdr21={CFG_STR_UTF8,"Messages path",0,63};
__root const char MESSAGES_PATH[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Templates\\Messages.txt";

__root const CFG_HDR cfghdr22={CFG_STR_UTF8,"Smiles path",0,63};
__root const char SMILES_PATH[64]=DEFAULT_DISK_CFG ":\\ZBin\\SieJC\\Templates\\Smiles.txt";

__root const CFG_HDR cfghdr23={CFG_STR_WIN1251,"Default disc",0,127};
__root const char DEFAULT_DISC[128]=DEFAULT_DISK_CFG;

__root const CFG_HDR cfghdr_PATHS_LEVEL_END={CFG_LEVEL,"",0,0};

// EOL, EOF
