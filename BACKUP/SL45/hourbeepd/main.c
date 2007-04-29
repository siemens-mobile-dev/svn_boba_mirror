#include "sl45.h"
#include "reg167.h"

typedef unsigned short WORD;

typedef huge int p_onkey(DIALOG_WSK_RAM *data, struct MSG far *Msg);

struct DIALOG_WSK far my_dia;
p_onkey * far old_onkey;

static const struct 
{
 int ena_hour;
 int dis_hour;
 int tone;
} setup=
{
 6,
 22,
 0x36
};

int far mode;

int hook_onKey(DIALOG_WSK_RAM *data, struct MSG far *Msg)
{
 if (Msg->Msg==0xB8)
 {
  if (_minute==1)
  {
   if (mode>0) mode=0;
  }
  if (_minute==0)
  {
   if ((_hour==setup.ena_hour)&&(mode<0)) mode=0;
   if (_hour==setup.dis_hour) mode=-1;
   if (mode==0)
   {
    if (!IsCallActive())
	{
	 playToneifEna(setup.tone);
    }
	mode=1;
   }
  }
 }
 return old_onkey(data,Msg);
}

void main(void)
{
 DIALOG_WSK_RAM far *dp;
 mode=0; //Готов
 AcquireGbsLock();
 dp=FindDialogWithSoftKey_by_ID(IDLEDIALOG_ID);
 memcpy(&my_dia,dp->constr,sizeof(my_dia));
 old_onkey=my_dia.onKeyPress;
 my_dia.onKeyPress=hook_onKey;
 dp->constr=&my_dia;
 FreeGbsLock();
}
