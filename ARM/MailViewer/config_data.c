#include "..\inc\cfg_items.h"
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_STR_WIN1251,"Login",0,63};
__root const char MAIL_LOGIN[64]="";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char MAIL_PASS[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"IP adress",0,31};
__root const char IP_ADRESS[32]="217.197.114.149:110";

__root const CFG_HDR cfghdr3={CFG_STR_WIN1251,"Eml Files Path",0,63};
__root const char HIST_PATH[64]="0:\\EML\\";

__root const CFG_HDR cfghdr4={CFG_CBOX,"Load",0,2};
__root const int LOAD_ONLY = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"Only Headers","Headers+Text"};

__root const CFG_HDR cfghdr5={CFG_STR_UTF8,"Icon Message Unreaded",3,63};
__root const char ICON0[64]="0:\\ZBin\\Mailer\\message_unread.png";

__root const CFG_HDR cfghdr6={CFG_STR_UTF8,"Icon Message readed",3,63};
__root const char ICON1[64]="0:\\ZBin\\Mailer\\message_read.png";

__root const CFG_HDR cfghdr7={CFG_STR_UTF8,"Icon Only Header",3,63};
__root const char ICON2[64]="0:\\ZBin\\Mailer\\only_head.png";

__root const CFG_HDR cfghdr8={CFG_STR_UTF8,"Icon Header with attach",3,63};
__root const char ICON3[64]="0:\\ZBin\\Mailer\\only_head_att.png";

__root const CFG_HDR cfghdr9={CFG_STR_UTF8,"Message with attach",3,63};
__root const char ICON4[64]="0:\\ZBin\\Mailer\\mes_with_att.png";

__root const CFG_HDR cfghdr10={CFG_STR_UTF8,"Icon Message for download",3,63};
__root const char ICON5[64]="0:\\ZBin\\Mailer\\mes_for_download.png";

__root const CFG_HDR cfghdr11={CFG_STR_UTF8,"Icon Message for delete",3,63};
__root const char ICON6[64]="0:\\ZBin\\Mailer\\mes_for_delete.png";



