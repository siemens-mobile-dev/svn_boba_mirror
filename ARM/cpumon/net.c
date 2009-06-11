#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"

extern int const cfgNetType;
extern unsigned int const cfgNetDiv;
extern const char cfgNet900[4];
extern const char cfgNete900[4];
extern const char cfgNet1800[4];
extern unsigned int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick(const unsigned int);
static unsigned int getValue(const unsigned int x,const unsigned int h);
static unsigned int getColor(const unsigned int x,const unsigned int h);
TSensor net_sensor={0,init,deinit,tick,getValue,getColor};

static RAMNET *ramnet;
static unsigned char *values,*bands;
static unsigned int nTicks,colors[3],minp=0xff,maxp,avgp,lastp,lastb;

static void init(int rewidth){
  colors[0] = RGB16(cfgNet900[0],cfgNet900[1],cfgNet900[2]);
  colors[1] = RGB16(cfgNete900[0],cfgNete900[1],cfgNete900[2]);
  colors[2] = RGB16(cfgNet1800[0],cfgNet1800[1],cfgNet1800[2]);
  net_sensor.type=cfgNetType;
  if ((rewidth)||(values==0)){
    deinit();
    values = malloc(uiWidth);
    zeromem(values, uiWidth);
    bands = malloc(uiWidth);
    ramnet=RamNet();
  }
}

static void deinit(){
  mfree(values);
  values=0;
  mfree(bands);
  bands=0;
}

int ch2band(int ch){
  if (ch>=975)
    return 1;
  else if (ch>=512)
    return 2;
  else
    return 0;
}

static void tick(const unsigned int h){
  if (nTicks==0){
    unsigned int p = ramnet->power;
    if (p){
      if (p>maxp) { maxp=p; avgp=maxp-minp; }
      if (p<minp) { minp=p; avgp=maxp-minp; }
      lastp = uiHeight * (maxp-p) / avgp;
      lastb = ch2band(ramnet->ch_number);
    }else lastp=0;
    nTicks=cfgNetDiv;
  }
  values[h]=lastp;
  bands[h]=lastb;
  nTicks--;
}

static unsigned int getValue(const unsigned int x,const unsigned int h){
  return values[h];
}

static unsigned int getColor(const unsigned int x,const unsigned int h){
  return colors[bands[h]];
}
