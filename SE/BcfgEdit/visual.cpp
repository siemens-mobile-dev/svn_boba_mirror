#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"

#include "..\\include\cfg_items.h"
#include "main.h"
#include "visual.h"

static const char EditColorGuiName[]="Gui_EditColor";
int colors[4]={0xFFFF0000,0xFF00FF00,0xFF0000FF,0x80C6AAAF};

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
    if (db->current_column==i)
      DrawRect(x1+start_column-2,y1+fsize-2,x1+start_column+column_width+2,y1+fsize+column_height+2,clBlack,clWhite);

    DrawRect(x1+start_column,y1+fsize,x1+start_column+column_width,y1+fsize+column_height,clBlack,colors[i]);
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
  GUI_Free(myBook->color);
  myBook->color=NULL;
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
  GUI_Free(myBook->color);
  myBook->color=NULL;
}

GUI_COLOR *CreateEditColorGUI(MyBOOK * myBook, COLOR_TYPE color_type, int type)
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
    char *color=color_type.char_color;
    disp_obj->r=color[0];
    disp_obj->g=color[1];
    disp_obj->b=color[2];
    disp_obj->a=color[3];   
  }
  else
  {
    unsigned int color=color_type.int_color;
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
  AddMSGHook(myBook->color,ACTION_BACK, OnBackColorEdit);
  AddMSGHook(myBook->color,ACTION_SELECT1, OnOkColorEdit);
  AddCommand(myBook->color,ACTION_SELECT1,STR("OK"));
  ShowWindow(myBook->color);
  return gui_color;
}

// --------------------------------------------------------------------------------------------------------------------------------
static const char CooordinatesGuiName[]="Gui_Coordinates";

int CoordinatesGuiOnCreate(DISP_OBJ_COORD *db)
{
  db->str_id=LGP_NULL;
  db->need_str=1;
  db->cstep=1;
  return (1);
}

void CoordinatesGuiOnClose(DISP_OBJ_COORD *db)
{
  TextFree(db->str_id);
}


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

  if (db->need_str)
  {
    snwprintf(ustr,MAXELEMS(ustr)-1,L"%d,%d",db->x,db->y);
    TextFree(db->str_id);
    db->str_id=Str2ID(ustr,0,SID_ANY_LEN);
    db->need_str=0;
  }
  DrawString(db->str_id,0, 3,scr_h-fsize-2,scr_w-4, scr_h-1,0,0,clBlack,0x00000000);
  
  old_pen=GC_GetPenColor(gc);
  GC_SetPenColor(gc,clBlack);
  GC_DrawLine(gc,db->x-3,db->y,db->x+3,db->y);
  GC_DrawLine(gc,db->x,db->y-3,db->x,db->y+3);
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

void OnBackCoordinatesEdit(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  GUI_Free(myBook->coord);
  myBook->coord=NULL;
}

