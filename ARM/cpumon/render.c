//render распростран€етс€ под ∆оѕЋv8 и ниипет.
//вы видели код, теперь вы мне должны.

#include "conf_loader.h"
#include "cpumon.h"

extern TSensor Sensors[];
extern int nSensors;
extern unsigned int uiWidth, uiHeight;
extern unsigned int hhh;
extern unsigned short* img1_bmp;

/*static unsigned int GetMinSensor(const unsigned int x, const unsigned int h,
                          const unsigned int sY,unsigned int *minColor,unsigned int *minSolidColor){
  unsigned int v,y=0xffffffff,minYSC=0xffffffff;
  *minSolidColor=0xe000;
  for(unsigned int n=0; n<nSensors; n++)
  {
    v=Sensors[n].getValue(x,h);
    if (sY<=v)
    {
      if (y>v)
      {
        y=v;
        *minColor=Sensors[n].getColor(x,h);
      }
      if ((Sensors[n].type==2)&&(minYSC>v))
      {
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
*/
unsigned MidRGB(unsigned c1, unsigned c2)
{
  if(c1 == 0xFFFF) return c2;
  unsigned R =  ((((c1>>11)&0x1F)+((c2>>11)&0x1F))>>1)<<3;
  unsigned G =  ((((c1>>5)&0x3F)+((c2>>5)&0x3F))>>1)<<2;
  unsigned B =  (((c1&0x1F)+(c2&0x1F))>>1)<<3;
  return RGB16(R, G, B);
}

void render()
{
  unsigned int p=hhh;
  unsigned y1, y2, dy2, dx1, dx2;
  for(unsigned int x = 0; x < uiWidth; x++)
  {
    for(int y = 0; y < uiHeight; y++)
      img1_bmp[x + y * uiWidth] = 0xe000; //RGB16(0xff,0xff,0xff);
   for(unsigned int n=0; n<nSensors; n++)
    {
      unsigned y, ptr;
      switch(Sensors[n].type)
      {
        case 1:
         
          y2 = Sensors[n].getValue(x,p);
          if(!p)
            y1 = Sensors[n].getValue(uiWidth-1,uiWidth-1);
          else
            y1 = Sensors[n].getValue(x-1,p-1);
          if(y2>y1)
          {
            dy2 = (y2-y1)>>1;
            dx1 = y1;
            dx2 = y1 = y1+dy2; 
          }
          else
          {
            dy2 = (y1-y2)>>1;
            dx2 = y2;
            y2 = dx1 = y1-dy2;
          }
          if(x)
            while(dx1 <= y1)
            {
              ptr = x-1 + (uiHeight - dx1++) * uiWidth;
              if(!p)
                img1_bmp[ptr] = MidRGB(img1_bmp[ptr], Sensors[n].getColor(uiWidth-1,uiWidth-1));                 
              else 
                img1_bmp[ptr] = MidRGB(img1_bmp[ptr], Sensors[n].getColor(x-1,p-1)); 
            }
          while(dx2 <= y2)
          {
            ptr = x + (uiHeight - dx2++) * uiWidth;
            img1_bmp[ptr] = MidRGB(img1_bmp[ptr], Sensors[n].getColor(x,p)); 
          }
        break;
        case 2:
          y = (Sensors[n].getValue(x,p)>uiHeight)?uiHeight:Sensors[n].getValue(x,p);
          while(y)
          {
            ptr = x + (uiHeight - y--) * uiWidth;
            img1_bmp[ptr] = MidRGB(img1_bmp[ptr], Sensors[n].getColor(x,p)); 
          }
        break;
//        case 3:
//        break;
        default:
          img1_bmp[ptr]  = RGB16(0xff,0xff,0xff);
        break;
      }
    }
    if (++p >= uiWidth) p=0;
  }
  img1_bmp[0]=RGB16(0xff,0xff,0xff);
}
