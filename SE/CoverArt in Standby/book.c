#include "..\\include\Lib_Clara.h" 
#include "..\\include\Dir.h" 
#include "..\\include\cfg_items.h"
#include "conf_loader.h"
#include "config_data.h"

#include "Standby.h"
#include "revision.h"
#include "structs.h"
#include "Externals.h"

extern MUSIC nowmusic;
extern bool isInStandby;
extern LABEL labels[10];
extern UI_IMG img;

bool PlayerRunned=false;

int CurrentTime;
BOOK * CABook;
void decoder_Init(wchar_t *path, wchar_t *name);
TRACK_DESC *currenttrack=0;

void CA_Kill()
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
};

int CABook_Terminate(void * ,BOOK* book)
{
  FreeBook(book);
  return(1);
};

int CABook_ShowAuthorInfo(void *mess ,BOOK* book)
{
  MSG * msg = (MSG*)mess;
  wchar_t text[512];
  snwprintf( text, 512, L"CoverArt in Standby v3.51\r\n\r\n© UltraShot\n® IronMaster,\nJoker XT,\n2007KrasH\n\r\nbuild %d\n", BUILD );
  MessageBox( SID_NULL, Str2ID(text,0,SID_ANY_LEN), 0, 1, 5000, msg->book ); 
  return(1);
};

void CABook_OnClose(BOOK * book)
{
  if (book)
  {
    void Cover_Free();
    //Удаляем обложку, если она есть
    Cover_Free();
    // Закрываем всё открытое
    Lang_Remove();
    Images_Unload();
    Send_REDRAW_RELEASE();
    Anim_Free();
    Standby_ReturnRedraw();
    SUBPROC(CA_Kill);
  }
};

static int CABook_Reconfig(void *mess ,BOOK *book)
{
  // Переконфигурируемся
  RECONFIG_EVENT_DATA *reconf=(RECONFIG_EVENT_DATA *)mess;
  int result=0;
  // Если конфиг наш, перегрузим его
  if (wstrcmpi(reconf->path,successed_config_path)==0 && wstrcmpi(reconf->name,successed_config_name)==0)
  {
    if (AllowAnim==true)
    {
      //RemoveAnimation();
      Anim_Free();
    }
    InitConfig();
    Anim_Init();
    CustomLbls();
    InitLbls();
    //InitializeLabels();
    Images_Unload();
    Images_Load();
    if (AllowAnim==true)
    {
      locTime=AnimUpdateTime;
    }
    else
    {
      locTime=DefUpdateTime;
    }
    result=1;
  }
  return(result);
};

int isCoverArtBook(BOOK *bk)
{
  if (!strcmp(bk->xbook->name,"CoverArt in Standby")) { return 1; }
  else { return 0; }
};

void Send_REDRAW_RELEASE()
{
  // посылаем всем  мессагу со старым и своим методами Redraw
  SBY_REDRAW_RELEASE_MESSAGE * sbm= new SBY_REDRAW_RELEASE_MESSAGE;
  sbm->OldonRedraw=Standby_DefaultRedraw;
  sbm->NewonRedraw=Standby_CARedraw;
  UI_Event_wData(SBY_REDRAW_RELEASE_EVENT ,sbm,(void (*)(void*))mfree_adr());
};

int SB_ELF_Killed(void *mess ,BOOK* book)
{
  // если был убит эльф рисующий на ГЭ или просто нужно перетосовать методы
  SBY_REDRAW_RELEASE_MESSAGE * sbm=(SBY_REDRAW_RELEASE_MESSAGE*)mess;

  // его ли метод мы используем в качестве oldRedraw?
  if (sbm->NewonRedraw==Standby_DefaultRedraw)
  {
    SBY_REDRAW_RELEASE_MESSAGE * ms= new SBY_REDRAW_RELEASE_MESSAGE;

    // если он был убит, то заменяем свой oldRedraw на его..
    if (sbm->OldonRedraw) Standby_DefaultRedraw=sbm->OldonRedraw;

    // ставим сdой метод наверх
    DISP_DESC_SetOnRedraw(DISP_OBJ_GetDESC(Standby_DO),Standby_CARedraw);

    // и шлём мессагу снова, чтоб следующие эльфы сделали тоже самое
    ms->OldonRedraw=0;
    ms->NewonRedraw=Standby_CARedraw;
    UI_Event_wData(SBY_REDRAW_RELEASE_EVENT ,ms,(void (*)(void*))mfree_adr());
  }
  return(1);
};

