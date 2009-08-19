#include "header\mem.h"
#include "..\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "header\cl.h"
#include "header\accelctrl.h"
#include "header\wdisplay.h"
#include "header\images.h"
#include "..\\include\cfg_items.h"
#include "header\conf_loader.h"
#include "header\config_data.h"
#include "header\skin_data.h"
#include "header\skin_loader.h"

#include "header\rus.h"
#include "header\eng.h"


#define PROJECT_ABOUT "WalkmanDisplay v2.1\n� Fernandodks, UltraShot"
#define PROJECT_NAME "WalkmanDisplay"
#define PROJECT_BASEPAGE_NAME "WalkmanDisplay_Base_Page"

#define MP_STATUS_STOP 0
#define MP_STATUS_PLAY 1 
#define MP_STATUS_PAUSE 2
#define MP_STATUS_FORWARD 3
#define MP_STATUS_REWIND 5

DISP_DESC *Softkey_DD;
DISP_OBJ *Softkey_DO;
DISP_OBJ_ONREDRAW_METHOD Softkey_DefaultRedraw;


#ifndef NDEBUG
PROCESS proc_bench;
int bench_n=0;
#endif

extern MyBOOK *MyBook;
extern wchar_t playview_h;
extern wchar_t browserview_h;
extern wchar_t playnow_h;
extern wchar_t imageID;
extern wchar_t cover_h_;
extern wchar_t cover_v_;

extern wchar_t playicn;
extern wchar_t stopicn;
extern wchar_t pauseicn;
extern MUSIC nowmusic;

extern wchar_t v_MPA_ID;
extern bool accel_wrk;

bool RedrawUsed=false;
TRACK_DESC *currenttrack=0;
wchar_t intFolder[512];
wchar_t extFolder[512];
DISP_OBJ* g_DO=0;
CAccelCtrl *AccelCtrl=0;
bool working=false;
bool accel_wrk=true;
int curmode=2;
wchar_t v_MPA_ID=0xFFFF;
int StatusSize=18;
int SoftSize=26;
int ScreenHeight;
int ScreenWidth;
bool ELF_KILL_RECEIVED=false;
wchar_t skin_str[512];
bool inMenu=false;
wchar_t *modes[]={
  L"Stereo",
  L"Joint Stereo",
  L"Dual Channel",
  L"Single Channel"
};

void decoder_Init(wchar_t *path, wchar_t *name);
void TrackDesc_Free(TRACK_DESC *track_desc);
TRACK_DESC * TrackDesc_Get(BOOK *bk);
void onAccel(void*,int,int);
void init_resources(wchar_t*);
void deinit_resources();
int NewKey(int key,int,int mode);
GUI *FindGuiInBook(BOOK *bk, char *name);
void walkman_Redraw(DISP_OBJ* DO,int a,int b,int c);
void set_vertical();
void set_horizontal();

typedef int (*GetInfo)(void *, int id, wchar_t *path, int bufsize1, wchar_t *fname, int bufsize2);
typedef struct
{
  char dummy[0x2C];
  GetInfo GetINFO;
}CSkinManager_Table;

typedef struct
{
  CSkinManager_Table *SkinManager_Table;
}CSkinManager;

typedef struct
{
  char dummy[0xB0];
  CSkinManager *SkinManager;
}WBOOK;

#pragma pack(1)


typedef struct
{
  char dummy6[0x70];
  int artist_str;
  int title_str;
  int album_str;
  int fulltime;
  char dummy7[8];
  int currenttime;
}MM_INFO;
typedef struct
{
  char dummy[0xB6];
  char dummy2[0xC];
  char status;
  char dummy3[0x11];
  wchar_t artist_icn;
  wchar_t title_icn;
  wchar_t album_icn;
  char dummy4[0x74];
  wchar_t prev_icn;
  wchar_t prev_icn2;
  wchar_t next_icn;
  wchar_t next_icn2;
  wchar_t play_icn;
  wchar_t stop_icn;
  char dummy5[0x42];
  MM_INFO mm_info;
}DISP_OBJ_;

typedef struct
{
  char dummy[0x126];
  char dummy2[0x20];
  wchar_t up;
  wchar_t down;
}ARROWS;

typedef struct
{
  char dummy[0x19];
  char status;
  char dummy_[0x1A];
  char preset;
  char dummy2[0x27];
  wchar_t pos;
  char dummy3[14];
  BOOK_PLAYER_TRACK_DESC *dsc;
  char dummy5[2];
  wchar_t pos2;
  wchar_t pos3;
  char dummy6[22];
  int tracks_count;
  char dummy7[8];
  char RepeatMode;
  char ShuffleMode;
  char unk_[6];
  char StereoWidening;
}WALKMAN_BOOK;

typedef struct
{
  char dummy[0x22B];
  char status;
}DISP_OBJ_NOWPLAYING;

#pragma pack()
void elf_exit(void)
{
  trace_done();
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
};

class CMyBook:public CBook
{
public:
  CMyBook();
  void onClose();
};


#pragma optimize = s 9
__EVENT( TerminateElf )
{
  ELF_KILL_RECEIVED=true;
  if (BOOK*bk=FindBook(isAudioPlayerBook()))
  {
    BookObj_KillBook(bk);
  }
  ((CMyBook*)book)->destroy();
  return(1);
};

#pragma optimize = s 9
__EVENT( ShowAuthorInfo )
{
  MessageBox(0x6fFFFFFF,STR(PROJECT_ABOUT),0, 1 ,5000, ((BOOKMANMSG*)msg)->bookmanbook);
  return(1);
};

void Send_REDRAW_RELEASE();
void ReturnRedraw();

bool TrackDesc_Compare(TRACK_DESC *t1, TRACK_DESC *t2);

__EVENT( onPlayerClosed )
{
  if (ELF_KILL_RECEIVED)
    return 0;
  ModifyKeyHook(NewKey, 0);
  if (RedrawUsed)
  {
    Send_REDRAW_RELEASE();
    ReturnRedraw();
  }
  curmode=VERT;
  deinit_resources();
  working=false;
  if (AccelCtrl)
  {
    if (AccelCtrl->getstatus()==ACC_ACTIVE)
    {
      if (use_accel!=2)
        AccelCtrl->AddHook(onAccel,0);
    }
    delete(AccelCtrl);
    AccelCtrl=0;
  }
  if (currenttrack)
    TrackDesc_Free(currenttrack);
  currenttrack=0;
  g_DO=0;
  return 0;
};

DISP_OBJ_ONREDRAW_METHOD oldredr;

#pragma optimize = z 9
__EVENT( onPlayerOpened )
{
  if (working)
  {
    if (FindBook(isAudioPlayerBook()))
    {
      DISP_OBJ *DO=(DISP_OBJ*)msg;
      g_DO=DO;
      DISP_DESC_SetOnRedraw(DISP_OBJ_GetDESC(DO), walkman_Redraw);
    }
  }
  return 0;
};

#pragma optimize = s 9
__NEEDED_BOOK(isKeylockBook)
{
  if (!strcmp(book->xbook->name, "ScreensaverBook"))
    return 1;
  return 0;
};


__EVENT(onKeylock)
{
  if (working)
  {
    BOOK *bk=FindBook(isAudioPlayerBook());
    if (bk==DISPLAY_GetTopBook(0) && curmode==HOR)
    {
      if (DISPBASE_GetFocused(0)!=g_DO)
      {
        MediaPlayer_ShowNowPlaying(g_DO,1);
      }
      GUI *g=FindGuiInBook(bk,"MediaPlayer_Audio");
      DISP_OBJ_WALKMAN *d=(DISP_OBJ_WALKMAN*)GUIObj_GetDISPObj(g);
      GUI_SetStyle(g,2);
      d->imageID=v_MPA_ID;
      GuiObject_SetTitleType(g, 0);
      GUIObject_ShowSoftkeys(g);
      InvalidateRect(GUIObj_GetDISPObj(g),0);
      
      
      BookObj_Hide(bk,0);
      BookObj_Show(bk,0);
      
      curmode=VERT;
    }
  }
  return 0;
};

