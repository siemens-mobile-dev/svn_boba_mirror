#include "..\inc\swilib.h"
#include "lng.h"
#include "mc.h"

GBSTMR cmtm;

#define main_ind 0
#define misc_ind 1
#define view_ind 2
#define last_ind view_ind

int menu_id[last_ind+1]={0,0,0};

void CloseMenu()
{
  if(IsGuiOnTop(MAINGUI_ID))
  {
   for(int ii=0;ii<=last_ind;ii++)
   menu_id[ii]=0;
   return;
  }

  int ndr=0;
  for(int ii=0;ii<=last_ind;ii++)
  if(menu_id[ii])  
  {
    if(IsGuiOnTop(menu_id[ii]))
    {
      GeneralFunc_flag1(menu_id[ii],1);
      menu_id[ii] = 0;
    }
    else
    ndr = 1;
  }
  
  if(ndr)
  GBS_StartTimerProc(&cmtm,1,CloseMenu);
}

void CloseAllMenu()
{
  GeneralFuncF1(1);
  CloseMenu();
}





void MC_Exit(GUI *data)
{
  CloseMenu();
  DoExit();
}

void MC_AboutDlg(GUI *data)
{
  ShowMSG(2,(int)psz_about); 
}

void MC_Options(GUI *data)
{
  if(wsbuf)
  {
   wsprintf(wsbuf, _s_s, etc_path, cfg_file);
   ExecuteFile(wsbuf,0,0);
  } 
}

void Under(void)
{
  ShowMSG(2, (int) "...кто здесь?");
}

void MC_Past(GUI *data)
{
  DoPast();
  CloseAllMenu();
}

void MC_Cancel(GUI *data)
{
  DoCancel();
  CloseAllMenu();
}

void MC_Copy(GUI *data)
{
  DoCopy();
  CloseAllMenu();
}

void MC_Move(GUI *data)
{
  DoMove();
  CloseAllMenu();
}

void MC_Delit(GUI *data)
{
  DoDel();
  CloseAllMenu();
}

void MC_Chk(GUI *data)
{
  DoChk();
  CloseAllMenu();
}

void MC_ChkAll(GUI *data)
{
  DoChkAll();
  CloseAllMenu();
}

void MC_UnChkAll(GUI *data)
{
  DoUnCAll();
  CloseAllMenu();
}

void MC_InvChk(GUI *data)
{
  DoInvChk();
  CloseAllMenu();
}

void MC_BMAdd(GUI *data)
{
  DoBMAdd();
  CloseAllMenu();
}

void MC_BMList(GUI *data)
{
  DoBMList();
  CloseAllMenu();
}

void MC_SRTName(GUI *data)
{
  DoSortN();
  CloseAllMenu();
}

void MC_SRTExt(GUI *data)
{
  DoSortE();
  CloseAllMenu();
}

void MC_SRTSize(GUI *data)
{
  DoSortS();
  CloseAllMenu();
}

void MC_SRTDate(GUI *data)
{
  DoSortD();
  CloseAllMenu();
}

void MC_SRTRev(GUI *data)
{
  DoSortR();
  CloseAllMenu();
}

void MC_Refresh(GUI *data)
{
  DoRefresh();
  CloseAllMenu();
}

void MC_Filter(GUI *data)
{
  DoFilter();
  CloseAllMenu();
}







HEADER_DESC tmenuhdr={0,0,0,0,0,LGP_NULL/*(int)sz_mcmenu*/,LGP_NULL};
int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,LGP_NULL/*(int)sz_select*/},
  {0x0001,0x0000,LGP_NULL/*(int)sz_back*/},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};
SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};






MENUITEM_DESC opt_menuitems[]={
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};
const MENUPROCS_DESC opt_menuprocs[]={
  MC_Open, 
  MM_File,
  MM_Oper,
  MC_FileProp,
  MM_View,
  MM_Misc,
  MC_Exit,
};

MENU_DESC opt_menu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  opt_menuitems,
  opt_menuprocs,
  sizeof(opt_menuprocs)/sizeof(void *)
};






MENUITEM_DESC op_menuitems[]={
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};
const MENUPROCS_DESC op_menuprocs[]={
  MC_Past, 
  MC_Cancel, 
  MC_Copy, 
  MC_Move, 
  MC_Delit, 
  MC_Rename, 
  MC_NewDir, 
};

MENU_DESC op_menu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  op_menuitems,
  op_menuprocs,
  sizeof(op_menuprocs)/sizeof(void *)
};




