#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"
#include "bmp_setup.h"

struct MAINSCREEN far* EditorGetMainscreenVars(void);
static const unsigned int md_tab[]={0xFFF5};

//===================================================================
// MenuUNISetup
//===================================================================
static const MENUITEM_DESC far MenuUNISetupITEMS[10]=
{
 NULL, lng_ScreenSetup, lng_ScreenSetup, 0,NULL,3,0x2BC,
 NULL, lng_KeysSetup,   lng_KeysSetup,   0,NULL,3,0x2BC,
 NULL, lng_Dialogs,     lng_Dialogs,     0,NULL,3,0x2BC,
 NULL, lng_Messages,    lng_Messages,    0,NULL,3,0x2BC,
 NULL, lng_Security,    lng_Security,    0,NULL,3,0x2BC,
 NULL, lng_System,      lng_System,      0,NULL,3,0x2BC,
 NULL, lng_EndMenuLong, lng_EndMenuLong, 0,NULL,3,0x2BC, //lng_MP3
 NULL, lng_LoadDefaults,lng_LoadDefaults,0,NULL,3,0x2BC,
 NULL, lng_SaveSettings,lng_SaveSettings,0,NULL,3,0x2BC,
 NULL, lng_EndMenuLong, lng_EndMenuLong, 0,NULL,3,0x2BC
};

void MenuScreenSetup(void);
void MenuKeysSetup(void);
void MenuDialogs(void);
void MenuMessages(void);
void MenuSecurity(void);
void MenuSystem(void);
void MenuMP3(void){}
void LoadDefaults(void);
void SaveSettings(void);

static const MENUITEM_HNDL far MenuUNISetupHNDLS[10]=
{
 MenuScreenSetup, 
 MenuKeysSetup,   
 MenuDialogs,     
 MenuMessages,    
 MenuSecurity,
 MenuSystem,      
 MenuMP3,
 LoadDefaults,
 SaveSettings,
 rets
};

static const HEADER_DESC far MenuUNISetupHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_UNISetup,0x7FFF 
};

static const MENU_DESC far MenuUNISetupSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (MENUITEM_DESC far*)&MenuUNISetupITEMS,
 (MENUITEM_HNDL far*)&MenuUNISetupHNDLS,
 10
};
void MenuUNISetup(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuUNISetupSTRUCT,(HEADER_DESC far*)&MenuUNISetupHDR,0,0,0,0);
}
//===================================================================
// MenuScreenSetup
//===================================================================
static const MENUITEM_DESC far MenuScreenSetupITEMS[10]=
{
 NULL, lng_NormalScreen, lng_NormalScreen, 0,NULL,3,0x2BC,
 NULL, lng_HiddenIndic,  lng_HiddenIndic,  0,NULL,3,0x2BC,
 NULL, lng_KeylckScreen, lng_KeylckScreen, 0,NULL,3,0x2BC,
 NULL, lng_CopyToKeylck, lng_CopyToKeylck, 0,NULL,3,0x2BC,
 NULL, lng_EditorScreen, lng_EditorScreen, 0,NULL,3,0x2BC,
 NULL, lng_SetupOtherScr,lng_SetupOtherScr,0,NULL,3,0x2BC,
 NULL, lng_SelectSsaver, lng_SelectSsaver, 0,NULL,3,0x2BC,
 NULL, lng_EditConst,    lng_EditConst,    0,NULL,3,0x2BC,
 NULL, lng_EditAnClock,  lng_EditAnClock,  0,NULL,3,0x2BC,
 NULL, lng_EndMenuLong,  lng_EndMenuLong,  0,NULL,3,0x2BC
};

void MenuNormalScreen(void);
void MenuHiddenIndic(void);
void MenuKeylckScreen(void); 
void CopyToKeylck(void);
void EditorScreen(void);
void MenuSetupOtherScr(void);
void SelectSsaver(void);
void EditConst(void);
void EditAnClock(void);

static const MENUITEM_HNDL far MenuScreenSetupHNDLS[10]=
{
 MenuNormalScreen,
 MenuHiddenIndic,
 MenuKeylckScreen, 
 CopyToKeylck,
 EditorScreen,
 MenuSetupOtherScr,
 SelectSsaver,
 EditConst,
 EditAnClock,
 rets
};

static const HEADER_DESC far MenuScreenSetupHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_ScreenSetup,0x7FFF 
};

static const MENU_DESC far MenuScreenSetupSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (MENUITEM_DESC far*)&MenuScreenSetupITEMS,
 (MENUITEM_HNDL far*)&MenuScreenSetupHNDLS,
 10
};

void MenuScreenSetup(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuScreenSetupSTRUCT,(HEADER_DESC far*)&MenuScreenSetupHDR,0,0,0,0);
}
//===================================================================
// MenuNormalScreen
//===================================================================
static const MENUITEM_DESC far MenuNormalScreenITEMS[8]=
{
 NULL, lng_Logo,lng_Logo,              0,NULL,3,0x2BC,
 NULL, lng_Battery,lng_Battery,        0,NULL,3,0x2BC,
 NULL, lng_Antenna,lng_Antenna,        0,NULL,3,0x2BC,
 NULL, lng_DateFormat,lng_DateFormat,  0,NULL,3,0x2BC,
 NULL, lng_DatePos,lng_DatePos,        0,NULL,3,0x2BC,
 NULL, lng_TimePos,lng_TimePos,        0,NULL,3,0x2BC,
 NULL, lng_OtherScreen,lng_OtherScreen,0,NULL,3,0x2BC,
 NULL, lng_EndMenuLong,lng_EndMenuLong,0,NULL,3,0x2BC
};

void MenuNormalScreen0(void);
void MenuNormalScreen1(void);
void MenuNormalScreen2(void);
void MenuNormalScreen3(void);
void MenuNormalScreen4(void);
void MenuNormalScreen5(void);
void MenuNormalScreen6(void);

static const MENUITEM_HNDL far MenuNormalScreenHNDLS[8]=
{
 MenuNormalScreen0,
 MenuNormalScreen1,
 MenuNormalScreen2,
 MenuNormalScreen3,
 MenuNormalScreen4,
 MenuNormalScreen5,
 MenuNormalScreen6,
 rets
};

static const HEADER_DESC far MenuNormalScreenHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_NormalScreen,0x7FFF
};

static const MENU_DESC far MenuNormalScreenSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (MENUITEM_DESC far*)&MenuNormalScreenITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreenHNDLS,
 8
};

static const HEADER_DESC far MenuKeylckScreenHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_KeylckScreen,0x7FFF
};

void MenuNormalScreen(void)
{
 SSV.KeylckState=0;
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreenSTRUCT,(HEADER_DESC far*)&MenuNormalScreenHDR,0,0,0,0);
}
//===================================================================
// MenuHiddenIndic
//===================================================================
static const MENUITEM_DESC far MenuHiddenIndicITEMS[9]=
{
 (int far*)0xBAFC22,lng_HiddenIndic0,lng_HiddenIndic0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic1,lng_HiddenIndic1,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic2,lng_HiddenIndic2,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic3,lng_HiddenIndic3,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic4,lng_HiddenIndic4,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic5,lng_HiddenIndic5,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic6,lng_HiddenIndic6,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic7,lng_HiddenIndic7,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_HiddenIndic8,lng_HiddenIndic8,0,NULL,6,0x53A
};

void MenuHiddenIndicState(char far* Unk1, int Curr, int Unk3);
void MenuHiddenIndicSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuHiddenIndicHNDLS[16]=
{
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect,
 MenuHiddenIndicSelect
};

static const HEADER_DESC far MenuHiddenIndicHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_HiddenIndic,0x7FFF
};

static const MENU_DESC far MenuHiddenIndicSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuHiddenIndicState,
 (MENUITEM_DESC far*)&MenuHiddenIndicITEMS,
 (MENUITEM_HNDL far*)&MenuHiddenIndicHNDLS,
 9
};

void MenuHiddenIndic(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuHiddenIndicSTRUCT,(HEADER_DESC far*)&MenuHiddenIndicHDR,0,0,0,0);
}

void MenuHiddenIndicState(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 if (uni_vars.screen_vars.hide_bit>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuHiddenIndicSelect(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 uni_vars.screen_vars.hide_bit^=i;
 RefreshMenuItem(Unk1);
}
//===================================================================
// MenuKeylckScreen
//===================================================================
void MenuKeylckScreen(void)
{
 SSV.KeylckState=1;
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreenSTRUCT,(HEADER_DESC far*)&MenuKeylckScreenHDR,0,0,0,0);
}
//===================================================================
//MenuNormalScreen0
//===================================================================
static const MENUITEM_DESC far MenuNormalScreen0ITEMS[6]=
{
 (int far*)0xB867CE,lng_Logo0,lng_Logo0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Logo1,lng_Logo1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Logo2,lng_Logo2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Logo3,lng_Logo3,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Logo4,lng_Logo4,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Logo5,lng_Logo5,0,NULL,3,0x53A
};

void MenuNormalScreen0State(char far* Unk1, int Curr, int Unk3);
void MenuNormalScreen0Select(char far* Unk1);

static const MENUITEM_HNDL far MenuNormalScreen0HNDLS[6]=
{
 MenuNormalScreen0Select,
 MenuNormalScreen0Select,
 MenuNormalScreen0Select,
 MenuNormalScreen0Select,
 MenuNormalScreen0Select,
 MenuNormalScreen0Select
};

static const HEADER_DESC far MenuNormalScreen0HDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_Logo,0x7FFF
};

