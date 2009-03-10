#include "inc\lng.h"
#include "inc\mc.h"
#include "inc\zslib.h"
#include "inc\mui.h"
#include "inc\conf_loader.h"




GUI_LIST *menu_id[TOTAL_MENUS]={NULL,NULL,NULL};
GUI_LIST *cur_gui=NULL;

int CountMenuDesc(MENU_DESC *m, int max)
{
  int n=0;
  for (int i=0; i<max; i++)
  {
    if (m[i].enabled) n++;
  }
  return n;
}

void CloseMenu()
{
  for(int ii=0;ii<TOTAL_MENUS;ii++)
    FREE_GUI(menu_id[ii]);
}

void CloseAllMenu()
{
  FREE_GUI(cur_gui);
  CloseMenu();
}


void MC_Open(void)
{
  DoOpen();
  CloseMenu();
}


void MC_Chk(void)
{
  DoChk();
  CloseAllMenu();
}

void MC_ChkAll(void)
{
  DoChkAll();
  CloseAllMenu();
}

void MC_UnChkAll(void)
{
  DoUnCAll();
  CloseAllMenu();
}

void MC_InvChk(void)
{
  DoInvChk();
  CloseAllMenu();
}

MENU_DESC fl_menu[]={
  {ind_chk,     MC_Chk,0},
  {ind_chkall , MC_ChkAll,0},
  {ind_unchall, MC_UnChkAll,0},
  {ind_invchk,  MC_InvChk,0}
};

void MMF_OnBack(BOOK * bk, void *)
{
  FREE_GUI(cur_gui);
}

void MMF_onEnterPressed(BOOK * bk, void *)
{
  int item=ListMenu_GetSelectedItem(cur_gui);
  if (item<MAXELEMS(fl_menu))
  {
    if (fl_menu[item].proc) fl_menu[item].proc();
  }
}

void MM_File(void)
{
  GUI_LIST *lo;
  STRID sid;
  cur_gui=lo=CreateListObject(&MCBook->book,0);
  STRID texts[MAXELEMS(fl_menu)];
  wchar_t *names;
  for (int i=0; i<MAXELEMS(fl_menu); i++)
  {
    names=muitxt(fl_menu[i].id);
    texts[i]=Str2ID(names,0,SID_ANY_LEN);
  }
  OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,texts,MAXELEMS(fl_menu));
  sid=Str2ID(muitxt(ind_mcmenu),0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,MAXELEMS(fl_menu));
  SetCursorToItem(lo,0);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,MMF_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,MMF_OnBack);
  ShowWindow(lo);
}


void MC_Paste(void)
{
  DoPaste();
  CloseAllMenu();
}

void MC_Cancel(void)
{
  DoCancel();
  CloseAllMenu();
}

void MC_Copy(void)
{
  DoCopy();
  CloseAllMenu();
}

void MC_Move(void)
{
  DoMove();
  CloseAllMenu();
}

void MC_Delit(void)
{
  DoDel();
  CloseAllMenu();
}

void MC_NewDir(void)
{
  DoNewDir();
  CloseAllMenu();
}

void MC_Rename(void)
{
  DoRen();
  CloseAllMenu();
}

void MC_NewFile(void)
{
  //DoNewFile();
  CloseAllMenu();
}

MENU_DESC op_menu[]={
  {ind_past,    MC_Paste,0},
  {ind_cancel,  MC_Cancel,0},
  {ind_copy,    MC_Copy,0},
  {ind_move,    MC_Move,0},
  {ind_del,     MC_Delit,0},
  {ind_rename,  MC_Rename,0},
  {ind_newdir,  MC_NewDir,0},
  {ind_newfile, MC_NewFile,0}
};

void MMO_OnBack(BOOK * bk, void *)
{
  FREE_GUI(cur_gui);
}

void MMO_onEnterPressed(BOOK * bk, void *)
{
  int item=ListMenu_GetSelectedItem(cur_gui);
  int n=0;
  for (int i=0; i<MAXELEMS(op_menu); i++) {
    if (op_menu[i].enabled){
      if (item==n){
        if (op_menu[i].proc) op_menu[i].proc();
      }
      n++;
    }
  }
}

void MM_Oper(void)
{
  GUI_LIST *lo;
  STRID sid;
  cur_gui=lo=CreateListObject(&MCBook->book,0);
  STRID texts[MAXELEMS(op_menu)];
  if (IsInArchive())
  {
    for (int i=0; i<MAXELEMS(op_menu); i++) op_menu[i].enabled=0;   // Вырубаем все
    op_menu[2].enabled=2;  
  }
  else
  {
    for (int i=0; i<MAXELEMS(op_menu); i++) op_menu[i].enabled=1;   // Врубаем все
  }
  int n=0;
  wchar_t *names;
  for (int i=0; i<MAXELEMS(op_menu); i++)
  {
    if (op_menu[i].enabled)
    {
      names=muitxt(op_menu[i].id);
      texts[n]=Str2ID(names,0,SID_ANY_LEN);
      n++;
    }
  }
  OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,texts,n);
  sid=Str2ID(muitxt(ind_mcmenu),0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,n);
  SetCursorToItem(lo,0);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,MMO_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,MMO_OnBack);
  ShowWindow(lo);
}

