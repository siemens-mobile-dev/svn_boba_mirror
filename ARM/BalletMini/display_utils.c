#include "../inc/swilib.h"
#include "view.h"
#include "display_utils.h"

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
      h=GetFontYSIZE(p->bold?FONT_SMALL_BOLD:FONT_SMALL);
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
    case 0xE000:
      p->ref^=1;
      continue;
    case 0xE001:
      p->underline=1;
      continue;
    case 0xE002:
      p->underline=0;
      continue;
    case 0xE012:
      p->bold=0;
      continue;
    case 0xE013:
      p->bold=1;
      continue;
    case 0xE006:
      if (pos>(vd->rawtext_size-2)) goto LERR;
      p->ink1=vd->rawtext[pos++];
      p->ink2=vd->rawtext[pos++];
      continue;
    case 0xE007:
      if (pos>(vd->rawtext_size-2)) goto LERR;
      p->paper1=vd->rawtext[pos++];
      p->paper2=vd->rawtext[pos++];
      continue;
    case 0xE008:
    case 0xE009:
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

/*void PrepareLineCache(VIEWDATA *vd)
{
  int i=ScreenH();
  unsigned int pos=0;
  if (vd->lines_cache_size) return;
  do
  {
    AddLineToCache(vd,pos);
    pos=SearchNextDisplayLine(vd,pos);
    if (pos>=vd->rawtext_size) break;
  }
  while(--i);
}*/


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