static const MENU_DESC far MenuNormalScreen0STRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuNormalScreen0State,
 (MENUITEM_DESC far*)&MenuNormalScreen0ITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreen0HNDLS,
 6
};

void MenuNormalScreen0(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreen0STRUCT,(HEADER_DESC far*)&MenuNormalScreen0HDR,0,0,0,0);
}

void MenuNormalScreen0State(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 if (EditorGetMainscreenVars()->logotype==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuNormalScreen0Select(char far* Unk1)
{
 EditorGetMainscreenVars()->logotype=GetCurrentMenuItem(Unk1);
 GeneralFunc(0);
}
//===================================================================
//MenuNormalScreen1
//===================================================================
static const MENUITEM_DESC far MenuNormalScreen1ITEMS[4]=
{
 (int far*)0xB867CE,lng_Battery0,lng_Battery0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Battery1,lng_Battery1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Battery2,lng_Battery2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Battery3,lng_Battery3,0,NULL,3,0x53A,
};

void MenuNormalScreen1State(char far* Unk1, int Curr, int Unk2);
void MenuNormalScreen1Select(char far* Unk1);

static const MENUITEM_HNDL far MenuNormalScreen1HNDLS[4]=
{
 MenuNormalScreen1Select,
 MenuNormalScreen1Select,
 MenuNormalScreen1Select,
 MenuNormalScreen1Select
};

static const HEADER_DESC far MenuNormalScreen1HDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_Battery,0x7FFF
};

static const MENU_DESC far MenuNormalScreen1STRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuNormalScreen1State,
 (MENUITEM_DESC far*)&MenuNormalScreen1ITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreen1HNDLS,
 4
};

void MenuNormalScreen1(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreen1STRUCT,(HEADER_DESC far*)&MenuNormalScreen1HDR,0,0,0,0);
}

void MenuNormalScreen1State(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 unsigned int set1=EditorGetMainscreenVars()->set1;
 if ((set1>>_GET_BATTERY)==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuNormalScreen1Select(char far* Unk1)
{
 int i=GetCurrentMenuItem(Unk1)<<12;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 mainscreen->set1&=0xCFFF;
 mainscreen->set1|=i;
 GeneralFunc(0);
}
//===================================================================
//MenuNormalScreen2
//===================================================================
static const MENUITEM_DESC far MenuNormalScreen2ITEMS[4]=
{
 (int far*)0xB867CE,lng_Antenna0,lng_Antenna0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Antenna1,lng_Antenna1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Antenna2,lng_Antenna2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_Antenna3,lng_Antenna3,0,NULL,3,0x53A,
};

void MenuNormalScreen2State(char far* Unk1, int Curr, int Unk2);
void MenuNormalScreen2Select(char far* Unk1);

static const MENUITEM_HNDL far MenuNormalScreen2HNDLS[4]=
{
 MenuNormalScreen2Select,
 MenuNormalScreen2Select,
 MenuNormalScreen2Select,
 MenuNormalScreen2Select
};

static const HEADER_DESC far MenuNormalScreen2HDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_Antenna,0x7FFF
};

static const MENU_DESC far MenuNormalScreen2STRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuNormalScreen2State,
 (MENUITEM_DESC far*)&MenuNormalScreen2ITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreen2HNDLS,
 4
};

void MenuNormalScreen2(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreen2STRUCT,(HEADER_DESC far*)&MenuNormalScreen2HDR,0,0,0,0);
}

void MenuNormalScreen2State(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 unsigned int set1=EditorGetMainscreenVars()->set1;
 if ((set1>>_GET_ANTENNA)==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuNormalScreen2Select(char far* Unk1)
{
 int i=GetCurrentMenuItem(Unk1)<<14;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 mainscreen->set1&=0x3FFF;
 mainscreen->set1|=i;
 GeneralFunc(0);
}
//===================================================================
//MenuNormalScreen3
//===================================================================
static const MENUITEM_DESC far MenuNormalScreen3ITEMS[4]=
{
 (int far*)0xB867CE,lng_DateFormat0,lng_DateFormat0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_DateFormat1,lng_DateFormat1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_DateFormat2,lng_DateFormat2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_DateFormat3,lng_DateFormat3,0,NULL,3,0x53A,
};

void MenuNormalScreen3State(char far* Unk1, int Curr, int Unk3);
void MenuNormalScreen3Select(char far* Unk1);

static const MENUITEM_HNDL far MenuNormalScreen3HNDLS[4]=
{
 MenuNormalScreen3Select,
 MenuNormalScreen3Select,
 MenuNormalScreen3Select,
 MenuNormalScreen3Select
};

static const HEADER_DESC far MenuNormalScreen3HDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_DateFormat,0x7FFF
};

static const MENU_DESC far MenuNormalScreen3STRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuNormalScreen3State,
 (MENUITEM_DESC far*)&MenuNormalScreen3ITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreen3HNDLS,
 4
};

void MenuNormalScreen3(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreen3STRUCT,(HEADER_DESC far*)&MenuNormalScreen3HDR,0,0,0,0);
}

void MenuNormalScreen3State(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 unsigned int set1=EditorGetMainscreenVars()->set1;
 if ((set1>>_GET_DATE_FORMAT)==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuNormalScreen3Select(char far* Unk1)
{
 int i=GetCurrentMenuItem(Unk1)<<10;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 mainscreen->set1&=0xF3FF;
 mainscreen->set1|=i;
 GeneralFunc(0);
}
//===================================================================
//MenuNormalScreen4
//===================================================================
static const MENUITEM_DESC far MenuNormalScreen4ITEMS[4]=
{
 (int far*)0xB867CE,lng_DatePos0,lng_DatePos0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_DatePos1,lng_DatePos1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_DatePos2,lng_DatePos2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_DatePos3,lng_DatePos3,0,NULL,3,0x53A,
};

void MenuNormalScreen4State(char far* Unk1, int Curr, int Unk3);
void MenuNormalScreen4Select(char far* Unk1);

static const MENUITEM_HNDL far MenuNormalScreen4HNDLS[4]=
{
 MenuNormalScreen4Select,
 MenuNormalScreen4Select,
 MenuNormalScreen4Select,
 MenuNormalScreen4Select
};

static const HEADER_DESC far MenuNormalScreen4HDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_DatePos,0x7FFF
};

static const MENU_DESC far MenuNormalScreen4STRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuNormalScreen4State,
 (MENUITEM_DESC far*)&MenuNormalScreen4ITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreen4HNDLS,
 4
};

void MenuNormalScreen4(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreen4STRUCT,(HEADER_DESC far*)&MenuNormalScreen4HDR,0,0,0,0);
}

void MenuNormalScreen4State(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 unsigned int set1=EditorGetMainscreenVars()->set1;
 if ((set1>>_GET_DATE_POS)==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuNormalScreen4Select(char far* Unk1)
{
 int i=GetCurrentMenuItem(Unk1)<<8;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 mainscreen->set1&=0xFCFF;
 mainscreen->set1|=i;
 GeneralFunc(0);
}
//===================================================================
//MenuNormalScreen5
//===================================================================
static const MENUITEM_DESC far MenuNormalScreen5ITEMS[4]=
{
 (int far*)0xB867CE,lng_TimePos0,lng_TimePos0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_TimePos1,lng_TimePos1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_TimePos2,lng_TimePos2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_TimePos3,lng_TimePos3,0,NULL,3,0x53A,
};

void MenuNormalScreen5State(char far* Unk1, int Curr, int Unk3);
void MenuNormalScreen5Select(char far* Unk1);

static const MENUITEM_HNDL far MenuNormalScreen5HNDLS[4]=
{
 MenuNormalScreen5Select,
 MenuNormalScreen5Select,
 MenuNormalScreen5Select,
 MenuNormalScreen5Select
};

static const HEADER_DESC far MenuNormalScreen5HDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_TimePos,0x7FFF
};

static const MENU_DESC far MenuNormalScreen5STRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuNormalScreen5State,
 (MENUITEM_DESC far*)&MenuNormalScreen5ITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreen5HNDLS,
 4
};

void MenuNormalScreen5(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreen5STRUCT,(HEADER_DESC far*)&MenuNormalScreen5HDR,0,0,0,0);
}

