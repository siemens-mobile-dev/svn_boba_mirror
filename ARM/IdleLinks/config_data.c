#include "..\inc\cfg_items.h"
#include "..\inc\swilib.h"


#ifdef NEWSGOLD
#define DEFAULT_DISK "4"
#else
#define DEFAULT_DISK "0"
#endif

// Настройка клавиш

__root const CFG_HDR cfghdr0={CFG_LEVEL,"Keys",1,0};

  __root const CFG_HDR cfghdr0_1={CFG_KEYCODE,"Activate key",0,100};
  __root const unsigned int BCFG_CALL_BTN=RIGHT_SOFT;
  
  __root const CFG_HDR cfghdr0_2={CFG_CBOX,"Activate press type",0,2};
  __root const unsigned int BCFG_CALL_TYPE = 0;
  __root const CFG_CBOX_ITEM cfgcbox1[2]={"Short","Long"};

  __root const CFG_HDR cfghdr0_3={CFG_KEYCODE,"Close key",0,100};
  __root const unsigned int BCFG_CLOSE_BTN=RIGHT_SOFT;

__root const CFG_HDR cfghdr1={CFG_LEVEL,"",0,0};

// Настройка отображения

__root const CFG_HDR cfghdr2={CFG_LEVEL,"View",1,0};

  __root const CFG_HDR cfghdr2_1={CFG_CBOX,"Show on",0,3};
  __root const unsigned int BCFG_KBD_STATE = 1;
  __root const CFG_CBOX_ITEM cfgcbox2[3]={"Lock keyboard","Unlock keyboard", "Both"};
  
  __root const CFG_HDR cfghdr2_2={CFG_CBOX,"Cycled navigation",0,3};
  __root const unsigned int BCFG_CYCLED = 1;
  __root const CFG_CBOX_ITEM cfgcbox3[3]={"None","In current line", "Go to next line"};
  
__root const CFG_HDR cfghdr3={CFG_LEVEL,"",0,0};

// Настройка выбора

__root const CFG_HDR cfghdr4={CFG_LEVEL,"Select",1,0};

  __root const CFG_HDR cfghdr4_1={CFG_CBOX,"Select type",0,2};
  __root const int BCFG_SELECT_TYPE=1;
  __root const CFG_CBOX_ITEM cfgcbox4[2]={"Color","Picture"};

  __root const CFG_HDR cfghdr4_2={CFG_STR_UTF8,"Select picture",0,127};
  __root const char chpic[128]=DEFAULT_DISK ":\\ZBin\\IdleLinks\\choose.png";

  __root const CFG_HDR cfghdr14_3={8,"Cursor background color",0,0};
  __root const char cl[4]={185,113,13,50};

  __root const CFG_HDR cfghdr4_4={8,"Cursor color",0,0};
  __root const char frcol[4]={255,255,255,100};
  
  __root const CFG_HDR cfghdr4_5={CFG_UINT,"Cursor round on X",0,0xFFFFFFFF};
  __root const unsigned int BCFG_ROUND_X = 0;
  
  __root const CFG_HDR cfghdr4_6={CFG_UINT,"Cursor round on Y",0,0xFFFFFFFF};
  __root const unsigned int BCFG_ROUND_Y = 0;
  
__root const CFG_HDR cfghdr5={CFG_LEVEL,"",0,0};

// Настройка вывода имени ярлычка

__root const CFG_HDR cfghdr6={CFG_LEVEL,"Link name",1,0}; // Twitch - делаем подписи к ярлычкам

  __root const CFG_HDR cfghdr5_1={CFG_CBOX,"How draw link name",0,3};	// Как рисовать
  __root const unsigned int linkname_ena = 1;
  __root const CFG_CBOX_ITEM cfgcbox03[3]={"None", "For each", "Only selected"};
  
  __root const CFG_HDR cfghdr5_2={CFG_LEVEL,"Set for each",1,0};	//Если подпись к каждому
  
    __root const CFG_HDR cfghdr5_2_1={CFG_CBOX,"Where draw link name",0,2};	//Где рисовать
    __root const unsigned int linkname_wdraw = 0;
    __root const CFG_CBOX_ITEM cfgcbox04[2]={"under link", "over link"};
    
    __root const CFG_HDR cfghdr5_2_2={CFG_UINT,"Shift",0,12};			//смещение
    __root const unsigned int LN_SHIFT=2;
    
  __root const CFG_HDR cfghdr5_3={CFG_LEVEL,"",0,0};
  
  __root const CFG_HDR cfghdr5_4={CFG_LEVEL,"Set for selected",1,0};		//если только для выделенного
  
    __root const CFG_HDR cfghdr5_4_1={7,"Coords link name",0,0};		//где рисовать
    __root const unsigned int linkname_x=110;
    __root const unsigned int linkname_y=295;
  
    __root const CFG_HDR cfghdr5_4_2={CFG_CBOX,"Align link name",0,3};		//выравнивание
    __root const unsigned int linkname_al = 0;
    __root const CFG_CBOX_ITEM cfgcbox05[3]={"Left", "Center", "Right"};
    
  __root const CFG_HDR cfghdr5_5={CFG_LEVEL,"",0,0};
  
  __root const CFG_HDR cfghdr5_6={CFG_COLOR,"Link name color",0,0};	//цвет подписи
  __root const char LN_COLOR[4]={255,255,255,100};

  __root const CFG_HDR cfghdr5_7={CFG_UINT,"Link name font",0,12};	//шрифт
  __root const unsigned int LN_FONT=8;
  
  __root const CFG_HDR cfghdr5_8={CFG_UINT,"Link name sel. font",0,12};	//шрифт выделенного
  __root const unsigned int LN_SEL_FONT=9;

  __root const CFG_HDR cfghdr5_9 = {CFG_CHECKBOX, "Link name fring", 0, 2};	//обводка
  __root const int LN_FRING = 0;

  __root const CFG_HDR cfghdr5_10={CFG_COLOR,"Link name fring color",0,0};	//цвет обводки
  __root const char LN_FRING_COLOR[4]={0,255,0,100};

__root const CFG_HDR cfghdr7={CFG_LEVEL,"",0,0};
//




