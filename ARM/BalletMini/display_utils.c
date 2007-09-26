#include "../inc/swilib.h"
#include "view.h"
#include "display_utils.h"
#include "siemens_unicode.h"
#include "string_works.h"

int GetFontHeight(int font, int atribute)
{
  int height=GetFontYSIZE(font);
  if (atribute&TEXT_INVERT)   height+=1;
  if (atribute&TEXT_UNDERLINE)   height+=1;
  if (atribute&TEXT_OUTLINE)  height+=2;
  return height;
}

unsigned int SearchNextDisplayLine(VIEWDATA *vd, LINECACHE *p, unsigned int *max_h)
{
  int left=ScreenW();
  int c;
  int h;
  unsigned int pos=p->pos;
  while(pos<vd->rawtext_size)
  {
    c=vd->rawtext[pos++];
    if ((c&0xFF00)==0xE100)
    {
      h=GetImgHeight(GetPicNByUnicodeSymbol(c));
    }
    else
    {
      h=GetFontHeight(p->bold?FONT_SMALL_BOLD:FONT_SMALL,p->underline?TEXT_UNDERLINE:0+p->ref?TEXT_INVERT:0);
    }
    if (max_h)
    {
      if (h>*max_h) *max_h=h;
    }
    switch(c)
    {
    case 0x0A:
    case 0x0D:
      return(pos);
    case UTF16_DIS_INVERT:
      p->ref=0;
      continue;
    case UTF16_ENA_INVERT:
      p->ref=1;
      continue;
    case UTF16_DIS_UNDERLINE:
      p->underline=0;
      continue;
    case UTF16_ENA_UNDERLINE:
      p->underline=1;
      continue;
    case UTF16_FONT_SMALL:
      p->bold=0;
      continue;
    case UTF16_FONT_SMALL_BOLD:
      p->bold=1;
      continue;
    case UTF16_INK_RGBA:
      if (pos>(vd->rawtext_size-2)) goto LERR;
      p->ink1=vd->rawtext[pos++];
      p->ink2=vd->rawtext[pos++];
      continue;
    case UTF16_PAPER_RGBA:
      if (pos>(vd->rawtext_size-2)) goto LERR;
      p->paper1=vd->rawtext[pos++];
      p->paper2=vd->rawtext[pos++];
      continue;
    case UTF16_INK_INDEX:
    case UTF16_PAPER_INDEX:
      pos++;
      if (pos>=vd->rawtext_size) goto LERR;
      continue;
    }
    left-=GetSymbolWidth(c,p->bold?FONT_SMALL_BOLD:FONT_SMALL);
    if (left<0)
    {
      return pos-1;
    }
  }
LERR:
  return(vd->rawtext_size);
}

#define LINESCACHECHUNK 256

static void AddLineToCache(VIEWDATA *vd, LINECACHE *p)
{
  if ((vd->lines_cache_size%LINESCACHECHUNK)==0)
  {
    //Дошли до конца куска, реаллоцируем еще кусок
    vd->lines_cache=realloc(vd->lines_cache,(vd->lines_cache_size+LINESCACHECHUNK)*sizeof(LINECACHE));
  }
  memcpy(vd->lines_cache+(vd->lines_cache_size++),p,sizeof(LINECACHE));
}

int LineDown(VIEWDATA *vd)
{
  LINECACHE lc;
  unsigned int h;
  unsigned int pos=vd->view_pos;
  if (pos>=vd->rawtext_size) return 0;
  if (vd->view_line>=vd->lines_cache_size)
  {
    lc.ink1=0x0000;
    lc.ink2=0x0064;
    lc.paper1=0xFFFF;
    lc.paper2=0xFF64;
    lc.pixheight=0;
    lc.bold=0;
    lc.underline=0;
    lc.ref=0;
    lc.pos=pos;
    AddLineToCache(vd,&lc);
  }
  else
  {
    memcpy(&lc,vd->lines_cache+vd->view_line,sizeof(lc));
  }
  h=0;
  pos=SearchNextDisplayLine(vd,&lc,&h);
  (vd->lines_cache+vd->view_line)->pixheight=h;
  
  if (pos>=vd->rawtext_size) return 0;
  vd->view_pos=pos;
  lc.pos=pos;
  vd->view_line++;
  if (vd->view_line>=vd->lines_cache_size)
  {
    AddLineToCache(vd,&lc);
  }
  return 1;
}

int LineUp(VIEWDATA *vd)
{
  int vl=vd->view_line;
  if (vl)
  {
    vl--;
    vd->view_pos=vd->lines_cache[vl].pos;
    vd->view_line=vl;
    return 1;
  }
  else
    return 0;
}

