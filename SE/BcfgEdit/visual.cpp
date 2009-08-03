#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

#include "..\\include\cfg_items.h"
#include "main.h"
#include "visual.h"

void OnOkCoordinatesEdit(BOOK * bk, void *);
void OnBackCoordinatesEdit(BOOK * bk, void *);
void OnOrientationChangeCoordinatesEdit(BOOK *bk, void *);

static const char EditColorGuiName[]="Gui_EditColor";
int colors[4]={0xFFFF0000,0xFF00FF00,0xFF0000FF,0x80C6AAAF};

#define MIN(a,b) (a<b)?a:b
#define MAX(a,b) (a>b)?a:b

int ColorGuiOnCreate(DISP_OBJ_COLOR *db)
{
  db->str_id=LGP_NULL;
  db->need_str=1;
  db->current_column=0;
  db->cstep=1;
  return (1);
}

void ColorGuiOnClose(DISP_OBJ_COLOR *db)
{
  TextFree(db->str_id);
}


void ColorGuiOnRedraw(DISP_OBJ_COLOR *db,int ,int,int)
{
  RECT rc_old;
  int font_old, gc_xx;
  int font=FONT_E_20R, fsize=20;
  void *gc=get_DisplayGC();
  int column_height, column_width;
  int start_column, y_line;
  wchar_t ustr[32];
  int scr_w, scr_h, x1, y1;
  int testcolor;

  get_GC_RECT(gc, &rc_old);
  gc_xx=get_GC_xx(gc);
  set_GC_xx(gc,1);
  x1=rc_old.x1;
  y1=rc_old.y1;
  scr_w=rc_old.x2-x1;
  scr_h=rc_old.y2-y1;

  font_old=SetFont(font);
  column_height=scr_h-fsize-fsize;
  column_width=scr_w/9;
  DrawRect(x1,y1,x1+scr_w,y1+scr_h,clWhite,clWhite);
  if (db->need_str)
  {
    snwprintf(ustr,MAXELEMS(ustr)-1,L"%02X,%02X,%02X,%02X",db->r,db->g,db->b,db->a);
    TextFree(db->str_id);
    db->str_id=Str2ID(ustr,0,SID_ANY_LEN);
    db->need_str=0;
  }
  DrawString(db->str_id,0, x1+1,y1+1,x1+scr_w-30, y1+1+fsize+1,0,0,clBlack,0x00000000);
  fsize+=3;

  for (int i=0;i!=4;i++)
  {
    start_column=column_width+2*i*column_width;
    column_height++;  // Какая то фигня с DrawRect, координаты не совпадают с DrawLine
    if (db->current_column==i)
      DrawRect(x1+start_column-2,y1+fsize-2,x1+start_column+column_width+2,y1+fsize+column_height+2,clBlack,clWhite);

    DrawRect(x1+start_column,y1+fsize,x1+start_column+column_width,y1+fsize+column_height,clBlack,colors[i]);
    column_height--;
    switch(i)
    {
    case 0:
      y_line=fsize+column_height-(db->r*column_height)/0xFF;
      break;
    case 1:
      y_line=fsize+column_height-(db->g*column_height)/0xFF;
      break;
    case 2:
      y_line=fsize+column_height-(db->b*column_height)/0xFF;
      break;
    case 3:
      y_line=fsize+column_height-(db->a*column_height)/(db->type==0?0x64:0xFF);
      break;
    }
    int old_pen=GC_GetPenColor(gc);
    GC_SetPenColor(gc,clBlack);
    GC_DrawLine(gc,x1+start_column,y1+y_line,x1+start_column+column_width-1,y1+y_line);
    GC_SetPenColor(gc,old_pen);
  }
  testcolor=COLOR_RGBA(db->r,db->g,db->b,(db->type==0?(db->a*0xFF/0x64):(db->a)));
  DrawRect(x1+scr_w-fsize-1,y1+1,x1+scr_w-1,y1+fsize+1,clBlack,testcolor);

  SetFont(font_old);
  set_GC_xx(gc,gc_xx);
}

