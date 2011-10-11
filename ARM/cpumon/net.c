#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"
#include "render.h"

extern int const cfgNetType;
extern int const cfgNetDiv;
extern const char cfgNet900[4];
extern const char cfgNete900[4];
extern const char cfgNet1800[4];
extern int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick();
static int getValue(const int x,const int h);
static int getColor(const int x,const int h);
TSensor net_sensor={0,0,0,0,init,deinit,tick,getValue,0,getColor,0};

static RAMNET *ramnet;
static char *values,*bands;
static int colors[3],minp=0xff,maxp,avgp;

static void init(int rewidth){
  colors[0] = Color2ColorByRenderBit(&cfgNet900[0]);
  colors[1] = Color2ColorByRenderBit(&cfgNete900[0]);
  colors[2] = Color2ColorByRenderBit(&cfgNet1800[0]);
  net_sensor.type=cfgNetType;
  net_sensor.div=cfgNetDiv;
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

static void tick(){
  int p = ramnet->power;
  int h = net_sensor.hhh;
  if (p){
    if (p>maxp) { maxp=p; avgp=maxp-minp; }
    if (p<minp) { minp=p; avgp=maxp-minp; }
    values[h] = uiHeight * (maxp-p) / avgp;
  }else values[h]=0;
  bands[h] = ch2band(ramnet->ch_number);
  if(++h>=uiWidth) h=0;
  net_sensor.hhh=h;
}

static int getValue(const int x,const int h){
  return values[h];
}

static int getColor(const int x,const int h){
  return colors[bands[h]];
}
