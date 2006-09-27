#include "..\inc\swilib.h"

#define UPDATE_TIME (1*262)
#define WIDTH 50
#define HIGHT 20
#define X 131-50
#define Y 132

#define RGB8(R,G,B) (B+(G<<2)+(R<<5))

CSM_DESC icsmd;

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);

GBSTMR mytmr;

WSHDR *ws1;

unsigned char hhh;
char cop;

unsigned char img1_bmp[WIDTH*HIGHT-1];

const IMGHDR img1=
{
  WIDTH,
  HIGHT,
  5,
  0,
  (char *)img1_bmp
};

unsigned char loads[WIDTH-1];
unsigned char clocks[WIDTH-1];

DrwImg(IMGHDR *img, int x, int y, int *pen, int *brush)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc,x,y,img->w,img->h);
  SetPropTo_Obj5(&drwobj,&rc,0,img);
  SetColor(&drwobj,pen,brush);
  DrawObject(&drwobj);
}

void TimerProc(void)
{
  GBS_SendMessage(MMI_CEPID,0xDEAD,GetCPULoad());
  GBS_StartTimerProc(&mytmr,UPDATE_TIME,TimerProc);
}


int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
  int csm_result;
  unsigned int x;
#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])

  //Накапливаем значения
  if (msg->msg==0xDEAD)
  {
    LockSched();
    loads[hhh]=HIGHT*/*GetCPULoad()*/msg->submess/100;
    clocks[hhh]=GetCPUClock()/14;
    UnlockSched();
    hhh++;
    if (hhh>=WIDTH) {hhh=0; cop=1;}
    csm_result=0;
  }
  else
    csm_result=old_icsm_onMessage(data,msg); //Вызываем старый обработчик событий
  if (IsGuiOnTop(idlegui_id)&&IsKeybUnlock()) //Если IdleGui на самом верху
  {
    GUI *igui=GetTopGUI();
    if (igui) //И он существует
    {
      void *idata=GetDataOfItemByID(igui,2);
      if (idata)
      {
	void *canvasdata=((void **)idata)[DISPLACE_OF_IDLECANVAS/4];
	int y;
	DrawCanvas(canvasdata,X,Y,X+WIDTH,Y+HIGHT,1);
	//рисуем нашу требуху
	//     zeromem(img1_bmp,sizeof(img1_bmp));
	int h=hhh;
	for(x=0;x<WIDTH;x++){
	  for(y=0;y<HIGHT;y++){
	    if (y<loads[h]){
	      img1_bmp[x+(HIGHT-1-y)*WIDTH]=RGB8(clocks[h],0,0);
	    }else{
	      img1_bmp[x+(HIGHT-1-y)*WIDTH]=RGB8(6,0,0);
	    }
	  }
	  h++;
	  if (h>=WIDTH) h=0;
	}
	DrwImg((IMGHDR *)&img1,X,Y,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(1));
	if (cop==0)
	  DrawString(ws1,0,100,131,131,11,0x20,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(1));
      }
    }
  }
  return(csm_result);
}


int main(void)
{
  hhh=0;
  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onMessage=icsmd.onMessage;
  icsmd.onMessage=MyIDLECSM_onMessage;
  icsm->constr=&icsmd;
  ws1=AllocWS(100);
  wsprintf(ws1,"%t","CPUMon by BoBa, Rst7");
  GBS_StartTimerProc(&mytmr,UPDATE_TIME*10,TimerProc);
  UnlockSched();
  return 0;
}