void ColorGuiOnKey(DISP_OBJ_COLOR *db,int key,int,int repeat,int type)
{
  if (type==KBD_SHORT_RELEASE || type==KBD_REPEAT)
  {
    if (type==KBD_SHORT_RELEASE) db->cstep=1;
    else if (type==KBD_REPEAT && repeat>10) db->cstep=8;
    if (key==KEY_UP || key==KEY_DIGITAL_0+2)
    {
      switch(db->current_column)
      {
      case 0:
        if ((db->r+=db->cstep) >0xFF)
          db->r=0;
        break;
      case 1:
        if ((db->g+=db->cstep) >0xFF)
          db->g=0;
        break;
      case 2:
        if ((db->b+=db->cstep) >0xFF)
          db->b=0;
        break;
      case 3:
        if ((db->a+=db->cstep) > (db->type==0?0x64:0xFF))
          db->a=0;
        break;
      }
      db->need_str=1;
    }
    else if (key==KEY_DOWN || key==KEY_DIGITAL_0+8)
    {
      switch(db->current_column)
      {
      case 0:
        if ((db->r-=db->cstep) <0)
          db->r=0xFF;
        break;
      case 1:
        if ((db->g-=db->cstep) <0)
          db->g=0xFF;
        break;
      case 2:
        if ((db->b-=db->cstep) <0)
          db->b=0xFF;
        break;
      case 3:
        if ((db->a-=db->cstep) <0)
          db->a=(db->type==0?0x64:0xFF);
        break;
      }
      db->need_str=1;
    }
    else if (key==KEY_LEFT || key==KEY_DIGITAL_0+4)
    {
      if (--db->current_column<0)
        db->current_column=3;
    }
    else if (key==KEY_RIGHT || key==KEY_DIGITAL_0+6)
    {
      if (++db->current_column>3)
        db->current_column=0;
    }
  }
  if (type==KBD_LONG_RELEASE) db->cstep=1;
  InvalidateRect(&db->dsp_obj,0);
}



void EditColorGui_constr(DISP_DESC *desc)
{
  DISP_DESC_SetName(desc,EditColorGuiName);
  DISP_DESC_SetSize(desc,sizeof(DISP_OBJ_COLOR));
  DISP_DESC_SetOnCreate(desc,(DISP_OBJ_ONCREATE_METHOD)ColorGuiOnCreate);
  DISP_DESC_SetOnClose(desc,(DISP_OBJ_ONCLOSE_METHOD)ColorGuiOnClose);
  DISP_DESC_SetOnRedraw(desc,(DISP_OBJ_ONREDRAW_METHOD)ColorGuiOnRedraw);
  DISP_DESC_SetOnKey(desc,(DISP_OBJ_ONKEY_METHOD)ColorGuiOnKey);
}

void EditColorGui_destr(DISP_DESC *desc)
{
}

void OnBackColorEdit(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->color);
}

void OnOkColorEdit(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  DISP_OBJ_COLOR *disp_obj=(DISP_OBJ_COLOR *)GUIObj_GetDISPObj(myBook->color);
  if (disp_obj->type==0)
  {
    char *color=(char *)myBook->cur_hp+sizeof(CFG_HDR);
    color[0]=disp_obj->r;
    color[1]=disp_obj->g;
    color[2]=disp_obj->b;
    color[3]=disp_obj->a;
  }
  else
  {
    *((unsigned int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)))=COLOR_RGBA(disp_obj->r,disp_obj->g,disp_obj->b,disp_obj->a);
  }
  FREE_GUI(myBook->color);
}