void MC_FileProp(void)
{

}

void MC_SRTName(void)
{
  DoSortN();
  CloseAllMenu();
}

void MC_SRTExt(void)
{
  DoSortE();
  CloseAllMenu();
}

void MC_SRTSize(void)
{
  DoSortS();
  CloseAllMenu();
}

void MC_SRTDate(void)
{
  DoSortD();
  CloseAllMenu();
}

void MC_SRTRev(void)
{
  DoSortR();
  CloseAllMenu();
}


MENU_DESC st_menu[]={
  {ind_sortn, MC_SRTName,0},
  {ind_sorte, MC_SRTExt,0},
  {ind_sorts, MC_SRTSize,0},
  {ind_sortd, MC_SRTDate,0},
  {ind_sortr, MC_SRTRev,0}
};

void MMS_OnBack(BOOK * bk, void *)
{
  FREE_GUI(cur_gui);
}

void MMS_onEnterPressed(BOOK * bk, void *)
{
  int item=ListMenu_GetSelectedItem(cur_gui);
  if (item<MAXELEMS(st_menu))
  {
    if (st_menu[item].proc) st_menu[item].proc();
  }
}

int MMS_On_Msg(GUI_MESSAGE * msg)
{
  wchar_t icn=0;
  int d;
  STRID str=LGP_NULL;
  switch(msg->msg)
  {
  case 1:
    d=GUIonMessage_GetCreatedItemIndex(msg);
    if (d<MAXELEMS(st_menu))
    {
      str=Str2ID(muitxt(st_menu[d].id),0,SID_ANY_LEN);
      SetMenuItemText0(msg,str);
    }
    int sort=_CurTab->sort;
    switch(d)
    {
    case 0:
      icn=(sort&STV_MASK)==ST_NAME?STD_ICONS[ICN_CH_RB]:STD_ICONS[ICN_RB];
      break;
    case 1:
      icn=(sort&STV_MASK)==ST_EXT?STD_ICONS[ICN_CH_RB]:STD_ICONS[ICN_RB];
      break;
    case 2:
      icn=(sort&STV_MASK)==ST_SIZE?STD_ICONS[ICN_CH_RB]:STD_ICONS[ICN_RB];
      break;
    case 3:
      icn=(sort&STV_MASK)==ST_DATE?STD_ICONS[ICN_CH_RB]:STD_ICONS[ICN_RB];
      break;
    case 4:
      icn=(sort&STD_MASK)==ST_REVERS?STD_ICONS[ICN_CH_CB]:STD_ICONS[ICN_CB];
      break;
    }
    if (icn)  SetListObjectItemIcon(msg,0,icn);
  }
  return(1);
};

void MM_Sort(void)
{
  GUI_LIST *lo;
  STRID sid;
  wchar_t *head=muitxt(ind_mcmenu);
  cur_gui=lo=CreateListObject(&MCBook->book,0);
  sid=Str2ID(head,0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,MAXELEMS(st_menu));
  SetCursorToItem(lo,0);
  ListMenu_SetOnMessages(lo,MMS_On_Msg);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,MMS_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,MMS_OnBack);
  ShowWindow(lo);
}

void MC_Filter(void)
{
  DoFilter();
  CloseAllMenu();
}

void MC_Refresh(void)
{
  DoRefresh();
  CloseAllMenu();
}

MENU_DESC vw_menu[]={
  {ind_sort_m,  MM_Sort,0},
  {ind_filter,  MC_Filter,0},
  {ind_refresh, MC_Refresh,0}
};

void MMV_OnBack(BOOK * bk, void *)
{
  FREE_GUI(menu_id[view_ind]);
}

void MMV_onEnterPressed(BOOK * bk, void *)
{
  int item=ListMenu_GetSelectedItem(menu_id[view_ind]);
  if (item<MAXELEMS(vw_menu))
  {
    if (vw_menu[item].proc) vw_menu[item].proc();
  }
}

void MM_View(void)
{
  GUI_LIST *lo;
  STRID sid;
  menu_id[view_ind]=lo=CreateListObject(&MCBook->book,0);
  STRID texts[MAXELEMS(vw_menu)];
  wchar_t *names;
  for (int i=0; i<MAXELEMS(vw_menu); i++)
  {
    names=muitxt(vw_menu[i].id);
    texts[i]=Str2ID(names,0,SID_ANY_LEN);
  }
  OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,texts,MAXELEMS(vw_menu));
  sid=Str2ID(muitxt(ind_mcmenu),0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,MAXELEMS(vw_menu));
  SetCursorToItem(lo,0);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,MMV_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,MMV_OnBack);
  ShowWindow(lo);
}







void MC_BMAdd(void)
{
  DoBMAdd();
  CloseAllMenu();
}

