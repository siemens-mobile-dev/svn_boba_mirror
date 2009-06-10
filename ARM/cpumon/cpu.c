#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"

extern int const cfgLoadType;
extern unsigned int const cfgCpuDiv;
extern const char cfgLoad208[4];
extern const char cfgLoad104[4];
extern const char cfgLoad52[4];
extern unsigned int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick(const unsigned int);
static unsigned int getValue(const unsigned int x,const unsigned int h);
static unsigned int getColor(const unsigned int x,const unsigned int h);

TSensor cpu_sensor={0,init,deinit,tick,getValue,getColor};

static unsigned char* loads;
static unsigned char* clocks;
static unsigned int color52,color104,color208;
static unsigned int nTicks,lastLoad,lastClock;

#define CLOCK_26  26
#define CLOCK_52  52
#define CLOCK_104 104
#define CLOCK_208 208

static unsigned int ClocksColour(unsigned int clock){
  switch (clock){
    case CLOCK_26:
    case CLOCK_52:
      return color52;
    case CLOCK_104:
      return color104;
    case CLOCK_208:
      return color208;
    default:
      return (/*RGB16(0, 0, 0)*/ 0xFFFF);
  }
}

static void init(int rewidth){
  color52 =(RGB16(cfgLoad52[0],cfgLoad52[1],cfgLoad52[2]));
  color104=(RGB16(cfgLoad104[0], cfgLoad104[1], cfgLoad104[2]));
  color208=(RGB16(cfgLoad208[0], cfgLoad208[1], cfgLoad208[2]));
  cpu_sensor.type=cfgLoadType;
  if ((rewidth)||(loads==0)){
    deinit();
    loads = malloc(uiWidth);
    zeromem(loads, uiWidth);
    clocks = malloc(uiWidth);
    zeromem(clocks, uiWidth);
  }
}

static void deinit(){
  mfree(loads);
  loads=0;
  mfree(clocks);
  clocks=0;
}

static void tick(const unsigned int h){
  if (nTicks==0){
#ifdef NEWSGOLD
    if (!getCpuUsedTime_if_ena()){
      StartCpuUsageCount();
    }
#endif
    lastLoad=uiHeight * GetCPULoad() / 100;
    lastClock=GetCPUClock();
    nTicks=cfgCpuDiv;
  }
  loads[h]=lastLoad;
  clocks[h]=lastClock;
  nTicks--;
}

static unsigned int getValue(const unsigned int x,const unsigned int h){
  return loads[h];
}

static unsigned int getColor(const unsigned int x,const unsigned int h){
  return ClocksColour(clocks[h]);
}