__EVENT( onPlay )
{
  if (working)
  {
    if (FindBook(isAudioPlayerBook()))
    {
      TRACK_DESC *ntrack=TrackDesc_Get(FindBook(isAudioPlayerBook()));
      if (TrackDesc_Compare(currenttrack,ntrack)==false)
      {
        TrackDesc_Free(currenttrack);
        currenttrack=ntrack;
        decoder_Init(currenttrack->path,currenttrack->name);
        nowmusic.hdr.LengthInSeconds=currenttrack->fulltime;
      }
      else
      {
        TrackDesc_Free(ntrack);
      }
      if (g_DO)
        InvalidateRect(g_DO,0);
    }
  }
  return 0;
};

__EVENT( onUnlock )
{
  if (working)
  {
    if (FindBook(isAudioPlayerBook()))
    {
      if (BookObj_GetDisplayOrientation(FindBook(isAudioPlayerBook())))
      {
        set_horizontal();
      }
    }
  }
  return 0;
};

__EVENT( onReconfig )
{
    // �������������������
  RECONFIG_EVENT_DATA *reconf=(RECONFIG_EVENT_DATA *)msg;
  debug_printf("\nWalkmanDisplay: onReconfig: %ls/%ls asked. Our skin bcfg: %ls/%ls\n", reconf->path,reconf->name,successed_skin_path,successed_skin_name);
  int result=0;
  // ���� ������ ���, ���������� ���
  if (wstrcmpi(reconf->path,successed_config_path)==0 && wstrcmpi(reconf->name,successed_config_name)==0)
  {
    InitConfig();
    result=1;
  }
  if (wstrcmpi(reconf->path,successed_skin_path)==0 && wstrcmpi(reconf->name,successed_skin_name)==0)
  {
    skin_InitConfig(reconf->path, 1);
    result=1;
  }
  return(result);
};


#define EVENT_HOOK(a,b) a,b,
#define EVENT_HOOK_TABLE(x) const PAGE_MSG x[]@ "DYN_PAGE" = {
#define EVENT_HOOK_TABLE_END 0,0}

__EVENT ( onStyleChanged )
{
  if (working)
  {
    WBOOK *wb=(WBOOK*)FindBook(isAudioPlayerBook());
    if (wb)
    {
      wchar_t *str=new wchar_t[0x100];
      wchar_t *str2=new wchar_t[0x100];
      GetInfo SkinManager_GetSkinInfo=wb->SkinManager->SkinManager_Table->GetINFO;
      
      SkinManager_GetSkinInfo(wb->SkinManager,1,str,0xFF,str2,0xFF);
      if (wstrcmp(skin_str,str)!=0)
      {
        wstrcpy(skin_str,str);
        skin_InitConfig(skin_str, 0);
        deinit_resources();
        init_resources(skin_str);
        debug_printf("\nWalkmanDisplay: onStyleChanged. New skin: %ls\n", skin_str);
      }
      delete(str);
      delete(str2);
    }
  }
  return 0;
};

void ReturnRedraw()
{ 
  if (RedrawUsed==true)
  {
    RedrawUsed=false;
    DISP_DESC_SetOnRedraw(Softkey_DD, Softkey_DefaultRedraw);
    debug_printf("\nWalkmanDisplay: own redraw deinstalled\n");
  }
};


void softredraw(DISP_OBJ*,int,int,int);
int SB_ELF_Killed(void *mess ,BOOK* book)
{
  REDRAW_RELEASE_MESSAGE * sbm=(REDRAW_RELEASE_MESSAGE*)mess;
  REDRAW_RELEASE_MESSAGE *res=0;
  if (sbm->SK_NewOnRedraw==Softkey_DefaultRedraw)
  {
    if (!res)
    {
      res=new REDRAW_RELEASE_MESSAGE;
      memcpy(res,sbm,sizeof(REDRAW_RELEASE_MESSAGE));
    }
    if (sbm->SK_OldOnRedraw) Softkey_DefaultRedraw=sbm->SK_OldOnRedraw;
    DISP_DESC_SetOnRedraw(DISP_OBJ_GetDESC(Softkey_DO),softredraw);
    res->SK_OldOnRedraw=0;
    res->SK_NewOnRedraw=softredraw;
  }
  if (res)
  {
    UI_Event_wData(SBY_REDRAW_RELEASE_EVENT ,res,(void (*)(void*))mfree_adr());
    return 1;
  }
  return 0;
};

__EVENT( onBcfgConfig )
{
  if (successed_skin_path && successed_skin_name)
  {
    FILEITEM *fli=FILEITEM_Create();
    FILEITEM_SetPathAndContentType(fli,successed_skin_path);
    FILEITEM_SetFnameAndContentType(fli,successed_skin_name);
    int bookid=BOOK_GetBookID(&MyBook->bk);
    SUB_EXECUTE*se=DataBrowser_CreateSubExecute(bookid, fli);
    DataBrowser_ExecuteSubroutine(se,1,0);
    DataBrowser_ExecuteSubroutine(se,0x2D,0);
    debug_printf("\nWalkmanDisplay: onBcfgConfig: executing data browser's subroutine for %ls/%ls\n", successed_skin_path, successed_skin_name);
    FILEITEM_Destroy(fli);
  }
  else
  {
    MessageBox(0x6FFFFFFF,Str2ID(lng_msgSkinNotLoaded,0,SID_ANY_LEN),0,1,0,0);
  }
  return 1;
}

u16 timer;
void onTimer(u16 timerID, LPARAM lparam)
{
  BOOK *bk=FindBook(isAudioPlayerBook());
  if (bk)
  {
    if (BookObj_GetDisplayOrientation(bk))
    {
      InvalidateRect(g_DO,(RECT*)&volrect);
    }
  }
  Timer_Kill(&timer);
  timer=0;
};
__EVENT( onVolumeChange )
{
  if (working && g_DO)
  {
    BOOK *bk=FindBook(isAudioPlayerBook());
    if (bk)
    {
      if (BookObj_GetDisplayOrientation(bk))
      {
        InvalidateRect(g_DO,(RECT*)&volrect);
        if (timer==0)
          timer=Timer_Set(3000, onTimer, 0);
        else
          Timer_ReSet(&timer, 3000, onTimer, 0);
      }
    }
  }
  return 0;
};

//0x29CF - UI_MEDIAPLAYER_VISUALIZATION_OPTIONS_CHANGED_EVENT
EVENT_HOOK_TABLE(BasePageEvents)
  EVENT_HOOK(ELF_RECONFIG_EVENT, onReconfig)
  EVENT_HOOK(ELF_TERMINATE_EVENT , TerminateElf)
  EVENT_HOOK(ELF_SHOW_INFO_EVENT  , ShowAuthorInfo)
  EVENT_HOOK(SBY_REDRAW_RELEASE_EVENT  , SB_ELF_Killed)
  EVENT_HOOK(UI_MEDIAPLAYER_DESTROYED_EVENT_TAG , onPlayerClosed)
  EVENT_HOOK(UI_MEDIAPLAYER_PLAY_PRESSED_EVENT_TAG, onPlay)
  EVENT_HOOK(VOLUMEUPKEY_SHORT_PRESS_EVENT_TAG, onVolumeChange)
  EVENT_HOOK(VOLUMEDOWNKEY_SHORT_PRESS_EVENT_TAG, onVolumeChange)
  EVENT_HOOK(SESSION_DESTROYED_EVENT_TAG,onStyleChanged)
  EVENT_HOOK(993, onPlayerOpened)
  EVENT_HOOK(994, onBcfgConfig)
  EVENT_HOOK(UI_KEYLOCK_ACTIVATE_EVENT_TAG, onKeylock)
  EVENT_HOOK(UI_KEYLOCK_DEACTIVATE_EVENT_TAG, onUnlock)
EVENT_HOOK_TABLE_END;

const PAGE_DESC base_page ={PROJECT_BASEPAGE_NAME,0,BasePageEvents};

#pragma optimize = s 9
GUI *FindGuiInBook(BOOK *bk, char *name)
{
  LIST *lst=bk->xguilist->guilist;
  for (int x=0;x<lst->FirstFree;x++)
  {
    GUI * g=(GUI*)ListElement_GetByIndex(lst,x);
    if (strcmp(DISP_OBJ_GetName(GUIObj_GetDISPObj(g)),name)==0)
      return g;
  }
  return 0;
};