#define MAX_COLORS_IN_LINE 32
typedef struct {
  char color[4];
  int start_x;
  int end_x;
} COLOR_RC;

int RenderPage(VIEWDATA *vd, int do_draw)
{
  int scr_w=ScreenW()-1;
  int scr_h=ScreenH()-1;
  int sc;
  int dc;
//  VIEWDATA *vd=data->vd;
  WSHDR *ws=vd->ws;
  LINECACHE *lc;
  unsigned int vl;
  int ypos=YDISP;
  unsigned int store_pos=vd->view_pos;
  unsigned int store_line=vl=vd->view_line;
  unsigned int len;
  unsigned int y2;
  
  char def_ink[4];
  COLOR_RC rc[MAX_COLORS_IN_LINE];
  int cur_rc;
  int ws_width;
  
  int c;
  int ena_ref=0;
  
  int result=1;

  unsigned int _ref=0xFFFFFFFF;
  unsigned int flag=0;
  
  vd->pos_first_ref=0xFFFFFFFF;
  vd->pos_last_ref=0xFFFFFFFF;
  vd->pos_prev_ref=0xFFFFFFFF;
  vd->pos_next_ref=0xFFFFFFFF;
  
  while(ypos<scr_h)
  {
    if (LineDown(vd))
    {
      lc=vd->lines_cache+vl;
      dc=0;
      ws_width=0;
      cur_rc=1;
      
      rc[0].start_x=0;
      rc[0].end_x=scr_w;
      rc[0].color[0]=lc->paper1>>8;
      rc[0].color[1]=lc->paper1;
      rc[0].color[2]=lc->paper2>>8;
      rc[0].color[3]=lc->paper2;
      //ws->wsbody[++dc]=lc->bold?UTF16_FONT_SMALL_BOLD:UTF16_FONT_SMALL;
      //ws->wsbody[++dc]=lc->underline?UTF16_ENA_UNDERLINE:UTF16_DIS_UNDERLINE;
      if ((vl+1)<vd->lines_cache_size)
      {
	len=(lc[1]).pos-(lc[0]).pos;
      }
      else
	len=vd->rawtext_size-lc->pos;
      sc=lc->pos;
      if (ena_ref) ws->wsbody[++dc]=UTF16_ENA_INVERT;
      while(len>0&&(dc<32000))
      {
	c=vd->rawtext[sc];
	if (c==UTF16_ENA_INVERT)
	{
	  //Found begin of ref
	  _ref=sc;
/*	  if (vd->pos_first_ref==0xFFFFFFFF)
	  {
	    vd->pos_first_ref=sc;
	    vd->pos_last_ref=sc;
	  }*/
	  if (vd->pos_cur_ref!=sc)
	  {
	    if (flag==0) vd->pos_prev_ref=sc;
	    if (flag==1)
	    {
	      flag=2;
	    }
	    goto L1;
	  }
	  flag=1;
	  ena_ref=1;
	}
	if (c==UTF16_DIS_INVERT)
	{
//	  if ((scr_h-ypos)>lc->pixheight)
//	  {
//	    vd->pos_botview_ref=prepare_bot_ref;
//	  }
	  if (flag==2)
	  {
	    if ((scr_h-ypos)>lc->pixheight)
	    {
	      vd->pos_next_ref=_ref;
	    }
	    flag=3;
	  }
	  if ((scr_h-ypos)>lc->pixheight)
	  {
	    if (vd->pos_first_ref==0xFFFFFFFF) vd->pos_first_ref=_ref;
	    vd->pos_last_ref=_ref;
	  }
	  if (!ena_ref) goto L1;
	  ena_ref=0;
	}
        if (c==UTF16_PAPER_RGBA)
        {
          if (cur_rc<MAX_COLORS_IN_LINE)
          {
            COLOR_RC *prev=rc+cur_rc-1;
            rc[cur_rc].color[0]=vd->rawtext[sc+1]>>8;
            rc[cur_rc].color[1]=vd->rawtext[sc+1];
            rc[cur_rc].color[2]=vd->rawtext[sc+2]>>8;
            rc[cur_rc].color[3]=vd->rawtext[sc+2];
            if (memcmp((void *)rc[cur_rc].color,(void *)prev->color,4))  // Только если цвет не равен предыдущему (очередь объектов не резиновая ;))
            {
              prev->end_x=ws_width-1;
              rc[cur_rc].start_x=ws_width;
              rc[cur_rc].end_x=scr_w;
              cur_rc++;
            }
          }
        }         
	ws->wsbody[++dc]=c;
        ws_width+=GetSymbolWidth(c,lc->bold?FONT_SMALL_BOLD:FONT_SMALL);
	if ((c==UTF16_PAPER_RGBA)||(c==UTF16_INK_RGBA))
	{
	  len--;
	  ws->wsbody[++dc]=vd->rawtext[++sc];
	  len--;
	  ws->wsbody[++dc]=vd->rawtext[++sc];
	}
      L1:
	sc++;
	len--;
      }
      ws->wsbody[0]=dc;
      y2=lc->pixheight+ypos;
      if (do_draw)
      {
	  
/*	if (do_draw==2)
	{
	  //Dump rawtext
	  unsigned int ul;
	  int f;
          char fn[128];
	  sprintf(fn,"4:\\dump%d.raw",vl);
	  if ((f=fopen(fn,A_ReadWrite+A_Create+A_Truncate,P_READ+P_WRITE,&ul))!=-1)
	  {
	    fwrite(f,ws->wsbody,ws->wsbody[0]*2,&ul);
	    fclose(f,&ul);
	  }
	}*/
	def_ink[0]=lc->ink1>>8;
	def_ink[1]=lc->ink1;
	def_ink[2]=lc->ink2>>8;
	def_ink[3]=lc->ink2;
        for (int i=0; i!=cur_rc; i++) 
        {
          DrawRectangle(rc[i].start_x,ypos,rc[i].end_x,y2,
		      RECT_FILL_WITH_PEN,rc[i].color,rc[i].color);
        }
	DrawString(ws,0,ypos,scr_w,y2,
		   lc->bold?FONT_SMALL_BOLD:FONT_SMALL,TEXT_NOFORMAT
		     +(lc->underline?TEXT_UNDERLINE:0)
		       ,def_ink,GetPaletteAdrByColorIndex(23));
      }
      ypos=y2;
      vl++;
    }
    else
    {
      result=0;
//      if (ena_ref) vd->pos_botview_ref=prepare_bot_ref;
      break;
    }
  }
  vd->view_pos=store_pos;
  vd->view_line=store_line;
  return(result);
}

