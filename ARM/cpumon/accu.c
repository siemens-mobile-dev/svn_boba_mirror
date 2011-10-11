#include "..\inc\swilib.h"
#include "cpumon.h"
#include "conf_loader.h"

extern int const cfgCapaType;
extern int const cfgCapaDiv;
extern int const cfgCapaFont;
extern int const cfgCapaCorner;
extern const char cfgCapa[4];
extern int uiWidth, uiHeight;

static void init(int rewidth);
static void deinit();
static void tick();
static WSHDR *getText(const int,const int);
static int getColor(const int x,const int h);
static int getFont(const int x,const int h);
TSensor Capa_sensor={0,0,0,0,init,deinit,tick,0,getText,getColor,getFont};

static int Capa,oldCapa=0;
static WSHDR *ws=0;

static void init(int rewidth){
  if (cfgCapaType)
    Capa_sensor.type=3; 
   else
    Capa_sensor.type=0;
  Capa_sensor.corner=cfgCapaCorner;
  Capa_sensor.div=cfgCapaDiv;
  ws=AllocWS(10);
}

static void deinit(){
  FreeWS(ws);
}

static void tick(){
  Capa = NM_Get_IRE();
}

static WSHDR *getText(const int x,const int h){
  if(oldCapa!=Capa){
    wsprintf(ws,"%dmAh",Capa/10);
    oldCapa=Capa;
  }
  return ws;
}

static int getColor(const int x,const int h){
  return (int)cfgCapa;
}

static int getFont(const int x,const int h){
  return cfgCapaFont;
}