#pragma optimize = z 9
void set_horizontal()
{
  if (working)
  {
    debug_printf("\nWalkmanDisplay: horizontal mode setted\n");
    BOOK *mybook=FindBook(isAudioPlayerBook());
    if (DISPLAY_GetTopBook(0)==mybook)
    {
      BookObj_SetDisplayOrientation(mybook, 1);
      
      GUI *g=FindGuiInBook(mybook,"MediaPlayer_Audio");
      DISP_OBJ_WALKMAN *d=(DISP_OBJ_WALKMAN*)GUIObj_GetDISPObj(g);
      d->imageID=playview_h;
      GUI_SetStyle(g,4);
      GuiObject_SetTitleType(g, 1);
      InvalidateRect(GUIObj_GetDISPObj(g),0);
      
      BookObj_Hide(mybook, 0);
      BookObj_Show(mybook, 0);
    }
    curmode=HOR;
  }
};

#pragma optimize = z 9
void set_vertical()
{
  if (working)
  {
    debug_printf("\nWalkmanDisplay: vertical mode setted\n");
    BOOK *mybook=FindBook(isAudioPlayerBook());
    if (DISPLAY_GetTopBook(0)==mybook)
    {
      MediaPlayer_ShowNowPlaying(g_DO, 1);
      BookObj_SetDisplayOrientation(mybook, 0);
      
      GUI *g=FindGuiInBook(mybook,"MediaPlayer_Audio");
      DISP_OBJ_WALKMAN *d=(DISP_OBJ_WALKMAN*)GUIObj_GetDISPObj(g);
      d->imageID=v_MPA_ID;
      GUI_SetStyle(g,2);
      GuiObject_SetTitleType(g, 1);
      
      InvalidateRect(GUIObj_GetDISPObj(g),0);
      BookObj_Hide(mybook, 0);
      BookObj_Show(mybook, 0);
      curmode=VERT;
    }
  }
};

#pragma optimize = z 9
void onAccel(void* cac, int x, int y)
{
  CAccelCtrl*cac_=(CAccelCtrl*)cac;
  if(working && accel_wrk && isKeylocked()==0)
  {
    int mode=cac_->getorientation(x,y);
    WALKMAN_BOOK *wb=(WALKMAN_BOOK*)FindBook(isAudioPlayerBook()); 
    if (wb->status==MP_STATUS_FORWARD || wb->status==MP_STATUS_REWIND)
    {
      return;
    }
    if (mode==HOR && curmode!=HOR)
    {
      set_horizontal();
    }
    else if (mode==VERT && curmode!=VERT)
    {
      set_vertical();
    }
  }
};

#pragma optimize = z 9
void DrawLine(int font,wchar_t *NAME,int CEN, int XPos, int YPos, int XPos2, int YPos2, int NormalColor, int borderColor, bool clean, int type)
{
  if (!NAME)
    return;
  int width=0;
  int strID=Str2ID(NAME,0,SID_ANY_LEN);
  SetFont(font);
  if (CEN==2)
  {
    XPos=-500;
    width=Display_GetWidth(0);
    XPos2=width+500;
  }
  else if (CEN==0)
  {
    XPos2=XPos+500;
  }
  else if (CEN==1)
  {
    XPos=-500;
  }
  if (type==0)
  {
    DrawString(strID,CEN,XPos-1,YPos-1,XPos2-1,YPos2-1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos-1,XPos2,YPos2-1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos+1,YPos-1,XPos2+1,YPos2-1,0,0,borderColor,borderColor);
    
    DrawString(strID,CEN,XPos-1,YPos,XPos2-1,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos+1,YPos,XPos2+1,YPos2,0,0,borderColor,borderColor);
    
    DrawString(strID,CEN,XPos-1,YPos+1,XPos2-1,YPos2+1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos+1,XPos2,YPos2+1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos+1,YPos+1,XPos2+1,YPos2+1,0,0,borderColor,borderColor);
  }
  else if (type==1)
  {
    DrawString(strID,CEN,XPos+1,YPos,XPos2,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos+1,XPos2,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos-1,YPos,XPos2,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos-1,XPos2,YPos2,0,0,borderColor,borderColor);
  }
  else if (type==2)
  {
    DrawString(strID,CEN,XPos+1,YPos+1,XPos2+1,YPos2+1,0,0,borderColor,borderColor);
  }
  
  DrawString(strID,CEN,XPos,YPos,XPos2,YPos2,0,1,NormalColor,NormalColor);
  
  if (clean)
    TextFree(strID);
};

#pragma optimize = z 9
void DrawLine2(int font, int strID,int CEN, int XPos, int YPos, int XPos2, int YPos2, int NormalColor, int borderColor, int clean, int type)
{
  int width=0;
  SetFont(font);
    if (CEN==2)
  {
    XPos=-500;
    width=Display_GetWidth(0);
    XPos2=width+500;
  }
  else if (CEN==0)
  {
    XPos2=XPos+500;
  }
  else if (CEN==1)
  {
    XPos=-500;
  }
  if (type==0)
  {
    DrawString(strID,CEN,XPos-1,YPos-1,XPos2-1,YPos2-1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos-1,XPos2,YPos2-1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos+1,YPos-1,XPos2+1,YPos2-1,0,0,borderColor,borderColor);
    
    DrawString(strID,CEN,XPos-1,YPos,XPos2-1,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos+1,YPos,XPos2+1,YPos2,0,0,borderColor,borderColor);
    
    DrawString(strID,CEN,XPos-1,YPos+1,XPos2-1,YPos2+1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos+1,XPos2,YPos2+1,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos+1,YPos+1,XPos2+1,YPos2+1,0,0,borderColor,borderColor);
  }
  else if (type==1)
  {
    DrawString(strID,CEN,XPos+1,YPos,XPos2,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos+1,XPos2,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos-1,YPos,XPos2,YPos2,0,0,borderColor,borderColor);
    DrawString(strID,CEN,XPos,YPos-1,XPos2,YPos2,0,0,borderColor,borderColor);
  }
  else if (type==2)
  {
    DrawString(strID,CEN,XPos+1,YPos+1,XPos2+1,YPos2+1,0,0,borderColor,borderColor);
  }
  DrawString(strID,CEN,XPos,YPos,XPos2,YPos2,0,0,NormalColor,NormalColor);
  
  if (clean)
    TextFree(strID);
};

#pragma optimize = z 9
void DrawProgress(int bColor, int cColor, int oColor, RECT rect, int progress, int time, signed int PlusY, int type)
{
  // ������� �����������
  if (type)
  {
    DrawRect(rect.x1+1,rect.y1+PlusY,rect.x2-1,rect.y1+PlusY+1, bColor, bColor);
    DrawRect(rect.x2-1,rect.y1+PlusY,rect.x2,rect.y2+PlusY, bColor, bColor);
    
    DrawRect(rect.x1,rect.y1+PlusY,rect.x1+1,rect.y2+PlusY, bColor, bColor);
    DrawRect(rect.x1+1,rect.y2+PlusY-1,rect.x2-1,rect.y2+PlusY, bColor, bColor);
  }
  DrawRect(rect.x1+ (type ? 1 : 0),rect.y1+(type ? 1 : 0)+PlusY,rect.x2-(type ? 1 : 0),rect.y2-(type ? 1 : 0)+PlusY, oColor, oColor);
  float Length;
  float CurLength=(float)progress;
  float inLength=(float)time;
  float Procent=(float)CurLength/inLength;
  float size=(float)((float)rect.x2-(float)rect.x1);
  float start = (float)rect.x1;
  Length = start+((Procent)*size);
  DrawRect(rect.x1+(type ? 1 : 0), rect.y1+(type ? 1 : 0)+PlusY,(int)Length - (type ? 1 : 0),rect.y2-(type ? 1 : 0)+PlusY,cColor,cColor);
};

int isVolCtrl(BOOK *bk)
{
  if (isVolumeControllerBook(bk))
    return 1;
  return 0;
};

