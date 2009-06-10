#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"

extern int const cfgRamType;
extern unsigned int const cfgRamDiv;
extern const char cfgFreeRAM[4];
extern unsigned int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick(const unsigned int);
static unsigned int getValue(const unsigned int x,const unsigned int h);
static unsigned int getColor(const unsigned int x,const unsigned int h);
TSensor ram_sensor={0,init,deinit,tick,getValue,getColor};

static unsigned char* values;
static unsigned int nTicks,color,maxRAM,lastRAM;

static void init(int rewidth){
  color = RGB16(cfgFreeRAM[0],cfgFreeRAM[1],cfgFreeRAM[2]);
  ram_sensor.type=cfgRamType;
  if ((rewidth)||(values==0)){
    deinit();
    values = malloc(uiWidth);
    zeromem(values, uiWidth);
  }
}

static void deinit(){
  mfree(values);
  values=0;
}

static void tick(const unsigned int h){
  if (nTicks==0){
    unsigned int r = GetFreeRamAvail();
    if (r>maxRAM) maxRAM=r;
    lastRAM = uiHeight * r / maxRAM;
    nTicks=cfgRamDiv;
  }
  values[h]=lastRAM;
  nTicks--;
}

static unsigned int getValue(const unsigned int x,const unsigned int h){
  return values[h];
}

static unsigned int getColor(const unsigned int x,const unsigned int h){
  return color;
}
