#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "structs.h"
#include "Externals.h"

extern ANIM_LIST anlst;

int c2i(char *prt)
{
  int res=0; while (prt[0]!=';') { res=res * 10 + (prt[0]-0x30); *prt++; } return res;
};

void c2anim(char *line)
{
  // Добавляем анимацию в список
  int x=0;
  int lblIndex=0;
  signed int newx=0;
  signed int newy=0;
  signed int nmx=0;
  signed int nmy=0;
  int nColor=0;
  int nColor2=0;
  int nFont=0;
  int nCT=0;
  int DrawWithNext=0;
  while (line[x]!=':')
  {
    lblIndex=(lblIndex*10)+(line[x]-0x30);
    x++;
  }
  x++;
  bool isMinus=false;
  if (line[x]=='-')
  {
    x++;
    isMinus=true;
  }
  while (line[x]!=':')
  {
    newx=(newx*10)+(line[x]-0x30);
    x++;
  }
  if (isMinus==true)
  {
    newx=-1*newx;
  }
  x++;
  isMinus=false;
  if (line[x]=='-')
  {
    x++;
    isMinus=true;
  }
  while (line[x]!=':')
  {
    newy=(newy*10)+(line[x]-0x30);
    x++;
  }
  if (isMinus==true)
  {
    newy=-1*newy;
  }
  x++;
  isMinus=false;
  if (line[x]=='-')
  {
    x++;
    nmx=-1;
    isMinus=true;
  }
  while (line[x]!=':')
  {
    nmx=(nmx*10)+(line[x]-0x30);
    x++;
  }
  if (isMinus==true)
  {
    nmx=-1*nmx;
  }
  x++;
  isMinus=false;
  if (line[x]=='-')
  {
    x++;
    isMinus=true;
  }
  while (line[x]!=':')
  {
    nmy=(nmy*10)+(line[x]-0x30);
    x++;
  }
  if (isMinus==true)
  {
    nmy=-1*nmy;
  }
  x++;
  while (line[x]!=':')
  {
    nColor=(nColor*10)+(line[x]-0x30);
    x++;
  }
  x++;
  while (line[x]!=':')
  {
    nColor2=(nColor2*10)+(line[x]-0x30);
    x++;
  }
  x++;
  while (line[x]!=':')
  {
    nFont=(nFont*10)+(line[x]-0x30);
    x++;
  }
  x++;
  while (line[x]!=':')
  {
    nCT=(nCT*10)+(line[x]-0x30);
    x++;
  }
  x++;
  while (line[x]!=';')
  {
    DrawWithNext=(DrawWithNext*10)+(line[x]-0x30);
    x++;
  }
  ANIM_ELEM *anel=new ANIM_ELEM;
  anel->lblID=lblIndex;
  anel->X=newx;
  anel->Y=newy;
  anel->maxX=nmx;
  anel->maxY=nmy;
  anel->Color=nColor;
  anel->Color2=nColor2;
  anel->Font=nFont;
  anel->CT=nCT;
  anel->DrawWithNext=DrawWithNext;
  ListElement_Add(anlst.lst,anel);
};

int h2i(char * h)
{
  char c;
  int res=0;
  while((*h++)!='X');
  do
  {
    c=*h++;
    if (c>0x60) c-=0x20; else c-=0x30;
    if (c>9) c-=7;
    res<<=4;
    res|=c;
  }
  while(*h!='\0' || *h!=0);
  return(res);
};
