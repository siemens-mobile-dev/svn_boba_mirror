#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"


int GetDialogsState0(void)
{
 if (uni_vars.dialogs_vars.bit1&_DIA_POWERON) return 1;
  else return 0;
}

int PowerOffDia_onKey( int P1, int P2, int Key )
{
 if (Key==0x23) {
  PowerOff();
 }
 if ((Key==0x24)||(Key==RED_BUTTON)) {
  doIDLE();
  return 0x52;
 }
 return Key;
 P1=P1;
 P2=P2;
}

static const int far PowerOffDia_SID[]={0x69A,0x068,0x7FFF};
static const MSGBOX_DESC far PowerOffDia =
{
 0,0,2,0,0,0x7FFF,
 NULL,
 &PowerOffDia_onKey,
 NULL,
 (SOFTKEY_DESC far*)0xB8AC5A,
 20006,
 0,0
};

void PowerOff_Dialog(void)
{
 if (!(GetUniVars()->dialogs_vars.bit1&_DIA_POWEROFF)) {
  PowerOff();
  return;
 }
 ShowMessageDlgWithIcon(&PowerOffDia,(int far*)&PowerOffDia_SID);
}

void patch_longred(void)
{
#pragma asm
 cmp    r4, #0
 jmpr   cc_Z, _not_off
 calls  _PowerOff_Dialog
 mov    r4, #1
_not_off:
#pragma endasm
}

int SmsExitDia_onKey( int P1, int P2, int Key )
{
 if (Key==0x23) {
  #pragma asm
  mov     [-r0], r9
  mov     [-r0], r8
  sub     r0, #8
  calls   0DAh,0878Ah
  #pragma endasm
 } else return Key;
 P1=P1;
 P2=P2;
}

static const int far SmsExitDia_SID[]={0x4A0,0x29E,0x7FFF};
static const MSGBOX_DESC far SmsExitDia =
{
 0,0,2,0,0,0x7FFF,
 NULL,
 &SmsExitDia_onKey,
 NULL,
 (SOFTKEY_DESC far*)0xB8AC5A,
 20006,
 0,0
};

int My_SmsEdit_onKey(void far* data, char far* key_data)
{
 if (key_data[2]==1) 
  if (GetUniVars()->dialogs_vars.bit1&_DIA_SMSEXIT) {
   ShowMessageDlgWithIcon(&SmsExitDia,(int far*)&SmsExitDia_SID); 
   return (-1);
  }
 return SmsEdit_onKey(data, key_data);
}

int FlashSmsDelDia_onKey( int P1, int P2, int Key )
{
 if (Key==0x23) ClearFlashSms();
 return Key;
 P1=P1;
 P2=P2;
}

static const int far FlashSmsDelDia_SID[]={0x17C,0x273,0x7FFF};
static const MSGBOX_DESC far FlashSmsDelDia =
{
 0,0,2,0,0,0x7FFF,
 NULL,
 &FlashSmsDelDia_onKey,
 NULL,
 (SOFTKEY_DESC far*)0xB8AC5A,
 20006,
 0,0
};

void ClearFlashSMS_confirm(void)
{
 if (GetUniVars()->dialogs_vars.bit1&_DIA_FLASHSMSDEL) {
  ShowMessageDlgWithIcon(&FlashSmsDelDia,(int far*)&FlashSmsDelDia_SID); 
 } else ClearFlashSms();
}

int DictExitDia_onKey( int P1, int P2, int Key )
{
 if (Key==0x23) DictaphoneStop();
 return Key;
 P1=P1;
 P2=P2;
}

static const int far DictExitDia_SID[]={0x4A0,0x0B5,0x7FFF};
static const MSGBOX_DESC far DictExitDia =
{
 0,0,2,0,0,0x7FFF,
 NULL,
 &DictExitDia_onKey,
 NULL,
 (SOFTKEY_DESC far*)0xB8AC5A,
 20006,
 0,0
};

void ExitDict_confirm(void)
{
 if (GetUniVars()->dialogs_vars.bit1&_DIA_DICTEXIT) {
  ShowMessageDlgWithIcon(&DictExitDia,(int far*)&DictExitDia_SID); 
 } else DictaphoneStop();
}