GUI_COLOR *CreateEditColorGUI(MyBOOK * myBook, int type)
{
  wchar_t ustr[32];
  GUI_COLOR *gui_color=new GUI_COLOR;
  DISP_OBJ_COLOR *disp_obj;

  if (!CreateObject((GUI *)gui_color,EditColorGui_destr,EditColorGui_constr, &myBook->book,0,0,0))
  {
    myBook->color=NULL;
    delete gui_color;
    return 0;
  }
  disp_obj=(DISP_OBJ_COLOR *)GUIObj_GetDISPObj(gui_color);
  if (type==0)
  {
    char *color=(char *)myBook->cur_hp+sizeof(CFG_HDR);
    disp_obj->r=color[0];
    disp_obj->g=color[1];
    disp_obj->b=color[2];
    disp_obj->a=color[3];
  }
  else
  {
    unsigned int color=*((unsigned int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
    disp_obj->r=COLOR_GET_R(color);
    disp_obj->g=COLOR_GET_G(color);
    disp_obj->b=COLOR_GET_B(color);
    disp_obj->a=COLOR_GET_A(color);
  }
  disp_obj->type=type;

  myBook->color=(GUI *)gui_color;
  if (myBook) addGui2book(&myBook->book,(GUI*)gui_color);
  //GUI_SetStyle(myBook->color,4);
  win12512unicode(ustr,myBook->cur_hp->name,MAXELEMS(ustr)-1);
  GuiObject_SetTitleText(myBook->color, Str2ID(ustr,0,SID_ANY_LEN));
  GuiObject_SetTitleType(myBook->color, 3);
  //GUIObject_HideSoftkeys(myBook->color);
  GUIObject_Softkey_SetAction(myBook->color,ACTION_BACK, OnBackColorEdit);
  GUIObject_Softkey_SetAction(myBook->color,ACTION_SELECT1, OnOkColorEdit);
  GUIObject_Softkey_SetText(myBook->color,ACTION_SELECT1,STR("OK"));
  ShowWindow(myBook->color);
  return gui_color;
}

// --------------------------------------------------------------------------------------------------------------------------------
static const char CooordinatesGuiName[]="Gui_Coordinates";

int CoordinatesGuiOnCreate(DISP_OBJ_COORD *db)
{
  db->str_id=LGP_NULL;
  db->need_str=1;
  db->is_first_set=0;
  db->cstep=1;
  return (1);
}

void CoordinatesGuiOnClose(DISP_OBJ_COORD *db)
{
  TextFree(db->str_id);
}

void DrawOwnRect(int _x1, int _y1,int _x2 ,int _y2,int pen_color,int brush_color)
{
  int x1,x2,y1,y2;
  if (_x1>_x2){ x1=_x2;  x2=_x1;} else { x1=_x1;  x2=_x2;}
  if (_y1>_y2){ y1=_y2;  y2=_y1;} else { y1=_y1;  y2=_y2;}
  if (y1==y2 || x1==x2)
  {
    void *gc=get_DisplayGC();
    GC_SetPenColor(gc,pen_color);
    GC_SetBrushColor(gc,brush_color);
    GC_DrawLine(gc,x1,y1,x2,y2);
  }
  else
    DrawRect(x1,y1,x2,y2,pen_color,brush_color);
}

void DrawArrow(int x, int y, int color)
{
  DrawOwnRect(x,   y , x+13,y+2,clBlack,clBlack); 
  DrawOwnRect(x+11,y+2,x+13,y+6,clBlack,clBlack); 
  DrawOwnRect(x+8, y+5,x+16,y+6,clBlack,clBlack); 
  DrawOwnRect(x+9, y+6,x+15,y+7,clBlack,clBlack);
  DrawOwnRect(x+10,y+7,x+14,y+8,clBlack,clBlack);
  DrawOwnRect(x+11,y+8,x+13,y+9,clBlack,clBlack);
};

void CoordinatesGuiOnRedraw(DISP_OBJ_COORD *db,int ,int,int)
{
  RECT rc_old, rc_new;
  int font_old, gc_xx;
  int font=FONT_E_20R, fsize=20;
  void *gc=get_DisplayGC();
  wchar_t ustr[32];
  int old_pen;
  int scr_w=Display_GetWidth(0), scr_h=Display_GetHeight(0);

  get_GC_RECT(gc, &rc_old);
  gc_xx=get_GC_xx(gc);
  set_GC_xx(gc,1);

  rc_new.x1=0;
  rc_new.x2=scr_w;
  rc_new.y1=0;
  rc_new.y2=scr_h;
  GC_validate_RECT(gc,&rc_new);

  font_old=SetFont(font);
  DrawRect(0,0,scr_w,scr_h,clWhite,clWhite);
  old_pen=GC_GetPenColor(gc);
  GC_SetPenColor(gc,colors[3]);
  for (int y_0=0; y_0 < scr_h;y_0+=10)
  {
    GC_DrawLine(gc,0,y_0,scr_w,y_0);
  }
  for (int x_0=0; x_0<scr_w;x_0+=10)
  {
    GC_DrawLine(gc,x_0,0,x_0, scr_h);
  }
  GC_SetPenColor(gc,old_pen);
  if (db->type)  // Если нужен рект
  {
    DrawOwnRect(db->old_rect.x1,db->old_rect.y1,db->old_rect.x2,db->old_rect.y2,0xC0808080,0x00000000);  // Нарисуем старый рект
    if (db->is_first_set)
    {
      DrawOwnRect(db->x,db->y,db->x2,db->y2,clBlack,0x00000000);
      if (db->need_str)
      {
        snwprintf(ustr,MAXELEMS(ustr)-1,L"%d,%d,%d,%d",db->x2,db->y2,db->x,db->y);
        TextFree(db->str_id);
        db->str_id=Str2ID(ustr,0,SID_ANY_LEN);
        db->need_str=0;
      }
    }
    else
    {
      if (db->need_str)
      {
        snwprintf(ustr,MAXELEMS(ustr)-1,L"%d,%d,%d,%d",db->x,db->y,db->x2,db->y2);
        TextFree(db->str_id);
        db->str_id=Str2ID(ustr,0,SID_ANY_LEN);
        db->need_str=0;
      }
    }
  }
  else
  {
    if (db->need_str)
    {
      snwprintf(ustr,MAXELEMS(ustr)-1,L"%d,%d",db->x,db->y);
      TextFree(db->str_id);
      db->str_id=Str2ID(ustr,0,SID_ANY_LEN);
      db->need_str=0;
    }
  }
  DrawString(db->str_id,0, 3,scr_h-fsize-2,scr_w-4, scr_h-1,0,0,clBlack,0x00000000);

  old_pen=GC_GetPenColor(gc);
  GC_SetPenColor(gc,clBlack);
  GC_DrawLine(gc,db->x-3,db->y,db->x+3,db->y);
  GC_DrawLine(gc,db->x,db->y-3,db->x,db->y+3);
  
  if (BookObj_GetDisplayOrientation((BOOK*)db->mb)==0)
  {
    DrawArrow(scr_w-19, scr_h-13, clBlack);
  }
  else
  {
    DrawArrow(scr_w-19, 3, clBlack);
  }
  GC_SetPenColor(gc,old_pen);
  GC_validate_RECT(gc,&rc_old);
  SetFont(font_old);
  set_GC_xx(gc,gc_xx);
}

void CoordinatesGuiOnKey(DISP_OBJ_COORD *db,int key,int,int repeat,int type)
{
  if (type==KBD_SHORT_RELEASE || type==KBD_REPEAT)
  {
    int scr_w=Display_GetWidth(0), scr_h=Display_GetHeight(0);
    if (type==KBD_SHORT_RELEASE) db->cstep=1;
    else if (type==KBD_REPEAT && repeat>10) db->cstep=8;
    if (key==KEY_DIGITAL_0+1)
    {
      if ((db->x-=db->cstep)<0)  db->x=0;
      if ((db->y-=db->cstep)<0)  db->y=0;
    }
    else if (key==KEY_DIGITAL_0+2 || key==KEY_UP)
    {
      if ((db->y-=db->cstep)<0)   db->y=0;
    }
    else if (key==KEY_DIGITAL_0+3)
    {
      if ((db->x+=db->cstep)>scr_w)   db->x=scr_w;
      if ((db->y-=db->cstep)<0)    db->y=0;
    }
    else if (key==KEY_DIGITAL_0+4 || key==KEY_LEFT)
    {
      if ((db->x-=db->cstep)<0)   db->x=0;
    }
    else if (key==KEY_DIGITAL_0+6 || key==KEY_RIGHT)
    {
      if ((db->x+=db->cstep)>scr_w)
        db->x=scr_w;
    }
    else if (key==KEY_DIGITAL_0+7)
    {
      if ((db->x-=db->cstep)<0)  db->x=0;
      if ((db->y+=db->cstep)>scr_h)   db->y=scr_h;
    }
    else if (key==KEY_DIGITAL_0+8 || key==KEY_DOWN)
    {
      if ((db->y+=db->cstep)>scr_h)  db->y=scr_h;
    }
    else if (key==KEY_DIGITAL_0+9)
    {
      if ((db->x+=db->cstep)>scr_w)   db->x=scr_w;
      if ((db->y+=db->cstep)>scr_h)   db->y=scr_h;
    }
    db->need_str=1;

    if ((key==KEY_ENTER) || (key==KEY_LEFT_SOFT)) OnOkCoordinatesEdit((BOOK*)db->mb,0);
    if (key==KEY_ESC) OnBackCoordinatesEdit((BOOK*)db->mb,0);
    if (key==KEY_RIGHT_SOFT) OnOrientationChangeCoordinatesEdit((BOOK*)db->mb,0);
  }
  if (type==KBD_LONG_RELEASE) db->cstep=1;
  InvalidateRect(&db->dsp_obj,0);
}



void EditCoordinatesGui_constr(DISP_DESC *desc)
{
  DISP_DESC_SetName(desc,CooordinatesGuiName);
  DISP_DESC_SetSize(desc,sizeof(DISP_OBJ_COORD));
  DISP_DESC_SetOnCreate(desc,(DISP_OBJ_ONCREATE_METHOD)CoordinatesGuiOnCreate);
  DISP_DESC_SetOnClose(desc,(DISP_OBJ_ONCLOSE_METHOD)CoordinatesGuiOnClose);
  DISP_DESC_SetOnRedraw(desc,(DISP_OBJ_ONREDRAW_METHOD)CoordinatesGuiOnRedraw);
  DISP_DESC_SetOnKey(desc,(DISP_OBJ_ONKEY_METHOD)CoordinatesGuiOnKey);
}

void EditCoordinatesGui_destr(DISP_DESC *desc)
{
}

void OnOrientationChangeCoordinatesEdit(BOOK *bk, void *)
{
  char orientation=BookObj_GetDisplayOrientation(bk);
  if (orientation==0)
    BookObj_SetDisplayOrientation(bk,1);
  else
    BookObj_SetDisplayOrientation(bk,0);
};

void OnBackCoordinatesEdit(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  DISP_OBJ_COORD *disp_obj=(DISP_OBJ_COORD *)GUIObj_GetDISPObj(myBook->coord);
  int f=0;
  if (disp_obj->type)
  {
    if (disp_obj->is_first_set)
    {
      disp_obj->is_first_set=0;
      f=1;
      InvalidateRect((DISP_OBJ*)disp_obj,0);
    }
  }
  
  if (!f) 
  {
    BookObj_SetDisplayOrientation(bk, 0);
    FREE_GUI(myBook->coord);
  }
}

void OnOkCoordinatesEdit(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  DISP_OBJ_COORD *disp_obj=(DISP_OBJ_COORD *)GUIObj_GetDISPObj(myBook->coord);
  int f=0;
  if (disp_obj->type)
  {
    if (!disp_obj->is_first_set)
    {
      disp_obj->x2=disp_obj->x;
      disp_obj->y2=disp_obj->y;
      disp_obj->is_first_set=1;
      f=1;
    }
    else
    {
      RECT *rc=((RECT *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
      rc->x1=MIN(disp_obj->x2,disp_obj->x);
      rc->y1=MIN(disp_obj->y2,disp_obj->y);
      rc->x2=MAX(disp_obj->x2,disp_obj->x);
      rc->y2=MAX(disp_obj->y2,disp_obj->y);
    }
  }
  else
  {
    int *coordinates=((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
    coordinates[0]=disp_obj->x;
    coordinates[1]=disp_obj->y;
  }
  
  if (!f)
  {
    BookObj_SetDisplayOrientation(bk, 0);
    FREE_GUI(myBook->coord);
  }
}


GUI_COORDINATES *CreateEditCoordinatesGUI(MyBOOK * myBook, int type)
{
  GUI_COORDINATES *gui_coord=new GUI_COORDINATES;
  DISP_OBJ_COORD *disp_obj;
  if (!CreateObject((GUI *)gui_coord,EditCoordinatesGui_destr,EditCoordinatesGui_constr, &myBook->book,0,0,0))
  {
    delete gui_coord;
    return 0;
  }
  disp_obj=(DISP_OBJ_COORD *)GUIObj_GetDISPObj(gui_coord);
  if (type==0)
  {
    int *coordinates=((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
    disp_obj->x=coordinates[0];
    disp_obj->y=coordinates[1];
  }
  else
  {
    RECT *rc=((RECT *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
    disp_obj->old_rect.x1=disp_obj->x=rc->x1;
    disp_obj->old_rect.y1=disp_obj->y=rc->y1;
    disp_obj->old_rect.x2=disp_obj->x2=rc->x2;
    disp_obj->old_rect.y2=disp_obj->y2=rc->y2;
  }
  disp_obj->type=type;
  disp_obj->mb=myBook;
  myBook->coord=(GUI *)gui_coord;
  if (myBook) addGui2book(&myBook->book,myBook->coord);

  GUI_SetStyle(myBook->coord,4);
  GuiObject_SetTitleType(myBook->coord, 1);
  GUIObject_HideSoftkeys(myBook->coord);
  ShowWindow(myBook->coord);
  return gui_coord;
}

// --------------------------------------------------------------------------------------------------------------------------------
static const char FontSelectGuiName[]="Gui_FontSelect";


const char test_str[]="Test string 123";

wchar_t *Font_GetNameByFontId(int id)
{
  wchar_t *txt=L"Undefined";
  FONT_DESC *font=GetFontDesc();
  for (int i=0, max=*GetFontCount(); i<max; i++)
  {
    if (id==font[i].id)
    {
      txt=font[i].name;
      break;
    }
  }
  return txt;
}

int GetIdByFontId(int id)
{
  int r=0;
  FONT_DESC *font=GetFontDesc();
  for (int i=0, max=*GetFontCount(); i<max; i++)
  {
    if (id==font[i].id)
    {
      r=i;
      break;
    }
  }
  return r;
}

int FontSelectGuiOnCreate(DISP_OBJ_FONT_SEL *db)
{
  wchar_t ustr[64];
  int font_old;
  win12512unicode(ustr,test_str,MAXELEMS(test_str)-1);
  db->test_str_id=Str2ID(ustr,0,SID_ANY_LEN);
  db->total_fonts=*GetFontCount();
  font_old=SetFont(FONT_E_20R);
  db->font_heights=new u16[db->total_fonts];
  for (int i=0; i<db->total_fonts; i++)
  {
    SetFont(GetFontDesc()[i].id);
    db->font_heights[i]=GetImageHeight(' ');
  }
  SetFont(font_old);
  db->cur_offs=0;
  db->req_check_vis=1;
  return (1);
}

void FontSelectGuiOnClose(DISP_OBJ_FONT_SEL *db)
{
  TextFree(db->test_str_id);  db->test_str_id=LGP_NULL;
  delete db->font_heights;
}


void CheckStringVisibility(DISP_OBJ_FONT_SEL *db,int x1, int y1, int x2, int y2,int y_offs)
{
  if (db->cur_pos<db->cur_offs)  // строка выше смещения, поднимаем смещение
  {
    db->cur_offs=db->cur_pos;
  }
  else   // иначе проверяем в зоне видимости ли мы, если нет то опускаемся пока не увидим
  {
    int f=0;
    do {
      int cur_y=y_offs;
      for (int i=db->cur_offs, k=0; i<db->total_fonts; i++)
      {
        int sum;
        if (db->cur_pos==i) k=1;
        if (((sum=cur_y+db->font_heights[i]+2)+1)<y2 && k) f=1;
        cur_y=sum;
        if (cur_y>y2) break;
      }
    } while(!f && ++db->cur_offs<db->total_fonts);
  }
}

void FontSelectGuiOnRedraw(DISP_OBJ_FONT_SEL *db,int, RECT *cur_rc,int)
{

  int font_old, gc_xx;
  void *gc=get_DisplayGC();
  int x1,y1,x2,y2;
  int y_offs;
  STRID selfont;

  gc_xx=get_GC_xx(gc);
  set_GC_xx(gc,1);
  x1=cur_rc->x1;
  y1=cur_rc->y1;
  x2=cur_rc->x2;
  y2=cur_rc->y2;
  y_offs=y1+20+2;
  if (db->req_check_vis)
  {
    CheckStringVisibility(db,x1,y1,x2,y2,y_offs);
    db->req_check_vis=0;
  }

  font_old=SetFont(FONT_E_20R);
  DrawRect(x1,y1,x2,y2,clBlack,clBlack);

  selfont=Str2ID(GetFontDesc()[db->cur_pos].name,0,9);
  DrawString(selfont,0, x1+3,y1+1,x2-2, y_offs-1,0,0,0xFF0080FF,0x00000000);   // Рисуем выбранный шрифт в шапке меню
  TextFree(selfont);

  int cur_y=y_offs;
  for (int i=db->cur_offs; i<db->total_fonts; i++)
  {
    int font, font_h;
    font=GetFontDesc()[i].id;
    SetFont(font);
    font_h=db->font_heights[i];
    if (i==db->cur_pos)   // Если это выбранный шрифт
      DrawRect(x1+1,cur_y,x2-1,cur_y+font_h+2,0xFF00FF00,0xFF408000);
    DrawString(db->test_str_id,0,x1+2,cur_y+1,x2-2,cur_y+font_h+1,0,0,clWhite,0x00000000);
    cur_y+=font_h+2;
    if (cur_y>y2) break;
  }

  SetFont(font_old);
  set_GC_xx(gc,gc_xx);

}




void FontSelectGuiOnKey(DISP_OBJ_FONT_SEL *db,int key,int,int repeat,int type)
{
  if (type==KBD_SHORT_RELEASE || type==KBD_REPEAT)
  {
    if (key==KEY_UP || key==KEY_DIGITAL_0+2)
    {
      if ((--db->cur_pos)<0) db->cur_pos=db->total_fonts-1;
    }
    else if (key==KEY_DOWN || key==KEY_DIGITAL_0+8)
    {
      if ((++db->cur_pos)>=db->total_fonts) db->cur_pos=0;
    }
    db->req_check_vis=1;
    InvalidateRect(&db->dsp_obj,0);
  }
}

void OnBackFontSelect(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->font_select);
}

void OnOkFontSelect(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  DISP_OBJ_FONT_SEL *disp_obj=(DISP_OBJ_FONT_SEL *)GUIObj_GetDISPObj(myBook->font_select);
  *((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)))=GetFontDesc()[disp_obj->cur_pos].id;
  FREE_GUI(myBook->font_select);
}


void FontSelectGui_constr(DISP_DESC *desc)
{
  DISP_DESC_SetName(desc,FontSelectGuiName);
  DISP_DESC_SetSize(desc,sizeof(DISP_OBJ_FONT_SEL));
  DISP_DESC_SetOnCreate(desc,(DISP_OBJ_ONCREATE_METHOD)FontSelectGuiOnCreate);
  DISP_DESC_SetOnClose(desc,(DISP_OBJ_ONCLOSE_METHOD)FontSelectGuiOnClose);
  DISP_DESC_SetOnRedraw(desc,(DISP_OBJ_ONREDRAW_METHOD)FontSelectGuiOnRedraw);
  DISP_DESC_SetOnKey(desc,(DISP_OBJ_ONKEY_METHOD)FontSelectGuiOnKey);
}



void FontSelectGui_destr(DISP_DESC *desc)
{
}

GUI_FONT_SEL *CreateFontSelectGUI(MyBOOK * myBook)
{

  GUI_FONT_SEL *gui_fontsel=new GUI_FONT_SEL;
  DISP_OBJ_FONT_SEL *disp_obj;

  wchar_t ustr[64];
  if (!CreateObject((GUI *)gui_fontsel,FontSelectGui_destr,FontSelectGui_constr, &myBook->book,0,0,0))
  {
    delete gui_fontsel;
    return 0;
  }


  disp_obj=(DISP_OBJ_FONT_SEL *)GUIObj_GetDISPObj(gui_fontsel);

  disp_obj->cur_pos=GetIdByFontId(*((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR))));


  myBook->font_select=(GUI *)gui_fontsel;
  if (myBook) addGui2book(&myBook->book,myBook->font_select);
  //GUI_SetStyle(myBook->font_select,4);
  win12512unicode(ustr,myBook->cur_hp->name,MAXELEMS(ustr)-1);
  GuiObject_SetTitleText(myBook->font_select, Str2ID(ustr,0,SID_ANY_LEN));
  GuiObject_SetTitleType(myBook->font_select, 3);
  //GUIObject_HideSoftkeys(myBook->font_select);
  GUIObject_Softkey_SetAction(myBook->font_select,ACTION_BACK, OnBackFontSelect);
  GUIObject_Softkey_SetAction(myBook->font_select,ACTION_SELECT1, OnOkFontSelect);
  ShowWindow(myBook->font_select);
  return gui_fontsel;
}


typedef struct
{
  void *next;
  int is_folder;
  wchar_t *fullname;  
  wchar_t *name;  
}FLIST;


volatile FLIST *fltop=NULL;

enum TYPES {IS_BACK, IS_FOLDER, IS_FILE}; 
const wchar_t back[]=L"..";     

void Free_FLIST(void)
{
  FLIST *fl=(FLIST *)fltop;
  fltop=0;
  while(fl)
  {
    FLIST *fl_prev=fl;
    delete fl_prev->fullname;
    delete fl_prev->name;
    fl=(FLIST *)fl->next;
    delete fl_prev;
  }
}

#define FREE_FLGUI(a) Free_FLIST();\
  FREE_GUI(a);

FLIST *AddToFList(const wchar_t* full_name, const wchar_t *name, int is_folder)
{
  //int l_fname;
  FLIST *fl;
  FLIST *fn=new FLIST;
  fn->fullname=new wchar_t[wstrlen(full_name)+1];
  fn->name=new wchar_t[wstrlen(name)+1];
  wstrcpy(fn->fullname,full_name);
  wstrcpy(fn->name,name);
  fn->is_folder=is_folder;
  fn->next=0;
  fl=(FLIST *)fltop;
  if (fl)
  {
    FLIST *pr;
    pr=(FLIST *)&fltop;
    while(wstrcmpi(fl->name,fn->name)<0)
    {
      pr=fl;
      fl=(FLIST *)fl->next;
      if (!fl) break;
    }
    fn->next=fl;
    pr->next=fn;
  }
  else
  {
    fltop=fn;
  }
  return (fn);
}

int FindFiles(wchar_t *str, int type)  // type == 0 SelectFolder, type == 1 SelectFile
{
  int i, c, n=0;  
  Free_FLIST();
  wchar_t *path=new wchar_t[256];
  wchar_t *name=new wchar_t[64];
  wchar_t *rev=NULL, *d=name, *s=str;
  while((c=*s++))
  {
    *d=c;
    if (c=='/' && *s!='\0') rev=d;
    d++;
  }
  if(rev)
  {
    if (rev==name)  // Если идем на корень
      *(rev+1)=0;
    else
      *rev=0;
    AddToFList(name,back,IS_BACK);
    n++;
  }
  void *dir=w_diropen(str);
  if (dir)
  {
    wchar_t *next;
    w_chdir(str);
    while((next=w_dirread(dir)))
    {
      W_FSTAT fs;
      w_fstat(next,&fs);
      i=wstrlen(str);
      wstrcpy(path,str);
      if (rev)
      {
        path[i++]='/';
      }
      wstrcpy(path+i,next);
      if (fs.attr&0x4000)
      {
        snwprintf(name,63,L"[%ls]",next);
        AddToFList(path,name,IS_FOLDER);
        n++;
      }
      else
      {
        if (type==SFILE)
        {
          AddToFList(path,next,IS_FILE);
          n++;
        }
      }
    }
    w_dirclose(dir);
  }
  delete name;
  delete path;
  return n;
}

FLIST *FindFLISTtByNS(int *i, int si)
{
  FLIST *fl;
  fl=(FLIST *)fltop;
  while(fl)
  {
    if (fl->is_folder==si)
    {
      if (!(*i)) return (fl);
      (*i)--;
    }    
    fl=(FLIST *)fl->next;
  }
  return fl;
}
  
FLIST *FindFLISTtByN(int n)
{
  FLIST *fl;
  fl=FindFLISTtByNS(&n,IS_BACK); if ((!n)&&(fl)) return(fl);
  fl=FindFLISTtByNS(&n,IS_FOLDER); if ((!n)&&(fl)) return(fl);
  fl=FindFLISTtByNS(&n,IS_FILE); if ((!n)&&(fl)) return(fl);
  return fl;
}


int OnMessage(GUI_MESSAGE * msg)
{
  int d;
  STRID str=LGP_NULL;
  FLIST *f;
  switch(msg->msg)
  {
  case 1:
    d=GUIonMessage_GetCreatedItemIndex(msg);
    f=FindFLISTtByN(d);
    str=Str2ID(f->name,0,SID_ANY_LEN);
    SetMenuItemText0(msg,str);
    break;
  }
  return(1);
};

void Self_OnBack(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_FLGUI(myBook->selectf);
}

void Self_onEnterPressed(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(myBook->selectf);
  FLIST *fl=FindFLISTtByN(item);
  if (fl) 
  {
    wchar_t *path=new wchar_t[256];
    if (fl->is_folder==IS_FOLDER || fl->is_folder==IS_BACK)
    {
      wstrncpy(path, fl->fullname,255);
      FREE_FLGUI(myBook->selectf);
      myBook->selectf=CreateFileFolderSelect(myBook, path);
    }
    else
    {
      STRID str=Str2ID(fl->fullname,0,SID_ANY_LEN);
      StringInput_DispObject_SetText(GUIObj_GetDISPObj(myBook->text_input),str);  
      FREE_FLGUI(myBook->selectf);
    }
    delete path;
  }
}

void Self_onSelectPressed(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(myBook->selectf);

  FLIST *fl=FindFLISTtByN(item);
  if (fl) 
  {
    if ((fl->is_folder==IS_FOLDER && myBook->type==SFOLDER) || fl->is_folder==IS_FILE)
    {
      STRID str=Str2ID(fl->fullname,0,SID_ANY_LEN);
      StringInput_DispObject_SetText(GUIObj_GetDISPObj(myBook->text_input),str);
      FREE_FLGUI(myBook->selectf);
    }
  }
}

int isdir(wchar_t *name){
  W_FSTAT fs;
  if (w_fstat(name,&fs)!=-1)
    return(fs.attr&0x4000);
  else return 0;
}

GUI_LIST *CreateFileFolderSelect(MyBOOK * myBook, wchar_t *str)
{
  wchar_t *ustr=new wchar_t[256];
  wchar_t *s;
  GUI_LIST *lo;
  int n;
  STRID sid=Str2ID(myBook->type==SFOLDER?L"Select folder":L"Select file",0,SID_ANY_LEN);
  lo=CreateListObject(&myBook->book,0);
  GuiObject_SetTitleText(lo,sid);
  wstrcpy(ustr,str);
  str=ustr;
  int f=0;
  do
  {
    if (isdir(ustr))
    {
      f=1;
      break;
    }
    s=wstrrchr(ustr,L'/');
    if (s==ustr) break;
    if (s) *s=0;
  }
  while(s);
  if (!f)
    str=L"/";
  
  n=FindFiles(str,myBook->type);
  SetNumOfMenuItem(lo,n);
  SetCursorToItem(lo,0);
  ListMenu_SetOnMessages(lo,OnMessage);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,Self_onEnterPressed); 
  GUIObject_Softkey_SetText(lo,ACTION_SELECT1,Str2ID(L"Open",0,SID_ANY_LEN));
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,Self_OnBack);
  GUIObject_Softkey_SetAction(lo,1,Self_onSelectPressed); 
  GUIObject_Softkey_SetText(lo,1,Str2ID(L"Select",0,SID_ANY_LEN));
  ShowWindow(lo);
  delete ustr;
  return (lo);
}


