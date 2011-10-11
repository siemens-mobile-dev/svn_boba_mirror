//render распростран€етс€ под ∆оѕЋv8 и ниипет.
//вы видели код, теперь вы мне должны.

#include "..\inc\swilib.h"
#include "conf_loader.h"
#include "cpumon.h"

extern TSensor *Sensors[];
extern int nSensors;
extern TSensor *TexSensors[];
extern int nTexSensors;
extern int uiWidth, uiHeight;
extern int uiBits;
extern const RECT position;
extern const int cfgRender;
extern const int cfgTextRender;
extern IMGHDR img1;
extern char* img1_bmp;

int Color2ColorByRenderBit(const char *color){
  switch (uiBits){
   case 0: return  RGB8 (color[0],color[1],color[2]);
   case 1: return  RGB16(color[0],color[1],color[2]);
   default: return RGB24(color[0],color[1],color[2],color[3]);
  }
}

static void SetColorByRenderBit(const int index,const int color){
  switch (uiBits){
   case 0:
     img1_bmp[index]=color;
     break;
   case 1:
     ((short *)img1_bmp)[index]=color;
     break;
   case 2:
     ((int *)img1_bmp)[index]=color;
     break;
  }
}

static int GetColorByRenderBit(const int index){
  switch (uiBits){
   case 0:
     return img1_bmp[index];
   case 1:
     return ((unsigned short *)img1_bmp)[index];
   case 2:
  default:
     return ((int *)img1_bmp)[index];
  }
}

int min(int a,int b){
  if (a<b) return a; else return b;
}

int min3(int a,int b,int c){
  int d=min(a,b);
  if (c<d) return c; else return d;
}

int max(int a,int b){
  if (a>b) return a; else return b;
}

int max3(int a,int b,int c){
  int d=max(a,b);
  if (c>d) return c; else return d;
}

static int RenderBit2Transparent[3]={0xc0,0xe000,0x0};
static int hhh[NSENSORS];

static int GetMinSensor(const int x, const int sY,
                        int *minColor, int *minSolidColor){
  int minY=0x7fffffff,minYSC=0x7fffffff;
  *minSolidColor=RenderBit2Transparent[uiBits];
  for(int n=0; n<nSensors; n++){
    int h=hhh[n];
    int t=Sensors[n]->type;
    int v=Sensors[n]->getValue(x,h);
    if (t==2){  //line
      int v1,v2,minv,maxv;
      if (x>0){
        v1=(Sensors[n]->getValue(x-1, h==0 ? uiWidth-1 : h-1)+v)/2;
      }else v1=v;
      if (x<uiWidth-1){
        v2=(Sensors[n]->getValue(x+1, (h>=uiWidth-1) ? 0 : h+1)+v)/2;
      }else v2=v;
      minv=min3(v1,v,v2);
      if (sY<minv){
        t=1;  //doted
        v=minv;
      }else{
        maxv=max3(v1-1,v,v2-1);
        v=maxv;
        if (sY<maxv){
          t=3;  //solid
        }else{
          t=1;  //doted
        }
      }
    }
    if (sY<v){
      if (v<minY){
        minY=v;
        *minColor=Sensors[n]->getColor(x,h);
      }
      if ((t==3)&&(v<minYSC)){
        minYSC=v;
        *minSolidColor=Sensors[n]->getColor(x,h);
      }
    }
  }
  return minY;
}

void render_BoBa(){
  for(int n=0; n<nSensors; n++){
    hhh[n]=Sensors[n]->hhh;
  }
  for(int x = 0; x < uiWidth; x++){
    int y=1,yy,color,solidColor;
    while ((yy=GetMinSensor(x,y,&color,&solidColor))<=uiHeight){
      while(y<yy){
        SetColorByRenderBit(x + (uiHeight - y++) * uiWidth, solidColor);
      }
      SetColorByRenderBit(x + (uiHeight - y++) * uiWidth, color);
    }
    while(y<=uiHeight){
      SetColorByRenderBit(x + (uiHeight - y++) * uiWidth, RenderBit2Transparent[uiBits]);
    }
    for(int n=0; n<nSensors; n++){
      if(++hhh[n]>=uiWidth) hhh[n]=0;
    }
  }
}