void MenuNormalScreen5State(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 unsigned int set1=EditorGetMainscreenVars()->set1;
 if ((set1>>_GET_TIME_POS)==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuNormalScreen5Select(char far* Unk1)
{
 int i=GetCurrentMenuItem(Unk1)<<6;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 mainscreen->set1&=0xFF3F;
 mainscreen->set1|=i;
 GeneralFunc(0);
}
//===================================================================
//MenuNormalScreen6
//===================================================================
static const MENUITEM_DESC far MenuNormalScreen6ITEMS[15]=
{
 (int far*)0xBAFC22,lng_OtherScreen0,lng_OtherScreen0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen1,lng_OtherScreen1,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen2,lng_OtherScreen2,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen3,lng_OtherScreen3,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen4,lng_OtherScreen4,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen5,lng_OtherScreen5,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen6,lng_OtherScreen6,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen7,lng_OtherScreen7,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen8,lng_OtherScreen8,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreen9,lng_OtherScreen9,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreenA,lng_OtherScreenA,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreenB,lng_OtherScreenB,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreenC,lng_OtherScreenC,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreenD,lng_OtherScreenD,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_OtherScreenE,lng_OtherScreenE,0,NULL,6,0x53A
};

void MenuNormalScreen6State(char far* Unk1, int Curr, int Unk3);
void MenuNormalScreen6Select(char far* Unk1);

static const MENUITEM_HNDL far MenuNormalScreen6HNDLS[15]=
{
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select,
 MenuNormalScreen6Select
};

static const HEADER_DESC far MenuNormalScreen6HDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_OtherScreen,0x7FFF
};

static const MENU_DESC far MenuNormalScreen6STRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuNormalScreen6State,
 (MENUITEM_DESC far*)&MenuNormalScreen6ITEMS,
 (MENUITEM_HNDL far*)&MenuNormalScreen6HNDLS,
 15
};

void MenuNormalScreen6(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuNormalScreen6STRUCT,(HEADER_DESC far*)&MenuNormalScreen6HDR,0,0,0,0);
}

void MenuNormalScreen6State(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 unsigned int bit1=EditorGetMainscreenVars()->bit1;
 if (bit1>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuNormalScreen6Select(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 EditorGetMainscreenVars()->bit1^=i;
 RefreshMenuItem(Unk1);
}

//===================================================================
// CopyToKeylck
//===================================================================
int CopyToKeylckDia_onKey( int P1, int P2, int Key )
{
 if (Key==0x23) {
  memcpy(&uni_vars.screen_vars.screen_lock,&uni_vars.screen_vars.screen,sizeof(struct MAINSCREEN));
  DisplayMessageBoxV(1,0x55A);
 }
 return Key;
 P1=P1;
 P2=P2;
}

static const int far CopyToKeylckDia_SID[]={0x692,lng_CopyToKeylck,0x7FFF};
static const MSGBOX_DESC far CopyToKeylckDia =
{
 0,0,2,0,0,0x7FFF,
 NULL,
 &CopyToKeylckDia_onKey,
 NULL,
 (SOFTKEY_DESC far*)0xB8AC5A,
 20006,
 0,0
};

void CopyToKeylck(void)
{
 ShowMessageDlgWithIcon(&CopyToKeylckDia,(int far*)&CopyToKeylckDia_SID); 
}

//===================================================================
// EditorScreen
//===================================================================
void EditorScreenOnCreate(void);
int  EditorScreenOnKey(CSM_RAM far* data, GBS_MSG far* Msg);
void EditorScreenDraw(void);
void EditorScreenOnClose(void){}

static const CSM_DESC EditorScreenHNDL=
{
 EditorScreenOnKey,
 EditorScreenOnCreate,
 EditorScreenOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};


struct MAINSCREEN far* EditorGetMainscreenVars(void)
{
 if (!SSV.KeylckState) return &uni_vars.screen_vars.screen;
  else return &uni_vars.screen_vars.screen_lock;
}


void EditorScreen(void)
{
 char Buff[0x2C]; 
 CreateCSM(&EditorScreenHNDL,Buff,0);
}

void EditorGetSelected(void)
{
 char i=0;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 unsigned int set1=mainscreen->set1;
 unsigned int bit1=mainscreen->bit1;
 char logotype=mainscreen->logotype;
 char far* inc=SSV.inc;

 inc[i++]=datetime;
 inc[i++]=opname;
 if (SSV.OpName2Show) inc[i++]=opname2;
 if (SSV.CB_MessShow) inc[i++]=cbmess;
 inc[i++]=indic_one;
 inc[i++]=indic_two1;
 inc[i++]=indic_two2;

 if ((set1>>_GET_ANTENNA)!=0) inc[i++]=ant;
 if ((set1>>_GET_BATTERY)!=0) inc[i++]=bat;
 if (bit1&_SHOW_DB)         inc[i++]=db; 
 if (bit1&_SHOW_TEMP)       inc[i++]=temp; 
 if (bit1&_SHOW_MA)         inc[i++]=ma; 
 if (bit1&_SHOW_VOLTS)      inc[i++]=volts;
 if (bit1&_SHOW_PROFILE)    inc[i++]=profile;
 if (bit1&_SHOW_FREESMS)    inc[i++]=freesms;
 if (bit1&_SHOW_EVENTS )    inc[i++]=event;
 switch (logotype){
    case 1: //anim
            inc[i++]=anim_logo;
            break;
    case 2: //clock
            inc[i++]=digit_clock;
            break;
    case 3: //clock+anim
            inc[i++]=anim_clock_logo;
            inc[i++]=anim_clock;
            break;
    case 4: //analog clock
            inc[i++]=analog_clock;
            break;
    case 5: //supernet
            inc[i++]=supernet;
            break;
 }
 inc[i]=0;
 SSV.sel=0;
}
static const char far h_by_f[]={8,16,12,12,7,7,5,10,10,10,18};

void EditorFillSelect(char sel)
{
 char x=0,y=0,w=0,h=0;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 switch (sel){
    case datetime:      x=0;
                        y=mainscreen->datetime_y;
                        w=101;
                        h=h_by_f[mainscreen->datetime_f];
                        break;
    case cbmess:        x=0;
                        y=mainscreen->cbmess_y;
                        w=101;
                        h=h_by_f[mainscreen->cbmess_f];
                        break;
    case opname:        x=0;
                        y=mainscreen->opname_y;
                        w=101;
                        h=h_by_f[mainscreen->opname_f];
                        break;
    case opname2:       x=0;
                        y=mainscreen->opname2_y;
                        w=101;
                        h=h_by_f[mainscreen->opname2_f];
                        break;
    case indic_one:     x=mainscreen->indic_one_x;
                        y=mainscreen->indic_one_y;
                        w=19;
                        h=7;
                        break;
    case indic_two1:    x=mainscreen->indic_two1_x;
                        y=mainscreen->indic_two1_y;
                        w=19;
                        h=7;
                        break;
    case indic_two2:    x=mainscreen->indic_two2_x;
                        y=mainscreen->indic_two2_y;
                        w=19;
                        h=7;
                        break;
    case ant:           x=mainscreen->ant_x;
                        y=mainscreen->ant_y;
                        w=31;
                        h=7;
                        break;
    case bat:           x=mainscreen->bat_x;
                        y=mainscreen->bat_y;
                        w=26;
                        h=7;
                        break;
    case anim_logo:     x=mainscreen->anim_logo_x;
                        y=mainscreen->anim_logo_y;
                        w=101;
                        h=47;
                        break;
    case digit_clock:   x=mainscreen->digit_clock_x;
                        y=mainscreen->digit_clock_y;
                        w=101;
                        h=47;
                        break;
    case anim_clock:    x=mainscreen->anim_clock_x;
                        y=mainscreen->anim_clock_y;
                        w=46;
                        h=19;
                        break;
    case anim_clock_logo:x=mainscreen->anim_clock_logo_x;
                        y=mainscreen->anim_clock_logo_y;
                        w=101;
                        h=47;
                        break;
    case analog_clock:  x=mainscreen->analog_clock_x;
                        y=mainscreen->analog_clock_y;
                        w=101;
                        h=47;
                        break;
    case db:            x=mainscreen->db_x;
                        y=mainscreen->db_y;
                        w=20;
                        h=5;
                        break;
    case temp:          x=mainscreen->temp_x;
                        y=mainscreen->temp_y;
                        w=20;
                        h=5;
                        break;
    case ma:            x=mainscreen->mA_x;
                        y=mainscreen->mA_y;
                        w=20;
                        h=5;
                        break;
    case volts:         x=mainscreen->volts_x;
                        y=mainscreen->volts_y;
                        w=20;
                        h=5;
                        break;
    case profile:       x=mainscreen->profile_x;
                        y=mainscreen->profile_y;
                        w=19;
                        h=7;
                        break;
    case freesms:       x=mainscreen->freesms_x;
                        y=mainscreen->freesms_y;
                        w=19;
                        h=7;
                        break;
    case supernet:      x=0;
                        y=mainscreen->supernet_y;
                        w=101;
                        h=47;
                        break;
    case event :        x=mainscreen->events_x;
                        y=mainscreen->events_y;
                        w=8;
                        h=11;
                        break;
 }
 FillRect(x,y,w,h,xor);
}

#define inc_x  1
#define dec_x  2
#define inc_y  3
#define dec_y  4
#define set_0  5

void EditorMoveSel(char sel, char action)
{
 char dx=0, dy=0, f;
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();

 switch (action)
 {
  case inc_x: dx++; break;
  case dec_x: dx--; break;
  case inc_y: dy++; break;
  case dec_y: dy--; break;
 }
 switch (sel){
    case datetime:      mainscreen->datetime_y+=dy;
                        f=mainscreen->datetime_f;
                        f+=dx;
                        if (f>=0&&f<=10) mainscreen->datetime_f=f;
                        break;
    case cbmess:        mainscreen->cbmess_y+=dy;
                        f=mainscreen->cbmess_f;
                        f+=dx;
                        if (f>=0&&f<=10) mainscreen->cbmess_f=f;
                        break;
    case opname:        mainscreen->opname_y+=dy;
                        f=mainscreen->opname_f;
                        f+=dx;
                        if (f>=0&&f<11) mainscreen->opname_f=f;
                        break;
    case opname2:        mainscreen->opname2_y+=dy;
                        f=mainscreen->opname2_f;
                        f+=dx;
                        if (f>=0&&f<11) mainscreen->opname2_f=f;
                        break;
    case indic_one:     mainscreen->indic_one_x+=dx;
                        mainscreen->indic_one_y+=dy;
                        break;
    case indic_two1:    mainscreen->indic_two1_x+=dx;
                        mainscreen->indic_two1_y+=dy;
                        break;
    case indic_two2:    mainscreen->indic_two2_x+=dx;
                        mainscreen->indic_two2_y+=dy;
                        break;
    case ant:           mainscreen->ant_x+=dx;
                        mainscreen->ant_y+=dy;
                        break;
    case bat:           mainscreen->bat_x+=dx;
                        mainscreen->bat_y+=dy;
                        break;
    case anim_logo:     mainscreen->anim_logo_x+=dx;
                        mainscreen->anim_logo_y+=dy;
                        break;
    case digit_clock:   mainscreen->digit_clock_x+=dx;
                        mainscreen->digit_clock_y+=dy;
                        break;
    case anim_clock:    mainscreen->anim_clock_x+=dx;
                        mainscreen->anim_clock_y+=dy;
                        break;
    case anim_clock_logo:mainscreen->anim_clock_logo_x+=dx;
                        mainscreen->anim_clock_logo_y+=dy;
                        break;
    case analog_clock:  mainscreen->analog_clock_x+=dx;
                        mainscreen->analog_clock_y+=dy;
                        break;
    case db:            mainscreen->db_x+=dx;
                        mainscreen->db_y+=dy;
                        break;
    case temp:          mainscreen->temp_x+=dx;
                        mainscreen->temp_y+=dy;
                        break;
    case ma:            mainscreen->mA_x+=dx;
                        mainscreen->mA_y+=dy;
                        break;
    case volts:         mainscreen->volts_x+=dx;
                        mainscreen->volts_y+=dy;
                        break;
    case profile:       mainscreen->profile_x+=dx;
                        mainscreen->profile_y+=dy;
                        break;
    case freesms:       mainscreen->freesms_x+=dx;
                        mainscreen->freesms_y+=dy;
                        break;
    case supernet:      mainscreen->supernet_y+=dy;
                        break;
    case event:         mainscreen->events_x+=dx;
                        mainscreen->events_y+=dy;
                        break;
 }
}

void EditorScreenOnCreate(void)
{
 StartAndCloseDummyDialog();
 SSV.KeylckState=0;
 SSV.OpName2Show=0;
 SSV.CB_MessShow=0;
 EditorGetSelected();
 EditorScreenDraw();
}

int EditorScreenOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 char Action=0;
 char far* inc=SSV.inc;
 char far* sel=&SSV.sel;
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  EditorScreenDraw(); //Перерисовываем по таймеру
  return(1);
 }
 if (Msg->Msg==KEY_UP) return(0);
 if ((Msg->Msg==KEY_DOWN)||(Msg->Msg==LONG_PRESS)) {
  switch(Msg->Param[0]) {
   case RIGHT_SOFT:     if (inc[*sel+1]!=0) (*sel)++;                   break;
   case LEFT_SOFT:      if (*sel!=0) (*sel)--;                          break;
   case UP_BUTTON:      Action=dec_y;                                   break;
   case DOWN_BUTTON:    Action=inc_y;                                   break;
   case LEFT_BUTTON:    Action=dec_x;                                   break;
   case RIGHT_BUTTON:   Action=inc_x;                                   break;
   case '1':            SSV.OpName2Show^=1; EditorGetSelected();        break;
   case '2':            SSV.CB_MessShow^=1; EditorGetSelected();        break;
   case '3':            SSV.KeylckState^=1; EditorGetSelected();        break;
   case RED_BUTTON:     CloseCSM(((int far *)data)[6]);  return(0);
  }
  if (Action!=0) EditorMoveSel(inc[*sel],Action);
  EditorScreenDraw();
  return(0);
 }
 return(1);
}

