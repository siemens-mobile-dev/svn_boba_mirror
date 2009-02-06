#include "..\\include\Lib_Clara.h" 
#include "..\\include\Dir.h"
#include "header\structs.h"
#include "header\extern.h"
extern const PAGE_DESC bk_skin;

UI_SOFTKEY sk[3]={ {0,296,0,0x18,0,220,1,0x12}, {0,293,2,0x2C,0,115,1,0x2C}, {240,296,1,0x18,0,0,1,0x12} };

wchar_t *horiz[]={L"Lower softkey",L"Middle softkey",L"Upper softkey"};
wchar_t *vert[]={L"Left softkey",L"Middle softkey",L"Right softkey"};
wchar_t *cts[]={L"Left",L"Right",L"Center"};

int getint(char *line, int *pos,char end);
char *getparam(char *buf, char *mask, char end, int fsize);
int isSBook(BOOK *bk);
int isSoftEditBook(BOOK*);

void DrawHighlightID2(int font,int text,int ct, int XPos, int YPos, int borderColor, int NormalColor, bool clean);

void addui(char *line,int elem)
{
  int x=0;
  //--------------------
  sk[elem].x=getint(line, &x,':');
  sk[elem].y=getint(line, &x,':');
  sk[elem].ct=getint(line, &x,':');
  sk[elem].font=getint(line, &x,':');
  sk[elem].hx=getint(line, &x,':');
  sk[elem].hy=getint(line, &x,':');
  sk[elem].hct=getint(line, &x,':');
  sk[elem].hfont=getint(line, &x,';');
  //--------------------
};

int set_ui(char *buf, int fsize)
{
  int x;
  for (x=0;x<3;x++)
  {
    char pattern[128];
    sprintf(pattern,"%dsk)",x);
    char *param;
    if (param=getparam(buf,pattern,';', fsize))
    {
      addui(param,x);
      delete(param);
    }
    else
    {
      break;
    }
  }
  return 1;
};

int SkinGui_OnCreate(DISP_OBJ_SKIN *db)
{
  return 1;
};

void SkinGui_OnClose(DISP_OBJ_SKIN *db)
{
  delete(db->fonts);
};

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
};
void SkinGui_OnRedraw(DISP_OBJ_SKIN *db,int ,int,int)
{
  int width=Display_GetWidth(0);
  int height=Display_GetHeight(0);
  DrawRect(0, 0, 500, 500, clBlack,    clBlack);
  DrawRect(0, 0, width,  25, 0xAA00FF00, 0xAA00FF00);
  DrawRect(0, 0, width,  25, 0xAA00FF00, 0xAA00FF00);
  if (BookObj_GetDisplayOrientation(FindBook(isSoftEditBook))==0)
  {
    wchar_t hz[50];
    snwprintf(hz, 49, L"%d-%d", sk[db->curkey].x, sk[db->curkey].y);
    DrawHighlightID2(FONT_E_20R, Str2ID(L"SoftkeysUI", 0, SID_ANY_LEN), 2, 0, 0,  clBlack, clWhite, true);
    DrawHighlightID2(FONT_E_20R, Str2ID(vert[db->curkey], 0, SID_ANY_LEN), 0, 0, 22, clBlack, clWhite, true);
    DrawHighlightID2(FONT_E_20R, Str2ID(hz,0,SID_ANY_LEN), 0, 0, 44, clBlack, clWhite, true);
    DrawHighlightID2(FONT_E_20R, Str2ID(cts[sk[db->curkey].ct],0,SID_ANY_LEN), 0, 0, 66, clBlack, clWhite, true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press vol up/down",0,SID_ANY_LEN),1,width,24,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press joystick",0,SID_ANY_LEN),1,width,46,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press press 'C'",0,SID_ANY_LEN),1,width,68,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_20R,Str2ID(Font_GetNameByFontId(sk[db->curkey].font),0,SID_ANY_LEN),0,0,90,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press 1/3",0,SID_ANY_LEN),1,width,90,clBlack,clWhite,true);
  }
  else
  {
    wchar_t hz[50];
    snwprintf(hz,49,L"%d-%d",sk[db->curkey].hx,sk[db->curkey].hy);
    DrawHighlightID2(FONT_E_20R,Str2ID(L"SoftkeysUI", 0, SID_ANY_LEN),2,0,0,clBlack,clGreen, true);
    DrawHighlightID2(FONT_E_20R,Str2ID(horiz[db->curkey],0,SID_ANY_LEN),0,0,22,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_20R,Str2ID(hz,0,SID_ANY_LEN),0,0,44,clBlack,clWhite, true);
    DrawHighlightID2(FONT_E_20R,Str2ID(cts[sk[db->curkey].hct],0,SID_ANY_LEN),0,0,66,clBlack,clWhite, true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press vol up/down",0,SID_ANY_LEN),1,width,24,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press joystick",0,SID_ANY_LEN),1,width,46,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press press 'C'",0,SID_ANY_LEN),1,width,68,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_20R,Str2ID(Font_GetNameByFontId(sk[db->curkey].hfont),0,SID_ANY_LEN),0,0,90,clBlack,clWhite,true);
    DrawHighlightID2(FONT_E_14R,Str2ID(L"press 1/3",0,SID_ANY_LEN),1,width,90,clBlack,clWhite,true);
  }
};

