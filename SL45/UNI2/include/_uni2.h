#define verhigh                 0
#define verlow                  64

#define eep_block               5106

#define datetime         0
#define opname           1
#define opname2          2
#define cbmess           3
#define indic_one        4
#define indic_two1       5
#define indic_two2       6
#define ant              7
#define bat              8
#define db               9
#define temp            10
#define ma              11
#define volts           12
#define profile         13
#define freesms         14
#define anim_logo       15
#define digit_clock     16
#define anim_clock      17
#define anim_clock_logo 18
#define analog_clock    19
#define supernet        20
#define logo            21
#define event           22

struct MAINSCREEN
{
 unsigned int set1; // настройки по два бита
#define _GET_ANTENNA            14&3
#define _GET_BATTERY            12&3
#define _GET_DATE_FORMAT        10&3
#define _GET_DATE_POS           8&3
#define _GET_TIME_POS           6&3
/*
; 15---/ Антенна: нет, 
; 14---\ Стандартная, db, 6 баз
; 13---/ Батарея: нет, 
; 12---\ Стандартная, mV, проценты, 
; 11---/ Формат даты: Стандартный   
; 10---\ +3 вида
; 9 ---/ Дата: справа, слева,       
; 8 ---\ центр, нет
; 7 ---/ Время: справа, слева,
; 6 ---\ центр, нет
; 5 ---/ 
; 4 ---\ 
; 3 ---/ 
; 2 ---\ 
; 1 ---/ 
; 0 ---\ 
*/
 unsigned int bit1; // настройки по биту
#define _SHOW_DB                0x1
#define _SHOW_MA                0x2
#define _SHOW_TEMP              0x4
#define _SHOW_VOLTS             0x8
#define _SHOW_PROFILE           0x10
#define _SHOW_FREESMS           0x20
#define _SHOW_VIBRAON           0x40
#define _SHOW_VIBRAOFF          0x80
#define _HIDE_SOFTS             0x100
#define _INVERTSCR              0x200
#define _FLIPSCR                0x400
#define _HIDE_ALARM             0x800
#define _OTHER_ALARM            0x1000
#define _SHOW_EVENTS            0x2000
#define _SHOW_EVENTSFULLSCR     0x4000
 unsigned int bit2;
 char logotype;
//----------------------------------------
 char ant_x;
 char ant_y;
 char bat_x;
 char bat_y;
 char indic_one_x;
 char indic_one_y;
 char indic_two1_x;
 char indic_two1_y;
 char indic_two2_x;
 char indic_two2_y;
 char anim_logo_x;
 char anim_logo_y;
 char digit_clock_x;
 char digit_clock_y;
 char anim_clock_x;
 char anim_clock_y;
 char anim_clock_logo_x;
 char anim_clock_logo_y;
 char analog_clock_x;
 char analog_clock_y;
 char db_x;
 char db_y;
 char volts_x;
 char volts_y;
 char mA_x;
 char mA_y;
 char temp_x;
 char temp_y;
 char profile_x;
 char profile_y;
 char freesms_x;
 char freesms_y;
 char supernet_y;
 char logo_y;
 char datetime_y;
 char datetime_f;
 char opname_y;
 char opname_f;
 char opname2_y;
 char opname2_f;
 char cbmess_y;
 char cbmess_f;
 char events_x;
 char events_y;
};

struct SCREEN_VARS
{
 unsigned int bat_vmax;
 unsigned int bat_vmin;
 int temp_correct; 
 struct MAINSCREEN screen;
 struct MAINSCREEN screen_lock;
 char analog_clock_xc;
 char analog_clock_yc;
 char analog_clock_l;
 unsigned int hide_bit;
#define _HIDE_DB                0x1
#define _HIDE_MA                0x2
#define _HIDE_TEMP              0x4
#define _HIDE_VOLTS             0x8
#define _HIDE_PROFILE           0x10
#define _HIDE_FREESMS           0x20
#define _HIDE_ANT               0x40
#define _HIDE_BAT               0x80
#define _HIDE_INDIC             0x100
#define _HIDE_EVENTS            0x200
 unsigned char otherscreen_bit;
#define _LWC                    0x1
#define _SSAVER_CUST            0x2
#define _SSAVER_AUTO            0x4
#define _INVER_SOFT             0x8
#define _WIDHT_SOFT             0x10
#define _ROUND_SOFT             0x20
#define _MOVTXT_SFT             0x40
#define _SKIN_SOFT              0x80
 unsigned char otherscreen_bit2;
 char ssaver_name[10];
};