void DrawSofts(struct MAINSCREEN far* mainscreen)
{
 if (mainscreen->bit1&_HIDE_SOFTS) return;
 DrawImage(0,67,47,13,(IMGHDR far*)&hdr_soft1);
 DrawImage(54,67,47,13,(IMGHDR far*)&hdr_soft2);
}

void EditorScreenDraw(void)
{
 struct MAINSCREEN far* mainscreen=EditorGetMainscreenVars();
 ClearRect(0,0,101,80);
 IdleKeyFlag=1;
 DrawString(0,mainscreen->opname_y,101,20,"OpName",mainscreen->opname_f);
 if (SSV.OpName2Show) DrawString(0,mainscreen->opname2_y,101,20,"OpName2",mainscreen->opname2_f);
 DrawString(0,mainscreen->datetime_y,101,20,"Date-Time",mainscreen->datetime_f);
 DrawSofts(mainscreen);
 DrawSelectableOpsosLogo(mainscreen);
 DrawSignalLevel(mainscreen,356);
 if (SSV.inc[SSV.sel]==indic_one) {
  DrawPredefinedImage(mainscreen->indic_one_x,mainscreen->indic_one_y,329);
 } else {
  DrawPredefinedImage(mainscreen->indic_two1_x,mainscreen->indic_two1_y,329);
  DrawPredefinedImage(mainscreen->indic_two2_x,mainscreen->indic_two2_y,340);
 }
 DrawBatteryLevel(mainscreen,325);
 DrawExtIndicators(mainscreen);
 if (mainscreen->bit1&_SHOW_EVENTS) {
  DrawPredefinedImage(mainscreen->events_x,mainscreen->events_y,pict_EventSms);
 }
 if (SSV.CB_MessShow) DrawString(0,mainscreen->cbmess_y,101,20,"CB-Message",mainscreen->cbmess_f);
 IdleKeyFlag=0;
 EditorFillSelect(SSV.inc[SSV.sel]);
}
//===================================================================
// MenuSetupOtherScr
//===================================================================
static const MENUITEM_DESC far MenuSetupOtherScrITEMS[8]=
{
 (int far*)0xBAFC22,lng_SetupOtherScr0,lng_SetupOtherScr0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SetupOtherScr1,lng_SetupOtherScr1,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SetupOtherScr2,lng_SetupOtherScr2,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SetupOtherScr3,lng_SetupOtherScr3,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SetupOtherScr4,lng_SetupOtherScr4,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SetupOtherScr5,lng_SetupOtherScr5,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SetupOtherScr6,lng_SetupOtherScr6,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SetupOtherScr7,lng_SetupOtherScr7,0,NULL,6,0x53A,
};

void MenuSetupOtherScrState(char far* Unk1, int Curr, int Unk3);
void MenuSetupOtherScrSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuSetupOtherScrHNDLS[8]=
{
 MenuSetupOtherScrSelect,
 MenuSetupOtherScrSelect,
 MenuSetupOtherScrSelect,
 MenuSetupOtherScrSelect,
 MenuSetupOtherScrSelect,
 MenuSetupOtherScrSelect,
 MenuSetupOtherScrSelect,
 MenuSetupOtherScrSelect
};

static const HEADER_DESC far MenuSetupOtherScrHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_SetupOtherScr,0x7FFF
};

static const MENU_DESC far MenuSetupOtherScrSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuSetupOtherScrState,
 (MENUITEM_DESC far*)&MenuSetupOtherScrITEMS,
 (MENUITEM_HNDL far*)&MenuSetupOtherScrHNDLS,
 8
};

void MenuSetupOtherScr(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuSetupOtherScrSTRUCT,(HEADER_DESC far*)&MenuSetupOtherScrHDR,0,0,0,0);
}