int get_fontindex(DISP_OBJ_SKIN *db, int font)
{
  int x;
  for (x=0;x<db->total_fonts;x++)
  {
    if (db->fonts[x]==font)
    {
      return x;
    }
  }
  return 0;
};
void SkinGui_OnKey(DISP_OBJ_SKIN *db,int key,int,int repeat,int type)
{
  if (type==KBD_SHORT_PRESS || type==KBD_LONG_PRESS || type==KBD_LONG_RELEASE)return;
  if (type==KBD_SHORT_RELEASE) db->step=1;
  else if (type==KBD_REPEAT && repeat>10) db->step=8;
  int stat=BookObj_GetDisplayOrientation(FindBook(isSoftEditBook));
  if (key==KEY_VOL_DOWN)
  {
    if (db->curkey==0)
    {
      db->curkey=2;
    }
    else
    {
      db->curkey--;
    }
    if (stat==0)
    {
      db->cur=get_fontindex(db, sk[db->curkey].font);
    }
    else
    {
      db->cur=get_fontindex(db, sk[db->curkey].hfont);
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else if (key==KEY_VOL_UP)
  {
    if (db->curkey==2)
    {
      db->curkey=0;
    }
    else
    {
      db->curkey++;
    }
    if (stat==0)
    {
      db->cur=get_fontindex(db, sk[db->curkey].font);
    }
    else
    {
      db->cur=get_fontindex(db, sk[db->curkey].hfont);
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else if (key==KEY_UP)
  {
    if (stat==0)
    {
      sk[db->curkey].y=sk[db->curkey].y-db->step;
    }
    else
    {
      sk[db->curkey].hy=sk[db->curkey].hy-db->step;
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else if (key==KEY_DOWN)
  {
    if (stat==0)
    {
      sk[db->curkey].y=sk[db->curkey].y+db->step;;
    }
    else
    {
      sk[db->curkey].hy=sk[db->curkey].hy+db->step;;
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else if (key==KEY_LEFT)
  {
    if (stat==0)
    {
      sk[db->curkey].x=sk[db->curkey].x-db->step;
    }
    else
    {
      sk[db->curkey].hx=sk[db->curkey].hx-db->step;
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else if (key==KEY_RIGHT)
  {
    if (stat==0)
    {
      sk[db->curkey].x=sk[db->curkey].x+db->step;
    }
    else
    {
      sk[db->curkey].hx=sk[db->curkey].hx+db->step;
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else if (key==KEY_DEL)
  {
    if (stat==0)
    {
      if (sk[db->curkey].ct==2)
      {
        sk[db->curkey].ct=0;
      }
      else
      {
        sk[db->curkey].ct++;
      }
    }
    else
    {
      if (sk[db->curkey].hct==2)
      {
        sk[db->curkey].hct=0;
      }
      else
      {
        sk[db->curkey].hct++;
      }
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  else if (key-KEY_DIGITAL_0==1)
  {
    if (db->cur==0)
    {
      db->cur=db->total_fonts;
    }
    else 
    {
      db->cur--;
    }
    if (stat==0)
    {
      sk[db->curkey].font=db->fonts[db->cur];
    }
    else
    {
      sk[db->curkey].hfont=db->fonts[db->cur];
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
    else if (key-KEY_DIGITAL_0==3)
  {
    if (db->cur==db->total_fonts)
    {
      db->cur=0;
    }
    else 
    {
      db->cur++;
    }
    if (stat==0)
    {
      sk[db->curkey].font=db->fonts[db->cur];
    }
    else
    {
      sk[db->curkey].hfont=db->fonts[db->cur];
    }
    InvalidateRect((DISP_OBJ*)db,0);
  }
  SKBOOK *sb=(SKBOOK*)FindBook(isSBook);
  if (sb)
  {
    if (sb->change)
    {
      sb->change(&sk[0],0);
      sb->change(&sk[1],1);
      sb->change(&sk[2],2);
    }
  }
};

static const char SkinGuiName[]="Gui_SkinEditor";

void SkinGui_constr(DISP_DESC *desc)
{
  DISP_DESC_SetName(desc,SkinGuiName);
  DISP_DESC_SetSize(desc,sizeof(DISP_OBJ_SKIN));
  DISP_DESC_SetOnCreate(desc,(DISP_OBJ_ONCREATE_METHOD)SkinGui_OnCreate);
  DISP_DESC_SetOnClose(desc,(DISP_OBJ_ONCLOSE_METHOD)SkinGui_OnClose);
  DISP_DESC_SetOnRedraw(desc,(DISP_OBJ_ONREDRAW_METHOD)SkinGui_OnRedraw);
  DISP_DESC_SetOnKey(desc,(DISP_OBJ_ONKEY_METHOD)SkinGui_OnKey);
};

void SkinGui_destr(DISP_DESC *desc)
{
  return;
};

void SkinGui_OnBack(BOOK * bk, void *)
{
  return;
};

GUI_SKIN *CreateSkinGUI(MyBOOK *mbk)
{
  GUI_SKIN *gui_skin=new GUI_SKIN;
  if (!CreateObject((GUI*)gui_skin,SkinGui_destr, SkinGui_constr,&mbk->book,0,0,0))
  {
    delete gui_skin;
    return 0;    
  }
  if (mbk) addGui2book(&mbk->book,(GUI*)gui_skin);
  mbk->skin=(GUI *)gui_skin;
  return gui_skin;
};

void SkinGUI_OnBack(BOOK *bk, void *)
{
  BookObj_SetDisplayOrientation(bk,0);
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
};

void SkinGUI_Test(BOOK *bk, void* )
{
  MyBOOK *mbk=(MyBOOK*)bk;
  DISP_OBJ_SKIN *db=(DISP_OBJ_SKIN*)GUIObj_GetDISPObj(mbk->skin);
  if (BookObj_GetDisplayOrientation(bk)==0)
  {
    BookObj_SetDisplayOrientation(bk,1);
    GUIObject_Softkey_SetText(mbk->skin,0,Str2ID(L"Normal",0,SID_ANY_LEN));
    db->cur=get_fontindex(db, sk[db->curkey].hfont);
  }
  else
  {
    BookObj_SetDisplayOrientation(bk,0);
    GUIObject_Softkey_SetText(mbk->skin,0,Str2ID(L"Landscape",0,SID_ANY_LEN));
    db->cur=get_fontindex(db, sk[db->curkey].font);
  }
  BookObj_Hide(bk,0);
  BookObj_Show(bk,0);
};

void SkinGUI_Dummy(BOOK *bk, void *)
{
  return;
};
int Skin_OnEnter(void *, BOOK * bk)
{
  GUI * skin_gui=(GUI *)CreateSkinGUI((MyBOOK*)bk);
  bool pl;
  switch(GetChipID())
  {
  case 0x7100:
  case 0x8000:
  case 0x8040:
    GUI_SetStyle(skin_gui,3);
    pl=0;
    break;
  case 0x9900:
    GUI_SetStyle(skin_gui,4);
    pl=1;
    break;
  }
  GuiObject_SetTitleType(skin_gui, 1);
  GUIObject_Softkey_SetAction(skin_gui,ACTION_BACK, SkinGUI_OnBack);
  GUIObject_Softkey_SetAction(skin_gui,0,SkinGUI_Test);
  if (BookObj_GetDisplayOrientation(bk)==0)
  {
    GUIObject_Softkey_SetText(skin_gui,0,Str2ID(L"Landscape",0,SID_ANY_LEN));
  }
  else
  {
    GUIObject_Softkey_SetText(skin_gui,0,Str2ID(L"Normal",0,SID_ANY_LEN));
  }
  GUIObject_SoftKey_SetEnable(skin_gui,0,pl);
  GUIObject_Softkey_SetAction(skin_gui,1,SkinGUI_Dummy);
  GUIObject_Softkey_SetText(skin_gui,1,Str2ID(L"-test-",0,SID_ANY_LEN));
  
  DISP_OBJ_SKIN *db=(DISP_OBJ_SKIN*)GUIObj_GetDISPObj(skin_gui);
  db->curkey=0;
  db->setting=0;
  db->total_fonts=*GetFontCount();
  db->fonts=new u16[db->total_fonts];
  for (int i=0; i<db->total_fonts; i++)
  {
    db->fonts[i]=GetFontDesc()[i].id;
  }
  db->cur=0;
  db->step=1;
  ShowWindow(skin_gui);
  BookObj_Hide(bk,0);
  BookObj_Show(bk,0);
  return (1);
};

int Skin_OnExit(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  if (mbk->skin)GUI_Free((GUI*)mbk->skin);
  mbk->skin=0;
  return (1);
};

const PAGE_MSG bk_msglst_skin[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,            Skin_OnEnter,
  PAGE_EXIT_EVENT_TAG,             Skin_OnExit,
  NIL_EVENT_TAG,                   NULL
};

const PAGE_DESC bk_skin = {"SoftEdit_SkinEditor",0,bk_msglst_skin};