struct KEYS_VARS
{
 unsigned char keysunlock;
 unsigned char unlock_key;
 unsigned char keyswhlock_bit;
#define _LEFT_SOFT              0x1
#define _KEY_MP3                0x2
#define _KEY_DICT               0x4
#define _KEY_PLUS               0x8
#define _KEY_UP                 0x10
#define _KEY_DOWN               0x20
#define _KEY_LEFT               0x40
#define _KEY_RIGHT              0x80

 unsigned char keysptt_set;
#define _PTTSHORT_IDLE          6&3
#define _PTTSHORT_LOCK          4&3
#define _PTT_LONG_IDLE          2&3
#define _PTT_LONG_LOCK          0&3
 unsigned char keysgreen;
 unsigned char other_bit;
#define _AUTOLOCK_START         0x1
#define _CALLMENU               0x2
 unsigned char callmenu[5];
};
struct DIALOGS_VARS
{
 unsigned char bit1;
#define _DIA_POWERON        0x1
#define _DIA_POWEROFF       0x2
#define _DIA_SMSEXIT        0x4
#define _DIA_FLASHSMSDEL    0x8
#define _DIA_DICTEXIT       0x10
};

struct MESSAGES_VARS
{
 unsigned char bit1;
#define _MESS_ALLTONESON        0x1
#define _MESS_ALLTONESOFF       0x2
#define _MESS_KEYLOCKON         0x4
#define _MESS_BATEMPTY          0x8
#define _MESS_NEWSMS            0x10
#define _MESS_SMSDELETED        0x20
};

struct PASSWORDS_VARS
{
 char password[9];
 unsigned char bit1;
#define _PASS_UNLOCK            0x1
#define _PASS_WRITESMS          0x2
#define _PASS_READSMS           0x4
#define _PASS_ARCHIVESMS        0x8
};


struct SYSTEM_VARS
{
 char freq;
};

struct MP3_VARS
{
 char keys_map[72];//24 клавиши по 3 действия
 unsigned char bit1;
 unsigned char bit2;
 unsigned char bit3;
 unsigned char bit4;
};

struct UNI_VARS
{
 char ver_high;
 char ver_low;
 struct SCREEN_VARS    screen_vars;
 struct KEYS_VARS      keys_vars;
 struct DIALOGS_VARS   dialogs_vars;
 struct MESSAGES_VARS  messages_vars;
 struct PASSWORDS_VARS passwords_vars;
 struct SYSTEM_VARS    system_vars;
 struct MP3_VARS       mp3_vars;
};

extern struct UNI_VARS far* GetUniVars(void);

