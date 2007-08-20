#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
CSM_DESC icsmd;
int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);

GBSTMR mytmr;
extern const unsigned int BUT_ENA;
extern const unsigned int but_X;
extern const unsigned int but_Y;

extern const unsigned int DATE_ENA;
extern const unsigned int date_X;
extern const unsigned int date_Y;
extern const char DATE_FMT[16];

extern const unsigned int TIME_ENA;
extern const unsigned int time_X;
extern const unsigned int time_Y;
extern const char TIME_FMT[16];

extern const unsigned int color;
extern const unsigned int date_font;
extern const unsigned int time_font;

extern const unsigned int FRINGING_ENA;
extern const unsigned int FRINGING_color;

extern const char pic_path[];

WSHDR *ws1;

#define TMR_SECOND 216

int get_string_width(WSHDR *ws, int font)
{
  int width=0;
  unsigned short *body=ws->wsbody;
  int len=body[0];
  while(len)
  {
    width+=GetSymbolWidth(body[len],font);
    len--;
  }
  return (width+1);//хз, без этого тупит
}

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

const char ipc_my_name[]="PicPusk";
#define IPC_UPDATE_STAT 1

const IPC_REQ gipc={
  ipc_my_name,
  ipc_my_name,
  NULL
};

void TimerProc(void)
{
  GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_UPDATE_STAT,&gipc);
  GBS_StartTimerProc(&mytmr,TMR_SECOND,TimerProc);
}

int MyIDLECSM_onMessage(CSM_RAM* data, GBS_MSG* msg)
{
  int csm_result;
  
  if(msg->msg == MSG_RECONFIGURE_REQ) // Перечитывание конфига по сообщению
  {
    extern const char *successed_config_filename;
    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      InitConfig();
      ShowMSG(1,(int)"PicPusk config updated!");
    }
  }
  csm_result=old_icsm_onMessage(data, msg); //Вызываем старый обработчик событий  
  
  if (IsGuiOnTop(idlegui_id)) //Если IdleGui на самом верху
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
           if (BUT_ENA)
           {
             FSTATS fstats;
             unsigned int err;
             if (GetFileStats(pic_path,&fstats,&err)!=-1)
               {
                 DrawCanvas(canvasdata, but_X, but_Y, but_X + GetImgWidth((int)pic_path), but_Y + GetImgHeight((int)pic_path), 1);
	         DrawImg(but_X, but_Y, (int)pic_path);
               }
           }
           
           TDate date;
           TTime time;
           GetDateTime(&date, &time); 
           
	   char *mmonth[12]={"Янв","Фев","Мар","Апр","Май","Июн","Июл","Авг","Сен","Окт","Ноя","Дек"};
	   char *dday[7]={"Пн","Вт","Ср","Чт","Пт","Сб","Вс"};
           
           //char *mmonth[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	   //char *dday[7]={"Mon","Tue","Wen","Thu","Fri","Sat","Sun"};

           if (DATE_ENA)
           {           
            wsprintf(ws1, DATE_FMT, dday[GetWeek(&date)], date.day, mmonth[date.month-1]);
            DrawCanvas(canvasdata, date_X, date_Y, date_X+get_string_width(ws1, date_font), date_Y+GetFontYSIZE(date_font), 1); 
            switch (FRINGING_ENA)
              {
                case 0:
                  DrawString(ws1, date_X, date_Y, date_X+get_string_width(ws1, date_font), date_Y+GetFontYSIZE(date_font), date_font,0,GetPaletteAdrByColorIndex(color),GetPaletteAdrByColorIndex(23)); 
                break;
                case 1:
                  DrawString(ws1, date_X, date_Y, date_X+get_string_width(ws1, date_font), date_Y+GetFontYSIZE(date_font), date_font,0x20,GetPaletteAdrByColorIndex(color),GetPaletteAdrByColorIndex(FRINGING_color)); 
                break;
              }
           }
           
           if (TIME_ENA)
           {
            wsprintf(ws1, TIME_FMT, time.hour, time.min);
            DrawCanvas(canvasdata, time_X, time_Y, time_X+get_string_width(ws1, time_font), time_Y+GetFontYSIZE(time_font), 1); 
            switch (FRINGING_ENA)
              {
                case 0:
                  DrawString(ws1,time_X,time_Y,time_X+get_string_width(ws1, time_font), time_Y+GetFontYSIZE(time_font),time_font,0,GetPaletteAdrByColorIndex(color),GetPaletteAdrByColorIndex(23)); 
                break;
                case 1:
                  DrawString(ws1,time_X,time_Y,time_X+get_string_width(ws1, time_font), time_Y+GetFontYSIZE(time_font),time_font,0x20,GetPaletteAdrByColorIndex(color),GetPaletteAdrByColorIndex(FRINGING_color)); 
                break;
              } 
           }
           
                     
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
  FreeWS(ws1);   
  GBS_DelTimer(&mytmr);
  seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}

int main(void)
{
  InitConfig();
   
  ws1 = AllocWS(20);
  
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;
  icsm->constr=&icsmd;  
  TimerProc();


  return 0;
}
