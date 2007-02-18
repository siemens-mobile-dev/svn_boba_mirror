#include "..\inc\cfg_items.h"
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_STR_WIN1251,"Login",0,31};
__root const char USERNAME[32]="";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char PASSWORD[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"Server",0,127};
__root const char JABBER_SERVER[128]="jabber.ru";

__root const CFG_HDR cfghdr3={CFG_STR_WIN1251,"Host",0,127};
__root const char JABBER_HOST[128]="jabber.ru";

__root const CFG_HDR cfghdr4={CFG_UINT,"Port",0,65535};
__root const unsigned int JABBER_PORT=5222;

__root const CFG_HDR cfghdr5={CFG_STR_WIN1251,"History path",0,255};
__root const char HIST_PATH[256]="4:\\SieJC_history";