void MC_BMList(void)
{
  DoBMList();
  CloseAllMenu();
}

MENU_DESC bm_menu[]={
  {ind_add, MC_BMAdd,0},
  {ind_bml, MC_BMList,0},
};

void MMBM_OnBack(BOOK * bk, void *)
{
  FREE_GUI(cur_gui);
}

void MMBM_onEnterPressed(BOOK * bk, void *)
{
  int item=ListMenu_GetSelectedItem(cur_gui);
  if (item<MAXELEMS(bm_menu))
  {
    if (bm_menu[item].proc) bm_menu[item].proc();
  }
}

void MM_BM(void)
{
  GUI_LIST *lo;
  STRID sid;
  wchar_t *head=muitxt(ind_mcmenu);
  cur_gui=lo=CreateListObject(&MCBook->book,0);
  STRID texts[MAXELEMS(bm_menu)];
  wchar_t *names;
  for (int i=0; i<MAXELEMS(bm_menu); i++)
  {
    names=muitxt(bm_menu[i].id);
    texts[i]=Str2ID(names,0,SID_ANY_LEN);
  }
  OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,texts,MAXELEMS(bm_menu));
  sid=Str2ID(head,0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,MAXELEMS(bm_menu));
  SetCursorToItem(lo,0);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,MMBM_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,MMBM_OnBack);
  ShowWindow(lo);
}

void MC_DrvInf(void)
{
  CloseMenu();
}

void MC_Options(void)
{
  ExecuteFile(successed_config_path, successed_config_name);
  CloseMenu();
}

void MC_AboutDlg(void)
{
  CloseMenu();
}

MENU_DESC mis_menu[]={
  {ind_bm_m,     MM_BM,0},
  {ind_drvinf,   MC_DrvInf,0},
  {ind_settings, MC_Options,0},
  {ind_about_m,  MC_AboutDlg,0}  
};

void MMM_OnBack(BOOK * bk, void *)
{
  FREE_GUI(menu_id[misc_ind]);
}

void MMM_onEnterPressed(BOOK * bk, void *)
{
  int item=ListMenu_GetSelectedItem(menu_id[misc_ind]);
  if (item<MAXELEMS(mis_menu))
  {
    if (mis_menu[item].proc) mis_menu[item].proc();
  }
}

void MM_Misc(void)
{
  GUI_LIST *lo;
  STRID sid;
  wchar_t *head=muitxt(ind_mcmenu);
  menu_id[misc_ind]=lo=CreateListObject(&MCBook->book,0);
  STRID texts[MAXELEMS(mis_menu)];
  wchar_t *names;
  for (int i=0; i<MAXELEMS(mis_menu); i++)
  {
    names=muitxt(mis_menu[i].id);
    texts[i]=Str2ID(names,0,SID_ANY_LEN);
  }
  OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,texts,MAXELEMS(mis_menu));
  sid=Str2ID(head,0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,MAXELEMS(mis_menu));
  SetCursorToItem(lo,0);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,MMM_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,MMM_OnBack);
  ShowWindow(lo);
}

void MC_Exit(void)
{
  CloseMenu();
  DoExit();
}


MENU_DESC opt_menu[]={
  {ind_open,   MC_Open,0},
  {ind_file_m, MM_File,0},
  {ind_oper_m, MM_Oper,0},
  {ind_prop,   MC_FileProp,0},
  {ind_view_m, MM_View,0},
  {ind_misc_m, MM_Misc,0},
  {ind_exit,   MC_Exit,0}
};

int act_mmid=0;

void MM_OnBack(BOOK * bk, void *)
{
  CloseMenu();
}

void MM_onEnterPressed(BOOK * bk, void *)
{
  int item=ListMenu_GetSelectedItem(menu_id[main_ind]);
  int n=0;
  for (int i=0; i<MAXELEMS(opt_menu); i++)
  {
    if (opt_menu[i].enabled)
    {
      if (item==n)
      {
        if (opt_menu[i].proc) opt_menu[i].proc();
      }
      n++;
    }
  }
}

void MM_Main(void)
{
  GUI_LIST *lo;
  STRID sid;
  menu_id[main_ind]=lo=CreateListObject(&MCBook->book,0);
  STRID texts[MAXELEMS(opt_menu)];
  for (int i=0; i<MAXELEMS(opt_menu); i++) opt_menu[i].enabled=1;   // Врубаем все
  if (IsInArchive())
  {
    opt_menu[0].enabled=0;
  }
  int n=0;
  wchar_t *names;
  for (int i=0; i<MAXELEMS(opt_menu); i++)
  {
    if (opt_menu[i].enabled)
    {
      names=muitxt(opt_menu[i].id);
      texts[n]=Str2ID(names,0,SID_ANY_LEN);
      n++;
    }
  }
  OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,texts,n);
  sid=Str2ID(muitxt(ind_mcmenu),0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,n);
  SetCursorToItem(lo,0);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,MM_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,MM_OnBack);
  ShowWindow(lo);
}
