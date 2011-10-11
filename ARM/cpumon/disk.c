#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"
#include "render.h"

#ifdef NEWSGOLD
 #define nDISKS 5
#else
 #define nDISKS 1
#endif

extern int const cfgDiskType;
extern int const cfgDiskDiv;
extern const char cfgDisk[4];
extern int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick();
static int getValue(const int x,const int h);
static int getColor(const int x,const int h);
TSensor disk_sensor={0,0,0,0,init,deinit,tick,getValue,0,getColor,0};

static int color;
static char values[nDISKS];

static void init(int rewidth){
  color = Color2ColorByRenderBit(&cfgDisk[0]);
  disk_sensor.type=cfgDiskType;
  disk_sensor.div=cfgDiskDiv;
}

static void deinit(){
}

static void tick(){
  unsigned int err;
  for(int n=0; n<nDISKS; n++)
    values[n]=uiHeight*(GetFreeFlexSpace(n,&err)>>16)/(GetTotalFlexSpace(n,&err)>>16);
}

static int getValue(const int x,const int h){
#ifdef NEWSGOLD
  if (x>=20)
    return 0;
  else if (x<12)
    return values[x>>2];
  else if (x<16)
    return values[4];
  else 
    return 0;
#else
  if (x>=4)
    return 0;
  else
    return values[0];
#endif
  
}

static int getColor(const int x,const int h){
  return color;
}