#define BENCH_START bench_n=0;
#define BENCH_GET bench_n
#pragma optimize= z 9
void walkman_Redraw(DISP_OBJ* DO,int a,int b,int c)
{
#ifndef NDEBUG
  BENCH_START
#endif
#define putchar_(a,b,c,d,e,f) if (f && f!=0xFFFF) \
                              putchar(a,b,c,d,e,f)
  
    BOOK *audio=FindBook(isAudioPlayerBook());
  void *GC=get_DisplayGC();
  int orient=BookObj_GetDisplayOrientation(audio);
  if (!currenttrack)
  {
    currenttrack=TrackDesc_Get(audio);
    decoder_Init(currenttrack->path,currenttrack->name);
    nowmusic.hdr.LengthInSeconds=currenttrack->fulltime;
  }
  if (working)
  {
      //-------------------------------------------------------------------
      //checking for closed menu and setting correct horizontal mode, if needed
      if (inMenu==true)
      {
        accel_wrk=true;
        inMenu=false;
        if (curmode==HOR)
        {
          set_horizontal();
          BookObj_SetDisplayOrientation(audio,1);
        }
      }
      //-------------------------------------------------------------------
      g_DO=DO;
      DispObject_SetLayerColor(DO, 0);
      //-------------------------------------------------------------------
  }
  //-------------------------------------------------------------------
  if (working==false)
  { 
    //-------------------------------------------------------------------
    //Create new accelerometer
    if (!AccelCtrl)
      AccelCtrl=new CAccelCtrl();
    if (use_accel!=2)
    {
      AccelCtrl->AddHook(onAccel,1);
    }
    //-------------------------------------------------------------------
    WBOOK *wb=(WBOOK*)audio;
    wchar_t *str=new wchar_t[0x100];
    wchar_t *str2=new wchar_t[0x100];
    if (wb->SkinManager)
    {
      if (wb->SkinManager->SkinManager_Table)
      {
        if (wb->SkinManager->SkinManager_Table->GetINFO)
        {
          GetInfo SkinManager_GetSkinInfo=wb->SkinManager->SkinManager_Table->GetINFO;
          
          SkinManager_GetSkinInfo(wb->SkinManager,1,str,0xFF,str2,0xFF);
          wstrcpy(skin_str,str);
          skin_InitConfig(skin_str, 0);
          deinit_resources();
          init_resources(skin_str);
          goto L_SkinManager_Detected;
        }
      }
    }
    MessageBox(0x6FFFFFFF,Str2ID(lng_msgNotSupported,0,SID_ANY_LEN),0,1,0,0);
    UI_Event_toBookID(ELF_TERMINATE_EVENT,BOOK_GetBookID(&MyBook->bk));
    return;
  L_SkinManager_Detected:
    delete(str);
    delete(str2);
    //-------------------------------------------------------------------
    //Fixing gui options and setting correct imageID inside, if needed
    GUI *g=FindGuiInBook(audio,"MediaPlayer_Audio");
    DISP_OBJ_WALKMAN *d=(DISP_OBJ_WALKMAN*)GUIObj_GetDISPObj(g);
    if (orient==0)
    {
      if (d->imageID!=0xFFFF)
        v_MPA_ID=d->imageID;
    }
    GUI_SetStyle(g,2);
    GuiObject_SetTitleType(g, 0);
    if (cfg_orient==1)
      set_horizontal();
    InvalidateRect(GUIObj_GetDISPObj(g),0);
    //-------------------------------------------------------------------
    working=true;
  }

  //-------------------------------------------------------------------
  //Use standart player redraw
  if (orient==0)
  {
    orient=1;
    if (standartwnd)
    {
      oldredr(DO, a, b, c);
      return;
    }
  }  
  else
    orient=0;
  //-------------------------------------------------------------------
  //If keylock is on top, need to draw vertical orientation
  if (DISPLAY_GetTopBook(0)!=audio)
  {
    orient=1;
    if (standartwnd)
    {
      oldredr(DO, a, b, c);
      return;
    }
  }
  if (orient==0)/*>0 && DISPLAY_GetTopBook(0)==audio)*/
  {
    //typedef void (*T)(wchar_t imageID, int x, int y, int a, int black, int white);
    //T putc=(T)(0x4535DC18+1);
    //putc(playnow_h, 0, 0, a, clBlack, clWhite);
    putchar_(GC,0,0,0,0,playnow_h);
  }
  //-------------------------------------------------------------------
  WALKMAN_BOOK *wb=(WALKMAN_BOOK*)audio;
  DISP_OBJ_ *do_=(DISP_OBJ_*)DO;
  
  RECT gc_rc,rc;
  get_GC_RECT(GC, &gc_rc);
  rc.y1 = gc_rc.y1;
  rc.y2 = gc_rc.y2;
  putchar_(GC, 
           orient ? cover_rect.x1 : cover_rect_h.x1, 
           orient ? cover_rect.y1 - StatusSize : cover_rect_h.y1,
           orient ? cover_rect.x2-cover_rect.x1 : cover_rect_h.x2-cover_rect_h.x1,
           orient ? cover_rect.y2-cover_rect.y1 : cover_rect_h.y2-cover_rect_h.y1,
           orient ? cover_v_ : cover_h_);
    
  putchar_(GC, 
           orient ? cover_rect.x1 : cover_rect_h.x1, 
           orient ? cover_rect.y1 - StatusSize : cover_rect_h.y1,
           orient ? cover_rect.x2-cover_rect.x1 : cover_rect_h.x2-cover_rect_h.x1,
           orient ? cover_rect.y2-cover_rect.y1 : cover_rect_h.y2-cover_rect_h.y1,
           nowmusic.meta.ImageID);
  MM_INFO *mm_info=&((DISP_OBJ_*)DO)->mm_info;
#define PHONE_AUTO 0
#define PHONE_W580 1
#define PHONE_W610 2
  if (phone==PHONE_W610)
    mm_info=(MM_INFO*)(((int)(mm_info))+0xC);
  else if (phone==PHONE_AUTO)
  {
    int SaveConfigData(const wchar_t *path,const wchar_t *fname);
    if (mm_info->title_str>0x80000000 && mm_info->title_str<0x90000000)
    {
      int *integer=(int*)&phone;
      integer[0]=PHONE_W580;
      MessageBox(0x6FFFFFFF,Str2ID(lng_msgW580detected,0,SID_ANY_LEN),0,1,0,0);
    }
    else
    {
      int *integer=(int*)&phone;
      integer[0]=PHONE_W610;
      MessageBox(0x6FFFFFFF,Str2ID(lng_msgW610detected,0,SID_ANY_LEN),0,1,0,0);
    }
    SaveConfigData(successed_config_path,successed_config_name);
  }
  int cur=mm_info->currenttime;
  
  //----------------------------------------------------------------------------
  //Progressbar
  if (orient ? ProgressBar_Show : ProgressBar_Show_h)
  {
    DrawProgress(orient ? ProgressBar_bColor : ProgressBar_bColor_h, 
                 orient ? ProgressBar_cColor : ProgressBar_cColor_h, 
                 orient ? ProgressBar_oColor : ProgressBar_oColor_h, 
                 orient ? pbrect : pbrect_h, 
                 cur, 
                 mm_info->fulltime,
                 orient ? - StatusSize : 0, 
                 orient ? ProgressBar_highlight : ProgressBar_highlight_h);
  }
  
  //----------------------------------------------------------------------------
  //Volumebar
  if (orient==0)
  {
    typedef struct
    {
      char dummy[0x2C];
      wchar_t volume;
    }VOLCONTR;
    VOLCONTR *book=(VOLCONTR*)FindBook(isVolCtrl);
    if (book)
    {
      if (book->volume && VolumeBar_Show)
      {
        DrawProgress(VolumeBar_bColor, 
                     VolumeBar_cColor, 
                     VolumeBar_oColor, 
                     volrect, 
                     book->volume, 
                     15,
                     0, 
                     VolumeBar_highlight);
      }
    }
  }
  //----------------------------------------------------------------------------
  wchar_t buf[50];
  //----------------------------------------------------------------------------
  //Artist icon
  if (orient ? aricn_show : aricn_show_h)
  {
    putchar_(GC, 
            orient ? aricn_rect.x1 : aricn_rect_h.x1, 
            orient ? aricn_rect.y1 - StatusSize : aricn_rect_h.y1,
            orient ? aricn_rect.x2 - aricn_rect.x1 : aricn_rect_h.x2 - aricn_rect_h.x1,
            orient ? aricn_rect.y2 - aricn_rect.y1 : aricn_rect_h.y2 - aricn_rect_h.y1,
            ICON(main_icons[ICN_ARTIST],do_->artist_icn));
  }
  //----------------------------------------------------------------------------
  //Album icon
  if (orient ? alicn_show : alicn_show_h)
  {
    putchar_(GC, 
            orient ? alicn_rect.x1 : alicn_rect_h.x1, 
            orient ? alicn_rect.y1 - StatusSize: alicn_rect_h.y1,
            orient ? alicn_rect.x2 - alicn_rect.x1 : alicn_rect_h.x2 - alicn_rect_h.x1,
            orient ? alicn_rect.y2 - alicn_rect.y1 : alicn_rect_h.y2 - alicn_rect_h.y1,
            ICON(main_icons[ICN_ALBUM],do_->album_icn));
  }
  //----------------------------------------------------------------------------
  //Title icon
  if (orient ? ticn_show : ticn_show_h)
  {
    putchar_(GC, 
            orient ? ticn_rect.x1 : ticn_rect_h.x1, 
            orient ? ticn_rect.y1 - StatusSize : ticn_rect_h.y1,
            orient ? ticn_rect.x2 - ticn_rect.x1 : ticn_rect_h.y2 - ticn_rect_h.y1,
            orient ? ticn_rect.y2 - ticn_rect.y1 : ticn_rect_h.y2 - ticn_rect_h.y1,
            ICON(main_icons[ICN_TITLE],do_->title_icn));
  }
  
  //----------------------------------------------------------------------------
  //Equalizer preset icon
  if (orient ? eq_preset_show : eq_preset_show_h)
  {
    putchar_(GC, 
             orient ? eq_preset_rect.x1 : eq_preset_rect_h.x1, 
             orient ? eq_preset_rect.y1 - StatusSize : eq_preset_rect_h.y1,
             orient ? eq_preset_rect.x2 - eq_preset_rect.x1 : eq_preset_rect_h.y2 - eq_preset_rect_h.y1,
             orient ? eq_preset_rect.y2 - eq_preset_rect.y1: eq_preset_rect_h.y2 - eq_preset_rect_h.y1,
             presets[wb->preset < 7 ? wb->preset : 6]);
  }
  
  //----------------------------------------------------------------------------
  //Play/Stop icon
  if (orient ? pls_show : pls_show_h)
  {
    putchar_(GC, 
            orient ? pls_rect.x1 : pls_rect_h.x1, 
            orient ? pls_rect.y1 - StatusSize : pls_rect_h.y1,
            orient ? pls_rect.x2 - pls_rect.x1 : pls_rect_h.x2 - pls_rect_h.x1,
            orient ? pls_rect.y2 - pls_rect.y1 : pls_rect_h.y2 - pls_rect_h.y1,
            wb->status==MP_STATUS_PLAY ? playicn : 
              wb->status==MP_STATUS_PAUSE ? use_pauseicn ? pauseicn : stopicn : 
                wb->status==MP_STATUS_FORWARD ? media_images[IMG_FORWARD] : 
                  wb->status==MP_STATUS_REWIND ? media_images[IMG_REWIND] : stopicn);
  }
  int MIN,SEC;
  rc.y1=gc_rc.y1;
  rc.y2=gc_rc.y2;
  //----------------------------------------------------------------------------
  //Current time
  if (orient ? ctime_show : ctime_show_h)
  {
    MIN=cur/60;
    SEC=cur%60;
    snwprintf(buf,49, orient ? ctime_mask : ctime_mask_h, MIN, SEC);
    
    rc.x1 = orient ? ctime_rc.x1 : ctime_rc_h.x1;
    rc.x2 = orient ? ctime_rc.x2 : ctime_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine(orient ? ctime_font : ctime_font_h, 
             buf, 
             orient ? ctime_ct : ctime_ct_h, 
             rc.x1, 
             orient ? ctime_rc.y1 - StatusSize : ctime_rc_h.y1, 
             rc.x2,
             orient ? ctime_rc.y2 - StatusSize : ctime_rc_h.y2,
             orient ? ctime_color : ctime_color_h, 
             orient ? ctime_scolor : ctime_scolor_h,
             true,
             orient ? ctime_hl : ctime_hl_h);
  }
  //----------------------------------------------------------------------------
  //Full time
  if (orient ? ftime_show : ftime_show_h)
  {
    MIN=mm_info->fulltime/60;
    SEC=mm_info->fulltime%60;
    snwprintf(buf,49,orient ? ftime_mask : ftime_mask_h,MIN,SEC);
    rc.x1 = orient ? ftime_rc.x1 : ftime_rc_h.x1;
    rc.x2 = orient ? ftime_rc.x2 : ftime_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine(orient ? ftime_font : ftime_font_h, 
             buf, 
             orient ? ftime_ct : ftime_ct_h, 
             rc.x1, 
             orient ? ftime_rc.y1 - StatusSize : ftime_rc_h.y1, 
             rc.x2,
             orient ? ftime_rc.y2 - StatusSize : ftime_rc_h.y2,
             orient ? ftime_color : ftime_color_h, 
             orient ? ftime_scolor : ftime_scolor_h,
             true,
             orient ? ftime_hl : ftime_hl_h);
    
  }
  //----------------------------------------------------------------------------
  //Time left
  if (orient ? ltime_show : ltime_show_h)
  {
    MIN=(mm_info->fulltime-cur)/60;
    SEC=(mm_info->fulltime-cur)%60;
    snwprintf(buf,49,orient ? ltime_mask : ltime_mask_h,MIN,SEC);
    rc.x1 = orient ? ltime_rc.x1 : ltime_rc_h.x1;
    rc.x2 = orient ? ltime_rc.x2 : ltime_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine(orient ? ltime_font : ltime_font_h, 
             buf, 
             orient ? ltime_ct : ltime_ct_h, 
             rc.x1, 
             orient ? ltime_rc.y1 - StatusSize : ltime_rc_h.y1, 
             rc.x2,
             orient ? ltime_rc.y2 - StatusSize : ltime_rc_h.y2,
             orient ? ltime_color : ltime_color_h, 
             orient ? ltime_scolor : ltime_scolor_h,
             true,
             orient ? ltime_hl : ltime_hl_h);
  }
  //----------------------------------------------------------------------------
  //Title
  if (orient ? title_show : title_show_h)
  {
    rc.x1 = orient ? title_rc.x1 : title_rc_h.x1;
    rc.x2 = orient ? title_rc.x2 : title_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine2(orient ? title_font : title_font_h, 
              mm_info->title_str, 
              orient ? title_ct : title_ct_h, 
              rc.x1, 
              orient ? title_rc.y1 - StatusSize : title_rc_h.y1, 
              rc.x2,
              orient ? title_rc.y2 - StatusSize : title_rc_h.y2,
              orient ? title_color : title_color_h, 
              orient ? title_scolor : title_scolor_h,
              false,
              orient ? title_hl : title_hl_h);
  }
  //----------------------------------------------------------------------------
  //Album
  if (restr_CoverAlbum && nowmusic.meta.ImageID==0xFFFF)
    goto L_SkipAlbum;
  if (orient ? album_show : album_show_h)
  {
    rc.x1 = orient ? album_rc.x1 : album_rc_h.x1;
    rc.x2 = orient ? album_rc.x2 : album_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine2(orient ? album_font : album_font_h, 
              mm_info->album_str, 
              orient ? album_ct : album_ct_h, 
              rc.x1, 
              orient ? album_rc.y1 - StatusSize : album_rc_h.y1, 
              rc.x2,
              orient ? album_rc.y2 - StatusSize : album_rc_h.y2,
              orient ? album_color : album_color_h, 
              orient ? album_scolor : album_scolor_h,
              false,
              orient ? album_hl : album_hl_h);
  }
L_SkipAlbum:
  //----------------------------------------------------------------------------
  //Artist
  if (orient ? artist_show : artist_show_h)
  {
    rc.x1 = orient ? artist_rc.x1 : artist_rc_h.x1;
    rc.x2 = orient ? artist_rc.x2 : artist_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine2(orient ? artist_font : artist_font_h, 
              mm_info->artist_str, 
              orient ? artist_ct : artist_ct_h, 
              rc.x1, 
              orient ? artist_rc.y1 - StatusSize : artist_rc_h.y1, 
              rc.x2,
              orient ? artist_rc.y2 - StatusSize : artist_rc_h.y2,
              orient ? artist_color : artist_color_h, 
              orient ? artist_scolor : artist_scolor_h,
              false,
              orient ? artist_hl : artist_hl_h);
  }
  //----------------------------------------------------------------------------
  //Genre
  if (orient ? genre_show : genre_show_h)
  {
    rc.x1 = orient ? genre_rc.x1 : genre_rc_h.x1;
    rc.x2 = orient ? genre_rc.x2 : genre_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine(orient ? genre_font : genre_font_h, 
             nowmusic.meta.Genre, 
             orient ? genre_ct : genre_ct_h, 
             rc.x1, 
             orient ? genre_rc.y1 - StatusSize : genre_rc_h.y1, 
             rc.x2,
             orient ? genre_rc.y2 - StatusSize : genre_rc_h.y2,
             orient ? genre_color : genre_color_h, 
             orient ? genre_scolor : genre_scolor_h,
             true,
             orient ? genre_hl : genre_hl_h);
  }
  //----------------------------------------------------------------------------
  //Year
  if (orient ? year_show : year_show_h)
  {
    rc.x1 = orient ? year_rc.x1 : year_rc_h.x1;
    rc.x2 = orient ? year_rc.x2 : year_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine(orient ? year_font : year_font_h, 
             nowmusic.meta.Year, 
             orient ? year_ct : year_ct_h, 
             rc.x1, 
             orient ? year_rc.y1 - StatusSize : year_rc_h.y1, 
             rc.x2,
             orient ? year_rc.y2 - StatusSize : year_rc_h.y2,
             orient ? year_color : year_color_h, 
             orient ? year_scolor : year_scolor_h,
             true,
             orient ? year_hl : year_hl_h);
  }
  //----------------------------------------------------------------------------
  //Track number
  if (orient ? trackn_show : trackn_show_h)
  {
    BOOK_PLAYER_2020 *bkp=(BOOK_PLAYER_2020*)audio;
    snwprintf(buf, 49, orient ? trackn_mask : trackn_mask_h, bkp->pos2+1);
    rc.x1 = orient ? trackn_rc.x1 : trackn_rc_h.x1;
    rc.x2 = orient ? trackn_rc.x2 : trackn_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine(orient ? trackn_font : trackn_font_h, 
             buf, 
             orient ? trackn_ct : trackn_ct_h, 
             rc.x1, 
             orient ? trackn_rc.y1 - StatusSize : trackn_rc_h.y1, 
             rc.x2,
             orient ? trackn_rc.y2 - StatusSize : trackn_rc_h.y2,
             orient ? trackn_color : trackn_color_h, 
             orient ? trackn_scolor : trackn_scolor_h,
             true,
             orient ? trackn_hl : trackn_hl_h);
  }
  //----------------------------------------------------------------------------
  //Tracks count
  if (orient ? tracks_show : tracks_show_h)
  {
    BOOK_PLAYER_2020 *bkp=(BOOK_PLAYER_2020*)audio;
    snwprintf(buf,49, orient ? tracks_mask : tracks_mask_h, bkp->tracks_count);
    rc.x1 = orient ? tracks_rc.x1 : tracks_rc_h.x1;
    rc.x2 = orient ? tracks_rc.x2 : tracks_rc_h.x2;
    GC_validate_RECT(GC,&rc);
    DrawLine(orient ? tracks_font : tracks_font_h, 
             buf, 
             orient ? tracks_ct : tracks_ct_h, 
             rc.x1, 
             orient ? tracks_rc.y1 - StatusSize : tracks_rc_h.y1, 
             rc.x2,
             orient ? tracks_rc.y2 - StatusSize : tracks_rc_h.y2,
             orient ? tracks_color : tracks_color_h, 
             orient ? tracks_scolor : tracks_scolor_h,
             true,
             orient ? tracks_hl : tracks_hl_h);
  }
  int format=nowmusic.format;
  if (format==FORMAT_MP3 || format==FORMAT_OTHER)
  {
    //----------------------------------------------------------------------------
    //Bitrate
    if (orient ? bitrate_show : bitrate_show_h)
    {
      wchar_t const *vbr=0;
      if (nowmusic.hdr.VBR && use_vbr)
      {
        if (orient)
          vbr=&bitrate_vbr[0];
        else
          vbr=&bitrate_vbr_h[0];
      }
      else
        snwprintf(buf,49, orient ? bitrate_mask : bitrate_mask_h, nowmusic.hdr.bitrate);
      rc.x1 = orient ? bitrate_rc.x1 : bitrate_rc_h.x1;
      rc.x2 = orient ? bitrate_rc.x2 : bitrate_rc_h.x2;
      GC_validate_RECT(GC,&rc);
      DrawLine(orient ? bitrate_font : bitrate_font_h, 
               vbr ? (wchar_t*)vbr : buf, 
               orient ? bitrate_ct : bitrate_ct_h, 
               rc.x1, 
               orient ? bitrate_rc.y1 - StatusSize : bitrate_rc_h.y1, 
               rc.x2,
               orient ? bitrate_rc.y2 - StatusSize : bitrate_rc_h.y2,
               orient ? bitrate_color : bitrate_color_h, 
               orient ? bitrate_scolor : bitrate_scolor_h,
               true,
               orient ? bitrate_hl : bitrate_hl_h);
    }
    //----------------------------------------------------------------------------
    //Frequency
    if (orient ? freq_show : freq_show_h)
    {
      snwprintf(buf,49, orient ? freq_mask : freq_mask_h,nowmusic.hdr.frequency);
      rc.x1 = orient ? freq_rc.x1 : freq_rc_h.x1;
      rc.x2 = orient ? freq_rc.x2 : freq_rc_h.x2;
      GC_validate_RECT(GC,&rc);
      DrawLine(orient ? freq_font : freq_font_h, 
               buf, 
               orient ? freq_ct : freq_ct_h, 
               rc.x1, 
               orient ? freq_rc.y1 - StatusSize : freq_rc_h.y1, 
               rc.x2,
               orient ? freq_rc.y2 - StatusSize : freq_rc_h.y2,
               orient ? freq_color : freq_color_h, 
               orient ? freq_scolor : freq_scolor_h,
               true,
               orient ? freq_hl : freq_hl_h);
    }
    //----------------------------------------------------------------------------
    //Channels
    if (orient ? ch_show : ch_show_h)
    {
      int mode=nowmusic.hdr.mode;
      wchar_t *ch=0;
      if (mode<3)
      {
        ch=modes[mode];
      }
      else
      {
        ch=modes[3];
      }
      rc.x1 = orient ? ch_rc.x1 : ch_rc_h.x1;
      rc.x2 = orient ? ch_rc.x2 : ch_rc_h.x2;
      GC_validate_RECT(GC,&rc);
      DrawLine(orient ? ch_font : ch_font_h, 
               ch, 
               orient ? ch_ct : ch_ct_h, 
               rc.x1, 
               orient ? ch_rc.y1 - StatusSize : ch_rc_h.y1, 
               rc.x2,
               orient ? ch_rc.y2 - StatusSize : ch_rc_h.y2,
               orient ? ch_color : ch_color_h, 
               orient ? ch_scolor : ch_scolor_h,
               true,
               orient ? ch_hl : ch_hl_h);
      
    }
  }
  GC_validate_RECT(GC, &gc_rc);
  //----------------------------------------------------------------------------
  //Navigation image
  if (orient ? navigimg_show : navigimg_show_h)
  {
    putchar_(GC, 
            orient ? navigimg_rect.x1 : navigimg_rect_h.x1, 
            orient ? navigimg_rect.y1 - StatusSize: navigimg_rect_h.y1,
            orient ? navigimg_rect.x2 - navigimg_rect.x1 : navigimg_rect_h.x2 - navigimg_rect_h.x1,
            orient ? navigimg_rect.y2 - navigimg_rect.y1 : navigimg_rect_h.y2 - navigimg_rect_h.y1,
            navigimg);
  }
  
  //----------------------------------------------------------------------------
  //Up arrow
  if (orient ? upimg_show : upimg_show_h)
  {
    putchar_(GC, 
            orient ? upimg_rect.x1 : upimg_rect_h.x1, 
            orient ? upimg_rect.y1 - StatusSize: upimg_rect_h.y1,
            orient ? upimg_rect.x2 - upimg_rect.x1 : upimg_rect_h.x2 - upimg_rect_h.x1,
            orient ? upimg_rect.y2 - upimg_rect.y1 : upimg_rect_h.y2 - upimg_rect_h.y1,
            images[IMG_UP]);
  }
  //----------------------------------------------------------------------------
  //Down arrow
  if (orient ? downimg_show : downimg_show_h)
  {
    putchar_(GC, 
            orient ? downimg_rect.x1 : downimg_rect_h.x1, 
            orient ? downimg_rect.y1 - StatusSize: downimg_rect_h.y1,
            orient ? downimg_rect.x2 - downimg_rect.x1 : downimg_rect_h.x2 - downimg_rect_h.x1,
            orient ? downimg_rect.y2 - downimg_rect.y1 : downimg_rect_h.y2 - downimg_rect_h.y1,
            images[IMG_DOWN]);
  }
  //----------------------------------------------------------------------------
  //Left arrow
  if (orient ? leftimg_show : leftimg_show_h)
  {
    putchar_(GC, 
             orient ? leftimg_rect.x1 : leftimg_rect_h.x1, 
             orient ? leftimg_rect.y1 - StatusSize: leftimg_rect_h.y1,
             orient ? leftimg_rect.x2 - leftimg_rect.x1 : leftimg_rect_h.x2 - leftimg_rect_h.x1,
             orient ? leftimg_rect.y2 - leftimg_rect.y1 : leftimg_rect_h.y2 - leftimg_rect_h.y1,
             wb->status==5 ? images[IMG_PREV2] : images[IMG_PREV]);
  }
  //----------------------------------------------------------------------------
  //Right arrow
  if (orient ? rightimg_show : rightimg_show_h)
  {
    putchar_(GC, 
             orient ? rightimg_rect.x1 : rightimg_rect_h.x1, 
             orient ? rightimg_rect.y1 - StatusSize: rightimg_rect_h.y1,
             orient ? rightimg_rect.x2 - rightimg_rect.x1 : rightimg_rect_h.x2 - rightimg_rect_h.x1,
             orient ? rightimg_rect.y2 - rightimg_rect.y1 : rightimg_rect_h.y2 - rightimg_rect_h.y1,
             wb->status==3 ? images[IMG_NEXT2] : images[IMG_NEXT]);
  }
  //----------------------------------------------------------------------------
  //Play/Stop mini icon
  if (orient ? centerimg_show : centerimg_show_h)
  {
    putchar_(GC, 
             orient ? centerimg_rect.x1 : centerimg_rect_h.x1, 
             orient ? centerimg_rect.y1 - StatusSize: centerimg_rect_h.y1,
             orient ? centerimg_rect.x2 - centerimg_rect.x1 : centerimg_rect_h.x2 - centerimg_rect_h.x1,
             orient ? centerimg_rect.y2 - centerimg_rect.y1 : centerimg_rect_h.y2 - centerimg_rect_h.y1,
             wb->status==1 ? images[IMG_STOP] : images[IMG_PLAY]);
  }
      //----------------------------------------------------------------------------
  //Repeat icon
  if (orient ? repeat_show : repeat_show_h)
  {
    putchar_(GC, 
             orient ? repeat_rect.x1 : repeat_rect_h.x1, 
             orient ? repeat_rect.y1 - StatusSize: repeat_rect_h.y1,
             orient ? repeat_rect.x2 - repeat_rect.x1 : repeat_rect_h.x2 - repeat_rect_h.x1,
             orient ? repeat_rect.y2 - repeat_rect.y1 : repeat_rect_h.y2 - repeat_rect_h.y1,
            wb->RepeatMode==1 ? media_images[IMG_LOOP] : media_images[IMG_LOOP_OFF]);
  }
  //----------------------------------------------------------------------------
  //Shuffle icon
    if (orient ? shuffle_show : shuffle_show_h)
  {
    putchar_(GC, 
            orient ? shuffle_rect.x1 : shuffle_rect_h.x1, 
            orient ? shuffle_rect.y1 - StatusSize: shuffle_rect_h.y1,
            orient ? shuffle_rect.x2 - shuffle_rect.x1 : shuffle_rect_h.x2 - shuffle_rect_h.x1,
            orient ? shuffle_rect.y2 - shuffle_rect.y1 : shuffle_rect_h.y2 - shuffle_rect_h.y1,
            wb->ShuffleMode==1 ? media_images[IMG_RANDOM] : media_images[IMG_RANDOM_OFF]);
  }
  //----------------------------------------------------------------------------
  //Stereo widening icon
  if (orient ? stereo_show : stereo_show_h)
  {
    putchar_(GC, 
            orient ? stereo_rect.x1 : stereo_rect_h.x1, 
            orient ? stereo_rect.y1 - StatusSize: stereo_rect_h.y1,
            orient ? stereo_rect.x2 - stereo_rect.x1 : stereo_rect_h.x2 - stereo_rect_h.x1,
            orient ? stereo_rect.y2 - stereo_rect.y1 : stereo_rect_h.y2 - stereo_rect_h.y1,
            wb->StereoWidening==1 ? media_images[IMG_STEREOWIDENING] : media_images[IMG_STEREOWIDENING_OFF]);
  }
#ifndef NDEBUG
  rc.x1 = 0;
  rc.x2 = orient ? 240 : 320;
  GC_validate_RECT(GC,&rc);
  DrawLine2(FONT_E_20B, 
            int2strID(BENCH_GET), 
            2, 
            0, 
            0, 
            orient ? 240 : 320,
            orient ? 320 : 240,
            orient ? title_color : title_color_h, 
            orient ? title_scolor : title_scolor_h,
            true,
            0);
  
    GC_validate_RECT(GC,&gc_rc);
#endif
};