void OnOkCoordinatesEdit(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  DISP_OBJ_COORD *disp_obj=(DISP_OBJ_COORD *)GUIObj_GetDISPObj(myBook->coord);
  int *coordinates=((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
  coordinates[0]=disp_obj->x;
  coordinates[1]=disp_obj->y;
  GUI_Free(myBook->coord);
  myBook->coord=NULL;
}

GUI_COORDINATES *CreateEditCoordinatesGUI(MyBOOK * myBook)
{
  GUI_COORDINATES *gui_coord=new GUI_COORDINATES;
  DISP_OBJ_COORD *disp_obj;
  int *coordinates;
  
  if (!CreateObject((GUI *)gui_coord,EditCoordinatesGui_destr,EditCoordinatesGui_constr, &myBook->book,0,0,0))
  {
    delete gui_coord;
    return 0;    
  }
  disp_obj=(DISP_OBJ_COORD *)GUIObj_GetDISPObj(gui_coord);
  coordinates=((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)));
  disp_obj->x=coordinates[0];
  disp_obj->y=coordinates[1];
  myBook->coord=(GUI *)gui_coord;
  if (myBook) addGui2book(&myBook->book,myBook->coord);
  GUI_SetStyle(myBook->coord,4);
  GuiObject_SetTitleType(myBook->coord, 1);
  GUIObject_HideSoftkeys(myBook->coord);
  AddMSGHook(myBook->coord,ACTION_BACK, OnBackCoordinatesEdit);
  AddMSGHook(myBook->coord,ACTION_SELECT1, OnOkCoordinatesEdit);
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
  win12512unicode(ustr,test_str,MAXELEMS(test_str)-1);
  db->test_str_id=Str2ID(ustr,0,SID_ANY_LEN);
  db->total_fonts=*GetFontCount();
  return (1);
}

void FontSelectGuiOnClose(DISP_OBJ_FONT_SEL *db)
{
  TextFree(db->test_str_id);  db->test_str_id=LGP_NULL;
}


void FontSelectGuiOnRedraw(DISP_OBJ_FONT_SEL *db,int ,int,int)
{
  RECT rc_old, rc_new;
  int font_old, gc_xx;
  int tfont=FONT_E_20R, tfsize=20;
  void *gc=get_DisplayGC();
  STRID selfont;
  int scr_w=Display_GetWidth(0), scr_h=Display_GetHeight(0);
  
  get_GC_RECT(gc, &rc_old);
  gc_xx=get_GC_xx(gc);
  set_GC_xx(gc,1);
  rc_new.x1=0;
  rc_new.x2=scr_w;
  rc_new.y1=0;
  rc_new.y2=scr_h;
  GC_validate_RECT(gc,&rc_new);
  font_old=SetFont(tfont);
  DrawRect(0,0,scr_w,scr_h,clBlack,clBlack);
  
  selfont=Str2ID(GetFontDesc()[db->cur_pos].name,0,9);
  DrawString(selfont,0, 3,1,scr_w-2, tfsize+1,0,0,0xFF0080FF,0x00000000);   // Рисуем выбранный шрифт в шапке меню
  TextFree(selfont);
  
  int cur_y=tfsize+2;
  for (int i=db->cur_offs; i<db->total_fonts; i++)
  {
    int font, font_h;
    font=GetFontDesc()[i].id;
    SetFont(font);
    font_h=GetImageHeight(' ');
    if (i==db->cur_pos)   // Если это выбранный шрифт
      DrawRect(1,cur_y,scr_w-1,cur_y+font_h+2,0xFF00FF00,0xFF408000);
    DrawString(db->test_str_id,0,2,cur_y+1,scr_w-2,cur_y+font_h+1,0,0,clWhite,0x00000000);
    cur_y+=font_h+2;
    if (cur_y>scr_h) break;    
  }

  GC_validate_RECT(gc,&rc_old);
  SetFont(font_old);
  set_GC_xx(gc,gc_xx);
}

void FontSelectGuiOnKey(DISP_OBJ_FONT_SEL *db,int key,int,int repeat,int type)
{
  if (type==KBD_SHORT_RELEASE || type==KBD_REPEAT)
  {
    int scr_w=Display_GetWidth(0), scr_h=Display_GetHeight(0);
    if (key==KEY_UP || key==KEY_DIGITAL_0+2)
    {
      if ((--db->cur_pos)<0) db->cur_pos=0;
      if (db->cur_pos<db->cur_offs)
      {
        db->cur_offs=db->cur_pos;
      }
    }
    else if (key==KEY_DOWN || key==KEY_DIGITAL_0+8)
    {
      if ((++db->cur_pos)>=db->total_fonts) db->cur_pos=db->total_fonts-1;
      int f=0;
      do {
        int cur_y=20+2;
        int k=0;
        for (int i=db->cur_offs; i<db->total_fonts; i++)
        {
          int old_font, font, font_h,sum;
          font=GetFontDesc()[i].id;
          old_font=SetFont(font);
          font_h=GetImageHeight(' ');
          SetFont(old_font);
          if (db->cur_pos==i) k=1;
          if ((sum=cur_y+font_h+3)<scr_h && k) f=1;
          cur_y=sum;
          if (cur_y>scr_h) break;
        }
      } while(!f && ++db->cur_offs<db->total_fonts);
    }
  }
  InvalidateRect(&db->dsp_obj,0);
}

void OnBackFontSelect(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  GUI_Free(myBook->font_select);
  myBook->font_select=NULL;
}

void OnOkFontSelect(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  DISP_OBJ_FONT_SEL *disp_obj=(DISP_OBJ_FONT_SEL *)GUIObj_GetDISPObj(myBook->font_select);
  *((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR)))=GetFontDesc()[disp_obj->cur_pos].id;
  GUI_Free(myBook->font_select);
  myBook->font_select=NULL;
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
  
  if (!CreateObject((GUI *)gui_fontsel,FontSelectGui_destr,FontSelectGui_constr, &myBook->book,0,0,0))
  {
    delete gui_fontsel;
    return 0;
  }
  disp_obj=(DISP_OBJ_FONT_SEL *)GUIObj_GetDISPObj(gui_fontsel);
  disp_obj->cur_pos=disp_obj->cur_offs=GetIdByFontId(*((int *)((char *)myBook->cur_hp+sizeof(CFG_HDR))));

  myBook->font_select=(GUI *)gui_fontsel;
  if (myBook) addGui2book(&myBook->book,myBook->font_select);
  
  GUI_SetStyle(myBook->font_select,4);
  GuiObject_SetTitleType(myBook->font_select, 1);
  GUIObject_HideSoftkeys(myBook->font_select);
  AddMSGHook(myBook->font_select,ACTION_BACK, OnBackFontSelect);
  AddMSGHook(myBook->font_select,ACTION_SELECT1, OnOkFontSelect);
  ShowWindow(myBook->font_select);
  return gui_fontsel;
}
