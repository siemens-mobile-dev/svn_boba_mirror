#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"
#include "bmp_screen.h"

static const char far s02d[]="%02d";
static const char far s03d[]="%03d";
static const char far s04d[]="%04d";

//===================================================================
int  DrawFullScreenEvent(struct MAINSCREEN far* mainscreen);
void DrawSelectableOpsosLogo(struct MAINSCREEN far* mainscreen);
void DrawSignalLevel(struct MAINSCREEN far* mainscreen, int SignalPict);
void DrawBatteryLevel(struct MAINSCREEN far* mainscreen, int BatteryPict);
void DrawExtIndicators(struct MAINSCREEN far* mainscreen);
//===================================================================
int FullScreenEvent(void)
{
 return DrawFullScreenEvent(GetMainscreenVars());
}
//===================================================================
void SelectableOpsosLogo(void)
{
 DrawSelectableOpsosLogo(GetMainscreenVars());
}
//===================================================================
void SignalLevel(int SignalPict)
{
 DrawSignalLevel(GetMainscreenVars(), SignalPict);
}
//===================================================================
void BatteryLevel(int BatteryPict)
{
 DrawBatteryLevel(GetMainscreenVars(), BatteryPict);
}
//===================================================================
void ExtIndicators(void)
{
 DrawExtIndicators(GetMainscreenVars());
}
//===================================================================
void DrawImprovAnt(unsigned int x, unsigned int y)
{
 int i, pict;
 unsigned char RXLVL;
 for (i=0;i<7;i++)
 {
  pict=0;
  RXLVL=NETMON7_TAB[i].RXLVL;
  if ((RXLVL!=0)&&(RXLVL!=255)) {
   pict++;
   if (RXLVL<105) pict++;
   if (RXLVL<97) pict++;
   if (RXLVL<88) pict++;
   if (RXLVL<80) pict++;
   if (RXLVL<68) pict++;
   if (RXLVL<57) pict++;
   if (NETMON7_TAB[i].CH>124) pict+=8; //1800
  }
  DrawPredefinedImage(x,y,pict+pict_Antenna900);
  x+=*AntennaImrovXRel;
 }
}

void DrawRXAnt(char x, char y, int SignalPict)
{
 char s[10];
 SignalPict-=351;
 SignalPict+=pict_AntennaExt;
 if (NETMON7_TAB[0].CH>124) SignalPict+=6;
 DrawPredefinedImage(x,y,SignalPict);
 sprintf(s,(STR)&s03d,NETMON7_TAB[0].RXLVL);
 x+=10;
 DrawString(x,y,19,6,s,0xC);
}

void DrawSignalLevel(struct MAINSCREEN far* mainscreen, int SignalPict)
{
  char i,x,y;
  i=mainscreen->set1>>_GET_ANTENNA;
  if (i==0) return;
  if (uni_vars.screen_vars.hide_bit&_HIDE_ANT) if (!IdleKeyFlag) return;
  x=mainscreen->ant_x;
  y=mainscreen->ant_y;
  ClearRect(x,y,31,7);
  if (SignalPict==-1) return;
  switch (i) {
    case 1: DrawPredefinedImage(x,y,SignalPict);
            break;
    case 2: DrawRXAnt(x,y,SignalPict);
            break;
    case 3: DrawImprovAnt(x,y);
  }
}

int GetBatteryPercent(unsigned int VB)
{
 unsigned int i;
 struct UNI_VARS far* uni_vars=GetUniVars();
 unsigned int bat_vmin=uni_vars->screen_vars.bat_vmin;
 unsigned int bat_vmax=uni_vars->screen_vars.bat_vmax;
 i=(VB-bat_vmin)*100/(bat_vmax-bat_vmin);
 if (i>100) i=100;
 return i;
}

