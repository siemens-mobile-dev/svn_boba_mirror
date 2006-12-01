#include "..\inc\cfg_items.h"
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_STR_WIN1251,"Login",0,63};
__root const char MAIL_LOGIN[64]="";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char MAIL_PASS[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"Eml Files Path",0,63};
__root const char HIST_PATH[64]="0:\\EML\\";

__root const CFG_HDR cfghdr3={CFG_CBOX,"Load",0,2};
__root const int LOAD_ONLY = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"Only Headers","Headers+Text"};

__root const CFG_HDR cfghdr4={CFG_UINT,"Message",0,32767};
__root const unsigned int ICON0=1004;

__root const CFG_HDR cfghdr6={CFG_UINT,"Only Header",0,32767};
__root const unsigned int ICON1=1005;

__root const CFG_HDR cfghdr7={CFG_UINT,"Message for download",0,32767};
__root const unsigned int ICON2=1010;

__root const CFG_HDR cfghdr8={CFG_UINT,"Message for delete",0,32767};
__root const unsigned int ICON3=1006;