void MenuSetupOtherScrState(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 unsigned int bit1=uni_vars.screen_vars.otherscreen_bit;
 if (bit1>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuSetupOtherScrSelect(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 uni_vars.screen_vars.otherscreen_bit^=i;
 RefreshMenuItem(Unk1);
}
//===================================================================
// SelectSsaver
//===================================================================
static const STR far bin_files_masks[]={ "*.bin", NULL };

void SelectSsaverOnCreate(CSM_RAM far * data)
{
 StartAndCloseDummyDialog();
 SendMessage2toMMCEXPL(&dir_req,(STR)"A:\\zbin\\screensavers\\",bin_files_masks,1,0x41,12345);
 data->state=0;
 ClearRect(0,0,101,80);
 DrawString(0,0,101,80,"Loading...",8);
}

void DrawNames(void)
{
 WORD i;
 ClearRect(0,0,101,80);
 DrawString(50,70,51,10,"Select",8);
 for (i=0;i<7;i++) {
  if (i>(dir_req.total_files-1)) break;
  DrawString(0,i*10+1,101,10,dir_req.filelist[i].filename,7);
 }
 FillRect(0,SSV.sel*10,101,10,xor);
}

int  SelectSsaverOnKey(CSM_RAM far * data, GBS_MSG far* Msg)
{
 char fname[127];
 if (Msg->Msg==0xF9) {
  //Сообщение от эксплорера
  void far *mp=*((void far * far *)(Msg->Param+2));
  if (*((unsigned long *)mp)!=12345) return(1); //Не наше сообщение
  EX_heap_free_with_lock(mp); //Освобождаем ответ
  SSV.sel=0;
  data->state=1;
  DrawNames();
  return(0);
 }
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  if (!data->state) DrawNames();
  return(1);
 }
 if ((Msg->Msg==KEY_DOWN)||(Msg->Msg==LONG_PRESS)) {
  switch(Msg->Param[0])  {
   case RIGHT_BUTTON:
   case RIGHT_SOFT: 
                    if (!data->state) return 0; 
                    strcpy((STR)&fname,dir_req.filelist[SSV.sel].filename);
                    fname[strlen((STR)&fname)-4]=0;
                    if (strlen((STR)&fname)>9) {
                     DisplayMessageBoxV(1,0x80B);
                     break;
                    }
                    strcpy((STR)&uni_vars.screen_vars.ssaver_name,(STR)&fname);
   case LEFT_BUTTON:  
   case LEFT_SOFT:  
   case RED_BUTTON: 
                    CloseCSM(((int far *)data)[6]); break;
   case UP_BUTTON:  if (!data->state) return 0;
                    if (SSV.sel) SSV.sel--;
                    DrawNames();
                    break;
   case DOWN_BUTTON:if (!data->state) return 0;
                    if (SSV.sel<(dir_req.total_files-1)) SSV.sel++;
                    DrawNames();
                    break;
  }
  return(0);
 }
 if (Msg->Msg==KEY_UP) return(0);
 return(1);
 
}

void SelectSsaverOnClose(CSM_RAM far * data)
{
 FreeDataConstructedByEXPL(&dir_req);
 data=data;
}

static const CSM_DESC SelectSsaverHNDL=
{
 SelectSsaverOnKey,
 SelectSsaverOnCreate,
 SelectSsaverOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

void SelectSsaver(void)
{
 char Buff[0x2C]; 
 CreateCSM(&SelectSsaverHNDL,Buff,0);
}
//===================================================================
// EditConst
//===================================================================
void EditConstDraw(void)
{
 char s[20];
 unsigned int VB=NETMON_VB;
 int TC=uni_vars.screen_vars.temp_correct;
 int TB=NETMON_TB-2730-TC;
 char y=0;
 ClearRect(0,0,101,80);
 DrawString(0,70,50,10,"Reset",8);
 DrawString(50,70,50,10,"OK",8);
 sprintf(s,(STR)"vmax <%u>",uni_vars.screen_vars.bat_vmax);
 DrawString(0,01,101,10,s,7);
 sprintf(s,(STR)"vmin <%u>",uni_vars.screen_vars.bat_vmin);
 DrawString(0,11,101,10,s,7);
 sprintf(s,(STR)"Curr %u",VB);
 DrawString(0,21,101,10,s,7);
 sprintf(s,(STR)"=>   %u%%",GetBatteryPercent(VB));
 DrawString(0,31,101,10,s,7);
 sprintf(s,(STR)"<-%d,%u tC>",TC/10,TC%10);
 DrawString(0,41,101,10,s,7);
 sprintf(s,(STR)"  %d,%u tC",TB/10,TB%10);
 DrawString(0,51,101,10,s,7);
 switch (SSV.sel) {
  case 1: y=10; break;
  case 2: y=40; break;
 }
 FillRect(0,y,101,10,xor);
}

void EditConstOnCreate(void){
 StartAndCloseDummyDialog();
 SSV.sel=0;
 EditConstDraw();
}

void EditConstAction(int sel, int action)
{
 switch (sel) {
  case 0: uni_vars.screen_vars.bat_vmax+=action; break;
  case 1: uni_vars.screen_vars.bat_vmin+=action; break;
  case 2: if (uni_vars.screen_vars.temp_correct==0) if (action==-1) break;
          uni_vars.screen_vars.temp_correct+=action; break;
 }
}

int  EditConstOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  EditConstDraw();
  return(1);
 }
 if ((Msg->Msg==KEY_DOWN)||(Msg->Msg==LONG_PRESS)) {
  switch(Msg->Param[0])  {
   case LEFT_SOFT:  uni_vars.screen_vars.bat_vmax=4175;
                    uni_vars.screen_vars.bat_vmin=3600;
                    uni_vars.screen_vars.temp_correct=0;
                    break;
   case RIGHT_SOFT: 
   case RED_BUTTON: CloseCSM(((int far *)data)[6]); return(0);
   case UP_BUTTON:  if (SSV.sel) SSV.sel--; break;
   case DOWN_BUTTON:if (SSV.sel<2) SSV.sel++; break;
   case LEFT_BUTTON: EditConstAction(SSV.sel,-1); break;
   case RIGHT_BUTTON:EditConstAction(SSV.sel,1); break;
  }
  EditConstDraw();
  return(0);
 }
 if (Msg->Msg==KEY_UP) return(0);
 return(1);
}

void EditConstOnClose(void){}

static const CSM_DESC EditConstHNDL=
{
 EditConstOnKey,
 EditConstOnCreate,
 EditConstOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

void EditConst(void)
{
 char Buff[0x2C]; 
 CreateCSM(&EditConstHNDL,Buff,0);
}
//===================================================================
// EditAnClock
//===================================================================
void EditAnClockDraw(void)
{
 ClearRect(0,0,101,80);
 DrawString(0,70,50,10,"Reset",8);
 DrawString(50,70,50,10,"OK",8);
 AnalogClock(0,0,uni_vars.screen_vars.analog_clock_xc,uni_vars.screen_vars.analog_clock_yc,uni_vars.screen_vars.analog_clock_l);
}

void EditAnClockOnCreate(void)
{
 StartAndCloseDummyDialog();
 EditAnClockDraw();
}

void EditAnClockAction(int action)
{
 char dx=0, dy=0;
 switch (action)
 {
  case inc_x: dx++; break;
  case dec_x: dx--; break;
  case inc_y: dy++; break;
  case dec_y: dy--; break;
 }
 uni_vars.screen_vars.analog_clock_xc+=dx;
 uni_vars.screen_vars.analog_clock_yc+=dy;
}

void EditAnClockActionLen(int action)
{
 if (uni_vars.screen_vars.analog_clock_l==0) if (action==-1) return;
 uni_vars.screen_vars.analog_clock_l+=action;
}

int  EditAnClockOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  EditAnClockDraw();
  return(1);
 }
 if ((Msg->Msg==KEY_DOWN)||(Msg->Msg==LONG_PRESS)) {
  switch(Msg->Param[0])  {
   case LEFT_SOFT:   uni_vars.screen_vars.analog_clock_xc=21;
                     uni_vars.screen_vars.analog_clock_yc=22;
                     uni_vars.screen_vars.analog_clock_l=19;
                     break;
   case RIGHT_SOFT: 
   case RED_BUTTON:  CloseCSM(((int far *)data)[6]); return(0);
   case UP_BUTTON:   EditAnClockAction(dec_y); break;
   case DOWN_BUTTON: EditAnClockAction(inc_y); break;
   case LEFT_BUTTON: EditAnClockAction(dec_x); break;
   case RIGHT_BUTTON:EditAnClockAction(inc_x); break;
   case '1':         EditAnClockActionLen(-1); break;
   case '3':         EditAnClockActionLen(1);  break;
  }
  EditAnClockDraw();
  return(0);
 }
 if (Msg->Msg==KEY_UP) return(0);
 return(1);
}

void EditAnClockOnClose(void){}