void DrawBatteryLevel(struct MAINSCREEN far* mainscreen, int BatteryPict)
{
  char i,x,y,w;
  char s[10];
  char far* sf;
  unsigned int VB;
  i=mainscreen->set1>>_GET_BATTERY;
  if (i==0) return;
  if (uni_vars.screen_vars.hide_bit&_HIDE_BAT) if (!IdleKeyFlag) return;
  x=mainscreen->bat_x;
  y=mainscreen->bat_y;
  ClearRect(x,y,26,7);
  if (BatteryPict==-1) return;
  if (i!=1) {
    if ((BatteryPict>=326)&&(BatteryPict<=328)) BatteryPict-=325;
         else BatteryPict=0;
    DrawImage(x,y,25,7,(IMGHDR far*)&hdr_bat_persent[BatteryPict]);
    VB=NETMON_VB;
    if (i==3) { //percent
     w=20;
     VB=GetBatteryPercent(VB);
     sf=(STR)&s03d;
    } else {
     w=26;
     sf=(STR)&s04d;
    }
     sprintf(s,sf,VB);
    DrawString(x,y,w,6,s,0xC);
  } else DrawPredefinedImage(x,y,BatteryPict);
}

void LightWhenCharge(void)
{
 if (GetUniVars()->screen_vars.otherscreen_bit&_LWC)
  if (NETMON_LS!=0) TurnLightIfON(); 
}

void IndicatorsFill(void)
{
  struct MAINSCREEN far* mainscreen=GetMainscreenVars();
  if (uni_vars.screen_vars.hide_bit&_HIDE_INDIC) if (!IdleKeyFlag) return;
  ClearRect(mainscreen->indic_one_x,mainscreen->indic_one_y,19,7);
  ClearRect(mainscreen->indic_two1_x,mainscreen->indic_two1_y,19,7);
  ClearRect(mainscreen->indic_two2_x,mainscreen->indic_two2_y,19,7);
}

void IndicatorsGetPos(int i)
{
 int x=0,y=0;
 struct MAINSCREEN far* mainscreen=GetMainscreenVars();
 if (uni_vars.screen_vars.hide_bit&_HIDE_INDIC) if (!IdleKeyFlag) i=-1;
 switch (i)
 {
   case 0: 
           x=mainscreen->indic_one_x;
           y=mainscreen->indic_one_y;
           break;
   case 1:
           x=mainscreen->indic_two1_x;
           y=mainscreen->indic_two1_y;
           break;
   case 2:
           x=mainscreen->indic_two2_x;
           y=mainscreen->indic_two2_y;
           break;
   case -1:
           x=101;                //
           y=80;
 }
#pragma asm(@w1=x, @w2=y)
 mov r12, @w1
 mov r13, @w2
#pragma endasm()
}