int GetIconID(wchar_t *txt);

#pragma optimize = z 9
wchar_t getimgID(wchar_t old)
{
  return old;
};
void Softkey_SetOnRedraw();
void GetRedraw(DISP_OBJ_ONREDRAW_METHOD draw)
{
  oldredr=draw;
  debug_printf("\nWalkmanDisplay: old redraw: %X\n", oldredr);
  Softkey_SetOnRedraw();
  ModifyKeyHook(NewKey, 1);
};

int isBrowserItemBook(BOOK *bk)
{
  if (strcmp(bk->xbook->name,"BrowserItemBook")==0)
    return 1;
  return 0;
};

void PageHook(void*, BOOK *book, int id)
{
  if (working)
  {
    BookObj_SetDisplayOrientation(FindBook(isAudioPlayerBook()),0);
    BookObj_SetDisplayOrientation(book,0);
    if (id==4)
    {
      BOOK *b=DISPLAY_GetTopBook(0);
      BookObj_SetDisplayOrientation(b,0);
    }
    if (BOOK *bk=FindBook(isBrowserItemBook))
    {
      BookObj_SetDisplayOrientation(bk,0);
    }
    BookObj_SetDisplayOrientation(FindBook(isAudioPlayerBook()),0);
    BookObj_SetDisplayOrientation(book,0);
    debug_printf("\nWalkmanDisplay: pagehook called from %s\n",book->xbook->name); 
    accel_wrk=false;
    inMenu=true;
  }
};

