#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"

extern int const cfgNetType;
extern unsigned int const cfgNetDiv;
extern const char cfgNet[4];
extern unsigned int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick(const unsigned int);
static unsigned int getValue(const unsigned int x,const unsigned int h);
static unsigned int getColor(const unsigned int x,const unsigned int h);
TSensor net_sensor={0,init,deinit,tick,getValue,getColor};

static RAMNET *ramnet;
static unsigned char* values;
static unsigned int nTicks,color,minp=0xff,maxp,avgp,lastp;

static void init(int rewidth){
  color = RGB16(cfgNet[0],cfgNet[1],cfgNet[2]);
  net_sensor.type=cfgNetType;
  if ((rewidth)||(values==0)){
    deinit();
    values = malloc(uiWidth);
    zeromem(values, uiWidth);
    ramnet=RamNet();
  }
}

static void deinit(){
  mfree(values);
  values=0;
}

static void tick(const unsigned int h){
  if (nTicks==0){
    unsigned int p = ramnet->power;
    if (p>maxp) { maxp=p; avgp=maxp-minp; }
    if (p<minp) { minp=p; avgp=maxp-minp; }
    lastp = uiHeight * (maxp-p) / avgp;
    nTicks=cfgNetDiv;
  }
  values[h]=lastp;
  nTicks--;
}

static unsigned int getValue(const unsigned int x,const unsigned int h){
  return values[h];
}

static unsigned int getColor(const unsigned int x,const unsigned int h){
  return color;
}
