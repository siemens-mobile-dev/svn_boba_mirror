#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"

extern int const cfgDiskType;
extern unsigned int const cfgDiskDiv;
extern const char cfgDisk[4];
extern unsigned int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick(const unsigned int);
static unsigned int getValue(const unsigned int x,const unsigned int h);
static unsigned int getColor(const unsigned int x,const unsigned int h);
TSensor disk_sensor={0,init,deinit,tick,getValue,getColor};

static int nTicks,color;
static char values[5];

static void init(int rewidth){
  color = RGB16(cfgDisk[0],cfgDisk[1],cfgDisk[2]);
  disk_sensor.type=cfgDiskType;
}

static void deinit(){
}

static void tick(const unsigned int h){
  if (nTicks==0){
    unsigned int err;
    for(int n=0; n<5; n++)
      values[n]=uiHeight*(GetFreeFlexSpace(n,&err)>>16)/(GetTotalFlexSpace(n,&err)>>16);
    nTicks=cfgDiskDiv;
  }
  nTicks--;
}

static unsigned int getValue(const unsigned int x,const unsigned int h){
  if (x>=20)
    return 0;
  else if (x<12)
    return values[x>>2];
  else if (x<16)
    return values[4];
  else 
    return 0;
}

static unsigned int getColor(const unsigned int x,const unsigned int h){
  return color;
}
