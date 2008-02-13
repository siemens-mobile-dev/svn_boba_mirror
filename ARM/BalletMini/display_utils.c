#include "../inc/swilib.h"
#include "view.h"
#include "display_utils.h"
#include "siemens_unicode.h"
#include "string_works.h"

#define ITEM_EDITBOX_SIZE 255

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
  int f=0;
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
    switch(c)
    {
    case 0xE11F:
    case 0xE11E:
      f=1;
      REFCACHE *rf=FindReference(vd,pos);
      if (rf) pos=rf->end;
      break;
    case 0x0A:
    case 0x0D:
      f=1;
      break;
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
    if (left<0)   return pos-1;
    if (max_h)
    {
      if (h>*max_h)  *max_h=h;
    }
    if (f) return pos;
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
  //VIEWDATA *vd=data->vd;
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
        if (c==0x0000) goto L1;
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
          //if ((scr_h-ypos)>lc->pixheight)
          //{
          //  vd->pos_botview_ref=prepare_bot_ref;
          //}
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
        /*if (do_draw==2)
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
      
      if (y2>=scr_h||vl==vd->lines_cache_size-2)
      {
        int b=(store_pos*1000)/vd->rawtext_size*scr_h/1000-2;
        int e=(lc->pos*1000)/vd->rawtext_size*scr_h/1000+2;
        for (int i=b;i<=e;i++)
        {
          SetPixel(scr_w,i,GetPaletteAdrByColorIndex(2));
          SetPixel(scr_w-1,i,GetPaletteAdrByColorIndex(2));
        }
        //WSHDR *ws;
        //ws=AllocWS(128);
        //wsprintf(ws,"%u %u",vd->view_line,vd->lines_cache[vd->view_line].pixheight);
        //DrawString(ws,3,14,scr_w,scr_h,FONT_SMALL,TEXT_NOFORMAT,GetPaletteAdrByColorIndex(2),GetPaletteAdrByColorIndex(0));
        //wsprintf(ws,"%u %u",(vd->lines_cache[fl].pos*1000)/vd->rawtext_size*scr_h,(vd->lines_cache[fl].pos*1000)/vd->rawtext_size*scr_h/1000);
        //DrawString(ws,3,28,scr_w,scr_h,FONT_SMALL,TEXT_NOFORMAT,GetPaletteAdrByColorIndex(2),GetPaletteAdrByColorIndex(0));
        //FreeWS(ws);
      }
      ypos=y2;
      vl++;
    }
    else
    {
      result=0;
      //if (ena_ref) vd->pos_botview_ref=prepare_bot_ref;
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

int FindReferenceById(VIEWDATA *vd, unsigned int id, int i) // Находит index'ы всех ref'ов с заданным id
{
  unsigned int sz=*(vd->oms+id);
  sz<<=8;
  sz|=*(vd->oms+id+1);
  while(i<vd->ref_cache_size)
  {
    if (!omstrcmp(vd,(vd->ref_cache+i)->id,id)) return i;
    i++;
  }
  return -1;
}

// EDIT BOX
char *work_begin;
REFCACHE *cur_ref;
VIEWDATA *cur_vd;

static const SOFTKEY_DESC input_box_menu_sk[]=
{
  {0x0018,0x0000,(int)"Ok"},
  {0x0001,0x0000,(int)"Отмена"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

static const SOFTKEYSTAB input_box_menu_skt=
{
  input_box_menu_sk,0
};

static const HEADER_DESC input_box_hdr={0,0,0,0,NULL,(int)"Ввод",LGP_NULL};

static void input_box_ghook(GUI *data, int cmd)
{
  static SOFTKEY_DESC sk={0x0FFF,0x0000,(int)"Ок"};
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
  if (cmd==7)
  {
    SetSoftKey(data,&sk,SET_SOFT_KEY_N);
  }
}

static void input_box_locret(void){}

static int input_box_onkey(GUI *data, GUI_MSG *msg)
{
  EDITCONTROL ec;
  if (msg->keys==0xFFF)
  {
    // set value
    ExtractEditControl(data,1,&ec);
    WSHDR *sw=AllocWS(ec.pWS->wsbody[0]);
    wstrcpy(sw,ec.pWS);
    memset(work_begin,0,ITEM_EDITBOX_SIZE*2-2);
    if (sw->wsbody[0]>=ITEM_EDITBOX_SIZE) sw->wsbody[0]=ITEM_EDITBOX_SIZE-1;
    memcpy(work_begin,sw->wsbody+1,sw->wsbody[0]*2);
    return (0xFF);
  }
  return (0);
}

static const INPUTDIA_DESC input_box_desc =
{
  1,
  input_box_onkey,
  input_box_ghook,
  (void *)input_box_locret,
  0,
  &input_box_menu_skt,
  {0,0,0,0},
  FONT_SMALL,
  100,
  101,
  0,
  //  0x00000001 - Выровнять по правому краю
  //  0x00000002 - Выровнять по центру
  //  0x00000004 - Инверсия знакомест
  //  0x00000008 - UnderLine
  //  0x00000020 - Не переносить слова
  //  0x00000200 - bold
  0,
  //  0x00000002 - ReadOnly
  //  0x00000004 - Не двигается курсор
  //  0x40000000 - Поменять местами софт-кнопки
  0x40000000
};

int CreateInputBox(char *text_begin)
{
  work_begin=text_begin;
  
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  
  eq=AllocEQueue(ma,mfree_adr());    // Extension
  WSHDR *ews=AllocWS(ITEM_EDITBOX_SIZE);
  
  memcpy(ews->wsbody+1,text_begin,ITEM_EDITBOX_SIZE*2);
  int i=0;
  for (;i<ITEM_EDITBOX_SIZE*2;i+=2) if (text_begin[i]==0x0000) break;
  ews->wsbody[0]=i/2;
  
  PrepareEditControl(&ec);
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,ews,1024);
  AddEditControlToEditQend(eq,&ec,ma);   //2

  FreeWS(ews);
  patch_header(&input_box_hdr);
  patch_input(&input_box_desc);
  return CreateInputTextDialog(&input_box_desc,&input_box_hdr,eq,1,0);
}


// SELECTION MENU
typedef struct
{
  void *next;
  char name[64];
  unsigned short value;
  unsigned short id2;
}SEL_STRUCT;

char *collectItemsParams(VIEWDATA *vd, REFCACHE *rf);

int sel_menu_onkey(void *gui, GUI_MSG *msg)
{
  extern char *goto_url;
  extern char *from_url;
  extern char *goto_params;
  extern int quit_reqired;
  
  SEL_STRUCT *ustop=MenuGetUserPointer(gui);
  if (msg->keys==0x3D || msg->keys==0x18)
  {
    int i=GetCurMenuItem(gui);
    for (int n=0; n!=i; n++) ustop=ustop->next;
    if (ustop)
    {
      cur_ref->value=ustop->value;
      cur_ref->id2=ustop->id2;
      if (!cur_ref->no_upload)
      {
        goto_url=malloc(strlen(cur_vd->pageurl)+1);
        memcpy(goto_url,cur_vd->pageurl,strlen(cur_vd->pageurl));
        goto_url[strlen(cur_vd->pageurl)]=0;
        from_url=malloc(strlen(cur_vd->pageurl));
        strcpy(from_url,cur_vd->pageurl);
        goto_params=collectItemsParams(cur_vd,cur_ref);
        quit_reqired=1;
      }
      cur_ref=NULL;
      return 0xFF;
    }
  }
  return (0);
}

void sel_menu_ghook(void *gui, int cmd)
{
  SEL_STRUCT *ustop=MenuGetUserPointer(gui);
  if (cmd==3)
  {
    while(ustop)
    {
      SEL_STRUCT *us=ustop;
      ustop=ustop->next;
      mfree(us);
    }    
  }
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

void sel_menu_iconhndl(void *gui, int cur_item, void *user_pointer)
{
  SEL_STRUCT *ustop=user_pointer;
  WSHDR *ws;
  int len;
  for (int n=0; n!=cur_item; n++) ustop=ustop->next;
  void *item=AllocMenuItem(gui);
  if (ustop)
  {
    len=strlen(ustop->name);
    ws=AllocMenuWS(gui,len+4);
    oms2ws(ws,ustop->name,len);
  }
  else
  {
    ws=AllocMenuWS(gui,10);
    ascii2ws(ws,"Ошибка");
  }
  SetMenuItemText(gui, item, ws, cur_item);
}

int sel_softkeys[]={0,1,2};
SOFTKEY_DESC sel_sk[]=
{
  {0x0018,0x0000,(int)"Select"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB sel_skt=
{
  sel_sk,0
};

HEADER_DESC sel_HDR={0,0,0,0,NULL,(int)"Выбор",LGP_NULL};


MENU_DESC sel_STRUCT=
{
  8,sel_menu_onkey,sel_menu_ghook,NULL,
  sel_softkeys,
  &sel_skt,
  0x10,
  sel_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};

static unsigned int _rshort(char *p)
{
  unsigned int r=*p++;
  r<<=8;
  r|=*p;
  return r;
}

int ChangeMenuSelection(VIEWDATA *vd, REFCACHE *rf)
{
  int start=0;
  SEL_STRUCT *ustop=0, *usbot=0;
  SEL_STRUCT *us;
  unsigned short i;
  vd->oms_pos=rf->id;
  i=_rshort(vd->oms+vd->oms_pos);
  vd->oms_pos+=2+i+1;
  int n_sel=_rshort(vd->oms+vd->oms_pos);
  vd->oms_pos+=2;
  for(int o=0;o<n_sel;o++)
  {
    vd->oms_pos+=1;
    if (rf->value==vd->oms_pos) start=o;
    us=malloc(sizeof(SEL_STRUCT));
    char *d=extract_omstr(vd,vd->oms_pos);
    strcpy(us->name,d);
    mfree(d);
    us->value=vd->oms_pos;
    int i=_rshort(vd->oms+vd->oms_pos);
    vd->oms_pos+=2+i;
    us->id2=vd->oms_pos;
    i=_rshort(vd->oms+vd->oms_pos);
    vd->oms_pos+=2+i+1;
    us->next=0;
    if (usbot)
    {
      usbot->next=us;
      usbot=us;
    }
    else
    {
      ustop=us;
      usbot=us;
    }
  }
  cur_ref=rf;
  cur_vd=vd;
  patch_header(&sel_HDR);
  return CreateMenu(0,0,&sel_STRUCT,&sel_HDR,start,n_sel,ustop,0);
}