extern struct
{
//+op1+op2+date+cb
 unsigned int Op1Y1; unsigned int Op1F1; unsigned int Zero0;    unsigned int One0; 
 unsigned int Op2Y1; unsigned int Op2F1; unsigned int Zero1;    unsigned int One1; 
 unsigned int DatY1; unsigned int DatF1; unsigned int Zero2;    unsigned int One2; 
 unsigned int CB_Y1; unsigned int CB_F1; unsigned int Zero3;    unsigned int One3; 
//+op1-op2+date-cb
 unsigned int Op1Y2; unsigned int Op1F2; unsigned int Zero4;    unsigned int One4; 
 unsigned int DatY2; unsigned int DatF2; unsigned int Zero5;    unsigned int One5; 
//+op1-op2-date+cb
 unsigned int Op1Y3; unsigned int Op1F3; unsigned int Zero6;    unsigned int One6; 
 unsigned int CB_Y3; unsigned int CB_F3; unsigned int Zero7;    unsigned int One7; 
//+op1+op2+date-cb
 unsigned int Op1Y4; unsigned int Op1F4; unsigned int Zero8;    unsigned int One8; 
 unsigned int Op2Y4; unsigned int Op2F4; unsigned int Zero9;    unsigned int One9; 
 unsigned int DatY4; unsigned int DatF4; unsigned int ZeroA;    unsigned int OneA;
//+op1+op2-date+cb
 unsigned int Op1Y5; unsigned int Op1F5; unsigned int ZeroB;    unsigned int OneB;
 unsigned int Op2Y5; unsigned int Op2F5; unsigned int ZeroC;    unsigned int OneC; 
 unsigned int CB_Y5; unsigned int CB_F5; unsigned int ZeroD;    unsigned int OneD; 
//+logo
 unsigned int DatY6; unsigned int DatF6; unsigned int ZeroE;    unsigned int OneE; 
 unsigned int UnkY6; unsigned int UnkF6; unsigned int ZeroF;    unsigned int OneF; 
 unsigned int CB_Y6; unsigned int CB_F6; unsigned int ZeroG;    unsigned int OneG; 
} buff4SDM;

extern IMGHDR far hdr_TmpImg;
extern char IdleKeyFlag;
extern char ImgIdx;
extern char ColonIdx;
extern void far* PtrScreenSz;
extern struct  //SetupScreenVars
{ 
 char KeylckState;
 char OpName2Show;
 char CB_MessShow;
 char sel; 
 char inc[22];
} SSV;

extern unsigned long CopyNumFiles;
extern unsigned long CopyFileSz;
extern unsigned long CopyCopiedSz;
extern unsigned long CopyTimer;

extern int Started;
extern int MP3_Started;
extern int wCLST1;
extern EXPL_REQ far dir_req;
extern EXPL_DATA far* pEXPL_DATA;
extern int id_dia;
extern int Event;
extern int IsBlankScreen;
extern struct UNI_VARS far uni_vars;


#define pict_AnimLogo           720
#define pict_DigitClock         730
#define pict_AnimClock_Logo     742
#define pict_AnimClock_Clock    747

#define pict_AntennaExt         759
#define pict_Antenna900         771
#define pict_Antenna1800        779

#define pict_Profile            787
#define pict_UnreadSMS          793
#define pict_FreeSMS            794
#define pict_VibraIndic         795
#define pict_VibraOffIndic      796

#define pict_SupernetBack       797
#define pict_AnalogClockBack    798

#define pict_EventSms           799
#define pict_EventDial          800
#define pict_EventAlarm         801
#define pict_EventMixed         802
#define pict_EventFullSms       803

#define pict_EventSmsBIG        804
#define pict_EventDialBIG       805
#define pict_EventAlarmBIG      806
#define pict_EventMixedBIG      807
#define pict_EventFullSmsBIG    808

#define pict_SoftSkin           809


#define digit_clock_digit_w     (char far*)0xEBD6D0
#define digit_clock_colon_w     (char far*)0xEBD720
#define anim_clock_digit_w      (char far*)0xEBD758
#define anim_clock_colon_w      (char far*)0xEBD7A8
#define AntennaImrovXRel        (char far*)0xEBD818
#define SupernetBack            (IMGHDR far*)0xEBD8E8;
#define AnalogClockBack         (IMGHDR far*)0xEBD8F0;

#define lng_EndMenuLong     0x128
#define lng_EndMenuShort    0x129

#define lng_UNISetup        0x900 
#define lng_ScreenSetup     0x901
#define lng_KeysSetup       0x28B
#define lng_Dialogs         0x902
#define lng_Messages        0x191
#define lng_Security        0x22C
#define lng_System          0x903
#define lng_MP3             0x1B6
#define lng_LoadDefaults    0x904
#define lng_SaveSettings    0x905

