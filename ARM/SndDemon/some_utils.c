#ifdef NEWSGOLD
#include "../inc/swilib.h"
#include "some_utils.h"
  int  GetValFromCache(char *filename,char *name){
  int f;
  unsigned int ul;
  char *buf,*buf2;
  int val=-1;
  unsigned int len;
  if ((f=fopen(filename,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)  {
    len=lseek(f,0,S_END,&ul,&ul);;
    if (!(buf2=(char*)malloc(len))) {
      fclose(f,&ul);
      return val;
    }
    lseek(f,0,S_SET,&ul,&ul);;
    if (fread(f,buf2,len,&ul)==len)    {
      buf=buf2;
      int next=0;
      char *str=malloc(256);
      do {
        sscanf(buf,"%6d:T:%[^=]=%d",&next,str,&val);//==3;
        if (!strcmp(str,name))          break;
        buf+=next+2;
      }while ((buf-buf2)<len);
      mfree (str);
      mfree (buf2);
      fclose(f,&ul);
    }
    else
      fclose(f,&ul);
  }
  return val;
};
/*
MediaProc_IsBackgroundFree

//arm
//pattern_ELKA= 08 00 90 E5 01 00 70 E2 00 00 A0 33 08 80 BD E8 -24
//thumb
//pattern_NEWSGOLD= ????01208968002900D0+1

 MEDIA_PLAYER_TOGGLE
 MEDIA_PLAYLAST-8
*/

//some cheats but doesn`t work ??
#pragma swi_number=0x81FF
__swi __arm int* MEDIA_PLAYLASTadr();

typedef void ( local)(void);
void MEDIA_PLAYER(void){
  local *m;
m=(local*)(MEDIA_PLAYLASTadr());
m=(local*)(((int)m));

m();
m=(local*)(((int)m)-0x8);
              if (IsUnlocked())
m();
}

#endif
