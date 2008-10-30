#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "config_data.h"
#include "structs.h"


TRAF TrafCost_Get(int rcv, int sent)
{
  TRAF traf;
  int full = rcv+sent;
  if (full<(cfgNotCountedTraf*1024))
  {
    traf.cost=0;
  }
  else
  {
    int cfgMinLength2=cfgMinLength*1024;
    if (full<=cfgMinLength2)
    {
      full=cfgMinLength2;
    }
    else
    {
      if ((full%cfgMinLength2)>0)
      {
        full=full-(full%cfgMinLength2)+cfgMinLength2;
      }
    }
    float f_full, f_pkglen, f_cost, f_length;
    f_full=(float)full;
    f_length=(float)cfgLength;
    f_pkglen=f_length*1024.0;
    f_cost=(float)cfgCost;
    traf.cost=f_full/f_pkglen * f_cost;
  }
  traf.traf=full;
  return traf;
};

wchar_t *get_gprstxt(int stat)
{
  if (stat) return L"disconnected";
  return L"last session";
}

//____________________
void Feedback_OnClose(BOOK *bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  if (mbk->mbox)GUI_Free(mbk->mbox);
  mbk->mbox=0;
  if (!mbk->ShouldClose)
  {
    BookObj_Hide(bk,0);
  }
  else
  {
    FreeBook(bk);
  }
}

void Feedback_OnOK(BOOK *bk, void *)
{
  Feedback_OnClose(bk);
}

void mbox_Create(BOOK *bk, int type)
{
  MyBOOK *mbk=(MyBOOK*)bk;
  GPRS_SESSION_INFO gprs;
  GPRS_GetLastSessionInfo(0,&gprs);
  wchar_t str[512];
  TRAF traf=TrafCost_Get(gprs.RcvBytes,gprs.SentBytes);
  if (type)
  {
    snwprintf(str,512,L"GPRS %ls\n\nTime: %02d:%02d\nSent: %d KB\nReceived: %d KB\nReal traf: %d KB\nCost: %0.2f %s", 
              get_gprstxt(mbk->stat),
              gprs.SesTime/60,
              gprs.SesTime%60, 
              gprs.SentBytes/1024, 
              gprs.RcvBytes/1024, 
              traf.traf /1024, 
              traf.cost, 
              cfgCurrency);
  }
  else
  {
    snwprintf(str,512,L"GPRS %ls\n\nTime: %02d:%02d\nSent: %d KB\nReceived: %d KB\nReal traf: %d KB\nCost: %0.2f %s\n© UltraShot", 
              get_gprstxt(mbk->stat),
              gprs.SesTime/60,
              gprs.SesTime%60, 
              gprs.SentBytes/1024, 
              gprs.RcvBytes/1024, 
              traf.traf /1024, 
              traf.cost, 
              cfgCurrency);
  }
  mbk->mbox=TextFeedbackWindow(&mbk->book,0);
  Feedback_SetTextExtended(mbk->mbox,Str2ID(str,0,SID_ANY_LEN),0);
  GUI_SetStyle(mbk->mbox,1);
  GUIObject_Softkey_SetAction(mbk->mbox,ACTION_SELECT1,Feedback_OnOK);
  GUIObject_Softkey_SetAction(mbk->mbox,ACTION_BACK,Feedback_OnOK);
  Feedback_SetOnClose(mbk->mbox,Feedback_OnClose);
  Feedback_SetTimeout(mbk->mbox,cfgTimeout * 1000);
  ShowWindow(mbk->mbox);
  mbk->stat=0;
}
//______________
static int MainPageOnCreate(void *, BOOK *bk)
{
  mbox_Create(bk, 1);
  return (1);
}

const PAGE_MSG bk_msglst_main[] @ "DYN_PAGE"  =
{
  PAGE_ENTER_EVENT_TAG,    MainPageOnCreate,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_main = {"GSI_Main_Page",0,bk_msglst_main};

void elf_exit(void)
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

static void onMyBookClose(BOOK * book)
{
  SUBPROC(elf_exit);
}
int isRealGSIBook(BOOK * struc)
{
  return(struc->onClose==(void*)onMyBookClose);
}

//_____________________
int main()
{
  int ShouldClose=false;
  if (FindBook(isGSIBook))ShouldClose=true;
  MyBOOK * myBook=new MyBOOK;
  memset(myBook,0,sizeof(MyBOOK));
  if (!CreateBook(&myBook->book,onMyBookClose,&bk_base,"GSI",-1,0))
  {
    delete myBook;
    SUBPROC(elf_exit);
    return (0);
  }
  InitConfig();
  if (ShouldClose)
  {
    myBook->ShouldClose=true;
    BookObj_GotoPage(&myBook->book,&bk_main);
  }
  return 0;
}
