#include "..\inc\swilib.h"
#include "..\inc\mplayer.h"

void Send_MPlayer_Command(unsigned short cmd, short substract)
{
  GBS_SendMessage(0x4209, 0x8053, 0, *((unsigned int*)RamMPlayer_CtrlCode())-substract,cmd);
}


void MPlayer_Stop() {Send_MPlayer_Command(PLAYER_STOP,0);}
void MPlayer_Start(){Send_MPlayer_Command(PLAYER_PLAY,0);}
void MPlayer_Next(){Send_MPlayer_Command(PLAYER_FWD,0);}
void MPlayer_Prev(){Send_MPlayer_Command(PLAYER_RWD,0);}
void MPlayer_Toggle(){Send_MPlayer_Command(PLAYER_TOGGLE,0);}

void MPlayer_VolChange(short dir)
{
  int cmd;
  if(dir==1)
  {
    cmd = PLAYER_VOL_UP;
  }
  else
  {
    cmd = PLAYER_VOL_DN;
  }
  Send_MPlayer_Command(cmd,2);
  Send_MPlayer_Command(cmd,0);
}