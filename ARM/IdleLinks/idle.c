#include "..\inc\swilib.h"
#include "IdleLinks.h"
#include "conf_loader.h"

TLabelData LabelData[15];

extern unsigned long  strtoul (const char *nptr,char **endptr,int base);

int CalcPic(char* picture)
{
  int pic = strtoul(picture, 0, 10);
  if ( (pic <= 4) && (strlen(picture) > 1) )
  {
     pic = (int)picture;
  }
  return pic;
}

void Draw_DoLabel(int pos)
{
        switch (LabelData[pos-1].Type)
        {
        case 0:
          {
            WSHDR *elfname=AllocWS(256);
            wsprintf(elfname,LabelData[pos-1].FileName);
            ExecuteFile(elfname,NULL,NULL);
            FreeWS(elfname);
          }
          break;
        case 1:
          {
            unsigned int* addr = (unsigned int*)GetFunctionPointer(LabelData[pos-1].FileName);
            if (addr)
            {
              typedef void (*voidfunc)(); 
      #ifdef NEWSGOLD          
              voidfunc pp=(voidfunc)*(addr+4);
      #else 
              voidfunc pp=(voidfunc)addr; 
      #endif 
              SUBPROC((void*)pp);
            }
          }
          break;
        case 2:
          {
              typedef void (*voidfunc)(); 
              unsigned int addr=strtoul( LabelData[pos-1].FileName, 0, 16 );
              voidfunc pp=(voidfunc)addr;
              SUBPROC((void*)pp);
          }
          break;          
        }  
}

void Draw_NSD(int i)
{
  int pic;
  pic = CalcPic(LabelData[i].Pic);
  void *canvasdata = BuildCanvas();
  DrawCanvas(canvasdata,LabelData[i].x-1,
                          LabelData[i].y-1, 
                          LabelData[i].x+GetImgWidth(pic)+1, 
                          LabelData[i].y+GetImgHeight(pic)+1, 1); 
  DrawImg(LabelData[i].x,LabelData[i].y,pic);  
}

void Draw_GUI(int i, int pos)
{
  int pic;
  void *canvasdata = BuildCanvas();
  pic = CalcPic(LabelData[i].Pic);           
  if (!vybor)
  {                
    DrawCanvas(canvasdata,LabelData[i].x-1, LabelData[i].y-1,
                                LabelData[i].x+GetImgWidth(pic)+1,
                                LabelData[i].y+GetImgHeight(pic)+1, 1);
        
    if (i==pos-1)
    {
      DrawRoundedFrame(LabelData[pos-1].x-1, LabelData[pos-1].y-1,
                               LabelData[pos-1].x+GetImgWidth(pic)+1,
                               LabelData[pos-1].y+GetImgHeight(pic)+1,0,0,0,frcol,cl);                      
    }
  }
  else
    DrawCanvas(canvasdata,LabelData[i].x-1, LabelData[i].y-1,
                                  LabelData[i].x+GetImgWidth(pic)+1,
                                  LabelData[i].y+GetImgHeight(pic)+1, 1);  
      
  DrawImg(LabelData[i].x,LabelData[i].y,pic);    
}

void Draw_Select(int pos)
{
  int pic;
  pic = CalcPic(LabelData[pos-1].Pic); 
  DrawImg(LabelData[pos-1].x+GetImgWidth(pic)-GetImgWidth((int)chpic),
              LabelData[pos-1].y+GetImgHeight(pic)-GetImgHeight((int)chpic),(int)chpic);    
}

void RereadSettings(void)
{
  InitConfig();

  LabelData[0].Type=type1;
  LabelData[1].Type=type2;
  LabelData[2].Type=type3;
  LabelData[3].Type=type4;
  LabelData[4].Type=type5;
  LabelData[5].Type=type6;
  LabelData[6].Type=type7;
  LabelData[7].Type=type8;
  LabelData[8].Type=type9;
  LabelData[9].Type=type10;
  LabelData[10].Type=type11;
  LabelData[11].Type=type12;
  LabelData[12].Type=type13;
  LabelData[13].Type=type14;
  LabelData[14].Type=type15;
  
  LabelData[0].x=x1;   LabelData[0].y=y1;    LabelData[0].Pic=(char*)pic1;   LabelData[0].FileName=(char*)file1;
  LabelData[1].x=x2;   LabelData[1].y=y2;    LabelData[1].Pic=(char*)pic2;   LabelData[1].FileName=(char*)file2;
  LabelData[2].x=x3;   LabelData[2].y=y3;    LabelData[2].Pic=(char*)pic3;   LabelData[2].FileName=(char*)file3;
  LabelData[3].x=x4;   LabelData[3].y=y4;    LabelData[3].Pic=(char*)pic4;   LabelData[3].FileName=(char*)file4;
  LabelData[4].x=x5;   LabelData[4].y=y5;    LabelData[4].Pic=(char*)pic5;   LabelData[4].FileName=(char*)file5;
  LabelData[5].x=x6;   LabelData[5].y=y6;    LabelData[5].Pic=(char*)pic6;   LabelData[5].FileName=(char*)file6;
  LabelData[6].x=x7;   LabelData[6].y=y7;    LabelData[6].Pic=(char*)pic7;   LabelData[6].FileName=(char*)file7;
  LabelData[7].x=x8;   LabelData[7].y=y8;    LabelData[7].Pic=(char*)pic8;   LabelData[7].FileName=(char*)file8;
  LabelData[8].x=x9;   LabelData[8].y=y9;    LabelData[8].Pic=(char*)pic9;   LabelData[8].FileName=(char*)file9;
  LabelData[9].x=x10;  LabelData[9].y=y10;   LabelData[9].Pic=(char*)pic10;  LabelData[9].FileName=(char*)file10;
  LabelData[10].x=x11; LabelData[10].y=y11;  LabelData[10].Pic=(char*)pic11; LabelData[10].FileName=(char*)file11;
  LabelData[11].x=x12; LabelData[11].y=y12;  LabelData[11].Pic=(char*)pic12; LabelData[11].FileName=(char*)file12;
  LabelData[12].x=x13; LabelData[12].y=y13;  LabelData[12].Pic=(char*)pic13; LabelData[12].FileName=(char*)file13;
  LabelData[13].x=x14; LabelData[13].y=y14;  LabelData[13].Pic=(char*)pic14; LabelData[13].FileName=(char*)file14;
  LabelData[14].x=x15; LabelData[14].y=y15;  LabelData[14].Pic=(char*)pic15; LabelData[14].FileName=(char*)file15;  
}
