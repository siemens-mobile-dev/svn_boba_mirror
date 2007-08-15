#include "../inc/swilib.h"
#include "view.h"
#include "additems.h"


#ifdef NEWSGOLD
#define CBOX_CHECKED 0xE116
#define CBOX_UNCHECKED 0xE117
#else
#define CBOX_CHECKED 0xE10B
#define CBOX_UNCHECKED 0xE10C
#endif

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
  RawInsertChar(vd,(vd->current_tag_s.red<<3)+(vd->current_tag_s.green<<10));
  RawInsertChar(vd,(vd->current_tag_s.blue<<11)+100);
  RawInsertChar(vd,0xE007);
  RawInsertChar(vd,(vd->current_tag_d.red<<3)+(vd->current_tag_d.green<<10));
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

void AddPictureItem(VIEWDATA *vd, void *picture)
{
  //Prepare Wide String
  RawInsertChar(vd,0xE115);
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