#define lng_Others          0x240
#define lng_NormalScreen    0x910
#define lng_HiddenIndic     0x911
#define lng_KeylckScreen    0x912
#define lng_CopyToKeylck    0x913
#define lng_EditorScreen    0x914
#define lng_SetupOtherScr   lng_Others
#define lng_SelectSsaver    0x915
#define lng_EditConst       0x916
#define lng_EditAnClock     0x917

#define lng_Logo            0x920
#define lng_Battery         0x921
#define lng_Antenna         0x922
#define lng_DateFormat      0x0B1
#define lng_DatePos         0x923
#define lng_TimePos         0x924
#define lng_OtherScreen     lng_Others

#define lng_Standart        0x930
#define lng_Logo0           lng_Standart
#define lng_Logo1           0x931
#define lng_Logo2           0x932
#define lng_Logo3           0x933
#define lng_Logo4           0x934
#define lng_Logo5           0x935

#define lng_NotShow         0x936
#define lng_Antenna0        lng_NotShow
#define lng_Antenna1        lng_Standart
#define lng_Antenna2        0x937
#define lng_Antenna3        0x938
#define lng_Battery0        lng_NotShow
#define lng_Battery1        lng_Standart
#define lng_Battery2        0x939
#define lng_Battery3        0x93A

#define lng_DateFormat0     0x93B
#define lng_DateFormat1     0x93C
#define lng_DateFormat2     0x93D
#define lng_DateFormat3     0x93E

#define lng_DatePos0        0x93F
#define lng_DatePos1        0x940
#define lng_DatePos2        0x941
#define lng_DatePos3        lng_NotShow

#define lng_TimePos0        0x93F
#define lng_TimePos1        0x940
#define lng_TimePos2        0x941
#define lng_TimePos3        lng_NotShow

#define lng_OtherScreen0    0x942
#define lng_OtherScreen1    0x943
#define lng_OtherScreen2    0x944
#define lng_OtherScreen3    0x945
#define lng_OtherScreen4    0x946
#define lng_OtherScreen5    0x947
#define lng_OtherScreen6    0x948
#define lng_OtherScreen7    0x949
#define lng_OtherScreen8    0x94A
#define lng_OtherScreen9    0x94B
#define lng_OtherScreenA    0x94C
#define lng_OtherScreenB    0x94D
#define lng_OtherScreenC    0x94E
#define lng_OtherScreenD    0x94F
#define lng_OtherScreenE    0x950
#define lng_OtherScreenF    0x951

#define lng_HiddenIndic0    lng_OtherScreen0
#define lng_HiddenIndic1    lng_OtherScreen1
#define lng_HiddenIndic2    lng_OtherScreen2
#define lng_HiddenIndic3    lng_OtherScreen3
#define lng_HiddenIndic4    lng_OtherScreen4
#define lng_HiddenIndic5    lng_OtherScreen5
#define lng_HiddenIndic6    lng_Antenna
#define lng_HiddenIndic7    lng_Battery
#define lng_HiddenIndic8    0x92F
#define lng_HiddenIndic9    lng_OtherScreenD

#define lng_SetupOtherScr0  0x952
#define lng_SetupOtherScr1  0x953
#define lng_SetupOtherScr2  0x954
#define lng_SetupOtherScr3  0x955
#define lng_SetupOtherScr4  0x956
#define lng_SetupOtherScr5  0x957
#define lng_SetupOtherScr6  0x958
#define lng_SetupOtherScr7  0x959

#define lng_Dy              0x95A
#define lng_Mon             0x961
#define lng_Day             0x96D
#define lng_Month           0x974

#define lng_KeysUnlock      0x990
#define lng_KeysLockKey     0x991
#define lng_KeysWhLock      0x992
#define lng_KeysGreen       0x993
#define lng_KeysOther       lng_Others
#define lng_KeysCallMenu    0x994
#define lng_KeysPTT0        0x995
#define lng_KeysPTT1        0x996
#define lng_KeysPTT2        0x997
#define lng_KeysPTT3        0x998

#define lng_KeysUnlock0     0x9A0
#define lng_KeysUnlock1     0x9A1
#define lng_KeysUnlock2     0x9A2
#define lng_KeysUnlock3     0x9A3