static const CSM_DESC EditAnClockHNDL=
{
 EditAnClockOnKey,
 EditAnClockOnCreate,
 EditAnClockOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

void EditAnClock(void)
{
 char Buff[0x2C]; 
 CreateCSM(&EditAnClockHNDL,Buff,0);
}
//===================================================================
// MenuKeysSetup
//===================================================================
static const MENUITEM_DESC far MenuKeysSetupITEMS[11]=
{
 NULL, lng_KeysUnlock,  lng_KeysUnlock,  0,NULL,3,0x2BC,
 NULL, lng_KeysLockKey, lng_KeysLockKey, 0,NULL,3,0x2BC,
 NULL, lng_KeysWhLock,  lng_KeysWhLock,  0,NULL,3,0x2BC,
 NULL, lng_KeysGreen,   lng_KeysGreen,   0,NULL,3,0x2BC,
 NULL, lng_KeysOther,   lng_KeysOther,   0,NULL,3,0x2BC,
 NULL, lng_KeysCallMenu,lng_KeysCallMenu,0,NULL,3,0x2BC,
 NULL, lng_KeysPTT0,    lng_KeysPTT0,    0,NULL,3,0x2BC,
 NULL, lng_KeysPTT1,    lng_KeysPTT1,    0,NULL,3,0x2BC,
 NULL, lng_KeysPTT2,    lng_KeysPTT2,    0,NULL,3,0x2BC,
 NULL, lng_KeysPTT3,    lng_KeysPTT3,    0,NULL,3,0x2BC,
 NULL, lng_EndMenuLong, lng_EndMenuLong, 0,NULL,3,0x2BC
};

void MenuKeysUnlock(void);  
void KeysLockKey(void); 
void MenuKeysWhLock(void);  
void MenuKeysGreen(void);   
void MenuKeysOther(void);   
void KeysCallMenu(void);
void MenuKeysPTT0(void);    
void MenuKeysPTT1(void);    
void MenuKeysPTT2(void);    
void MenuKeysPTT3(void);    

static const MENUITEM_HNDL far MenuKeysSetupHNDLS[11]=
{
 MenuKeysUnlock,
 KeysLockKey,
 MenuKeysWhLock,
 MenuKeysGreen,
 MenuKeysOther,
 KeysCallMenu,
 MenuKeysPTT0,
 MenuKeysPTT1,
 MenuKeysPTT2,
 MenuKeysPTT3,
 rets
};

static const HEADER_DESC far MenuKeysSetupHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_KeysSetup,0x7FFF 
};

static const MENU_DESC far MenuKeysSetupSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (MENUITEM_DESC far*)&MenuKeysSetupITEMS,
 (MENUITEM_HNDL far*)&MenuKeysSetupHNDLS,
 11
};
void MenuKeysSetup(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuKeysSetupSTRUCT,(HEADER_DESC far*)&MenuKeysSetupHDR,0,0,0,0);
}
//===================================================================
// MenuKeysUnlock
//===================================================================
static const MENUITEM_DESC far MenuKeysUnlockITEMS[4]=
{
 (int far*)0xB867CE,lng_KeysUnlock0,lng_KeysUnlock0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_KeysUnlock1,lng_KeysUnlock1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_KeysUnlock2,lng_KeysUnlock2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_KeysUnlock3,lng_KeysUnlock3,0,NULL,3,0x53A
};

void MenuKeysUnlockState(char far* Unk1, int Curr, int Unk3);
void MenuKeysUnlockSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuKeysUnlockHNDLS[4]=
{
 MenuKeysUnlockSelect,
 MenuKeysUnlockSelect,
 MenuKeysUnlockSelect,
 MenuKeysUnlockSelect
};

static const HEADER_DESC far MenuKeysUnlockHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_KeysUnlock,0x7FFF
};

static const MENU_DESC far MenuKeysUnlockSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuKeysUnlockState,
 (MENUITEM_DESC far*)&MenuKeysUnlockITEMS,
 (MENUITEM_HNDL far*)&MenuKeysUnlockHNDLS,
 4
};

void MenuKeysUnlock(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuKeysUnlockSTRUCT,(HEADER_DESC far*)&MenuKeysUnlockHDR,0,0,0,0);
}

void MenuKeysUnlockState(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 if (uni_vars.keys_vars.keysunlock==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuKeysUnlockSelect(char far* Unk1)
{
 uni_vars.keys_vars.keysunlock=GetCurrentMenuItem(Unk1);
 GeneralFunc(0);
}
//===================================================================
// KeysLockKey
//===================================================================
void KeysLockKeyDraw(void)
{
 char s[20];
 ClearRect(0,0,101,80);
 DrawString(0,70,50,10,"Cancel",8);
 DrawString(50,70,50,10,"OK",8);
 switch (SSV.sel) {
  case RECORD_BUTTON:   strcpy((STR)s,"Key: Record"); break;
  case GREEN_BUTTON:    strcpy((STR)s,"Key: Green"); break;
  case VOL_UP_BUTTON:   strcpy((STR)s,"Key: Vol Up"); break;
  case VOL_DOWN_BUTTON: strcpy((STR)s,"Key: Vol Down"); break;
  case UP_BUTTON:       strcpy((STR)s,"Key: Up Arrow"); break;
  case DOWN_BUTTON:     strcpy((STR)s,"Key: Down Arrow"); break;
  case LEFT_BUTTON:     strcpy((STR)s,"Key: Left Arrow"); break;
  case RIGHT_BUTTON:    strcpy((STR)s,"Key: Right Arrow"); break;
  case PLAY_BUTTON:     strcpy((STR)s,"Key: Play"); break;
  default:              sprintf(s,(STR)"Key: %c",SSV.sel);
 }
 DrawString(0,40,101,10,s,8);
}

void KeysLockKeyOnCreate(void)
{
 StartAndCloseDummyDialog();
 SSV.sel=uni_vars.keys_vars.unlock_key;
 KeysLockKeyDraw();
}

int  KeysLockKeyOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  KeysLockKeyDraw();
  return(1);
 }
 if (Msg->Msg==KEY_DOWN) {
  switch(Msg->Param[0])  {
   case RIGHT_SOFT:  uni_vars.keys_vars.unlock_key=SSV.sel;
   case RED_BUTTON:
   case LEFT_SOFT:   CloseCSM(((int far *)data)[6]); return(0);
   case PTT_BUTTON:  break;
   default:          SSV.sel=Msg->Param[0];
  }
  KeysLockKeyDraw();
  return(0);
 }
 if (Msg->Msg==KEY_UP) return(0);
 return(1);


}

void KeysLockKeyOnClose(void){}

static const CSM_DESC KeysLockKeyHNDL=
{
 KeysLockKeyOnKey,
 KeysLockKeyOnCreate,
 KeysLockKeyOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

void KeysLockKey(void)
{
 char Buff[0x2C]; 
 CreateCSM(&KeysLockKeyHNDL,Buff,0);
}
//===================================================================
// MenuKeysWhLock
//===================================================================
static const MENUITEM_DESC far MenuKeysWhLockITEMS[8]=
{
 (int far*)0xBAFC22,lng_KeysWhLock0,lng_KeysWhLock0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysWhLock1,lng_KeysWhLock1,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysWhLock2,lng_KeysWhLock2,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysWhLock3,lng_KeysWhLock3,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysWhLock4,lng_KeysWhLock4,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysWhLock5,lng_KeysWhLock5,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysWhLock6,lng_KeysWhLock6,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysWhLock7,lng_KeysWhLock7,0,NULL,6,0x53A
};

void MenuKeysWhLockState(char far* Unk1, int Curr, int Unk3);
void MenuKeysWhLockSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuKeysWhLockHNDLS[8]=
{
 MenuKeysWhLockSelect,
 MenuKeysWhLockSelect,
 MenuKeysWhLockSelect,
 MenuKeysWhLockSelect,
 MenuKeysWhLockSelect,
 MenuKeysWhLockSelect,
 MenuKeysWhLockSelect,
 MenuKeysWhLockSelect
};

static const HEADER_DESC far MenuKeysWhLockHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_KeysWhLock,0x7FFF
};

static const MENU_DESC far MenuKeysWhLockSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuKeysWhLockState,
 (MENUITEM_DESC far*)&MenuKeysWhLockITEMS,
 (MENUITEM_HNDL far*)&MenuKeysWhLockHNDLS,
 8
};

void MenuKeysWhLock(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuKeysWhLockSTRUCT,(HEADER_DESC far*)&MenuKeysWhLockHDR,0,0,0,0);
}

void MenuKeysWhLockState(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 unsigned int bit1=uni_vars.keys_vars.keyswhlock_bit;
 if (bit1>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuKeysWhLockSelect(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 uni_vars.keys_vars.keyswhlock_bit^=i;
 RefreshMenuItem(Unk1);
}
//===================================================================
// MenuKeysGreen
//===================================================================
static const MENUITEM_DESC far MenuKeysGreenITEMS[5]=
{
 (int far*)0xB867CE,lng_KeysGreen0,lng_KeysGreen0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_KeysGreen1,lng_KeysGreen1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_KeysGreen2,lng_KeysGreen2,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_KeysGreen3,lng_KeysGreen3,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_KeysGreen4,lng_KeysGreen4,0,NULL,3,0x53A
};

void MenuKeysGreenState(char far* Unk1, int Curr, int Unk3);
void MenuKeysGreenSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuKeysGreenHNDLS[5]=
{
 MenuKeysGreenSelect,
 MenuKeysGreenSelect,
 MenuKeysGreenSelect,
 MenuKeysGreenSelect,
 MenuKeysGreenSelect
};

static const HEADER_DESC far MenuKeysGreenHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_KeysGreen,0x7FFF
};

static const MENU_DESC far MenuKeysGreenSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuKeysGreenState,
 (MENUITEM_DESC far*)&MenuKeysGreenITEMS,
 (MENUITEM_HNDL far*)&MenuKeysGreenHNDLS,
 5
};

