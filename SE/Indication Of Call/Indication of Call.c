#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\cfg_items.h"
#include "config_data.h"
#include "conf_loader.h"

#define ELFNAME "Indication of Call 2.0.6"
#define LELFNAME L"Indication of Call"
#define LELFVERSION L"\nv2.0.6\n\n(c)Ploik & BigHercules\n\nRespect: Slawwan"
#define LINIFILENAME _T("Indication.ini")

void (*LEDControl_W580)(int,int id,int RED,int GREEN,int BLUE, int br, int delay)=(void (*)(int,int id,int RED,int GREEN,int BLUE,int br,int delay))(0x4529BFA9);

#define COLOR_GET_A(x) ((unsigned int)x>>24)
#define COLOR_GET_R(x) (((unsigned int)x>>16)&0xFF)
#define COLOR_GET_G(x) (((unsigned int)x>>8)&0xFF)
#define COLOR_GET_B(x) ((unsigned int)x&0xFF)

static char myappname[]=ELFNAME;

#pragma swi_number=0x1BB
__swi __arm void Vibra(int t1, int t2, int t3);
#pragma swi_number=0x248
__swi __arm void Vibra_Off(int t1);

int lamp = 0;
int screen=0;
u16 timerFlash = 0;
u16 timerLED = 0;
u16 timerScreen = 0;
u16 vibratimer = 0;
u16 vibraminutetimer = 0;
u16 vibraNminutetimer = 0;
u16 offtimer = 0;
u16 offtimerLED = 0;
u16 offtimerScreen = 0;

int skipevents;
int checkevents;
int LED=0;
int LED580=1;
int LEDnum=2;
int incom=0;
int connect=0;

void offTimerFlash(u16 timerID, LPARAM lparam);
void offTimerLED(u16 timerID, LPARAM lparam);
void offTimerScreen(u16 timerID, LPARAM lparam);

int DisableUP(UI_MESSAGE*)
{
  if((incom==1) && (cfg_flash_silent==0))
  {
     offTimerFlash(0,0);
  }
  if((incom==1) && (cfg_led_silent==0))
  {
     offTimerLED(0,0);
  }
  if((incom==1) && (cfg_screen_silent==0))
  {
     offTimerScreen(0,0);
  }
  return(0);
}
int DisableDOWN(UI_MESSAGE*)
{
  if((incom==1) && (cfg_flash_silent==0))
  {
     offTimerFlash(0,0);
  }
  if((incom==1) && (cfg_led_silent==0))
  {
     offTimerLED(0,0);
  }
  if((incom==1) && (cfg_screen_silent==0))
  {
     offTimerScreen(0,0);
  }
  return(0);
}

void myVibraOff(u16 timerID, LPARAM lparam)
{
  if(vibratimer)
  {
    Timer_Kill(&vibratimer);
    Vibra_Off(0);
    vibratimer=0;
  }
}

void offTimerFlash(u16 timerID, LPARAM lparam)
{
  if(timerFlash)
  {
    Timer_Kill(&timerFlash);
    timerFlash = 0;
    SetLampLevel(0);
  }
}

void offTimerLED(u16 timerID, LPARAM lparam)
{
   if(timerLED)
   {
     Timer_Kill(&timerLED);
     timerLED = 0;
   }
   if(cfg_phone_type == 1)
   {
     LEDControl_W580(1,0,0,0,0,0,1);
     LEDControl_W580(1,1,0,0,0,0,1);
   }
   else if(cfg_phone_type == 2)
   {
     for(int i=0;i<3;i++)
     OrangeLED_Control(1,i,0,50);
   }
   LED580 = 1;
   LEDnum=0;
   LED = 0;
}

void offTimerScreen(u16 timerID, LPARAM lparam)
{
  if(timerScreen)
  {
    Timer_Kill(&timerScreen);
    timerScreen = 0;
    IndicationDevice_Backlight_FadeToLevel(0,cfg_screen_level);
  }
}

void onVibraMinuteTimer(u16 timerID, LPARAM lparam)
{
   Vibra(400,50,1500);
   vibratimer=Timer_Set(cfg_vibra_at_minute_end_time,myVibraOff,NULL);
   Timer_ReSet(&vibraminutetimer,60000,onVibraMinuteTimer,0); 
}

void onVibraNMinuteTimer(u16 timerID, LPARAM lparam)
{
   Vibra(400,50,1500);
   vibratimer=Timer_Set(cfg_vibra_at_n_minute_end_time,myVibraOff,NULL);
}

void onTimerFlash(u16 timerID, LPARAM lparam)
{ 
    switch(GetChipID())
    {
        case 0x7100:
        case 0x8000:
        case 0x8040:
        {   
            SetLampLevel(lamp^=0x50);
        }
        break;
        case 0x9900:
        {
            SetLampLevel(lamp^=1);
        }
        break;
    }
    Timer_ReSet(&timerFlash,cfg_flash_blink_speed,onTimerFlash,0); 
}