void AddVibraIndicator(void)
{
  unsigned int bit1=GetMainscreenVars()->bit1;
  if (bit1&_SHOW_VIBRAON) {
#pragma asm
        extp    #0Dh, #1
        movbz   r13, 02489h
        shl     r13, #1
        extp    #0Dh, #1
        mov     r14, [r13+#249Ah]
        jnb     r14.4, _skip_vibraon_indic
        mov     r9, #795                        ;pict_VibraOnIndic
        mov     r12, r8
        shl     r12, #1
        add     r12, r0
        mov     [r12], r9
        add     r8, #1
 _skip_vibraon_indic:
#pragma endasm
 }
  if (bit1&_SHOW_VIBRAOFF) {
#pragma asm
        extp    #0Dh, #1
        movbz   r13, 02489h
        shl     r13, #1
        extp    #0Dh, #1
        mov     r14, [r13+#249Ah]
        jb      r14.4, _skip_vibraoff_indic
        mov     r9, #796                        ;pict_VibraOffIndic
        mov     r12, r8
        shl     r12, #1
        add     r12, r0
        mov     [r12], r9
        add     r8, #1
 _skip_vibraoff_indic:
#pragma endasm
 }
#pragma asm
 jmps   0A0h, 015ACh ; IsMultiline
#pragma endasm
}

void DrawPoint(int x, int y, unsigned char far *vr, int color, int bold_flag)
{
 int pos;
 pos=x+y*WIDTH;
 if (color) { //black
  vr[pos/8]|=1<<(7-pos%8);
  if (bold_flag) {
   vr[(pos-1)/8]|=1<<(7-(pos-1)%8);
   vr[(pos+WIDTH)/8]|=1<<(7-(pos+WIDTH)%8);
   vr[(pos-1+WIDTH)/8]|=1<<(7-(pos-1+WIDTH)%8);
  }
 } else {
  vr[pos/8]&=~(1<<(7-pos%8));
  if (bold_flag) {
   vr[(pos-1)/8]&=~(1<<(7-(pos-1)%8));
   vr[(pos+WIDTH)/8]&=~(1<<(7-(pos+WIDTH)%8));
   vr[(pos-1+WIDTH)/8]&=~(1<<(7-(pos-1+WIDTH)%8));
  }
 }
}

void line(int x1, int y1, int x2, int y2, unsigned char far *vr, int color, int bold_flag)
{
 int dx, dy, s, sx, sy, kl, swap, incr1, incr2; 
 sx=0; 
 if ((dx=x2-x1) < 0) {dx=-dx; --sx;} else if (dx>0) ++sx; 
 sy=0; 
 if ((dy=y2-y1) < 0) {dy=-dy; --sy;} else if (dy>0) ++sy; 
 swap=0;
 if ((kl=dx) < (s=dy)) 
 { 
  dx=s; dy=kl; kl=s; ++swap; 
 } 
 s=(incr1=dy<<1)-dx;
 incr2=dx<<1;
 DrawPoint(x1, y1, vr, color, bold_flag);
 while (--kl>=0) 
 { 
  if (s>=0) 
  { 
   if (swap) x1+=sx; else y1+=sy; 
   s -= incr2; 
  } 
  if (swap) y1+=sy; else x1+=sx; 
  s+=incr1; 
  DrawPoint(x1, y1, vr, color, bold_flag);
 }
}

const int far SineTable[]={ 0,107,213,316,416,512,602,685,761,828,887,935,974,1002,1018,1024 };

int sin(int i)
{
 while(i>60) i-=60;
 while(i<0)  i+=60;
 if(i<=15)         return SineTable[i];
 if(i>15 && i<=30) return sin(30-i);
                   return -sin(i-30);
}

int cos(int i) { return sin(i+15); }


void DrawAnalogClock(unsigned char far *vr, int PX, int PY, int LEN)
{
 int i,X,Y;
 i=-_minute+15;
 X= LEN*cos(i)/1024+PX;
 Y=-LEN*sin(i)/1024+PY;
 line(PX,PY,X,Y,vr,1,0);
 i=-(_hour%12)*60/12+15-(_minute*5)/60;
 X= LEN*cos(i)/1462+PX;
 Y=-LEN*sin(i)/1462+PY;
 line(PX,PY,X,Y,vr,1,1);

}


void AnalogClock(char x, char y, char PX, char PY, char LEN)
{
 int w, h;
 IMGHDR far* hdr_AnalogClockBack=AnalogClockBack;
 w=hdr_AnalogClockBack->w;
 h=hdr_AnalogClockBack->h;
 memcpy(&hdr_TmpImg,hdr_AnalogClockBack,sizeof(IMGHDR));
 hdr_TmpImg.bitmap=&PtrScreenSz;
 memcpy(hdr_TmpImg.bitmap,hdr_AnalogClockBack->bitmap,104*80/8);
 DrawAnalogClock(hdr_TmpImg.bitmap,PX,PY,LEN);

 DrawImage(x,y,w,h,&hdr_TmpImg);
}


void SOL_DrawAnim(char x, char y, char count, int pictID)
{
  ImgIdx++;
  if (ImgIdx>count-1) ImgIdx=0;
  DrawPredefinedImage(x,y,pictID+ImgIdx);
}

void SOL_DrawClock(char x, char y, char far* digit_w, char far *colon_w, int pictID)
{
  char i;
  char time[5];
  time[0]=_hour/10;
  time[1]=_hour%10;
  time[3]=_minute/10;
  time[4]=_minute%10;
  DrawPredefinedImage(x,y,pictID+11);
  for (i=0; i<5; i++) {
   if (i!=2) {
    DrawPredefinedImage(x,y,pictID+time[i]);
    x+=*digit_w;
   } else {
    ColonIdx^=1;
    if (ColonIdx) DrawPredefinedImage(x,y,pictID+10);
    x+=*colon_w;
   }
  }
}

void DrawSupernet(unsigned char far *vr)
{
 char w, i, y=0;
 unsigned char RXLVL;
 y+=7;
 for (i=0;i<7;i++) {
  y+=5;
  RXLVL=NETMON7_TAB[i].RXLVL;
  if (RXLVL>105 || RXLVL==0) continue;
  if (RXLVL<57) w=97;
   else w=(105-RXLVL)*2;
  line(0,y,  w+7,y,  vr,0,1); //white bold \ 
  line(0,y+2,w+7,y+2,vr,0,1); //white bold / 4px
  line(6,y+1,w+6,y+1,vr,1,1); //black bold 2px
//900
  line(0,y+1,  3,y+1,vr,1,1); //black bold 2px
  if (NETMON7_TAB[i].CH>124) { //1800
   DrawPoint(0,y+1,vr,0,0);
   DrawPoint(1,y+2,vr,0,0);
   DrawPoint(2,y+1,vr,0,0);
   DrawPoint(3,y+2,vr,0,0);
  }
 }
}

void Supernet(char y)
{
 int w, h;
 IMGHDR far* hdr_SupernetBack=SupernetBack;
 w=hdr_SupernetBack->w;
 h=hdr_SupernetBack->h;
 memcpy(&hdr_TmpImg,hdr_SupernetBack,sizeof(IMGHDR));
 hdr_TmpImg.bitmap=&PtrScreenSz;
 memcpy(hdr_TmpImg.bitmap,hdr_SupernetBack->bitmap,104*80/8);
 DrawSupernet(hdr_TmpImg.bitmap);

 DrawImage(0,y,w,h,&hdr_TmpImg);
}

void DrawSelectableOpsosLogo(struct MAINSCREEN far* mainscreen)
{
  char i;
  if (!IdleKeyFlag) return;
  i=mainscreen->logotype;
  if (i==0) return;
  switch (i) {
    case 1://anim
            SOL_DrawAnim(mainscreen->anim_logo_x,mainscreen->anim_logo_y,10,pict_AnimLogo);
            break;
    case 2://clock
            SOL_DrawClock(mainscreen->digit_clock_x,mainscreen->digit_clock_y,
              digit_clock_digit_w, digit_clock_colon_w,pict_DigitClock);
            break;
    case 3: //clock+anim
            SOL_DrawAnim(mainscreen->anim_clock_logo_x,mainscreen->anim_clock_logo_y,5,pict_AnimClock_Logo);
            SOL_DrawClock(mainscreen->anim_clock_x,mainscreen->anim_clock_y,
              anim_clock_digit_w, anim_clock_colon_w,pict_AnimClock_Clock);
            break;
    case 4: //analog clock
            AnalogClock(mainscreen->analog_clock_x,mainscreen->analog_clock_y,uni_vars.screen_vars.analog_clock_xc,uni_vars.screen_vars.analog_clock_yc,uni_vars.screen_vars.analog_clock_l);
            break;
    case 5: //supernet
            Supernet(mainscreen->supernet_y);
            break;
  }
}

void DrawFreeSms(int x, int y)
{
 int k, i;
 char s[10];
 k=NumMissedSMS;
 i=pict_UnreadSMS;
 if (k==0) {
  k=NumFreeSMS;
  i=pict_FreeSMS;
 }
 DrawPredefinedImage(x,y,i);
 sprintf(s,(STR)&s02d,k);
 x+=7; y++;
 DrawString(x,y,12,7,s,0xC);
}
int IsHaveMissedCalls(void)
{
 if (GetNumMissedCalls()) return 1;
  else return 0;
}
void DrawExtIndicators(struct MAINSCREEN far* mainscreen)
{
 unsigned int bit1=mainscreen->bit1;
 unsigned int bit2=uni_vars.screen_vars.hide_bit;
 char s[10];
 char x, y;
 int TB, Event;
 if (bit1&_SHOW_DB) {
  if (bit2&_HIDE_DB) if (!IdleKeyFlag) goto _ext_next1;
  sprintf(s,(STR)&s03d,NETMON7_TAB[0].RXLVL);
  x=mainscreen->db_x;
  y=mainscreen->db_y;
  DrawImage(x,y,5,5,(IMGHDR far*)&hdr_minus);
  DrawString(x+5,y,15,5,s,digit_5x5);
 }
_ext_next1:
 if (bit1&_SHOW_VOLTS) {
  if (bit2&_HIDE_VOLTS) if (!IdleKeyFlag) goto _ext_next2;
   sprintf(s,(STR)&s04d,NETMON_VB);
   DrawString(mainscreen->volts_x,mainscreen->volts_y,20,5,s,digit_5x5);
 }
_ext_next2:
 if (bit1&_SHOW_PROFILE) {
  if (bit2&_HIDE_PROFILE) if (!IdleKeyFlag) goto _ext_next3;
  DrawPredefinedImage(mainscreen->profile_x,mainscreen->profile_y,GetProfileNum()+pict_Profile);
 }
_ext_next3:
 if (bit1&_SHOW_FREESMS) {
  if (bit2&_HIDE_FREESMS) if (!IdleKeyFlag) goto _ext_next4;
  DrawFreeSms(mainscreen->freesms_x,mainscreen->freesms_y);
 }
_ext_next4:
 if (bit1&_SHOW_MA) {
  if (bit2&_HIDE_MA) if (!IdleKeyFlag) goto _ext_next5;
  sprintf(s,(STR)&s03d,NETMON_mA);
  x=mainscreen->mA_x;
  y=mainscreen->mA_y;
  DrawImage(x,y,5,5,(IMGHDR far*)&hdr_mA);
  DrawString(x+5,y,15,5,s,digit_5x5);
 }
_ext_next5:
 if (bit1&_SHOW_TEMP) {
  if (bit2&_HIDE_TEMP) if (!IdleKeyFlag) goto _ext_next6;
  TB=NETMON_TB-2730;
  TB-=GetUniVars()->screen_vars.temp_correct;
  x=mainscreen->temp_x;
  y=mainscreen->temp_y;
  DrawImage(x,y,20,5,(IMGHDR far*)&hdr_temp);
  sprintf(s,(STR)&s03d,TB);
  DrawString(x+12,y,5,5,&s[2],digit_5x5);
  s[2]=0;
  DrawString(x,y,10,5,s,digit_5x5);
 }
_ext_next6:
 if (bit1&_SHOW_EVENTS) {
  if (bit2&_HIDE_EVENTS) if (!IdleKeyFlag) return;
  Event=0;
  if (IsHaveMissedSMS())   Event=1;
  if (IsHaveMissedCalls()) if (!Event) Event=2; else Event=4;
  if (IsHaveMissedAlarm()) if (!Event) Event=3; else Event=4;
  if (IsHaveNoFreeSMS())   if (!Event) Event=5; else Event=4;
  if (!Event) return;
  DrawPredefinedImage(mainscreen->events_x,mainscreen->events_y,pict_EventSms+Event-1);
 }
}

int DrawFullScreenEvent(struct MAINSCREEN far* mainscreen)
{
 char s[10];
 int Event=0;
 if (mainscreen->bit1&_SHOW_EVENTSFULLSCR) {
  if (!IdleKeyFlag) return 0;
  if (IsHaveMissedSMS())   Event=1;
  if (IsHaveMissedCalls()) if (!Event) Event=2; else Event=4;
  if (IsHaveMissedAlarm()) if (!Event) Event=3; else Event=4;
  if (IsHaveNoFreeSMS())   if (!Event) Event=5; else Event=4;
  if (!Event) return 0;
  DrawPredefinedImage(0,0,pict_EventSmsBIG+Event-1);
  if (Event==1) {
   sprintf(s,(STR)&s02d,NumMissedSMS);
   DrawString(77,64,24,16,s,digit_big);
  }
  if (Event==2) {
   sprintf(s,(STR)&s02d,GetNumMissedCalls);
   DrawString(77,64,24,16,s,digit_big);
  }
 } 
 return Event;
}
//===================================================================
void Generate_buff4SDM(void)
{
#pragma asm
 mov [-r0], r12
 mov [-r0], r13
#pragma endasm
 struct MAINSCREEN far* mainscreen;
 int y, f;
 memcpy(&buff4SDM,(char far*)0xB8E652,0x88);

 mainscreen=GetMainscreenVars();

 y=mainscreen->datetime_y-8;
 buff4SDM.DatY1=y;
 buff4SDM.DatY2=y;
 buff4SDM.DatY4=y;
 buff4SDM.DatY6=y;

 f=mainscreen->datetime_f;
 buff4SDM.DatF1=f;
 buff4SDM.DatF2=f;
 buff4SDM.DatF4=f;
 buff4SDM.DatF6=f;

 y=mainscreen->opname_y-8;
 buff4SDM.Op1Y1=y;
 buff4SDM.Op1Y2=y;
 buff4SDM.Op1Y3=y;
 buff4SDM.Op1Y4=y;
 buff4SDM.Op1Y5=y;

 f=mainscreen->opname_f;
 buff4SDM.Op1F1=f;
 buff4SDM.Op1F2=f;
 buff4SDM.Op1F3=f;
 buff4SDM.Op1F4=f;
 buff4SDM.Op1F5=f;

 y=mainscreen->opname2_y-8;
 buff4SDM.Op2Y1=y;
 buff4SDM.Op2Y4=y;
 buff4SDM.Op2Y5=y;

 f=mainscreen->opname2_f;
 buff4SDM.Op2F1=f;
 buff4SDM.Op2F4=f;
 buff4SDM.Op2F5=f;

 y=mainscreen->cbmess_y-8;
 buff4SDM.CB_Y1=y;
 buff4SDM.CB_Y3=y;
 buff4SDM.CB_Y5=y;
 buff4SDM.CB_Y6=y;

 f=mainscreen->cbmess_f;
 buff4SDM.CB_F1=f;
 buff4SDM.CB_F3=f;
 buff4SDM.CB_F5=f;
 buff4SDM.CB_F6=f;
}
//===================================================================
//===================================================================
//===================================================================
void jaf_1(void)
{
#pragma asm
    mov     [-r0], r6
    mov     r6, #1
    mov     [-r0], r12
    mov     [-r0], r13
    mov     [-r0], r14
    mov     [-r0], r15
    CALLS   SEG _GetMainscreenVars,_GetMainscreenVars
    mov     r15, [r0+]
    mov     r14, [r0+]
    mov     r13, [r0+]
    mov     r12, [r0+]
    extp    r5, #1
    mov     r5, [r4+#2]
    jnb     r5.10, _not_flipscr1
    mov     r2, #160Ch
    sub     r10, r2
    mov     MDL, r10
    divu    r11
    add     r2, MDH
    mov     r10, #4Fh
    sub     r10, MDL
    mulu    r10, r11
    mov     r10, MDL
    add     r10, r2
    neg     r11
 _not_flipscr1:
    or      r10, #8000h
#pragma endasm
}

void jaf_2(void)
{
#pragma asm
    movb    rh4, [r10]
    mov     r11, r14
    jnb     r5.9, _not_invertscr
    cpl     r1
    cpl     r2
    cpl     r3
    cpl     r4
 _not_invertscr:
    jnb     r5.10, _not_filpscr2
    neg     r6
    cmp     r11, #1B6Fh
    jmpr    cc_NC, ff_001
    mov     r5, #3678h
    jmpr    cc_UC, ff_002
 ff_001:
    mov     r5, #3748h
 ff_002:
    sub     r5, r11
    mov     r11, r5
 _not_filpscr2:
#pragma endasm
}

void jaf_3(void)
{
#pragma asm
    mov     r6, [r0+]
    bmov    r5.7, r4.8
#pragma endasm
}
//===================================================================
//===================================================================
//===================================================================
void DoDate(struct MAINSCREEN far* mainscreen, char far* dest)
{
 DATE date_buff;
 unsigned char s[20];
 int f, i;
 unsigned int d;
 unsigned int m=(_month-1);
 CopyDate(&date_buff);
 d=GetWeekDay(&date_buff);
 f=GetDateFormat()^1;
 i=mainscreen->set1>>_GET_DATE_FORMAT;
 //0
 if (i==0) {
  DoDate0(&date_buff,dest,7,f);
  return;
 }
 //1
 if (i==1) {
  s[0]=0x95;s[1]=',';
  DoDate0(&date_buff,(STR)&s[2],7,f);
  s[8]=s[10];s[9]=s[11];s[10]=s[11]=0;
  GetStringByID1(d+lng_Dy,dest);
  memcpy(&dest[strlen(dest)],(STR)s,1+strlen((STR)s));
  return;
 }
 //2,3
 if (i==2) {
  d+=lng_Dy; 
  m+=lng_Mon;
 } else {
  d+=lng_Day; 
  m+=lng_Month;
 }
 GetStringByID1(d,dest);
 i=strlen(dest);
 memcpy(&dest[i],"• 00 ",5);
 FormatNumber(_day,&dest[i+2],2);
 GetStringByID1(m,&dest[i+5]);
}

#define DT_Right        0
#define DT_Left         1
#define DT_Center       2
#define DT_Not          3
void DoDateTime(WSHDR far* wbody)
{
 struct MAINSCREEN far* mainscreen=GetMainscreenVars();
 int dpos=mainscreen->set1>>_GET_DATE_POS;
 int tpos=mainscreen->set1>>_GET_TIME_POS;
 WSHDR wdate, wtime;
 unsigned int wstr1[30];
 unsigned int wstr2[10];
 char date[40];
 char time[6];
 ClearWSTR(&wdate,wstr1,30);
 ClearWSTR(&wtime,wstr2,10);
 if (tpos!=DT_Not) 
  switch (dpos) {
   case DT_Right:  tpos=DT_Left; break;
   case DT_Left:   tpos=DT_Right; break;
   case DT_Center: tpos=DT_Not; break;
  }
 if (dpos!=DT_Not) {
  DoDate(mainscreen,(STR)&date);
  ConvLangSTRToWSHDR(&wdate,(STR)&date,0,1);
  InsertCharToWSTR(&wdate,0xE01C+dpos,1);
 }
 if (tpos!=DT_Not) {
  sprintf((STR)&time,"%02u:%02u",_hour,_minute);
  ConvLangSTRToWSHDR(&wtime,(STR)&time,0,1);
  InsertCharToWSTR(&wtime,0xE01C+tpos,1);
 }

 if (tpos==DT_Left) ConcatWSTR(wbody,&wtime);
 ConcatWSTR(wbody,&wdate);
 if (tpos!=DT_Left) ConcatWSTR(wbody,&wtime); 

 if (mainscreen->bit1&_HIDE_ALARM) return;
 if (!HaveAlarm()) return;
 if ((tpos==DT_Not)&&(dpos==DT_Not)) AddCharToWSTR(wbody,0xE01C+DT_Right);
 AddCharToWSTR(wbody,(mainscreen->bit1&_OTHER_ALARM) ? 0xE403 : 0xE40A);
}

//===================================================================
//===================================================================
//===================================================================
static const char far ssaverfldr[]="A:\\zbin\\screensavers\\";
static const char far ext_bin[]=".bin";
void ScreenSaverLoad(void)
{
 int f;
 char fname[127];
 strcpy((STR)&fname,(STR)&ssaverfldr);
 strcat((STR)&fname,(STR)GetUniVars()->screen_vars.ssaver_name);
 strcat((STR)&fname,(STR)&ext_bin);
 f=FileOpen((STR)&fname,_O_RDONLY,0);
 if (f==-1) {
  LockKBD();
  doIDLE();
  return;
 }
 FileRead(f,(char far*)0x80000,0x3FFF);
 FileClose(f);
 TurnLightIfON();
 _binfile(0);
}

void ScreenSaverStart(int sel)
{
 if (GetUniVars()->screen_vars.otherscreen_bit&sel) FilesysICall(ScreenSaverLoad);
  else { LockKBD(); doIDLE(); }
}

void ScreenSaverStartManual(void)
{
 ScreenSaverStart(_SSAVER_CUST);
}

void ScreenSaverStartAuto(void)
{
 ScreenSaverStart(_SSAVER_AUTO);
}
//===================================================================
//===================================================================
//===================================================================
int GetMessagesByte(void)
{
 return (GetUniVars()->messages_vars.bit1);
}
//===================================================================
//===================================================================
//===================================================================
int SoftButtonWidth(void)
{
 if (GetUniVars()->screen_vars.otherscreen_bit&_WIDHT_SOFT) return 51; //Width
  else return 47;// std
}

int GetStateMST(void)
{
 if (uni_vars.screen_vars.otherscreen_bit&_MOVTXT_SFT) return 1;
  else return 0;
}

int GetStateIST(void)
{
 if (uni_vars.screen_vars.otherscreen_bit&_INVER_SOFT) return 0;
  else return 1;
}

void My_DrawAllSofts2(char far* data1, char far* data2, int soft)
{
 int type=*(int far*)(data2+0x04);
//1 DrawDisableSoft
//2 DrawTextSoft
//3 DrawImageSoft
 int pict=*(int far*)(data1+0x3A);
//0x4E26 //newsms
//0x4E2E //unanswered
//0x4E3B //memo 
//0x4E36 //mixed
//0x4E3D //smsfull
 switch (pict) {
  case 0x4E26: pict=1; break;
  case 0x4E2E: pict=2; break;
  case 0x4E3B: pict=3; break;
  case 0x4E36: pict=4; break;
  case 0x4E3D: pict=5; break;
  default: pict=0;
 }
 if (pict) if (GetMainscreenVars()->bit1&_SHOW_EVENTSFULLSCR) return;
 if (GetMainscreenVars()->bit1&_HIDE_SOFTS) if (IdleKeyFlag) return;
 if (soft==0) if (uni_vars.screen_vars.otherscreen_bit&_SKIN_SOFT) DrawPredefinedImage(0,67,pict_SoftSkin);
 if (type==1) if (uni_vars.screen_vars.otherscreen_bit&_INVER_SOFT) return;
 DrawAllSofts(data1, data2, soft);
}

void My_DrawAllSofts(void)
{
#pragma asm
 mov    [-r0], r8
 calls  _My_DrawAllSofts2
 add    r0, #2
#pragma endasm
}

void My_DrawTextSoft(void)
{
#pragma asm
;---------------------------------------
 calls  _GetStateMST
 cmp    r4, #0
 jmpr   cc_Z, _sfttext_center
 mov    r5, [r0+#1Ah] ;int soft
 cmp    r5, #0
 jmpr   cc_Z, _sfttext_left
_sfttext_right:
 mov    r6, #2
 jmpr   cc_UC, _set_sfttext
_sfttext_left:
 mov    r6, #0
 jmpr   cc_UC, _set_sfttext
_sfttext_center:
 mov    r6, #1
_set_sfttext:
;---------------------------------------
 mov    [-r0], r6 ;1-center,0-left,2-right
 mov    r7, [r0+#1Eh]
 mov    r12, [r0+#20h]
 extp   r12, #1
 movb   rl1, [r7+#52h]
 movbs  r13, rl1
 mov    [-r0], r13
 extp   r9, #2
 mov    r15, [r8+#2]
 mov    r14, [r8]
 mov    [-r0], r15
 mov    [-r0], r14
 calls  _GetStateIST
 cmp    r4, #0
 jmpr   cc_Z, _set_invtext
 mov    r4, #60h 
_set_invtext:
 mov    [-r0], r4 ;60h-invert,0-std
#pragma endasm
}

void HookAfterDrawSofts(void)
{
#pragma asm
 mov r8, [r0+]
 mov r9, [r0+]
#pragma endasm
 if (GetUniVars()->screen_vars.otherscreen_bit&_ROUND_SOFT) {
  ClearRect(0,67,1,1);
  ClearRect(0,79,1,1);
  ClearRect(100,67,1,1);
  ClearRect(100,79,1,1);
  if (!(GetUniVars()->screen_vars.otherscreen_bit&_WIDHT_SOFT)) {
   ClearRect(46,67,1,1);
   ClearRect(46,79,1,1);
   ClearRect(54,67,1,1);
   ClearRect(54,79,1,1);
  }
 }
}

