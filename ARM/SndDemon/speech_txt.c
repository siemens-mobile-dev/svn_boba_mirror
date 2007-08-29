
#include "..\inc\swilib.h"
//#include "..\inc\mplayer.h"
extern void Send_MPlayer_Command(unsigned short cmd, short substract);
#define PLAYER_TOGGLE	0x11
inline void MPlayer_Toggle2()	{Send_MPlayer_Command(PLAYER_TOGGLE,0);}// Пауза/воспроизведение

#include "swilib2.h"
#include "speech_txt.h"
#include "some_utils.h"
word next=0;
  byte gfp=0;
 WSHDR* sndPath;
  WSHDR* sndFName;

  #define PLAY_CEPID MMI_CEPID
TDate dt;
TTime tm;


//--------------------------------------------
inline word PlayF3(const char *fpath,const char *fname,int vol ){
  PLAYFILE_OPT _sfo1;
  word i;
  str_2ws(sndFName,fname,256);
  str_2ws(sndPath,fpath,256);
  if (!IsCalling())
  {
    zeromem(&_sfo1,sizeof(PLAYFILE_OPT));
    _sfo1.repeat_num=1;
    _sfo1.time_between_play=0;
    _sfo1.play_first=0;

#ifdef NEWSGOLD
    _sfo1.unk6=1;
    _sfo1.unk7=1;
    _sfo1.unk9=2;

    if (vol<7)_sfo1.volume=vol;    
      else  _sfo1.volume=GetValFromCache("1:\\default\\setup.pd","AUDIO_SYSTEMVOLUME")+vol-8;
    if (_sfo1.volume<0) _sfo1.volume=0;
    if (_sfo1.volume>6)_sfo1.volume=6;
    if (!_sfo1.volume)return 0;
    i=PlayFile(0x10, sndPath, sndFName,PLAY_CEPID , MSG_PLAYFILE_REPORT, &_sfo1);    
#else
    _sfo1.volume=vol;    
    _sfo1.unk5=1;    
  #ifdef X75
    _sfo1.unk4=0x80000000;
    i=PlayFile(0xC, sndPath, sndFName, 0, PLAY_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
   #else    
    i=PlayFile(0xC, sndPath, sndFName, PLAY_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
  #endif
    
#endif    
  }
  return i;
}

char gbs[128]; //check may be short 
void SpeechPhrases(char *);

//-------------------------------------------------------------------
void SpeechPhrasesChkNat(char *str){

  if (next)
    PlayMelody_StopPlayback(next);


#ifndef NOHEADSET  
  gfp=((_GetPlayStatus()==2));
  if (gfp&&spc_ign){gfp=0;return;}
  if(gfp!=0){

     MPlayer_Toggle2();
  } 
#endif  
  

  
  SpeechPhrases(str);
}
//-------------------------------------------------------------------
void SpeechPhrasesChk(char *str){  

  GetDateTime (&dt,&tm);
  if ((tm.hour<spc_mn)||(tm.hour>=spc_en))    return;
  SpeechPhrasesChkNat(str);
}
//-------------------------------------------------------------------
void SpeechPhrases(char *str){
  char tmp[128];
  int i;
  for (i=0;str[i]!=';'&&(i<strlen (str)) ;i++);
  strncpy(tmp,str,i+1);
  tmp[i]=0;
  strcpy(gbs,(str+i)+1);  
  if ((tmp[0]!='#')&&(next=PlayF3(DEFAULT_DISK ":\\Zbin\\SndDemon",tmp,spc_vol))) {
    ;  

  }
   else{
    next=0;
#ifndef NOHEADSET  
    if (gfp!=0)  MPlayer_Toggle2();      
    gfp=0;
 #else
#endif    
  }
}
