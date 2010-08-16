#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\cfg_items.h"
#include "conf_loader.h"
#include "config_data.h"

typedef struct
{
  BOOK * book;
}MSG;
extern DISP_OBJ *StatusIndication;
extern void (*SIonRedraw)(DISP_OBJ *,int r1,int r2,int r3);
extern void Draw(DISP_OBJ * db,int r1, int r2,int r3);
void Send_REDRAW_RELEASE()
{
  // ???????? ????  ??????? ?? ?????? ? ????? ???????? Redraw
  REDRAW_RELEASE_MESSAGE * sbm= new REDRAW_RELEASE_MESSAGE;
  memset(sbm,0,sizeof(REDRAW_RELEASE_MESSAGE));
  sbm->SB_OldOnRedraw=SIonRedraw;
  sbm->SB_NewOnRedraw=Draw;
  UI_Event_wData(SBY_REDRAW_RELEASE_EVENT ,sbm,(void (*)(void*))mfree_adr());
};
void elf_exit(void);
int TerminateSBDraw(void * ,BOOK* book)
{
  Send_REDRAW_RELEASE();
  FreeBook(book);
  return(1);
};

int ShowAuthorInfo(void *mess ,BOOK* book)
{
  MSG * msg = (MSG*)mess;
  MessageBox(EMPTY_SID,STR("EvtDesk\n(C) UltraShot"), NOIMAGE, 1, 5000,msg->book);
  return(1);
};


int SB_ELF_Killed(void *mess ,BOOK* book)
{
  // ???? ??? ???? ???? ???????? ?? ?? ??? ?????? ????? ???????????? ??????
  REDRAW_RELEASE_MESSAGE * sbm=(REDRAW_RELEASE_MESSAGE*)mess;
  // ??? ?? ????? ?? ?????????? ? ???????? oldRedraw?
  if (sbm->SB_NewOnRedraw==SIonRedraw)
  {
    REDRAW_RELEASE_MESSAGE *res=new REDRAW_RELEASE_MESSAGE;
    
    // ????????? ???????????? ?????? ? ???? ???????
    memcpy(res,sbm,sizeof(REDRAW_RELEASE_MESSAGE));
    
    // ???? ?? ??? ????, ?? ???????? ???? oldRedraw ?? ???..
    if (sbm->SB_OldOnRedraw!=EMPTY_REDRAW_METHOD) SIonRedraw=sbm->SB_OldOnRedraw;

    // ?????? ???? ????? ??????
    DISP_DESC_SetOnRedraw(DispObject_GetDESC(StatusIndication),Draw);

    // ? ???? ??????? ?????, ???? ????????? ????? ??????? ???? ?????
    res->SB_OldOnRedraw=EMPTY_REDRAW_METHOD;
    res->SB_NewOnRedraw=Draw;
    UI_Event_wData(SBY_REDRAW_RELEASE_EVENT ,res,(void (*)(void*))mfree_adr());
    return BLOCK_EVENT_GLOBALLY;
  }
  return 0;
};
static int OnReconfig(void *mess ,BOOK *book)
{
  RECONFIG_EVENT_DATA *reconf=(RECONFIG_EVENT_DATA *)mess;
  int result=0;
  if (wstrcmpi(reconf->path,successed_config_path)==0 && wstrcmpi(reconf->name,successed_config_name)==0)
  {
    InitConfig();
    result=1;
  }
  return(result);
};
const PAGE_MSG SBDraw_PageEvents[]@ "DYN_PAGE" ={
  SBY_REDRAW_RELEASE_EVENT  , SB_ELF_Killed,
  ELF_TERMINATE_EVENT       , TerminateSBDraw,
  ELF_SHOW_INFO_EVENT       , ShowAuthorInfo,
  ELF_RECONFIG_EVENT        , OnReconfig,
  NIL_EVENT_TAG             , NULL

};

PAGE_DESC base_page ={"SBDraw_BasePage",0,SBDraw_PageEvents};
extern u16 timer;
void onCloseMyBook(BOOK * book)
{
  if (book)
  {
    Timer_Kill(&timer);
    DISP_DESC_SetOnRedraw(DispObject_GetDESC(StatusIndication),SIonRedraw);
    SUBPROC(elf_exit);
  }
};

BOOK * CreateEvtDeskBook()
{
  BOOK * EvtDeskBook= new BOOK;
  CreateBook(EvtDeskBook,onCloseMyBook,&base_page,"EvtDesk",-1,0);
  return(EvtDeskBook);
};
