#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"
#include "render.h"

extern int const cfgLoadType;
extern int const cfgCpuDiv;
extern const char cfgLoad208[4];
extern const char cfgLoad104[4];
extern const char cfgLoad52[4];
extern int uiWidth, uiHeight;

static void init(const int);
static void deinit();
static void tick();
static int getValue(const int,const int);
static int getColor(const int,const int);
TSensor cpu_sensor={0,0,0,0,init,deinit,tick,getValue,0,getColor,0};

static char* loads;
static unsigned char* clocks;
static int color52,color104,color208;

#define CLOCK_26  26
#define CLOCK_52  52
#define CLOCK_104 104
#define CLOCK_208 208

static int ClocksColour(int clock){
  switch (clock){
    case CLOCK_208:
      return color208;
    case CLOCK_104:
      return color104;
    default:
      return color52;
  }
}

static void init(int rewidth){
  color52 =Color2ColorByRenderBit(&cfgLoad52[0]);
  color104=Color2ColorByRenderBit(&cfgLoad104[0]);
  color208=Color2ColorByRenderBit(&cfgLoad208[0]);
  cpu_sensor.type=cfgLoadType;
  cpu_sensor.div=cfgCpuDiv;
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
  mfree(clocks);
  loads=0;
  clocks=0;
}

static void tick(){
#ifdef NEWSGOLD
  if (!getCpuUsedTime_if_ena()){
    StartCpuUsageCount();
  }
#endif
  int h=cpu_sensor.hhh;
  loads[h]=uiHeight * GetCPULoad() / 100;
  clocks[h]=GetCPUClock();
  if(++h>=uiWidth) h=0;
  cpu_sensor.hhh=h;
}

static int getValue(const int x,const int h){
  return loads[h];
}

static int getColor(const int x,const int h){
  return ClocksColour(clocks[h]);
}
