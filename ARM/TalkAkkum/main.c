#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

#define UPDATE_TIME (1*216)

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

GBSTMR update_tmr;
GBSTMR level_tmr;
GBSTMR mytmr;

extern const char folder_path[128];
extern const int say_levels_up;
extern const int say_levels_down;
extern const unsigned int begin_from;

extern const unsigned int from_h;
extern const unsigned int to_h;
extern const unsigned int volume;

extern const int warn_temp;
extern const unsigned int warn_temp_value;
extern const unsigned int warn_interr;

extern const int show_icon;
extern const unsigned int cfgX;
extern const unsigned int cfgY;

extern const unsigned int vibra_count;
extern const unsigned int vibra_power;

extern const unsigned int dop_delay; 

// Глобальные переменные
unsigned int found_full_empty[3]={0,0,0}; //0-found, 1-full, 3-empty
unsigned int levels_up[9]   ={0,0,0,0,0,0,0,0};//0-10,...,8-90
unsigned int levels_down[10]={0,0,0,0,0,0,0,0,0};//0-5,...,9-90
unsigned int flag=0, flag1=0;
unsigned int vibra_count1;

char pic_path[];

//=============================Проигрывание звука===============================
void Play(const char *fpath, const char *fname)
{
      WSHDR* sndPath=AllocWS(128);
      WSHDR* sndFName=AllocWS(128);

      wsprintf(sndPath, fpath);
      wsprintf(sndFName, fname);
  

      PLAYFILE_OPT _sfo1;
      zeromem(&_sfo1,sizeof(PLAYFILE_OPT));
      _sfo1.repeat_num=1;
      _sfo1.time_between_play=0;
      _sfo1.play_first=0;
      _sfo1.volume=volume;
    
      #ifdef NEWSGOLD
        _sfo1.unk6=1;
        _sfo1.unk7=1;
        _sfo1.unk9=2;
        PlayFile(0x10, sndPath, sndFName, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
      #else
          #ifdef X75
            _sfo1.unk4=0x80000000;
            _sfo1.unk5=1;
            PlayFile(0xC, sndPath, sndFName, 0, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
          #else
            _sfo1.unk5=1;
            PlayFile(0xC, sndPath, sndFName, MMI_CEPID, MSG_PLAYFILE_REPORT, &_sfo1);
          #endif
      #endif 

      FreeWS(sndPath);
      FreeWS(sndFName);
}

//==============================================================================

GBSTMR tmr_vibra;

void start_vibra(void)
{
  void stop_vibra(void);
  
  SetVibration(vibra_power);
  GBS_StartTimerProc(&tmr_vibra, 50, stop_vibra);
}

void stop_vibra(void)
{
  SetVibration(0);
  if (--vibra_count1)
  {
    GBS_StartTimerProc(&tmr_vibra, 50, start_vibra);
  }
}

//==============================================================================

//============Проверка длительности вавки без переменного битрэйда==============

double GetWavkaLength(char *fpath, char *fname) //тиков
{
  int f;
  unsigned int ul;
  
  int DataLength;//4
  int BytePerSec;//28  
  
  char ffn[128];
  strcpy(ffn, fpath);
  strcat(ffn, fname);
  if ((f=fopen(ffn,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
  {
    lseek(f,4,S_SET,&ul,&ul);
    fread(f,&DataLength,sizeof(DataLength),&ul);
    
    lseek(f,28,S_SET,&ul,&ul);
    fread(f,&BytePerSec,sizeof(BytePerSec),&ul);
    
    fclose(f,&ul);
    
    return (((((double)DataLength/(double)BytePerSec)*(double)1000)*0.216)+dop_delay);
  }
    else
      return 0;
}
//============================================================================== 

void RereadSettings(void) //Считывание конфига
{
   LockSched();
    InitConfig();  
    strcpy(pic_path, folder_path);
    strcat(pic_path, "TalkAkkum.png");
   UnlockSched();
}

//==============================================================================

//Уровни
void SayPercent(void)
{
  Play(folder_path, "percent.wav");
}

void SayLevel(void)
{
  char level[6];
  sprintf(level, "%d.wav", *RamCap());
  Play(folder_path, level);
  GBS_StartTimerProc(&mytmr,(int)GetWavkaLength((char*)folder_path, level),SayPercent);
}

void SayUp(void)
{
  Play(folder_path, "up.wav");
  GBS_StartTimerProc(&mytmr,(int)GetWavkaLength((char*)folder_path, "up.wav"),SayLevel);
}

void SayDown(void)
{
  Play(folder_path, "down.wav");
  GBS_StartTimerProc(&mytmr,(int)GetWavkaLength((char*)folder_path, "down.wav"),SayLevel);
}

GBSTMR temp_tmr;
void SayTemp(void)
{
  if (!IsCalling())
    {
      Play(folder_path, "temp.wav");
  
      vibra_count1=vibra_count;
      start_vibra();
    }  
  
  if (flag==1) GBS_StartTimerProc(&temp_tmr, warn_interr*216, SayTemp);
}
// ----------------------------------------------------------------------------

void Delay(void)
{
  flag1=1;
  //found_full_empty[0]=1;
}

void Check(void)
{
  TDate date;
  TTime time;
  
  GetDateTime(&date, &time);
  if ((time.hour>=from_h)&&(time.hour<to_h)&&(!IsCalling()))
    {
      unsigned int state, cap_akku;
      
      LockSched();
      state=*RamLS();
      cap_akku=*RamCap();
      UnlockSched();
      
      //state 0-разрядка
      //      1-промежуточное значение
      //      2-зарядка
      //      3-полный  
      
      
      //unsigned int found_full_empty[3] //0-found, 1-full, 2-empty
      
      switch (state)
      {
        case 0:
          if (found_full_empty[0]||found_full_empty[1]) 
            {
              Play(folder_path, "out.wav");
          
              found_full_empty[0]=0;
              found_full_empty[1]=0;
          
              unsigned int i;
              for (i=0; i<=8; i++) {levels_up[i]=0;}              
            }
          
          if ((say_levels_down)&&(cap_akku<=begin_from))
            {//Говорить промежуточные уовни разрядки
              //unsigned int levels_down[10]={0,0,0,0,0,0,0,0,0};//0-5,...,9-90
              switch (cap_akku)
              {
                case 10:
                  if (!levels_down[1]) {levels_down[1]=1; SayDown();}
                break;
                case 20:
                  if (!levels_down[2]) {levels_down[2]=1; SayDown();}
                break;
                case 30:
                  if (!levels_down[3]) {levels_down[3]=1; SayDown();}
                break;
                case 40:
                  if (!levels_down[4]) {levels_down[4]=1; SayDown();}
                break;
                case 50:
                  if (!levels_down[5]) {levels_down[5]=1; SayDown();}
                break;
                case 60:
                  if (!levels_down[6]) {levels_down[6]=1; SayDown();}
                break;
                case 70:
                  if (!levels_down[7]) {levels_down[7]=1; SayDown();}
                break;
                case 80:
                  if (!levels_down[8]) {levels_down[8]=1; SayDown();}
                break;
                case 90:
                  if (!levels_down[9]) {levels_down[9]=1; SayDown();}
                break;
              }          
            } /*Говорить промежуточные уовни разрядки*/ 
          
          
          if ((!levels_down[0])&&(cap_akku==5))
            {//-5% Говорить в любом случае
              levels_down[0]=1;
              SayDown();
            }
      
          if ((!found_full_empty[2])&&(cap_akku==0))
            {//0% Говорить в любом случае
              found_full_empty[2]=1;
              Play(folder_path, "0.wav");
            }
        break;
        
        case 2:
          if (!found_full_empty[0])
            {// Найден источник

              Play(folder_path, "found.wav");
              flag1=0;
              GBS_StartTimerProc(&level_tmr,(int)GetWavkaLength((char*)folder_path, "found.wav")+216,Delay);
              
              found_full_empty[0]=1;
              
              //Обнуление флагов
              found_full_empty[1]=0;
              
              unsigned int i;
              for (i=0; i<=8; i++) {levels_up[i]=0;}
              for (i=0; i<=9; i++) {levels_down[i]=0;}
              
            }// Найден источник
          
          if ((found_full_empty[0])&&(say_levels_up)&&(flag1))
            {//Говорить промежуточные уровни зарядки
              switch (cap_akku)
              {
                case 10:
                  if (!levels_up[0]) {levels_up[0]=1; SayUp();}
                break;
                case 20:
                  if (!levels_up[1]) {levels_up[1]=1; SayUp();}
                break;
                case 30:
                  if (!levels_up[2]) {levels_up[2]=1; SayUp();}
                break;
                case 40:
                  if (!levels_up[3]) {levels_up[3]=1; SayUp();}
                break;
                case 50:
                  if (!levels_up[4]) {levels_up[4]=1; SayUp();}
                break;
                case 60:
                  if (!levels_up[5]) {levels_up[5]=1; SayUp();}
                break;
                case 70:
                  if (!levels_up[6]) {levels_up[6]=1; SayUp();}
                break;
                case 80:
                  if (!levels_up[7]) {levels_up[7]=1; SayUp();}
                break;
                case 90:
                  if (!levels_up[8]) {levels_up[8]=1; SayUp();}
                break;
              }
            }/*Говорить промежуточные уровни зарядки*/
        break;
        
        case 3:
          if (!found_full_empty[1])
            {// Энергия восстановлена
              found_full_empty[1]=1;

              Play(folder_path, "full.wav");
          
              //Обнуление флагов
              found_full_empty[0]=0;
              found_full_empty[2]=0;
              
              unsigned int i;
              for (i=0; i<=9; i++) {levels_down[i]=0;}          
            }// Энергия восстановлена          
        break;  
      }
      
    }//if ((time.hour>=from_h)&&(time.hour<=to_h))

      // Температура
      if (warn_temp)
        {
          if (((GetAkku(1,3)-0xAAA+15)/10)<(warn_temp_value) && (flag==0) )
            {
              flag=1;
              SayTemp();
            }
        }
      
      if  (flag==1)
        {
          if (((GetAkku(1,3)-0xAAA+15)/10)>=(warn_temp_value)) 
            {
              flag=0;
            }
        }
       //   

  GBS_StartTimerProc(&update_tmr,UPDATE_TIME,Check);
}

// ----------------------------------------------------------------------------
#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}
#pragma inline
int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  
  csm_result = old_icsm_onMessage(data, msg); //Вызываем старый обработчик событий
  
  if(msg->msg == MSG_RECONFIGURE_REQ) // Перечитывание конфига по сообщению
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"TalkAkkum config updated!");
      RereadSettings();
    }
  }
  
  
  if ((IsGuiOnTop(idlegui_id))&&(show_icon)) //Если IdleGui на самом верху
    {
      GUI *igui=GetTopGUI();
      if (igui) //И он существует
      {
#ifdef ELKA

	void *canvasdata = BuildCanvas();
#else
	void *idata = GetDataOfItemByID(igui, 2);
	if (idata)
	{
	  void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
#endif
            DrawCanvas(canvasdata, cfgX, cfgY, cfgX + GetImgWidth((int)pic_path), cfgY + GetImgHeight((int)pic_path), 1);
	    DrawImg(cfgX, cfgY, (int)pic_path);
#ifdef ELKA
#else
	}
#endif
      }
    }      
  
  return(csm_result);
}

void MyIDLECSM_onClose(CSM_RAM *data)
{
  extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
  extern void *ELF_BEGIN;
  GBS_DelTimer(&level_tmr);
  GBS_DelTimer(&update_tmr);
  GBS_DelTimer(&tmr_vibra);
  GBS_DelTimer(&temp_tmr);
  GBS_DelTimer(&mytmr);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  LockSched();
  RereadSettings();

  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;  
  icsm->constr=&icsmd;
  
  GBS_StartTimerProc(&update_tmr,216*5,Check);
  UnlockSched();
  return 0;
}

