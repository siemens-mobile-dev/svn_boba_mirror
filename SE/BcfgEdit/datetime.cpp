#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"

#include "..\\include\cfg_items.h"
#include "main.h"
#include "datetime.h"

void AcceptTIinput(BOOK *bk, void *data)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  unsigned int i=TimeInput_GetTimeInt(data);
  TIME *time=((TIME *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  time->hour=TI_GET_HOUR(i);
  time->min=TI_GET_MIN(i);
  time->sec=TI_GET_SEC(i);
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
}

void CloseTIinput(BOOK * bk, void *)
{
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
}

static int TI_OnEnter(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  TIME t;
  wchar_t ustr[64];
  TIME *time=((TIME *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  int time_format=4;
  char time_f;
  wchar_t icon_id;
  t.hour=time->hour;
  t.min=time->min;
  t.sec=time->sec;
  if (!REQUEST_TIMEFORMAT_GET(SYNC,&time_f)) time_format=time_f;
  win12512unicode(ustr,mbk->cur_hp->name,MAXELEMS(ustr)-1);
  mbk->tinput=CreateTimeInputVA(0,
                                VAR_HEADER_TEXT(Str2ID(ustr,0,SID_ANY_LEN)),
                                VAR_TIMEINP_TIMEFORMAT(time_format),
                                VAR_TIMEINP_TIME(&t),
                                VAR_BOOK(&mbk->book),
                                0);
  AddMSGHook(mbk->tinput,ACTION_ACCEPT, AcceptTIinput);
  AddCommand(mbk->tinput,ACTION_ACCEPT,STR("OK"));
  SoftKey_SetVisible(mbk->tinput,ACTION_ACCEPT,1);
  AddMSGHook(mbk->tinput,ACTION_BACK, CloseTIinput);
  if (iconidname2id(IDN_TIME_INPUT_ICON,-1,&icon_id))
    InputFeedback_SetIcon(mbk->tinput,icon_id);
  return (1);
}

static int TI_OnExit(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  if (mbk->tinput)
  {
    GUI_Free(mbk->tinput);
    mbk->tinput=NULL;
  }
  return (1);  
}
// ----------------------------------------------------------------
void AcceptDIinput(BOOK *bk, void *data)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  unsigned int i=DateInput_GetDateInt(data);
  DATE *date=((DATE *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  date->year=DI_GET_YEAR(i);
  date->mon=TI_GET_MONTH(i);
  date->day=TI_GET_DAY(i);
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
}

void CloseDIinput(BOOK * bk, void *)
{
  BookObj_ReturnPage(bk, PREVIOUS_EVENT);
}

static int DI_OnEnter(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  DATE d;
  wchar_t ustr[64];
  DATE *date=((DATE *)((char *)mbk->cur_hp+sizeof(CFG_HDR)));
  int date_format=4;
  char date_f;
  wchar_t icon_id;
  d.year=date->year;
  d.mon=date->mon;
  d.day=date->day;
  if (!REQUEST_DATEFORMAT_GET(SYNC,&date_f)) date_format=date_f;
  win12512unicode(ustr,mbk->cur_hp->name,MAXELEMS(ustr)-1);
  mbk->dinput=CreateDateInputVA(0,
                                VAR_HEADER_TEXT(Str2ID(ustr,0,SID_ANY_LEN)),
                                VAR_DATEINP_DATEFORMAT(date_format),
                                VAR_DATEINP_DATE(&d),
                                VAR_BOOK(&mbk->book),
                                0);
  AddMSGHook(mbk->dinput,ACTION_ACCEPT, AcceptDIinput);
  AddCommand(mbk->dinput,ACTION_ACCEPT,STR("OK"));
  SoftKey_SetVisible(mbk->dinput,ACTION_ACCEPT,1);
  AddMSGHook(mbk->dinput,ACTION_BACK, CloseDIinput);
  if (iconidname2id(IDN_DATE_INPUT_ICON,-1,&icon_id))
    InputFeedback_SetIcon(mbk->dinput,icon_id);
  return (1);
}

static int DI_OnExit(void *, BOOK * bk)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  if (mbk->dinput)
  {
    GUI_Free(mbk->dinput);
    mbk->dinput=NULL;
  }
  return (1);  
}

const PAGE_MSG bk_msglst_timeinput[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    TI_OnEnter,
  PAGE_EXIT_EVENT_TAG,     TI_OnExit,
  NIL_EVENT_TAG,           NULL
};

const PAGE_MSG bk_msglst_dateinput[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    DI_OnEnter,
  PAGE_EXIT_EVENT_TAG,     DI_OnExit,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_time_input = {"BcfgEdit_TimeInput_Page",0,bk_msglst_timeinput};
const PAGE_DESC bk_date_input = {"BcfgEdit_DateInput_Page",0,bk_msglst_dateinput};