void ReInit()
{
  if (FindBook(isAudioPlayerBook()))
  {
    TRACK_DESC *ntrack=TrackDesc_Get(FindBook(isAudioPlayerBook()));
    if (TrackDesc_Compare(currenttrack,ntrack)==false)
    {
      TrackDesc_Free(currenttrack);
      currenttrack=ntrack;
      decoder_Init(currenttrack->path,currenttrack->name);
      InitLbls();
      nowmusic.hdr.LengthInSeconds=currenttrack->fulltime;
    }
    else
    {
      TrackDesc_Free(ntrack);
    }
  }
};

int CABook_onPlay(void * ,BOOK* bk)
{ 
  ReInit();
#ifdef DEBUG
  MessageBox(SID_NULL,Str2ID(L"PlayEvent",0,SID_ANY_LEN),0,1,0,0);
#endif
  return 1;
};

int CABook_onCreate(void *, BOOK *bk)
{
  if (FindBook(isAudioPlayerBook()))
  {
    PlayerRunned=true;
#ifdef DEBUG
    MessageBox(SID_NULL,Str2ID(L"CreatedEvent",0,SID_ANY_LEN),0,1,0,0);
#endif
  }
  return 1;
}

int CABook_onDestroy(void *, BOOK *bk)
{
  PlayerRunned=false;
#ifdef DEBUG
  MessageBox(SID_NULL,Str2ID(L"DestroyedEvent",0,SID_ANY_LEN),0,1,0,0);
#endif
  return 1;
};

int CABook_onPlayTimer(void *message,BOOK* book)
{
  if (FindBook(isAudioPlayerBook()))
  {
    if (!currenttrack)
    {
      ReInit();
    }
    PlayerRunned=true;
    PLAY_TIME *pl=(PLAY_TIME*)message;
    CurrentTime=(pl->hour * 60)+ ((pl->min * 60) + pl->sec);
    optimized_InitLbls();
  }
  return 1;
};

int CABook_StandbyUnfocused(void *,BOOK* book)
{
  //Тел не на ГЭ
  isInStandby=false;
  return(0);
};

int CABook_StandbyFocused(void *,BOOK* book)
{
  //Тел на ГЭ
  isInStandby=true;
  return(0);
};

const PAGE_MSG CA_PageEvents[] @ "DYN_PAGE" ={
  SBY_REDRAW_RELEASE_EVENT  ,                   SB_ELF_Killed,
  ELF_TERMINATE_EVENT ,                         CABook_Terminate,
  ELF_SHOW_INFO_EVENT  ,                        CABook_ShowAuthorInfo,
  ELF_RECONFIG_EVENT,                           CABook_Reconfig,
  UI_MEDIAPLAYER_PLAY_PRESSED_EVENT_TAG,        CABook_onPlay,
  UI_MEDIAPLAYER_AUDIO_PLAYING_TIME_EVENT_TAG,  CABook_onPlayTimer,
  UI_MEDIAPLAYER_CREATED_EVENT_TAG,             CABook_onCreate,
  UI_MEDIAPLAYER_DESTROYED_EVENT_TAG,           CABook_onDestroy,
  STANDBY_NOT_IDLE_EVENT_TAG,                   CABook_StandbyUnfocused,
  STANDBY_IDLE_EVENT_TAG,                       CABook_StandbyFocused,
  NIL_EVENT_TAG,                                NULL
};

PAGE_DESC base_page ={"CoverArtpage",
                      0,
                      CA_PageEvents};


BOOK * CreateCABook()
{
  CABook= new BOOK;
  CreateBook(CABook,CABook_OnClose,&base_page,"CoverArt in Standby",-1,0);
  return(CABook);
};
