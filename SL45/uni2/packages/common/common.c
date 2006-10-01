#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"
//===================================================================
static const struct UNI_VARS uni_default_vars =
{
 verhigh, verlow,
//screen
 4175, 3600, 0,          //bat_vmax, bat_vmin, temp_correct;
/////////////////////////////////////////////////////////////////////////////////////////////
 0x5100,                 //set1;=0101000100000000b
 0,                      //bit1;
 0,                      //bit2;
 0,                      //logo;
 0,                      //ant_x;    
 0,                      //ant_y;
 75,                     //bat_x;
 0,                      //bat_y;
 42,                     //indic_one_x;
 0,                      //indic_one_y;
 34,                     //indic_two1_x;
 0,                      //indic_two1_y;
 53,                     //indic_two2_x;
 0,                      //indic_two2_y;
 0,                      //anim_logo_x;
 8,                      //anim_logo_y;
 0,                      //digit_clock_x;
 8,                      //digit_clock_y;
 0,                      //anim_clock_x;
 8,                      //anim_clock_y;
 0,                      //anim_clock_logo_x;
 8,                      //anim_clock_logo_y;
 0,                      //analog_clock_x;
 8,                      //analog_clock_y;
 0,                      //db_x;
 8,                      //db_y;
 81,                     //volts_x;
 8,                      //volts_y;
 53,                     //mA_x;
 8,                      //mA_y;
 28,                     //temp_x;
 8,                      //temp_y;
 0,                      //profile_x;
 13,                     //profile_y;
 81,                     //freesms_x;
 13,                     //freesms_y;
 8,                      //supernet_y;
 8,                      //logo_y;
 57,                     //datetime_y;
 7,                      //datetime_f;
 17,                     //opname_y;
 3,                      //opname_f;
 29,                     //opname2_y;
 3,                      //opname2_f;
 45,                     //cbmess_y;
 3,                      //cbmess_f;
 0,                      //events_x;
 69,                                     //events_y;
/////////////////////////////////////////////////////////////////////////////////////////////
 0x5100,                 //set1;=0101000100000000b
 0,                      //bit1;
 0,                      //bit2;
 0,                      //logo;
 0,                      //ant_x;    
 0,                      //ant_y;
 75,                     //bat_x;
 0,                      //bat_y;
 42,                     //indic_one_x;
 0,                      //indic_one_y;
 34,                     //indic_two1_x;
 0,                      //indic_two1_y;
 53,                     //indic_two2_x;
 0,                      //indic_two2_y;
 0,                      //anim_logo_x;
 8,                      //anim_logo_y;
 0,                      //digit_clock_x;
 8,                      //digit_clock_y;
 0,                      //anim_clock_x;
 8,                      //anim_clock_y;
 0,                      //anim_clock_logo_x;
 8,                      //anim_clock_logo_y;
 0,                      //analog_clock_x;
 8,                      //analog_clock_y;
 0,                      //db_x;
 8,                      //db_y;
 75,                     //volts_x;
 8,                      //volts_y;
 53,                     //mA_x;
 8,                      //mA_y;
 34,                     //temp_x;
 8,                      //temp_y;
 0,                      //profile_x;
 15,                     //profile_y;
 75,                     //freesms_x;
 15,                     //freesms_y;
 8,                      //supernet_y;
 8,                      //logo_y;
 57,                     //datetime_y;
 7,                      //datetime_f;
 17,                     //opname_y;
 3,                      //opname_f;
 29,                     //opname2_y;
 3,                      //opname2_f;
 45,                     //cbmess_y;
 3,                      //cbmess_f;
 0,                      //events_x;
 69,                     //events_y;
/////////////////////////////////////////////////////////////////////////////////////////////
 21,                     //analog_clock_xc;
 22,                     //analog_clock_yc;
 19,                     //analog_clock_l;
 0,                      //hide_bit;
 0,                      //otherscreen_bit;
 0,                      //otherscreen_bit2;
 'b','a','l','l','s',0,0,0,0,0, //ssaver_name
//keys
 0,                      //keysunlock;
 '#',                    //unlock_key;
 0,                      //keyswhlock_bit;
 0xFF,                   //keysptt_set;
 0,                      //keysgreen;
 0,                      //other_bit;
 '#','#','#',0,0,        //callmenu[5];
//dialogs
 0,                      //
//messages
 0,                      //
//passwords
 '0','0','0','0',0,0,0,0,0, //pass
 0,                         //bit1
//system
 0,                       //freq
//mp3
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,-1,-1,
 -1,14,-1,
  2,-1,-1,
  3,-1,-1,
  1, 6, 7,
  0, 5, 7,
 -1,-1,-1,
  0, 5, 7,
 -1,-1,-1,
 -1,-1,-1,
  8, 8,-1,
  9, 9,-1,
  0, 5, 7,
  0, 5, 7,
};

//===================================================================
struct UNI_VARS far* GetUniVars(void)
{
 return(&uni_vars);
}
//===================================================================
void SaveUniVars(void)
{
 WriteEEPROMData(eep_block,&uni_vars,0,sizeof(struct UNI_VARS),0,0,0);
}
//===================================================================
void LoadDefaultVars(void)
{
 memcpy(&uni_vars,&uni_default_vars,sizeof(struct UNI_VARS));
 SaveUniVars();
}
//===================================================================
void initproc(void)
{
 struct UNI_VARS far* uni_vars;
 HeapMalloc(&SystemFARHeap3,0x0596+sizeof(struct UNI_VARS));
 uni_vars=GetUniVars();
 ReadEEPROMData(eep_block,uni_vars,0,sizeof(struct UNI_VARS),0,0,0);
 if (uni_vars->ver_high!=verhigh || uni_vars->ver_low!=verlow) LoadDefaultVars();
}
//===================================================================
//void shutdown(void)
//{
// SaveUniVars();
//}
//===================================================================
struct MAINSCREEN far* GetMainscreenVars(void)
{
 if (GetKeyLockState1()) return (&uni_vars.screen_vars.screen);
  else return (&uni_vars.screen_vars.screen_lock);
}
//===================================================================
void timerproc(void)
{
 #pragma asm
 mov    [-r0], r6
 mov    [-r0], r7
 mov    [-r0], r8
 mov    [-r0], r9
 #pragma endasm
 LightWhenCharge();
 SetFreq();
 #pragma asm
 mov    r9, [r0+]
 mov    r8, [r0+]
 mov    r7, [r0+]
 mov    r6, [r0+]
 calls  0A4h, 0EEE2h
 #pragma endasm
}

void AutoStart(void)
{
 if (Started) return;
 Started=1;
 AutoLockAtStart();
}
//===================================================================
int rets0(CSM_RAM* data,GBS_MSG* Msg)
{
data=data;
Msg=Msg;
return 0;
}
static const DLG_DESC dummydialog={rets0,rets,rets,rets,DoBack0A,0x10,0xc4};

void StartAndCloseDummyDialog(void)
{
 DLG_BUFF Buff;
 CreateModalDialog_0_0(&dummydialog,&Buff);
}

unsigned int IsFocus(void far *data)
{
 int id;
 if (((int far *)data)[0x10]) return 0; //Есть меню поверх
 if (((int far *)data)[0x0A]) return 0; //Есть модальное окно поверх
 id=(*((int far * far *)(0x37*0x4000L+0x327A)))[6];
 if (((int far *)data)[6]!=id) return 0; //Есть диалог поверх
 //if (((int far *)data)[6]!=GetBotDialogWSK_ID()) return 0; //Есть диалог поверх
 return(1);
}


