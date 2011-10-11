#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"

extern int const cfgTempType;
extern int const cfgTempDiv;
extern int const cfgTempFont;
extern int const cfgTempCorner;
extern const char cfgTemp[4];
extern int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick();
static WSHDR *getText(const int,const int);
static int getColor(const int x,const int h);
static int getFont(const int x,const int h);
TSensor Temp_sensor={0,0,0,0,init,deinit,tick,0,getText,getColor,getFont};

static int Temp,oldTemp=0;
static WSHDR *ws=0;

static void init(int rewidth){
  if (cfgTempType)
    Temp_sensor.type=3; 
   else
    Temp_sensor.type=0;
  Temp_sensor.corner=cfgTempCorner;
  Temp_sensor.div=cfgTempDiv;
  ws=AllocWS(8);
}

static void deinit(){
  FreeWS(ws);
}

static void tick(){
  Temp = GetAkku(1,3);
}

static WSHDR *getText(const int x,const int h){
  if(oldTemp!=Temp){
    wsprintf(ws,"%d\xB0\x43",(Temp-0xAAA+5)/10);
    oldTemp=Temp;
  }
  return ws;
}

static int getColor(const int x,const int h){
  return (int)cfgTemp;
}

static int getFont(const int x,const int h){
  return cfgTempFont;
}
