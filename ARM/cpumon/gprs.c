#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"
#include "render.h"

extern int const cfgGPRSType;
extern int const cfgGPRSDiv;
extern const char cfgGPRS[4];
extern int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick();
static int getValue(const int x,const int h);
static int getColor(const int x,const int h);
TSensor GPRS_sensor={0,0,0,0,init,deinit,tick,getValue,0,getColor,0};

static char* values;
static int color,maxGPRS,prevGPRS;

static void init(int rewidth){
  color = Color2ColorByRenderBit(&cfgGPRS[0]);
  GPRS_sensor.type=cfgGPRSType;
  GPRS_sensor.div=cfgGPRSDiv;
  if ((rewidth)||(values==0)){
    deinit();
    values = malloc(uiWidth);
    zeromem(values, uiWidth);
    prevGPRS=*GetGPRSTrafficPointer();
  }
}

static void deinit(){
  mfree(values);
  values=0;
}

static void tick(){
  RefreshGPRSTraffic();
  int rr = *GetGPRSTrafficPointer();
  int r  = rr-prevGPRS;
  prevGPRS = rr;
  if (r>maxGPRS) maxGPRS = r;
  int h=GPRS_sensor.hhh;
  values[h] = uiHeight * r / maxGPRS;
  if(++h>=uiWidth) h=0;
  GPRS_sensor.hhh=h;
}

static int getValue(const int x,const int h){
  return values[h];
}

static int getColor(const int x,const int h){
  return color;
}
