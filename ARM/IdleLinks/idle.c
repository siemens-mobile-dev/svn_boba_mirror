#include "..\inc\swilib.h"
#include "IdleLinks.h"
#include "conf_loader.h"
#include "utils.h"
#include "navigation.h"

short *ScreenBuff;

// «десь лежат все функции по рисованию на идле...

extern unsigned long  strtoul (const char *nptr,char **endptr,int base);

extern const char cl[4];
extern const char chpic[128];
extern const char frcol[4];

// Twitch - делаем подписи к €рлычкам
extern const unsigned int linkname_x;
extern const unsigned int linkname_y;
extern const unsigned int linkname_ena;
extern const unsigned int linkname_wdraw;
extern const unsigned int linkname_al;
extern const char LN_COLOR[4];
extern const unsigned int LN_FONT;
extern const unsigned int LN_SEL_FONT;
extern const int LN_FRING;
extern const char LN_FRING_COLOR[4];
extern const unsigned int LN_SHIFT;
const char perc_t[2]="%t";
/////////////////////////////////////////////

int CalcPic(char* picture)
{
  if(file_exists(picture))
  {
    return (int)picture;
  }
  else
  {
    return strtoul(picture, 0, 10);
  }
}

void Draw_DoLabel(TLabelData *Link)
{
  switch (Link->Type)
  {
  case 0:
    {
      WSHDR *elfname=AllocWS(strlen(Link->FileName) +1);
      wsprintf(elfname,Link->FileName);
      ExecuteFile(elfname,NULL,NULL);
      FreeWS(elfname);
    }
    break;
  case 1:
    {
      unsigned int* addr = (unsigned int*)GetFunctionPointer(Link->FileName);
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
      unsigned int addr=strtoul(Link->FileName, 0, 16 );
      voidfunc pp=(voidfunc)addr;
      SUBPROC((void*)pp);
    }
    break;          
  }  
}

// Twitch - делаем подписи к €рлычкам
char symb[1]=" "; // дл€ Get_WS_width >_<

void Draw_LinkNameIfActive (TLabelData *link)
{
  WSHDR *ws = AllocWS(128);
  int i = 0,tattr=1;
  char *tempstr;
  int size=strlen(link->LinkName) + 1;
  tempstr = malloc(size);
  
  sprintf(tempstr, link->LinkName);
  del_ext(tempstr);
  strcat(tempstr,symb);

  str_2ws(ws,tempstr,size);
    
  i = Get_WS_width(ws,LN_SEL_FONT);

  tattr+=(linkname_al == 2) ? 3 : linkname_al;
  tattr+=(LN_FRING) ? 32 : 0;
  
  DrawString(ws,linkname_x,linkname_y,linkname_x+i,linkname_y+GetFontYSIZE(LN_SEL_FONT),LN_SEL_FONT,tattr,LN_COLOR,LN_FRING_COLOR);
  
  FreeWS(ws);
}

void Draw_LinkNameForEach (TLabelData *link, int ena)
{
  WSHDR *ws = AllocWS(128);
  int i = 0, tattr=2, font, x, y;
  int pic=CalcPic(link->Pic);
  char *tempstr;
  int size=strlen(link->LinkName) + 1;
  tempstr = malloc(size);
  
  strcpy(tempstr,link->LinkName);
  del_ext(tempstr);
  strcat(tempstr,symb);
  
  font = (ena) ? LN_SEL_FONT : LN_FONT;

  str_2ws(ws,tempstr,size);

  i = Get_WS_width(ws,font);

  x = link->x+(GetImgWidth(pic)/2)-i/2;
  y = (linkname_wdraw) ? link->y-LN_SHIFT-GetFontYSIZE(font) : link->y+GetImgHeight(pic)+LN_SHIFT;

  tattr+=(LN_FRING) ? 32 : 0;
  
  DrawString(ws,x,y,x+i,y+GetFontYSIZE(font),font,tattr,LN_COLOR,LN_FRING_COLOR);
  
  FreeWS(ws);
}
//////////////////////////////////////////

void Draw_NSD(TLabelData *Label)
{
  if (Label->Enabled)
  {
    int pic;
    pic = CalcPic(Label->Pic);
    void *canvasdata = BuildCanvas();
    DrawCanvas(canvasdata,Label->x-1,
                          Label->y-1, 
                          Label->x+GetImgWidth(pic)+1, 
                          Label->y+GetImgHeight(pic)+1, 1); 
    DrawImg(Label->x,Label->y,pic);
    if (linkname_ena == 1)		// Twitch - делаем подписи к €рлычкам
      Draw_LinkNameForEach (Label,0);	//
  }
}


void Draw_ScreenShot()
{ 

#ifdef LOG
  log("idle.c", "Draw screenshot", (char*)ScreenBuff);
#endif
  IMGHDR img={ScreenW(),ScreenH(),8 ,(char*)ScreenBuff};
  RECT rc;
  DRWOBJ drwobj;
  
  StoreXYWHtoRECT(&rc,0,0,ScreenW(),ScreenH());
  SetProp2ImageOrCanvas(&drwobj,&rc,0,&img,0,0);
  DrawObject(&drwobj);
}

void Draw_GUI(TLabelData *Label)
{
  //Draw_ScreenShot();
  if (Label->Enabled)
  {  
    int pic;
    pic = CalcPic(Label->Pic);     
    DrawImg(Label->x,Label->y,pic); 
  }

//    DrawCanvas(canvasdata,Label->x-1, Label->y-1,
//               Label->x+GetImgWidth(pic)+1,
//                                  Label->y+GetImgHeight(pic)+1, 1);  
     
}

void Draw_Select(TLabelData *Selected)
{
  if (Selected->Enabled)
  {
    extern const int BCFG_SELECT_TYPE;
    int pic;
    pic = CalcPic(Selected->Pic); 
    if (!BCFG_SELECT_TYPE)
    {
      extern const unsigned int BCFG_ROUND_X;
      extern const unsigned int BCFG_ROUND_Y;
      DrawRoundedFrame(Selected->x-1, Selected->y-1,
                               Selected->x+GetImgWidth(pic)+1,
                               Selected->y+GetImgHeight(pic)+1,BCFG_ROUND_X,BCFG_ROUND_Y,0,frcol,cl); 
      DrawImg(Selected->x,Selected->y,pic); 
    }
    else
    {
      DrawImg(Selected->x+GetImgWidth(pic)-GetImgWidth((int)chpic),
              Selected->y+GetImgHeight(pic)-GetImgHeight((int)chpic),(int)chpic);    
    }
    if (linkname_ena == 2)		// Twitch - делаем подписи к €рлычкам
      Draw_LinkNameIfActive (Selected);	//
    else if (linkname_ena == 1)		//
      Draw_LinkNameForEach (Selected,1);//
  }
}

