//render распростран€етс€ под ∆оѕЋv8 и ниипет.
//вы видели код, теперь вы мне должны.

#include "conf_loader.h"
#include "cpumon.h"

extern TSensor Sensors[];
extern int nSensors;
extern unsigned int uiWidth, uiHeight;
extern unsigned int hhh;
extern unsigned short* img1_bmp;

static unsigned int GetMinSensor(const unsigned int x, const unsigned int h,
                          const unsigned int sY,unsigned int *minColor,unsigned int *minSolidColor){
  unsigned int v,y=0xffffffff,minYSC=0xffffffff;
  *minSolidColor=0xe000;
  for(unsigned int n=0; n<nSensors; n++){
    v=Sensors[n].getValue(x,h);
    if (sY<=v){
      if (y>v){
        y=v;
        *minColor=Sensors[n].getColor(x,h);
      }
      if ((Sensors[n].type==2)&&(minYSC>v)){
        minYSC=v;
        *minSolidColor=Sensors[n].getColor(x,h);
      }
    }
  }
  return y;
}

void render(){
  unsigned int h=hhh;
  for(unsigned int x = 0; x < uiWidth; x++){
    unsigned int y=1,yy,color,solidColor;
    while ((yy=GetMinSensor(x,h,y,&color,&solidColor))<=uiHeight){
      while(y<yy){
        img1_bmp[x + (uiHeight - y++) * uiWidth] = solidColor;
      }
      img1_bmp[x + (uiHeight - y++) * uiWidth] = color;
    }
    while(y<=uiHeight){
      img1_bmp[x + (uiHeight - y++) * uiWidth] = 0xe000;
    }
    if (++h >= uiWidth) h=0;
  }
  img1_bmp[0]=RGB16(0xff,0xff,0xff);
}