wchar_t *checkfile_(wchar_t *name, wchar_t *skinpath)
{
  FSTAT fst;
  if (fstat(skinpath,name,&fst)==0)
  {
    return skinpath;
  }
  else if (fstat(extFolder,name,&fst)==0)
  {
    return extFolder;
  }
  else if (fstat(intFolder,name,&fst)==0)
  {
    return intFolder;
  }
  return intFolder;
};

CMyBook::CMyBook():CBook((char*)PROJECT_NAME,&::base_page)
{
  MyBook->redraw=walkman_Redraw;
  MyBook->getImgID=getimgID;
  MyBook->getOldRedr=GetRedraw;
  MyBook->pageHook=PageHook;
  wstrcpy(intFolder,GetDir(MEM_INTERNAL+DIR_ELFS_CONFIG));
  wstrcat(intFolder,L"/WalkmanDisplay/");
  wstrcpy(extFolder,GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG));
  wstrcat(extFolder,L"/WalkmanDisplay/");
};


void Send_REDRAW_RELEASE()
{
  // �������� ����  ������� �� ������ � ����� �������� Redraw
  REDRAW_RELEASE_MESSAGE * sbm= new REDRAW_RELEASE_MESSAGE;
  memset(sbm,0,sizeof(REDRAW_RELEASE_MESSAGE));
  sbm->SK_OldOnRedraw=Softkey_DefaultRedraw;
  sbm->SK_NewOnRedraw=softredraw;
  
  UI_Event_wData(SBY_REDRAW_RELEASE_EVENT ,sbm,(void (*)(void*))mfree_adr());
};


