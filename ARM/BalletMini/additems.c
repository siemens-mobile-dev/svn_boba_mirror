#include "../inc/swilib.h"
#include "view.h"
#include "additems.h"
#include "readpng.h"


#ifdef NEWSGOLD
#define CBOX_CHECKED 0xE116
#define CBOX_UNCHECKED 0xE117
#else
#define CBOX_CHECKED 0xE10B
#define CBOX_UNCHECKED 0xE10C
#endif

#define DP_IS_FRAME (-2)
#define DP_IS_NOINDEX (-1)
#define RAWTEXTCHUNK (16384)

static void RawInsertChar(VIEWDATA *vd, int wchar)
{
  if ((vd->rawtext_size%RAWTEXTCHUNK)==0)
  {
    //Дошли до конца куска, реаллоцируем еще кусок
    vd->rawtext=realloc(vd->rawtext,(vd->rawtext_size+RAWTEXTCHUNK)*2);
  }
  vd->rawtext[vd->rawtext_size++]=wchar;
}



//E001-E002 - underline on/off
//E003-E004 - invert on/off

//E006 - color RGB
//E007 - paper RGB
//E008 - color ID
//E009 - paper ID

//E012 - normal font
//E013 - bold font

//E01C/E01D - left/right align
//E01E/E01F - center off/on

void AddNewStyle(VIEWDATA *vd)
{
  RawInsertChar(vd,vd->current_tag_s.bold?0xE013:0xE012);
  RawInsertChar(vd,vd->current_tag_s.underline?0xE001:0xE002);
//  RawInsertChar(vd,vd->current_tag_s.center?0xE01F:0xE01E);
//  RawInsertChar(vd,vd->current_tag_s.right?0xE01D:0xE01C);
  RawInsertChar(vd,0xE006);
  RawInsertChar(vd,(vd->current_tag_s.red<<11)+(vd->current_tag_s.green<<2));
  RawInsertChar(vd,(vd->current_tag_s.blue<<11)+100);
  RawInsertChar(vd,0xE007);
  RawInsertChar(vd,(vd->current_tag_d.red<<11)+(vd->current_tag_d.green<<2));
  RawInsertChar(vd,(vd->current_tag_d.blue<<11)+100);
/*  SetPenColorToEditCOptions(&ec_options,1);
  ec_options->pen[0]=vd->current_tag_s.red;
  ec_options->pen[1]=vd->current_tag_s.green;
  ec_options->pen[2]=vd->current_tag_s.blue;
  ec_options->pen[3]=0;

  ec_options->brush[0]=vd->current_tag_d.red;
  ec_options->brush[1]=vd->current_tag_d.green;
  ec_options->brush[2]=vd->current_tag_d.blue;
  ec_options->brush[3]=0;*/
}

void AddBeginRef(VIEWDATA *vd)
{
  RawInsertChar(vd,0xE000);
}

void AddEndRef(VIEWDATA *vd)
{
  RawInsertChar(vd,0xE000);
}

void AddTextItem(VIEWDATA *vd, const char *text, int len)
{
  int c;
  while((len--)>0)
  {
    c=*text++;
    if ((c&0xE0)==0xC0)
    {
      if (len>0)
      {
	c&=0x1F;
	c<<=6;
	c|=(*text++)&0x3F;
	len-=1;
      }
    }
    else
      if ((c&0xF0)==0xE0)
      {
	if (len>1)
	{
	  c&=0x0F;
	  c<<=12;
	  c|=((*text++)&0x3F)<<6;
	  c|=((*text++)&0x3F)<<0;
	  len-=2;
	}
      }
    RawInsertChar(vd,c);
  }
}

void AddBrItem(VIEWDATA *vd)
{
  AddTextItem(vd,"\n",1);
}

void AddPItem(VIEWDATA *vd)
{
  AddTextItem(vd,"\n ",2);
}

void AddPictureItemIndex(VIEWDATA *vd, int index)
{
  int w_char=0xE115;
  OMS_DYNPNGLIST *dpl=vd->dynpng_list;
  while(dpl)
  {
    if (dpl->index==index) 
    {
      w_char=dpl->w_char;
      break;
    }
    dpl=dpl->dp.next;
  }
  RawInsertChar(vd,w_char);  
}