void onTimerLED(u16 timerID, LPARAM lparam)
{
  if(cfg_phone_type == 1)
  {
    Timer_ReSet(&timerLED,cfg_led_blink_speed,onTimerLED,0);
    if (cfg_led580_blink_mode == 2)
    {
      LEDControl_W580(1, 0, COLOR_GET_R(cfg_led580_color),COLOR_GET_G(cfg_led580_color),COLOR_GET_B(cfg_led580_color),0,LED580 ^= 1);
      LEDControl_W580(1, 1, COLOR_GET_R(cfg_led580_color),COLOR_GET_G(cfg_led580_color),COLOR_GET_B(cfg_led580_color),0,LED580);
    }
    else
    {
      LEDControl_W580(1, cfg_led580_blink_mode, COLOR_GET_R(cfg_led580_color),COLOR_GET_G(cfg_led580_color),COLOR_GET_B(cfg_led580_color),0,LED580 ^= 1);
    }
  }
  else if(cfg_phone_type == 2)
  {
    Timer_ReSet(&timerLED,100,onTimerLED,0);
    if (cfg_led_blink_mode)
    {
      OrangeLED_Control(1,LEDnum,0,cfg_led_blink_speed);
      if (LEDnum == 2) LEDnum=0;
      else LEDnum++;
      OrangeLED_Control(1,LEDnum,100,cfg_led_blink_speed);
    }
    else 
      OrangeLED_Control(1,0,LED^=0x64,cfg_led_blink_speed);
  }
}

void onTimerScreen(u16 timerID, LPARAM lparam)
{
    IndicationDevice_Backlight_FadeToLevel(0,screen ^= cfg_screen_level);
    Timer_ReSet(&timerScreen,cfg_screen_blink_speed,onTimerScreen,0); 
}