#define lng_KeysWhLock0     0x9A4
#define lng_KeysWhLock1     0x9A5
#define lng_KeysWhLock2     0x9A6
#define lng_KeysWhLock3     0x9A7
#define lng_KeysWhLock4     0x9A8
#define lng_KeysWhLock5     0x9A9
#define lng_KeysWhLock6     0x9AA
#define lng_KeysWhLock7     0x9AB

#define lng_KeysGreen0      lng_Standart
#define lng_KeysGreen1      0x9AC
#define lng_KeysGreen2      0x9AD
#define lng_KeysGreen3      0x9AE
#define lng_KeysGreen4      0x9AF

#define lng_KeysOther0      0x9B0
#define lng_KeysOther1      0x9B1

#define lng_Dialogs0        0x9C0
#define lng_Dialogs1        0x9C1
#define lng_Dialogs2        0x9C2
#define lng_Dialogs3        0x9C3
#define lng_Dialogs4        0x9C4

#define lng_Messages0       0x980
#define lng_Messages1       0x981
#define lng_Messages2       0x982
#define lng_Messages3       0x983
#define lng_Messages4       0x984
#define lng_Messages5       0x985
#define lng_Messages6       0x986
#define lng_Messages7       0x987

#define lng_SystemFreq      0x9D0

#define lng_SystemFreq0     0x9D1
#define lng_SystemFreq1     0x9D2
#define lng_SystemFreq2     0x9D3

#define lng_SecurityPass    0x9E0
#define lng_SecurityList0   0x9E1
#define lng_SecurityList1   0x82F
#define lng_SecurityList2   0x9E2
#define lng_SecurityList3   0x9E3

#define lng_noextfile       0x9F0
#define lng_nobinfile       0x9F1
#define lng_nothingrun      0x9F2


extern void rets(void);
extern void setExtSIDTable(void far *);
extern void _binfile(STR);
extern void CLST1(void);
extern void CLST3_1_HNDL(void);
extern void CLST3_2_HNDL(void);
extern void CLST3_3_HNDL(void);
extern void CLST4_1_HNDL(void);
extern void CLST4_2_HNDL(void);
extern void CLST4_3_HNDL(void);
int rand(void);
#define random(a) (rand()%(a))

extern int LogoFlag;

extern void LoadDefaultVars(void);
extern void SaveUniVars(void);
extern void StartAndCloseDummyDialog(void);
extern unsigned int IsFocus(void far *data);

extern struct MAINSCREEN far* GetMainscreenVars(void);
extern void DrawSelectableOpsosLogo(struct MAINSCREEN far* mainscreen);
extern void DrawSignalLevel(struct MAINSCREEN far* mainscreen, int SignalPict);
extern void DrawBatteryLevel(struct MAINSCREEN far* mainscreen, int BatteryPict);
extern void DrawExtIndicators(struct MAINSCREEN far* mainscreen);
extern void AnalogClock(char x, char y, char PX, char PY, char LEN);
extern int  GetBatteryPercent(unsigned int VB);
extern void LightWhenCharge(void);
extern void SetFreq(void);
extern void AutoLockAtStart(void);
extern void ScreenSaverStartManual(void);
extern void ScreenSaverStartAuto(void);
extern void PasswordDialog(int action);
extern void MenuSecurityStart(void);

extern void hookSetIdleBackTimer(void);
extern void My_StartTimerLightOff(void);
extern void My_StartTimerInputSymbol(void);
extern void My_StartTimerReenterSymbol(void);
extern void My_StartTimerInquirySymbol(void);
extern void hookSetTimerAutoKeyLock(char far*data, long ms,void huge* proc ,int _1);

struct XYWH
{
 unsigned int x;
 unsigned int y;
 unsigned int w;
 unsigned int h;
};

extern struct {
 char Playlist[8192];
 int Order[1024];
 int Pos;
 int Count;
 int Action;
 int Repeat;
} *pPls;