REFCACHE *FindReference(VIEWDATA *vd, unsigned int ref)
{
  int i=vd->ref_cache_size;
  REFCACHE *rf;
  if (!i) return NULL;
  rf=vd->ref_cache;
  do
  {
    if ((rf->begin<=ref)&&(ref<rf->end)) return rf;
    rf++;
  }
  while(--i);
  return NULL;
}

int FindReferenceById(VIEWDATA *vd, int i, unsigned int id, unsigned int form_id1, unsigned int form_id2, int tag)
{
  while(i<vd->ref_cache_size)
  {
    REFCACHE *rf=vd->ref_cache+i;
    if (!tag)
      goto LNOTAG;
    else
    {
      if (tag==rf->tag)
      {
      LNOTAG:
	if (id!=_NOREF)
	{
	  if (!omstrcmp(vd,rf->form_id1,form_id1)) 
	  {
	    if (!omstrcmp(vd,rf->form_id2,form_id2))
	    {
	      if (!omstrcmp(vd,rf->id,id)) return i;
	    }
	  }
	}
      }
    }
    i++;
  }
  return -1;
}

void ChangeRadioButtonState(VIEWDATA *vd, REFCACHE *rf)
{
  REFCACHE *rfp;
  int i;
  if (rf->tag!='r') return;
  if (rf->begin>=vd->rawtext_size) return;
  if (vd->rawtext[rf->begin+1]!=RADIOB_UNCHECKED) return;
  vd->rawtext[rf->begin+1]=RADIOB_CHECKED;
  rf->group_id=1;
  i=0;
  while((i=FindReferenceById(vd,i,rf->id,rf->form_id1,rf->form_id2,'r'))>=0)
  {
    rfp=vd->ref_cache+i;
    if (rfp!=rf)
    {
      if (rfp->begin<vd->rawtext_size)
      {
	if (vd->rawtext[rfp->begin+1]==RADIOB_CHECKED)
	{
	  vd->rawtext[rfp->begin+1]=RADIOB_UNCHECKED;
	  rfp->group_id=0;
	}
      }
    }
    i++;
  }
}

void ChangeCheckBoxState(VIEWDATA *vd, REFCACHE *rf)
{
  if (rf->tag!='c') return;
  if (rf->begin>=vd->rawtext_size) return;
  switch(vd->rawtext[rf->begin+1])
  {
  case CBOX_CHECKED:
    vd->rawtext[rf->begin+1]=CBOX_UNCHECKED;
    rf->group_id=0;
    break;
  case CBOX_UNCHECKED:
    vd->rawtext[rf->begin+1]=CBOX_CHECKED;
    rf->group_id=1;
    break;
  }
}