void elf_exit(void)
{
    kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

void bookOnDestroy(BOOK * book)
{
    if(timerFlash) 
    {
        Timer_Kill(&timerFlash);
        offTimerFlash(0,0);
    }
    if(offtimer) Timer_Kill(&offtimer);
    if(offtimerLED) 
    {
        Timer_Kill(&offtimerLED);
        offTimerLED(0,0);
    }
    if(vibratimer)
    {
        Timer_Kill(&vibratimer);
        myVibraOff(0,0);
    }
    if(timerScreen) offTimerScreen(0,0);
    if(offtimerScreen) Timer_Kill(&offtimerScreen);
    if(vibraminutetimer) Timer_Kill(&vibraminutetimer);
    if(vibraNminutetimer) Timer_Kill(&vibraNminutetimer);
    ModifyUIHook(VOLUMEUPKEY_SHORT_PRESS_EVENT,DisableUP,0);
    ModifyUIHook(VOLUMEDOWNKEY_SHORT_PRESS_EVENT,DisableDOWN,0);
    StatusIndication_ShowNotes(0x6FFFFFFF);
    SUBPROC(elf_exit);
}


int onExit(void* r0,BOOK* b)
{
    FreeBook(b);
    return 1;
}

int onAbout(void* mess,BOOK* b)
{
    MessageBox(0x6fFFFFFF,Str2ID( LELFNAME LELFVERSION,0,SID_ANY_LEN),0xFFFF, 1 ,5000, *(BOOK**)mess);
    return 1;
}

enum
{
    CALLMANAGER_IDLE            =0,
    CALLMANAGER_CALL_CONNECTED  =1, /*Поднятие трубки*/
    CALLMANAGER_CALL_SETUP      =2, /*Нажатие вызвать*/
    CALLMANAGER_CALL_ALERT      =4, /*Входящий вызов*/
    CALLMANAGER_CALL_WAITING    =5,
    CALLMANAGER_CALL_TERMINATED =6  /*Завершение соединения*/
};
     /*Начало гудков*/
int Connect(void* r0,BOOK* b)
{
    if(cfg_vibraring==1)
    {
        Vibra(400,50,1500);
        vibratimer=Timer_Set(cfg_vibraring_time,myVibraOff,NULL);
    }
    if(cfg_flashring==1)
    {
        timerFlash=Timer_Set(70,onTimerFlash,0);
        offtimer=Timer_Set(cfg_flashring_time*1000,offTimerFlash,0);
    }
    if(cfg_ledring==1)
    {
        timerLED=Timer_Set(cfg_led_blink_speed,onTimerLED,0);
        offtimerLED=Timer_Set(cfg_ledring_time*1000,offTimerLED,0);
    }
    if(cfg_screenring==1)
    {
        timerScreen=Timer_Set(cfg_screen_blink_speed,onTimerScreen,0);
        offtimerScreen=Timer_Set(cfg_screenring_time*1000,offTimerScreen,0);
    }
    return(0);
} 

int OnCallManagerEvent(void* r0,BOOK* b)
{
    switch(((CALLMANAGER_EVENT_DATA*)r0)->CallState)
    {
      /*Входящий вызов*/
        case CALLMANAGER_CALL_ALERT:
        {
            if(GetVibrator(0,0) && (cfg_vibra==0))
            {
               myVibraOff(0,0);
            }
            if(cfg_flash==1)
            {
                timerFlash=Timer_Set(cfg_flash_blink_speed,onTimerFlash,0);
                if(cfg_flash_time > 0) offtimer=Timer_Set(cfg_flash_time*1000,offTimerFlash,0);
                incom=1;
            }
            if(cfg_led==1)
            {
                timerLED=Timer_Set(cfg_led_blink_speed,onTimerLED,0);
                if(cfg_led_time > 0) offtimerLED=Timer_Set(cfg_led_time*1000,offTimerLED,0);
            }
            if(cfg_screen==1)
            {
                timerScreen=Timer_Set(cfg_screen_blink_speed,onTimerScreen,0);
                if(cfg_screen_time > 0) offtimerScreen=Timer_Set(cfg_screen_time*1000,offTimerScreen,0);
            }
            break; 
        }
       /*Поднятие трубки*/ 
        case CALLMANAGER_CALL_CONNECTED: 
        {
            if(timerFlash) offTimerFlash(0,0);
            if(timerLED) offTimerLED(0,0);
            if(timerScreen) offTimerScreen(0,0);

            if((cfg_vibracon==1) && (incom==0))
            {
                Vibra(400,50,1500);
                vibratimer=Timer_Set(cfg_vibracon_time,myVibraOff,NULL);
            }
            if((cfg_flashcon==1) && (incom==0))
            {
                timerFlash=Timer_Set(cfg_flash_blink_speed,onTimerFlash,0);
                offtimer=Timer_Set(cfg_flashcon_time*1000,offTimerFlash,0);
            }
            if((cfg_ledcon==1) && (incom==0))
            {
                timerLED=Timer_Set(cfg_led_blink_speed,onTimerLED,0);
                offtimerLED=Timer_Set(cfg_ledcon_time*1000,offTimerLED,0);
            }
            if((cfg_screencon==1) && (incom==0))
            {
                timerScreen=Timer_Set(cfg_screen_blink_speed,onTimerScreen,0);
                offtimerScreen=Timer_Set(cfg_screencon_time*1000,offTimerScreen,0);
            }
            if(cfg_vibra_at_minute_end==1)
            {
                   vibraminutetimer = Timer_Set((60 - cfg_vibra_at_minute_end_lead) * 1000 ,onVibraMinuteTimer,0); 
            }
            if(cfg_vibra_at_n_minute_end > 0)
            {
                   vibraNminutetimer = Timer_Set((cfg_vibra_at_n_minute_end * 60 - cfg_vibra_at_n_minute_end_lead) * 1000 ,onVibraNMinuteTimer,0); 
            }
            connect = 1;
            break;
        }
         /*Нажатие вызвать*/
        case CALLMANAGER_CALL_SETUP:
        {
            if(cfg_vibraon==1)
            {
                Vibra(400,50,1500);
                vibratimer=Timer_Set(cfg_vibraon_time,myVibraOff,NULL);
            }
            if(cfg_flashon==1)
            {
                timerFlash=Timer_Set(cfg_flash_blink_speed,onTimerFlash,0);
                offtimer=Timer_Set(cfg_flashon_time*1000,offTimerFlash,0);
            } 
            if(cfg_ledon==1)
            {
                timerLED=Timer_Set(cfg_led_blink_speed,onTimerLED,0);
                offtimerLED=Timer_Set(cfg_ledon_time*1000,offTimerLED,0);
            }
            if(cfg_screenon==1)
            {
                timerScreen=Timer_Set(cfg_screen_blink_speed,onTimerScreen,0);
                offtimerScreen=Timer_Set(cfg_screenon_time*1000,offTimerScreen,0);
            }
            break;
        }
         /*Завершение соединения*/
        case CALLMANAGER_CALL_TERMINATED:
        {
            if(cfg_flash  == 1) offTimerFlash(0,0);
            if(cfg_led    == 1) offTimerLED(0,0);
            if(cfg_screen == 1) offTimerScreen(0,0);

            if((cfg_vibraend == 1) && (connect == 1))
            {
                Vibra(400,50,1500);
                vibratimer=Timer_Set(cfg_vibraend_time,myVibraOff,NULL);
            }
            if((cfg_flashend == 1) && (connect == 1))
            {
                timerFlash=Timer_Set(cfg_flash_blink_speed,onTimerFlash,0);
                offtimer=Timer_Set(cfg_flashend_time*1000,offTimerFlash,0);
            } 
            if((cfg_screenend==1) && (connect == 1))
            {
                timerScreen=Timer_Set(cfg_screen_blink_speed,onTimerScreen,0);
                offtimerScreen=Timer_Set(cfg_screenend_time*1000,offTimerScreen,0);
            }
            if((cfg_ledend==1) && (connect == 1))
            {
                timerLED=Timer_Set(cfg_led_blink_speed,onTimerLED,0);
                offtimerLED=Timer_Set(cfg_ledend_time*1000,offTimerLED,0);
            }

            if(vibraminutetimer) 
            {
                Timer_Kill(&vibraminutetimer);
                vibraminutetimer = 0;
            }
            if(vibraNminutetimer)
            {
                Timer_Kill(&vibraNminutetimer);
                vibraNminutetimer = 0;
            }
            incom   = 0;
            connect = 0;
            break;
        } 
    }
    return 1;
}

static int onReconfigElf(void *mess ,BOOK *book)
{
  RECONFIG_EVENT_DATA *reconf=(RECONFIG_EVENT_DATA *)mess;
  int result=0;
  if (wstrcmpi(reconf->path,successed_config_path)==0 && wstrcmpi(reconf->name,successed_config_name)==0)
  {
    InitConfig();
    result=1;
  }
  return(result);
}


const PAGE_MSG evtlist[] @ "DYN_PAGE"=
{
  ELF_TERMINATE_EVENT,      onExit,
  ELF_SHOW_INFO_EVENT,      onAbout,
  ELF_RECONFIG_EVENT,       onReconfigElf,
  ON_CALLMANAGER_EVENT_TAG, OnCallManagerEvent,
  ONGOINGCALL_CALL_CONNECTED_EVENT_TAG  ,Connect,
  NIL_EVENT_TAG,            NULL
};

const PAGE_DESC defaultpage = {"Indication_Base_page",0,evtlist};

int isChallengeBook(BOOK * book)
{
        if(!strcmp(book->xbook->name,myappname)) return true;
        return false;
}

int main(wchar_t* filename)
{
        InitConfig();
        BOOK* alreadyrunned=FindBook(isChallengeBook);
        if(alreadyrunned)
        {
            #ifndef ENG
                MessageBox(0x6FFFFFFF, STR("Indication of Call\nуже запущен"), 0, 1 ,5000, 0);
            #else
                MessageBox(0x6FFFFFFF, STR("Indication of Call\nalready runned"), 0, 1 ,5000, 0);
            #endif
        }
        else
        {          
            ModifyUIHook(VOLUMEUPKEY_SHORT_PRESS_EVENT,DisableUP,1);
            ModifyUIHook(VOLUMEDOWNKEY_SHORT_PRESS_EVENT,DisableDOWN,1);		
            BOOK *myBook=(BOOK*)malloc(sizeof(BOOK));
            memset(myBook,0,sizeof(BOOK));
            CreateBook(myBook,bookOnDestroy,&defaultpage,myappname,-1,0);

            if(!wstrwstr(filename,GetDir(DIR_ELFS_DAEMONS)))
              StatusIndication_ShowNotes(0x6FFFFFFF);
        }        
        return 0;
}

/*
  Revision history.
    2.0.6
      + Добавлена поддержка неофициального ELF_BCFG_CONFIG_EVENT
    2.0.5
      + мелкие улучшения/исправления/оптимизация
    2.0.4
      + Изменения в структуре конфигурационного файла.
      + исправлен баг в алгоритме индикации.
      + алгоритм индикации светодиодами аналогичен алгоритму индикации фонарем
      + алгоритм индикации экраном аналогичен алгоритму индикации фонарем
    2.0.3
      + Изменения в структуре конфигурационного файла.
      + исправлен баг в алгоритме мигания светодиодами на W580
    2.0.2
      + Изменения в структуре конфигурационного файла.
      + мигание светодиодами на W580
    2.0.1
      + Изменения в структуре конфигурационного файла.
      + мигание дисплея
      + мигание светодиодами
      + длителность вибры задается в мсек
    2.0 beta 3
      + Поправлен параметр выбора в конце какой минуту вибрить
    2.0 beta2
      + английская версия конфигурационного файла. (Вибирается конфигурацией проекта Release_en)
        Важно! Нужно удалиь старый конфигурационный файл.
      + исправлены ошибка, при переходе на bcgf (DisableUP и DisableDOWN)
    2.0 beta
      + новая версия конфигурационного файла (bcfg)
*/
