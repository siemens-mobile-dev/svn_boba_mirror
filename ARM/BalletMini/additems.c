#include "../inc/swilib.h"
#include "view.h"
#include "additems.h"
#include "readimg.h"
#include "string_works.h"
#include "siemens_unicode.h"


#define DP_IS_FRAME (-2)
#define DP_IS_NOINDEX (-1)
#define RAWTEXTCHUNK (16384)

unsigned int wchar_radio_on=0xFFFF;
unsigned int wchar_radio_off=0xFFFF;

static void RawInsertChar(VIEWDATA *vd, int wchar)
{
  if ((vd->rawtext_size%RAWTEXTCHUNK)==0)
  {
    //Дошли до конца куска, реаллоцируем еще кусок
    vd->rawtext=realloc(vd->rawtext,(vd->rawtext_size+RAWTEXTCHUNK)*2);
  }
  vd->rawtext[vd->rawtext_size++]=wchar;
}

void AddNewStyle(VIEWDATA *vd)
{
  RawInsertChar(vd,vd->current_tag_s.bold?UTF16_FONT_SMALL_BOLD:UTF16_FONT_SMALL);
  RawInsertChar(vd,vd->current_tag_s.underline?UTF16_ENA_UNDERLINE:UTF16_DIS_UNDERLINE);
  RawInsertChar(vd,UTF16_INK_RGBA);
  RawInsertChar(vd,(vd->current_tag_s.red<<11)+(vd->current_tag_s.green<<2));
  RawInsertChar(vd,(vd->current_tag_s.blue<<11)+100);
  RawInsertChar(vd,UTF16_PAPER_RGBA);
  RawInsertChar(vd,(vd->current_tag_d.red<<11)+(vd->current_tag_d.green<<2));
  RawInsertChar(vd,(vd->current_tag_d.blue<<11)+100);
}

void AddBeginRef(VIEWDATA *vd)
{
  vd->work_ref.begin=vd->rawtext_size;
  RawInsertChar(vd,UTF16_ENA_INVERT);
}

void AddEndRef(VIEWDATA *vd)
{
  REFCACHE *p=vd->ref_cache;
  RawInsertChar(vd,UTF16_DIS_INVERT);
  if (!p)
  {
    zeromem(p=malloc(sizeof(REFCACHE)),sizeof(REFCACHE));
    vd->ref_cache=p;
  }
  else
  {
    while(p->next) p=p->next;
    zeromem(p->next=malloc(sizeof(REFCACHE)),sizeof(REFCACHE));
    p=p->next;
  }
  p->begin=vd->work_ref.begin;
  p->end=vd->rawtext_size;
  p->tag=vd->work_ref.tag;
  p->form_id1=globalstr(vd->work_ref.form_id1);
  p->form_id2=globalstr(vd->work_ref.form_id2);
  p->id=globalstr(vd->work_ref.id);
  p->value=globalstr(vd->work_ref.value);
  p->id2=globalstr(vd->work_ref.id2);
  p->group_id=vd->work_ref.group_id;
  p->checked=vd->work_ref.checked;
  if (vd->pos_cur_ref==0xFFFFFFFF)
  {
    vd->pos_cur_ref=vd->work_ref.begin;
  }
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
  AddTextItem(vd," ",1);
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
    if ((img=read_pngimg(picture)))
    {
      dpl=AddToDPngQueue(vd, img, 0);
      wchar=dpl->w_char;
    }
    else  if ((img=read_jpgimg(picture)))
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

void AddPictureItemHr(VIEWDATA *vd)
{
  int wchar=0xE115;
  IMGHDR *img;
  OMS_DYNPNGLIST *dpl;
  if (!vd->wchar_hr)
  {
    img=CreateDelimiter(ScreenW()-1,3,GetPaletteAdrByColorIndex(1));
    if (img)
    {
      dpl=AddToDPngQueue(vd, img, DP_IS_NOINDEX);
      vd->wchar_hr=wchar=dpl->w_char;
    }
  }
  else wchar=vd->wchar_hr;
  RawInsertChar(vd,wchar);
}

void AddRadioButton(VIEWDATA *vd, int checked)
{
  RawInsertChar(vd,checked?RADIOB_CHECKED:RADIOB_UNCHECKED);
}

void AddCheckBoxItem(VIEWDATA *vd, int checked)
{
  RawInsertChar(vd,checked?CBOX_CHECKED:CBOX_UNCHECKED);
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