//is_index >= 0 использовать последний
//is_index <0 - задать  принудительно
OMS_DYNPNGLIST *AddToDPngQueue(VIEWDATA *vd, IMGHDR *img, int is_index)
{
  int wchar, i, index;
  OMS_DYNPNGLIST *dpl;
  OMS_DYNPNGLIST *odp=malloc(sizeof(OMS_DYNPNGLIST));
  odp->dp.next=0;
  odp->dp.img=img;
  dpl=vd->dynpng_list;
  if (!dpl)
  {
    odp->dp.icon=GetPicNByUnicodeSymbol((wchar=FIRST_UCS2_BITMAP));
    odp->w_char=wchar;
    if (is_index>=0)  odp->index=0;
    else odp->index=is_index;
    LockSched();
    vd->dynpng_list=odp;
    UnlockSched();
    RefreshGUI();
  }
  else
  {
    i=0;
    index=0;
    OMS_DYNPNGLIST *d;
    do
    {
      d=dpl;
      if (is_index>=0 && d->index>=0) index++;
      i++;
    }
    while((dpl=dpl->dp.next));
    odp->dp.icon=GetPicNByUnicodeSymbol((wchar=FIRST_UCS2_BITMAP+i));
    odp->w_char=wchar;
    if (is_index>=0)  odp->index=index;
    else odp->index=is_index;
    LockSched();
    d->dp.next=odp;
    UnlockSched();
  }
  return odp;  
}

void AddPictureItem(VIEWDATA *vd, void *picture)
{
  int wchar=0xE115;
  IMGHDR *img;
  OMS_DYNPNGLIST *dpl;
  if (picture)
  {
    img=read_pngimg(picture);
    if (img)
    {
      dpl=AddToDPngQueue(vd, img, 0);
      wchar=dpl->w_char;
    }
  }
  //Prepare Wide String
  RawInsertChar(vd,wchar);
}

void AddPictureItemRGBA(VIEWDATA *vd, void *picture, int width, int height)
{
  int wchar=0xE115;
  IMGHDR *img;
  OMS_DYNPNGLIST *dpl;
  if (picture)
  {
    img=ConvertRGBAToRGB8(picture,width,height);
    if (img)
    {
      dpl=AddToDPngQueue(vd, img, DP_IS_NOINDEX);
      wchar=dpl->w_char;
    }
  }
  //Prepare Wide String
  RawInsertChar(vd,wchar);  
}

OMS_DYNPNGLIST *FindOmsFrameBySize(VIEWDATA *vd,int width,int height)
{
  IMGHDR *img;
  OMS_DYNPNGLIST *dpl=vd->dynpng_list;
  while(dpl)
  {
    if (dpl->index==DP_IS_FRAME)
    {
      if ((img=dpl->dp.img))
      {
        if (img->w==width && img->h==height) return (dpl);
      }
    }
    dpl=dpl->dp.next;
  }
  return (dpl);
}

void AddPictureItemFrame(VIEWDATA *vd,int width,int height)
{
  int wchar=0xE115;
  IMGHDR *img;
  OMS_DYNPNGLIST *dpl;
  dpl=FindOmsFrameBySize(vd,width,height);
  if (dpl) wchar=dpl->w_char;
  else
  {
    img=CreateFrame(width,height,GetPaletteAdrByColorIndex(3));
    if (img)
    {
      dpl=AddToDPngQueue(vd, img, DP_IS_FRAME);
      wchar=dpl->w_char;
    }
  }
  RawInsertChar(vd,wchar);  
}

void AddRadioButton(VIEWDATA *vd)
{
  RawInsertChar(vd,CBOX_CHECKED);
}

void AddCheckBoxItem(VIEWDATA *vd)
{
  RawInsertChar(vd,CBOX_UNCHECKED);
}

void AddInputItem(VIEWDATA *vd, const char *text, int len)
{
  RawInsertChar(vd,10);
  RawInsertChar(vd,0xE11F);
  AddTextItem(vd,text,len);
  RawInsertChar(vd,10);
}
  
void AddPassInputItem(VIEWDATA *vd, const char *text, int len)
{
  RawInsertChar(vd,10);
  RawInsertChar(vd,0xE11F);
  AddTextItem(vd,"****",4);
  RawInsertChar(vd,10);
}

void AddButtonItem(VIEWDATA *vd, const char *text, int len)
{
  RawInsertChar(vd,'[');
  AddTextItem(vd,text,len);
  RawInsertChar(vd,']');
}

void AddSelectItem(VIEWDATA *vd)
{
}