MENUITEM_DESC mis_menuitems[]={
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};
const MENUPROCS_DESC mis_menuprocs[]={
  MM_BM,
  MC_DrvInf,
  MC_Options,
  MC_AboutDlg,
};

MENU_DESC mis_menu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  mis_menuitems,
  mis_menuprocs,
  sizeof(mis_menuprocs)/sizeof(void *)
};



MENUITEM_DESC fl_menuitems[]={
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};
const MENUPROCS_DESC fl_menuprocs[]={
  MC_Chk,
  MC_ChkAll,
  MC_UnChkAll,
  MC_InvChk,
};

MENU_DESC fl_menu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  fl_menuitems,
  fl_menuprocs,
  sizeof(fl_menuprocs)/sizeof(void *)
};



MENUITEM_DESC bm_menuitems[]={
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};
const MENUPROCS_DESC bm_menuprocs[]={
  MC_BMAdd,
  MC_BMList,
};

MENU_DESC bm_menu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  bm_menuitems,
  bm_menuprocs,
  sizeof(bm_menuprocs)/sizeof(void *)
};

MENUITEM_DESC st_menuitems[]={
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};
const MENUPROCS_DESC st_menuprocs[]={
  MC_SRTName,
  MC_SRTExt,
  MC_SRTSize,
  MC_SRTDate,
  MC_SRTRev,
};

MENU_DESC st_menu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  st_menuitems,
  st_menuprocs,
  sizeof(st_menuprocs)/sizeof(void *)
};

MENUITEM_DESC vw_menuitems[]={
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
  {NULL,LGP_NULL,LGP_NULL,0,NULL,MENU_FLAG3,MENU_FLAG2},
};
const MENUPROCS_DESC vw_menuprocs[]={
  MM_Sort,
  MC_Filter,
  MC_Refresh,
};

MENU_DESC vw_menu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  0,
  NULL,
  vw_menuitems,
  vw_menuprocs,
  sizeof(vw_menuprocs)/sizeof(void *)
};

void MM_Main(GUI *data)
{
  menu_id[main_ind] = CreateMenu(0,0,&opt_menu,&tmenuhdr,0,1,0,0);
}

void MM_Oper(GUI *data)
{
  CreateMenu(0,0,&op_menu,&tmenuhdr,0,1,0,0);
}

void MM_Misc(GUI *data)
{
  menu_id[misc_ind] = CreateMenu(0,0,&mis_menu,&tmenuhdr,0,1,0,0);
}

void MM_File(GUI *data)
{
  CreateMenu(0,0,&fl_menu,&tmenuhdr,0,1,0,0);
}

void MM_BM(GUI *data)
{
  CreateMenu(0,0,&bm_menu,&tmenuhdr,0,1,0,0);
}

void MM_Sort(GUI *data)
{
  CreateMenu(0,0,&st_menu,&tmenuhdr,0,1,0,0);
}

void MM_View(GUI *data)
{
  menu_id[view_ind] = CreateMenu(0,0,&vw_menu,&tmenuhdr,0,1,0,0);
}

#pragma inline
void patch_header(HEADER_DESC* head)
{
  head->rc.x=0;
  head->rc.y=YDISP;
  head->rc.x2=ScreenW()-1;
  head->rc.y2=HeaderH()+YDISP-1;
}
#pragma inline
void patch_input(INPUTDIA_DESC* inp)
{
  inp->rc.x=0;
  inp->rc.y=HeaderH()+1+YDISP;
  inp->rc.x2=ScreenW()-1;
  inp->rc.y2=ScreenH()-SoftkeyH()-1;
}

void InitAllMD()
{
  patch_header(&tmenuhdr);
  
  extern HEADER_DESC txt_inp_hdr;
  extern INPUTDIA_DESC txt_inp_desc;
  extern HEADER_DESC file_prop_hdr;
  extern INPUTDIA_DESC file_prop_desc;
  extern HEADER_DESC disk_prop_hdr;
  extern INPUTDIA_DESC disk_prop_desc;
  
  patch_header(&txt_inp_hdr);
  patch_input(&txt_inp_desc);
  txt_inp_desc.rc.y+=20;
  txt_inp_desc.rc.x+=10;
  patch_header(&file_prop_hdr);
  patch_input(&file_prop_desc);
  patch_header(&disk_prop_hdr);
  patch_input(&disk_prop_desc);
}