int MidRGB(int c1, int c2)
{
  if(c1 == RenderBit2Transparent[uiBits]) return c2;
  int R =  ((((c1>>11)&0x1F)+((c2>>11)&0x1F))>>1)<<3;
  int G =  ((((c1>>5)&0x3F)+((c2>>5)&0x3F))>>1)<<2;
  int B =  (((c1&0x1F)+(c2&0x1F))>>1)<<3;
  return RGB16(R, G, B);
}

void render_Sinclair()
{
  int y1, y2, dy2, dx1, dx2;
  for(int n=0; n<nSensors; n++){
    hhh[n]=Sensors[n]->hhh;
  }
  for(int x = 0; x < uiWidth; x++)
  {
   for(int y = 0; y < uiHeight; y++)
    SetColorByRenderBit(x + y * uiWidth,RenderBit2Transparent[uiBits]);
   for(int n=0; n<nSensors; n++)
    {
      int p=hhh[n];
      int y, ptr;
      switch(Sensors[n]->type)
      {
        case 1:
        case 2:
          y2 = Sensors[n]->getValue(x,p);
          if(!p)
            y1 = Sensors[n]->getValue(uiWidth-1,uiWidth-1);
          else
            y1 = Sensors[n]->getValue(x-1,p-1);
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
                SetColorByRenderBit(ptr, MidRGB(GetColorByRenderBit(ptr), Sensors[n]->getColor(uiWidth-1,uiWidth-1)));
              else 
                SetColorByRenderBit(ptr, MidRGB(GetColorByRenderBit(ptr), Sensors[n]->getColor(x-1,p-1)));
            }
          while(dx2 <= y2)
          {
            ptr = x + (uiHeight - dx2++) * uiWidth;
            SetColorByRenderBit(ptr, MidRGB(GetColorByRenderBit(ptr), Sensors[n]->getColor(x,p)));
          }
        break;
        case 3:
          y = (Sensors[n]->getValue(x,p)>uiHeight)?uiHeight:Sensors[n]->getValue(x,p);
          while(y)
          {
            ptr = x + (uiHeight - y--) * uiWidth;
            SetColorByRenderBit(ptr, MidRGB(GetColorByRenderBit(ptr), Sensors[n]->getColor(x,p)));
          }
        break;
        default:
          SetColorByRenderBit(ptr,0xffffff);
        break;
      }
    }
    for(int n=0; n<nSensors; n++){
      if(++hhh[n]>=uiWidth) hhh[n]=0;
    }
  }
  SetColorByRenderBit(0,0xffffff);
}

void textrender(){
  int y[4];
  y[0]=position.y;
  y[1]=position.y;
  y[2]=position.y2;
  y[3]=position.y2;
  for(int n=0; n<nTexSensors; n++){
    int c=TexSensors[n]->getColor(0,0);
    int corner=TexSensors[n]->corner;
    int f=TexSensors[n]->getFont(0,0);
    int yy=GetFontYSIZE(f);
    int p=y[corner];
    y[corner] += corner<2 ? yy : -yy;
    DrawString(TexSensors[n]->getText(0,0),
               position.x,
               (corner<2 ? p : p-yy),
               position.x2,
               (corner<2 ? p+yy : p),
               f,
               (corner==0||corner==2) ? TEXT_ALIGNLEFT : TEXT_ALIGNRIGHT,
               (char *)c,
               0);
  }
}

void render(){
  if (cfgTextRender) textrender(); 

  if (!cfgRender) 
    render_BoBa(); 
  else {
    render_Sinclair();
  }
          
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc, position.x, position.y, img1.w, img1.h);
  SetPropTo_Obj5(&drwobj, &rc, 0, &img1);
  SetColor(&drwobj, 0, 0);
  DrawObject(&drwobj);

  if (!cfgTextRender) textrender(); 
}
