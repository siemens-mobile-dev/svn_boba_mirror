#include "..\inc\cfg_items.h"

#ifdef NEWSGOLD

__root const CFG_HDR cfghdr0={CFG_STR_WIN1251,"Login",0,63};
__root const char MAIL_LOGIN[64]="Rst7@list.ru";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char MAIL_PASS[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"POP3 adress",0,31};
__root const char POP3_ADRESS[32]="pop.list.ru";

__root const CFG_HDR cfghdr3={CFG_UINT,"POP3 port",0,65535};
__root const unsigned int POP3_PORT=110;

__root const CFG_HDR cfghdr4={CFG_STR_WIN1251,"Eml Files Path",0,63};
__root const char EML_PATH[64]="4:\\ZBin\\EMailClient\\data\\";

__root const CFG_HDR cfghdr5={CFG_CBOX,"Load",0,2};
__root const int DEFAULT_ACTION = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"Only Headers","Headers+Text"};

__root const CFG_HDR cfghdr6={CFG_CBOX,"Action if mail del from server:",0,2};
__root const int IF_DEL_AUX = 0;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"Delete From Phone","Nothing"};

__root const CFG_HDR cfghdr7={CFG_STR_UTF8,"Icon Message Unreaded",3,63};
__root const char I_UNREAD[64]="4:\\ZBin\\EMailClient\\img\\message_unread.png";

__root const CFG_HDR cfghdr8={CFG_STR_UTF8,"Icon Message readed",3,63};
__root const char I_READ[64]="4:\\ZBin\\EMailClient\\img\\message_read.png";

__root const CFG_HDR cfghdr9={CFG_STR_UTF8,"Icon Only Header",3,63};
__root const char I_HEADER[64]="4:\\ZBin\\EMailClient\\img\\only_head.png";

__root const CFG_HDR cfghdr10={CFG_STR_UTF8,"Icon Header with attach",3,63};
__root const char I_HEADER_ATT[64]="4:\\ZBin\\EMailClient\\img\\only_head_att.png";

__root const CFG_HDR cfghdr11={CFG_STR_UTF8,"Message with attach",3,63};
__root const char I_MES_ATT[64]="4:\\ZBin\\EMailClient\\img\\mes_with_att.png";

__root const CFG_HDR cfghdr12={CFG_STR_UTF8,"Icon Message for download",3,63};
__root const char I_MES_DOWN[64]="4:\\ZBin\\EMailClient\\img\\mes_for_download.png";

__root const CFG_HDR cfghdr13={CFG_STR_UTF8,"Icon Message for delete",3,63};
__root const char I_MES_DEL[64]="4:\\ZBin\\EMailClient\\img\\mes_for_delete.png";


#else
//Конфигурация
__root const CFG_HDR cfghdr0={CFG_STR_WIN1251,"Login",0,63};
__root const char MAIL_LOGIN[64]="aru55@km.ru";

__root const CFG_HDR cfghdr1={CFG_STR_PASS,"Password",0,31};
__root const char MAIL_PASS[32]="";

__root const CFG_HDR cfghdr2={CFG_STR_WIN1251,"POP3 adress",0,31};
__root const char POP3_ADRESS[32]="pop.km.ru";

__root const CFG_HDR cfghdr3={CFG_UINT,"POP3 port",0,65535};
__root const unsigned int POP3_PORT=110;

__root const CFG_HDR cfghdr4={CFG_STR_WIN1251,"Eml Files Path",0,63};
__root const char EML_PATH[64]="0:\\EML\\";

__root const CFG_HDR cfghdr5={CFG_CBOX,"Load",0,2};
__root const int DEFAULT_ACTION = 0;
__root const CFG_CBOX_ITEM cfgcbox1[2]={"Only Headers","Headers+Text"};

__root const CFG_HDR cfghdr6={CFG_CBOX,"Action if mail del from server:",0,2};
__root const int IF_DEL_AUX = 0;
__root const CFG_CBOX_ITEM cfgcbox2[2]={"Delete From Phone","Nothing"};

__root const CFG_HDR cfghdr7={CFG_STR_UTF8,"Icon Message Unreaded",3,63};
__root const char I_UNREAD[64]="0:\\ZBin\\Mails\\message_unread.png";

__root const CFG_HDR cfghdr8={CFG_STR_UTF8,"Icon Message readed",3,63};
__root const char I_READ[64]="0:\\ZBin\\Mails\\message_read.png";

__root const CFG_HDR cfghdr9={CFG_STR_UTF8,"Icon Only Header",3,63};
__root const char I_HEADER[64]="0:\\ZBin\\Mails\\only_head.png";

__root const CFG_HDR cfghdr10={CFG_STR_UTF8,"Icon Header with attach",3,63};
__root const char I_HEADER_ATT[64]="0:\\ZBin\\Mails\\only_head_att.png";

__root const CFG_HDR cfghdr11={CFG_STR_UTF8,"Message with attach",3,63};
__root const char I_MES_ATT[64]="0:\\ZBin\\Mails\\mes_with_att.png";

__root const CFG_HDR cfghdr12={CFG_STR_UTF8,"Icon Message for download",3,63};
__root const char I_MES_DOWN[64]="0:\\ZBin\\Mails\\mes_for_download.png";

__root const CFG_HDR cfghdr13={CFG_STR_UTF8,"Icon Message for delete",3,63};
__root const char I_MES_DEL[64]="0:\\ZBin\\Mails\\mes_for_delete.png";

#endif