void CMyBook::onClose()
{
#ifndef NDEBUG
  kill_proc(proc_bench);
#endif
  #define IDFREE(a) if (a!=0xFFFF && a!=0) {ImageID_Free(a);} a=0xFFFF
  IDFREE(nowmusic.meta.ImageID);
  ModifyKeyHook(NewKey, 0);
  if (RedrawUsed)
  {
    Send_REDRAW_RELEASE();
    ReturnRedraw();
  }
  if (timer)
    Timer_Kill(&timer);
  if (AccelCtrl)
    delete(AccelCtrl);
  deinit_resources();
  void Lang_Remove();
  Lang_Remove();
  if (currenttrack)
    TrackDesc_Free(currenttrack);
  SUBPROC(elf_exit);
};

__ONKEY_METHOD( NewKey )
{
  BOOK *bk;
  if (key==key_rotation && mode==key_rotation_mode)
  {
    bk=FindBook(isAudioPlayerBook());
    if (bk && bk==DISPLAY_GetTopBook(0))
    {
      int mode=BookObj_GetDisplayOrientation(bk);
      if (mode!=0 && curmode==HOR)
      {
        set_vertical();
        if (use_accel==1)
        {
          accel_wrk=true;
        }
      }
      else if (mode==0 && curmode==VERT)
      {
        set_horizontal();
        if (use_accel==1)
        {
          accel_wrk=false;
        }
      }
      return -1;
    }
    return -1;
  }
  return 0;
};