void MenuKeysGreen(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuKeysGreenSTRUCT,(HEADER_DESC far*)&MenuKeysGreenHDR,0,0,0,0);
}

void MenuKeysGreenState(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 if (uni_vars.keys_vars.keysgreen==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuKeysGreenSelect(char far* Unk1)
{
 uni_vars.keys_vars.keysgreen=GetCurrentMenuItem(Unk1);
 GeneralFunc(0);
}
//===================================================================
// MenuKeysOther
//===================================================================
static const MENUITEM_DESC far MenuKeysOtherITEMS[2]=
{
 (int far*)0xBAFC22,lng_KeysOther0,lng_KeysOther0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_KeysOther1,lng_KeysOther1,0,NULL,6,0x53A
};

void MenuKeysOtherState(char far* Unk1, int Curr, int Unk3);
void MenuKeysOtherSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuKeysOtherHNDLS[2]=
{
 MenuKeysOtherSelect,
 MenuKeysOtherSelect
};

static const HEADER_DESC far MenuKeysOtherHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_KeysOther,0x7FFF
};

static const MENU_DESC far MenuKeysOtherSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuKeysOtherState,
 (MENUITEM_DESC far*)&MenuKeysOtherITEMS,
 (MENUITEM_HNDL far*)&MenuKeysOtherHNDLS,
 2
};

void MenuKeysOther(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuKeysOtherSTRUCT,(HEADER_DESC far*)&MenuKeysOtherHDR,0,0,0,0);
}

void MenuKeysOtherState(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 unsigned int bit1=uni_vars.keys_vars.other_bit;
 if (bit1>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuKeysOtherSelect(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 uni_vars.keys_vars.other_bit^=i;
 RefreshMenuItem(Unk1);
}
//===================================================================
// KeysCallMenu
//===================================================================
void KeysCallMenuDraw(void)
{
 ClearRect(0,0,101,80);
 DrawString(0,70,50,10,"Cancel",8);
 DrawString(50,70,50,10,"OK",8);
 DrawString(0,40,101,10,(STR)&SSV.inc,8);
}

void LoadCallMenuStr(void)
{
 char callmenu[5];
 int i,l;
 strcpy((STR)&callmenu,(STR)&uni_vars.keys_vars.callmenu);
 l=strlen((STR)&callmenu);
 for (i=0;i<5;i++)
  if (l) { 
   SSV.inc[i]=callmenu[l-1];
   l--;
  } else SSV.inc[i]=0;
}

void SaveCallMenuStr(void)
{
 char callmenu[5];
 int i,l;
 strcpy((STR)&callmenu,(STR)&SSV.inc);
 l=strlen((STR)&callmenu); //3
 for (i=0;i<5;i++)
  if (l) { 
   SSV.inc[i]=callmenu[l-1];
   l--;
  } else SSV.inc[i]=0;
 strcpy((STR)&uni_vars.keys_vars.callmenu,(STR)&SSV.inc);
}

void KeysCallMenuOnCreate(void)
{
 StartAndCloseDummyDialog();
 LoadCallMenuStr();
 KeysCallMenuDraw();
}

void KeysCallMenuAction(char key)
{
 int i=strlen((STR)&SSV.inc);
 if (i==4) {
  SSV.inc[0]=0;
  i=0;
 }
 SSV.inc[i]=key;
 SSV.inc[i+1]=0;
}                                                                                       

int  KeysCallMenuOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  KeysCallMenuDraw();
  return(1);
 }
 if (Msg->Msg==KEY_DOWN) {
  switch(Msg->Param[0])  {
   case RIGHT_SOFT:  if (strlen(SSV.inc)<2) break;
                     SaveCallMenuStr();
   case RED_BUTTON:
   case LEFT_SOFT:   CloseCSM(((int far *)data)[6]); return(0);
   case PTT_BUTTON:  break;
   default:          KeysCallMenuAction(Msg->Param[0]);
  }
  KeysCallMenuDraw();
  return(0);
 }
 if (Msg->Msg==KEY_UP) return(0);
 return(1);


}
void KeysCallMenuOnClose(void){}

static const CSM_DESC KeysCallMenuHNDL=
{
 KeysCallMenuOnKey,
 KeysCallMenuOnCreate,
 KeysCallMenuOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

void KeysCallMenu(void)
{
 char Buff[0x2C]; 
 CreateCSM(&KeysCallMenuHNDL,Buff,0);
}
//===================================================================
// MenuKeysPTT
//===================================================================
static const MENUITEM_DESC far MenuKeysPTTITEMS[4]=
{
 (int far*)0xB867CE,0x1C0,0x1C0,0,NULL,3,0x53A,
 (int far*)0xB867CE,0x1B6,0x1B6,0,NULL,3,0x53A,
 (int far*)0xB867CE,0x0B6,0x0B6,0,NULL,3,0x53A,
 (int far*)0xB867CE,0x24E,0x24E,0,NULL,3,0x53A,
};

void MenuKeysPTTState(char far* Unk1, int Curr, int Unk2);
void MenuKeysPTTSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuKeysPTTHNDLS[4]=
{
 MenuKeysPTTSelect,
 MenuKeysPTTSelect,
 MenuKeysPTTSelect,
 MenuKeysPTTSelect
};

static const HEADER_DESC far MenuKeysPTTHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,0x6C,0x7FFF
};

static const MENU_DESC far MenuKeysPTTSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuKeysPTTState,
 (MENUITEM_DESC far*)&MenuKeysPTTITEMS,
 (MENUITEM_HNDL far*)&MenuKeysPTTHNDLS,
 4
};

void MenuKeysPTT(int sel)
{
 SSV.sel=sel;
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuKeysPTTSTRUCT,(HEADER_DESC far*)&MenuKeysPTTHDR,0,0,0,0);
}

void MenuKeysPTTState(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 if ((uni_vars.keys_vars.keysptt_set>>SSV.sel&3)==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuKeysPTTSelect(char far* Unk1)
{
 int i=GetCurrentMenuItem(Unk1)<<SSV.sel;
 uni_vars.keys_vars.keysptt_set&=((3<<SSV.sel)^0xFF);
 uni_vars.keys_vars.keysptt_set|=i;
 GeneralFunc(0);
}


void MenuKeysPTT0(void) { MenuKeysPTT(6); }
void MenuKeysPTT1(void) { MenuKeysPTT(4); }
void MenuKeysPTT2(void) { MenuKeysPTT(2); }
void MenuKeysPTT3(void) { MenuKeysPTT(0); }

//===================================================================
// MenuDialogs
//===================================================================
static const MENUITEM_DESC far MenuDialogsITEMS[5]=
{
 (int far*)0xBAFC22,lng_Dialogs0,lng_Dialogs0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Dialogs1,lng_Dialogs1,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Dialogs2,lng_Dialogs2,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Dialogs3,lng_Dialogs3,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Dialogs4,lng_Dialogs4,0,NULL,6,0x53A
};

void MenuDialogsState(char far* Unk1, int Curr, int Unk3);
void MenuDialogsSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuDialogsHNDLS[5]=
{
 MenuDialogsSelect,
 MenuDialogsSelect,
 MenuDialogsSelect,
 MenuDialogsSelect,
 MenuDialogsSelect
};

static const HEADER_DESC far MenuDialogsHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_Dialogs,0x7FFF
};

static const MENU_DESC far MenuDialogsSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuDialogsState,
 (MENUITEM_DESC far*)&MenuDialogsITEMS,
 (MENUITEM_HNDL far*)&MenuDialogsHNDLS,
 5
};

void MenuDialogs(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuDialogsSTRUCT,(HEADER_DESC far*)&MenuDialogsHDR,0,0,0,0);
}

void MenuDialogsState(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 unsigned int bit1=uni_vars.dialogs_vars.bit1;
 if (bit1>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuDialogsSelect(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 uni_vars.dialogs_vars.bit1^=i;
 RefreshMenuItem(Unk1);
}
//===================================================================
// MenuMessages
//===================================================================
static const MENUITEM_DESC far MenuMessagesITEMS[8]=
{
 (int far*)0xBAFC22,lng_Messages0,lng_Messages0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Messages1,lng_Messages1,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Messages2,lng_Messages2,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Messages3,lng_Messages3,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Messages4,lng_Messages4,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Messages5,lng_Messages5,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Messages6,lng_Messages6,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_Messages7,lng_Messages7,0,NULL,6,0x53A
};

void MenuMessagesState(char far* Unk1, int Curr, int Unk3);
void MenuMessagesSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuMessagesHNDLS[8]=
{
 MenuMessagesSelect,
 MenuMessagesSelect,
 MenuMessagesSelect,
 MenuMessagesSelect,
 MenuMessagesSelect,
 MenuMessagesSelect,
 MenuMessagesSelect,
 MenuMessagesSelect
};

static const HEADER_DESC far MenuMessagesHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_Messages,0x7FFF
};

static const MENU_DESC far MenuMessagesSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuMessagesState,
 (MENUITEM_DESC far*)&MenuMessagesITEMS,
 (MENUITEM_HNDL far*)&MenuMessagesHNDLS,
 8
};

