#include "..\inc\cfg_items.h"
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_STR_WIN1251,"Login",0,31};
__root const char USERNAME[32]="";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char PASSWORD[32]="";

__root const CFG_HDR cfghdr1_1={CFG_STR_WIN1251,"Server",0,127};
__root const char JABBER_HOST[128]="jabber.ru";

__root const CFG_HDR cfghdr1_2={CFG_UINT,"Port",0,65535};
__root const unsigned int JABBER_PORT=5222;