void GetSize()
{
  int a=Display_GetHeight(0);
  ScreenHeight=a;
  ScreenWidth=Display_GetWidth(0);
  StatusSize=DISP_OBJ_GetWindowHeight(*StatusRow_p());
  SoftSize=DISP_OBJ_GetWindowHeight(DispObject_SoftKeys_Get());
};

int isWalkmanDisplayBook(BOOK *bk)
{
  if (strcmp(bk->xbook->name,"WalkmanDisplay")==0)
    return 1;
  return 0;
};


wchar_t missedconsts[512];
#define CHECKCONST(a,b) if (a==0 || a==0xFFFFFFFF) {wstrcat(missedconsts,b);missed++;}

wchar_t *checkConsts()
{
  int missed=0;
  
  wstrcpy(missedconsts,lng_txtMissedConsts);
  CHECKCONST(UI_MEDIAPLAYER_DESTROYED_EVENT,L"UI_MEDIAPLAYER_DESTROYED_EVENT\n");
  CHECKCONST(UI_MEDIAPLAYER_PLAY_PRESSED_EVENT,L"UI_MEDIAPLAYER_PLAY_PRESSED_EVENT\n");
  CHECKCONST(UI_MEDIAPLAYER_AUDIO_PLAYING_TIME_EVENT,L"UI_MEDIAPLAYER_AUDIO_PLAYING_TIME_EVENT\n");
  CHECKCONST(UI_KEYLOCK_ACTIVATE_EVENT, L"UI_KEYLOCK_ACTIVATE_EVENT\n");
  CHECKCONST(UI_KEYLOCK_DEACTIVATE_EVENT, L"UI_KEYLOCK_DEACTIVATE_EVENT\n");
  if (missed)
    return missedconsts;
  return 0;
};

#ifndef NDEBUG
OS_PROCESS(worker_entrypoint)
{
  while (1)
  {
    bench_n++;
    delay(1);
  }
};
#endif

void softredraw(DISP_OBJ *DO, int a, int b, int c)
{
  BOOK *bk=FindBook(isAudioPlayerBook());
  BOOK *top=DISPLAY_GetTopBook(0);
  if (bk && top && bk==top)
  {
    int orient=BookObj_GetDisplayOrientation(bk);
    if (orient>0 && (hidesoftkeys==1 || hidesoftkeys==3))
      return;
    else if (orient==0 && hidesoftkeys>=2)
      return;
  }
  Softkey_DefaultRedraw(DO, a, b, c);
};

void Softkey_SetOnRedraw()
{
  if (RedrawUsed==false)
  {
    RedrawUsed=true;
    Softkey_DO=DispObject_SoftKeys_Get();
    Softkey_DefaultRedraw=DISP_OBJ_GetOnRedraw(Softkey_DO);
    Softkey_DD=DISP_OBJ_GetDESC(Softkey_DO);
    DISP_DESC_SetOnRedraw(Softkey_DD,softredraw);
    debug_printf("\nWalkmanDisplay: own softkey redraw setted\n", oldredr);
  }
};

int main()
{
  trace_init();
  if (FindBook(isWalkmanDisplayBook))
  {
    MessageBox(0x6FFFFFFF,Str2ID(lng_txtAlreadyRunned,0,SID_ANY_LEN),0,1,0,0);
    SUBPROC(elf_exit);
    return 0;
  }
  if (wchar_t *consts=checkConsts())
  {
    MessageBox(0x6FFFFFFF,Str2ID(consts,0,SID_ANY_LEN),0,1,0,0);
    SUBPROC(elf_exit);
    return 0;
  }
  if (GetChipID()!=0x9900)
  {
    MessageBox(0x6FFFFFFF,Str2ID(lng_msgNotSupported,0,SID_ANY_LEN),0,1,0,0);
    SUBPROC(elf_exit);
    return 0;
  }
  memset(&nowmusic,0,sizeof(MUSIC));
#ifndef NDEBUG
  proc_bench = create_process(0, "WD_benchmark", worker_entrypoint, 0x400, 0, (OSTIME) 0, (PROCESS) 0, NULL, (OSVECTOR) 0, (OSUSER) 0);
  start(proc_bench);
#else
  debug_printf("\nWalkmanDisplay: internal benchmark wasn't runned because of release configuration, so no execution time calculations will be performed\n");
#endif
  //Softkey_SetOnRedraw();
  //ModifyKeyHook(NewKey, 1);
  InitConfig();
  GetSize();
  char* Lang_Initialize();
  if (Lang_Initialize())
  {
    debug_printf("\nWalkmanDisplay: encoding.bin founded. Will be used in id3 v1 parser\n");
  }
  else
  {
    debug_printf("\nWalkmanDisplay: encoding.bin missed. str2wstr will be used in id3 v1 parser\n");
  }
  new CMyBook();
  debug_printf("\nWalkmanDisplay: initial loading finished\n");
  return 0;
};


/*
    2.1:
    -���������� ������ � ��������� bcfg
    -���������� ������������ ��� ��������������� ������ �� ���������� ������
    -�������� ��������� ���������
    -���������� ����������������� ����� � 2.0 (ENG/RUS) �� 2.1 ENG/RUS.
    -������ ���������

    2.0:
    -������ �����
*/