void MenuMessages(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuMessagesSTRUCT,(HEADER_DESC far*)&MenuMessagesHDR,0,0,0,0);
}

void MenuMessagesState(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 unsigned int bit1=uni_vars.messages_vars.bit1;
 if (bit1>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuMessagesSelect(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 uni_vars.messages_vars.bit1^=i;
 RefreshMenuItem(Unk1);
}
//===================================================================
// MenuSecurity
//===================================================================
static const MENUITEM_DESC far MenuSecurityITEMS[3]=
{
 NULL, lng_SecurityPass,lng_SecurityPass,0,NULL,3,0x2BC,
 NULL, lng_Security,    lng_Security,    0,NULL,3,0x2BC,
 NULL, lng_EndMenuLong, lng_EndMenuLong, 0,NULL,3,0x2BC
};

void SecurityPass(void);
void MenuSecurityList(void);

static const MENUITEM_HNDL far MenuSecurityHNDLS[3]=
{
 SecurityPass,
 MenuSecurityList,
 rets
};

static const HEADER_DESC far MenuSecurityHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_Security,0x7FFF 
};

static const MENU_DESC far MenuSecuritySTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (MENUITEM_DESC far*)&MenuSecurityITEMS,
 (MENUITEM_HNDL far*)&MenuSecurityHNDLS,
 3
};

void MenuSecurityStart(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuSecuritySTRUCT,(HEADER_DESC far*)&MenuSecurityHDR,0,0,0,0);
}

void MenuSecurity(void)
{
 PasswordDialog(0x7D);
}


//===================================================================
// SecurityPass
//===================================================================
void PassDialogDraw(void)
{
 ClearRect(0,0,101,80);
 DrawString(0,10,101,20,"Change password:\nmin 4 chars",8);
 DrawString(0,70,50,10,"<C",8);
 DrawString(50,70,50,10,"OK",8);
 DrawString(0,40,101,40,SSV.inc,digit_big);
}

void PassDialogOnCreate(void)
{
 StartAndCloseDummyDialog();
 strcpy((STR)&SSV.inc,(STR)&uni_vars.passwords_vars.password);
 PassDialogDraw();
}

int  PassDialogOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 char far* Input=(char far*)&SSV.inc;
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  PassDialogDraw();
  return(1);
 }
 if ((Msg->Msg==KEY_DOWN)||(Msg->Msg==KEY_LONG)) {
  switch(Msg->Param[0])  {
   case RIGHT_SOFT: if (strlen(Input)<4) break;
                    strcpy((STR)&uni_vars.passwords_vars.password,Input);
   case RED_BUTTON: CloseCSM(((int far *)data)[6]); return(0);
   case LEFT_SOFT:  if (Input[0]) Input[strlen(Input)-1]=0;
                    break;
  }
  if  ((Msg->Param[0]>='0'&&Msg->Param[0]<='9')||(Msg->Param[0]=='#')||(Msg->Param[0]=='*'))
   if (strlen(Input)<8) Input[strlen(Input)]=Msg->Param[0]; 
  PassDialogDraw();
  return(0);
 }
 if (Msg->Msg==KEY_UP) return(0);
 return(1);
}

void PassDialogOnClose(void){}

static const CSM_DESC PassDialogHNDL=
{
 PassDialogOnKey,
 PassDialogOnCreate,
 PassDialogOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

void SecurityPass(void)
{
 char Buff[0x2C]; 
 CreateCSM(&PassDialogHNDL,Buff,0);
}
//===================================================================
// MenuSecurityList
//===================================================================
static const MENUITEM_DESC far MenuSecurityListITEMS[4]=
{
 (int far*)0xBAFC22,lng_SecurityList0,lng_SecurityList0,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SecurityList1,lng_SecurityList1,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SecurityList2,lng_SecurityList2,0,NULL,6,0x53A,
 (int far*)0xBAFC22,lng_SecurityList3,lng_SecurityList3,0,NULL,6,0x53A
};

void MenuSecurityListState(char far* Unk1, int Curr, int Unk3);
void MenuSecurityListSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuSecurityListHNDLS[4]=
{
 MenuSecurityListSelect,
 MenuSecurityListSelect,
 MenuSecurityListSelect,
 MenuSecurityListSelect
};

static const HEADER_DESC far MenuSecurityListHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_Security,0x7FFF
};

static const MENU_DESC far MenuSecurityListSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuSecurityListState,
 (MENUITEM_DESC far*)&MenuSecurityListITEMS,
 (MENUITEM_HNDL far*)&MenuSecurityListHNDLS,
 2
};

void MenuSecurityList(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuSecurityListSTRUCT,(HEADER_DESC far*)&MenuSecurityListHDR,0,0,0,0);
}

void MenuSecurityListState(char far* Unk1, int Curr, int Unk2)
{
 int i=2;
 if (uni_vars.passwords_vars.bit1>>Curr&1) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuSecurityListSelect(char far* Unk1)
{
 int i=1<<GetCurrentMenuItem(Unk1);
 uni_vars.passwords_vars.bit1^=i;
 RefreshMenuItem(Unk1);
}
//===================================================================
// MenuSystem
//===================================================================
static const MENUITEM_DESC far MenuSystemITEMS[2]=
{
 NULL, lng_SystemFreq,  lng_SystemFreq,  0,NULL,3,0x2BC,
 NULL, lng_EndMenuLong, lng_EndMenuLong, 0,NULL,3,0x2BC
};

void MenuSystemFreq(void);  

static const MENUITEM_HNDL far MenuSystemHNDLS[2]=
{
 MenuSystemFreq,
 rets
};

static const HEADER_DESC far MenuSystemHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_System,0x7FFF 
};

static const MENU_DESC far MenuSystemSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x80,0x00,
 NULL,
 (MENUITEM_DESC far*)&MenuSystemITEMS,
 (MENUITEM_HNDL far*)&MenuSystemHNDLS,
 2
};
void MenuSystem(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuSystemSTRUCT,(HEADER_DESC far*)&MenuSystemHDR,0,0,0,0);
}
//===================================================================
// MenuSystemFreq
//===================================================================
static const MENUITEM_DESC far MenuSystemFreqITEMS[3]=
{
 (int far*)0xB867CE,lng_SystemFreq0,lng_SystemFreq0,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_SystemFreq1,lng_SystemFreq1,0,NULL,3,0x53A,
 (int far*)0xB867CE,lng_SystemFreq2,lng_SystemFreq2,0,NULL,3,0x53A
};

void MenuSystemFreqState(char far* Unk1, int Curr, int Unk3);
void MenuSystemFreqSelect(char far* Unk1);

static const MENUITEM_HNDL far MenuSystemFreqHNDLS[3]=
{
 MenuSystemFreqSelect,
 MenuSystemFreqSelect,
 MenuSystemFreqSelect
};

static const HEADER_DESC far MenuSystemFreqHDR=
{
 0x00,0x00,0x65,0x0C,(int far*)0xB88DB0,lng_SystemFreq,0x7FFF
};

static const MENU_DESC far MenuSystemFreqSTRUCT=
{
 NULL,NULL,NULL,NULL,
 (int far*)0xBAFC06,(SOFTKEYSTAB*)0xBAFBFC,
 0x01,0x00,
 MenuSystemFreqState,
 (MENUITEM_DESC far*)&MenuSystemFreqITEMS,
 (MENUITEM_HNDL far*)&MenuSystemFreqHNDLS,
 3
};

void MenuSystemFreq(void)
{
 CreateMenu02(menu_full,0,0,(MENU_DESC far*)&MenuSystemFreqSTRUCT,(HEADER_DESC far*)&MenuSystemFreqHDR,0,0,0,0);
}

void MenuSystemFreqState(char far* Unk1, int Curr, int Unk2)
{
 int i=0;
 if (uni_vars.system_vars.freq==Curr) i=1;
 SetMenuItemIcon(Unk1,Curr,Unk2,i);
}

void MenuSystemFreqSelect(char far* Unk1)
{
 uni_vars.system_vars.freq=GetCurrentMenuItem(Unk1);
 GeneralFunc(0);
}

//===================================================================
// LoadDefaults
//===================================================================
int LoadDefaultsDia_onKey( int P1, int P2, int Key )
{
 if (Key==0x23) PasswordDialog(0x7E);
 return Key;
 P1=P1;
 P2=P2;
}

static const int far LoadDefaultsDia_SID[]={0x692,lng_LoadDefaults,0x7FFF};
static const MSGBOX_DESC far LoadDefaultsDia =
{
 0,0,2,0,0,0x7FFF,
 NULL,
 &LoadDefaultsDia_onKey,
 NULL,
 (SOFTKEY_DESC far*)0xB8AC5A,
 20006,
 0,0
};

void LoadDefaults(void)
{
 ShowMessageDlgWithIcon(&LoadDefaultsDia,(int far*)&LoadDefaultsDia_SID); 
}
//===================================================================

//===================================================================
// SaveSettings
//===================================================================
void SaveSettings(void)
{
 SaveUniVars();
 DisplayMessageBoxV(1,0x55A);
}
