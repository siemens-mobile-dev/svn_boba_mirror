#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"

void PasswordDiaDraw(void)
{
 ClearRect(0,0,101,80);
 DrawString(0,10,101,20,"Enter password:",8);
 DrawString(0,70,50,10,"<C",8);
 DrawString(0,40,101,40,SSV.inc,digit_big);
}

void ClosePasswordDia(void)
{
 CloseCSM(*(int far*)&PtrScreenSz);
}

void PasswordDiaOnCreate(void)
{
 StartAndCloseDummyDialog();
 memset(SSV.inc,0,sizeof(SSV.inc));
 PasswordDiaDraw();
}

int  PasswordDiaOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 char far* Input=(char far*)&SSV.inc;
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xB8) {
  PasswordDiaDraw();
  if ((SSV.inc[10]++)==3) ClosePasswordDia();
  return(1);
 }

 if (SSV.sel==-1) { ClosePasswordDia(); return(0); }

 if (Msg->Msg==KEY_DOWN) {
  SSV.inc[10]=0;
  switch(Msg->Param[0])  {
   default:        ClosePasswordDia(); return(0);
   case LEFT_SOFT: 
                   if (Input[0]) Input[strlen(Input)-1]=0;
                   break;
   case '0': 
   case '1': 
   case '2': 
   case '3': 
   case '4': 
   case '5': 
   case '6': 
   case '7': 
   case '8': 
   case '9': 
   case '#':
   case '*':
                   if (strlen(Input)<8) Input[strlen(Input)]=Msg->Param[0]; 
                   break;
  }
  PasswordDiaDraw();
  if (!strcmp(Input,(STR)&uni_vars.passwords_vars.password)) {
   switch (SSV.sel) { //action 
    case 0: UnLockKBD(); doIDLE(); break; //_PASS_UNLOCK
    case 1: MenuPredef_message();  break; //_PASS_WRITESMS
    case 2: WriteMessageDialog(0,0,0,NULL); break; //_PASS_WRITESMS
    case _PASS_READSMS:    break;
    case _PASS_ARCHIVESMS: break;

    case 0x7D: MenuSecurityStart(); break;
    case 0x7E: LoadDefaultVars();
               DisplayMessageBoxV(1,0x55A);
               break;
   }
   SSV.sel=-1; //close
  }
  return(0);
 }
 if (Msg->Msg==KEY_UP) return(0);
 return(1);
}

void PasswordDiaOnClose(void){}

static const unsigned int md_tab[]={0xFFF5};

static const CSM_DESC PasswordDiaHNDL=
{
 PasswordDiaOnKey,
 PasswordDiaOnCreate,
 PasswordDiaOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};

void PasswordDialog(int action)
{
 char Buff[0x2C]; 
 SSV.sel=action;
 *(int far*)&PtrScreenSz=CreateCSM(&PasswordDiaHNDL,Buff,0);
}



void PassPredef_message(void)
{
 if (uni_vars.passwords_vars.bit1&_PASS_WRITESMS) PasswordDialog(1);
  else MenuPredef_message();
}

void PassWrite_message(void)
{
 if (uni_vars.passwords_vars.bit1&_PASS_WRITESMS) PasswordDialog(2);
  else WriteMessageDialog(0,0,0,NULL);
}
