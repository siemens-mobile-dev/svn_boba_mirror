#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"
#include "main.h"
#include "calc_object.h"

#include "calc_menu.h"

CALC_SETTINGS calc_set;

const wchar_t setting_file[]=L"SCalka.cfg";

void ReadCalcSettings(void)
{
  int f=-1;
  if (!w_chdir(GetDir(DIR_ELFS_CONFIG|MEM_EXTERNAL)))
    f=w_fopen(setting_file, WA_Read, 0x1FF, 0);
  if (f<0)
  {
    if (!w_chdir(GetDir(DIR_ELFS_CONFIG|MEM_INTERNAL)))
      f=w_fopen(setting_file, WA_Read, 0x1FF, 0);
  }
  if (f>=0)
  {
    if (w_lseek(f, 0, WSEEK_END)==sizeof(CALC_SETTINGS))
    {
      w_lseek(f, 0, WSEEK_SET);
      w_fread(f, &calc_set, sizeof(CALC_SETTINGS));      
    }
    w_fclose(f);    
  }
  else
  {
    strcpy(calc_set.fmt, "%1.10lg");
    calc_set.drg=DEGREES;
    calc_set.x=calc_set.y=0;
  }
}

void WriteCalcSettings(void)
{
  int f=-1;
  if (!w_chdir(GetDir(DIR_ELFS_CONFIG|MEM_EXTERNAL)))
    f=w_fopen(setting_file, WA_Write|WA_Create|WA_Truncate, 0x1FF, 0);
  if (f<0)
  {
    if (!w_chdir(GetDir(DIR_ELFS_CONFIG|MEM_INTERNAL)))
      f=w_fopen(setting_file, WA_Write|WA_Create|WA_Truncate, 0x1FF, 0);
  }
  if (f>=0)
  {
    w_fwrite(f, &calc_set, sizeof(CALC_SETTINGS));
    w_fclose(f);    
  }    
}

//==============================================================================
void IV_OnBack(BOOK * bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->calc_insvar);
}

void IV_onEnterPressed(BOOK * bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->calc_insvar);
  if (item<(VAR_Z-VAR_A+1))
    insert_operation(VAR_A+item);
  FREE_GUI(mbk->calc_insvar);
  FREE_GUI(mbk->calc_menu);
}

int IV_On_Msg(GUI_MESSAGE * msg)
{
  int d;
  STRID str=LGP_NULL;
  switch(msg->msg)
  {
  case 1:
    d=GUIonMessage_GetCreatedItemIndex(msg);
    if (d<(VAR_Z-VAR_A+1))
    {
      str=0x78000000|('a'+d);
      SetMenuItemText0(msg,str);
    }
  }
  return(1);
}

void InsertVar_CreateMenu(MyBOOK *mbk)
{
  GUI_LIST *lo;
  STRID sid;
  mbk->calc_insvar=lo=CreateListObject(&mbk->book,0);
  sid=Str2ID(L"Settings",0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,VAR_Z-VAR_A+1);
  SetCursorToItem(lo,0);
  //SetMenuItemStyle(lo,3);
  ListMenu_SetOnMessages(lo,IV_On_Msg);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,IV_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,IV_OnBack);
  ShowWindow(lo);
}
//==============================================================================

void AngleSelect_OnCloseCBoxGui(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->angle_sel_list);
}

void AngleSelect_OnSelectCBoxGui(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  int item=OneOfMany_GetSelected(mbk->angle_sel_list);
  calc_set.drg=item;
  FREE_GUI(mbk->angle_sel_list); 
}

const wchar_t *angle_names[]={
  L"Degrees",
  L"Radians",
  L"Grads"
};

void AngleSelect_CreateCBoxGui(MyBOOK *mbk)
{

  STRID strid[3];
  GUI_ONEOFMANY *om;
  mbk->angle_sel_list=om=CreateOneOfMany(&mbk->book);
  GuiObject_SetTitleText(om,Str2ID(L"Angle setup",0,SID_ANY_LEN));
  for (int i=0; i<3; i++) strid[i]=Str2ID(angle_names[i],0,SID_ANY_LEN);
  OneOfMany_SetTexts(om,strid,3);
  OneOfMany_SetChecked(om,calc_set.drg);
  GUIObject_Softkey_SetAction(om,ACTION_BACK,AngleSelect_OnCloseCBoxGui);
  GUIObject_Softkey_SetAction(om,ACTION_SELECT1,AngleSelect_OnSelectCBoxGui);
  ShowWindow(om);
}
//------------------------------------------------------------------------------
void OnBackEditFmtGui(BOOK * bk, u16 *string, int len)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->edit_fmtstr);
}

void OnOkEditFmtGui(BOOK * bk, wchar_t *string, int len)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  unicode2win1251(calc_set.fmt,string,sizeof(calc_set.fmt)-1);
  FREE_GUI(mbk->edit_fmtstr);
}

void CreateEditFmtStr(MyBOOK *myBook)
{
  wchar_t ustr[sizeof(calc_set.fmt)];
  STRID text, header_name;
  win12512unicode(ustr,calc_set.fmt,sizeof(calc_set.fmt)-1);
  text=Str2ID(ustr,0,SID_ANY_LEN);
  header_name=Str2ID("Edit format str",6,SID_ANY_LEN);
  myBook->edit_fmtstr=(GUI *)CreateStringInput(0,
                                               VAR_HEADER_TEXT(header_name),
                                               VAR_STRINP_MIN_LEN(1),
                                               VAR_STRINP_MAX_LEN(sizeof(calc_set.fmt)-1),
                                               VAR_STRINP_MODE(1),
                                               VAR_BOOK(myBook),
                                               VAR_STRINP_TEXT(text),
                                               VAR_PREV_ACTION_PROC(OnBackEditFmtGui),
                                               VAR_OK_PROC(OnOkEditFmtGui),
                                               0);
}
//------------------------------------------------------------------------------
void AutocalcSelect_OnCloseCBoxGui(BOOK * bk, void *)
{
  MyBOOK * myBook=(MyBOOK *)bk;
  FREE_GUI(myBook->autocalc_sel_list);
}

void AutocalcSelect_OnSelectCBoxGui(BOOK * bk, void *)
{
  MyBOOK * mbk=(MyBOOK *)bk;
  int item=OneOfMany_GetSelected(mbk->autocalc_sel_list);
  calc_set.auto_recalc=item;
  FREE_GUI(mbk->autocalc_sel_list); 
}

const wchar_t *dis_ena[]={
  L"Disable",
  L"Enable"
};

void AutocalcSelect_CreateCBoxGui(MyBOOK *mbk)
{

  STRID strid[2];
  GUI_ONEOFMANY *om;
  mbk->autocalc_sel_list=om=CreateOneOfMany(&mbk->book);
  GuiObject_SetTitleText(om,Str2ID(L"Realtime calc",0,SID_ANY_LEN));
  for (int i=0; i<2; i++) strid[i]=Str2ID(dis_ena[i],0,SID_ANY_LEN);
  OneOfMany_SetTexts(om,strid,2);
  OneOfMany_SetChecked(om,calc_set.auto_recalc);
  GUIObject_Softkey_SetAction(om,ACTION_BACK,AutocalcSelect_OnCloseCBoxGui);
  GUIObject_Softkey_SetAction(om,ACTION_SELECT1,AutocalcSelect_OnSelectCBoxGui);
  ShowWindow(om);
}

//------------------------------------------------------------------------------
void CS_OnBack(BOOK * bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->calc_settings);
}

void CS_onEnterPressed(BOOK * bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->calc_settings);
  switch(item)
  {
  case 0:
    AngleSelect_CreateCBoxGui(mbk);
    break;
  case 1:
    CreateEditFmtStr(mbk);
    break;
  case 2:
    AutocalcSelect_CreateCBoxGui(mbk);
    break;
  default:
    FREE_GUI(mbk->calc_settings);
    break;
  }
}


int CS_On_Msg(GUI_MESSAGE * msg)
{
  const wchar_t *names[]={
    L"Angle",
    L"Fmt str",
    L"Real-Time Calculation"
  };
  int d;
  STRID str=LGP_NULL;
  switch(msg->msg)
  {
  case 1:
    d=GUIonMessage_GetCreatedItemIndex(msg);
    if (d<3)
    {
      STRID tx=LGP_NULL;
      str=Str2ID(names[d],0,SID_ANY_LEN);
      SetMenuItemText0(msg,str);
      switch(d)
      {
      case 0:
        tx=Str2ID(angle_names[calc_set.drg],0,SID_ANY_LEN);
        break;
      case 1:
        tx=Str2ID(calc_set.fmt,6,SID_ANY_LEN);
        break;
      case 2:
        tx=Str2ID(dis_ena[calc_set.auto_recalc],0,SID_ANY_LEN);
        break;
      }
      SetMenuItemText1(msg,tx);
    }
  }
  return(1);
}

void CreateCalcSettings(MyBOOK *mbk)
{
  GUI_LIST *lo;
  STRID sid;
  mbk->calc_settings=lo=CreateListObject(&mbk->book,0);
  sid=Str2ID(L"Settings",0,SID_ANY_LEN);
  GuiObject_SetTitleText(lo,sid);
  SetNumOfMenuItem(lo,3);
  SetCursorToItem(lo,0);
  SetMenuItemStyle(lo,3);
  ListMenu_SetOnMessages(lo,CS_On_Msg);
  GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,CS_onEnterPressed); 
  GUIObject_Softkey_SetAction(lo,ACTION_BACK,CS_OnBack);
  ShowWindow(lo);
}

//==============================================================================

void CM_OnBack(BOOK * bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  FREE_GUI(mbk->calc_menu);
}

void CM_onEnterPressed(BOOK * bk, void *)
{
  MyBOOK *mbk=(MyBOOK *)bk;
  int item=ListMenu_GetSelectedItem(mbk->calc_menu);
  switch(item)
  {
  case 0:
    
    break;
  case 1:
    InsertVar_CreateMenu(mbk);
    break;
  case 2:
    
    break;
  case 3:
    
    break;
  case 4:
    calc_set.x=d_answer;
    FREE_GUI(mbk->calc_menu);
    break;
  case 5:
    calc_set.y=d_answer;
    FREE_GUI(mbk->calc_menu); 
    break;
  case 6:
    CreateCalcSettings(mbk);
    break;
  default:
    FREE_GUI(mbk->calc_menu);
    break;
  }
}

void CreateCalcMenu(void)
{
  GUI_LIST *lo;
  STRID sid;
  STRID texts[7];
  BOOK *bk=FindBook(isScalkaBook);
  if (bk)
  {
    MyBOOK *mbk=(MyBOOK *)bk;
    mbk->calc_menu=lo=CreateListObject(&mbk->book,0);
    sid=Str2ID(L"Menu",0,SID_ANY_LEN);
    texts[0]=Str2ID(L"Set formula",0,SID_ANY_LEN);
    texts[1]=Str2ID(L"Insert var",0,SID_ANY_LEN);
    texts[2]=Str2ID(L"Add formula",0,SID_ANY_LEN);
    texts[3]=Str2ID(L"View vars",0,SID_ANY_LEN);
    texts[4]=Str2ID(L"Ans To X",0,SID_ANY_LEN);
    texts[5]=Str2ID(L"Ans To Y",0,SID_ANY_LEN);
    texts[6]=Str2ID(L"Settings",0,SID_ANY_LEN);
    OneOfMany_SetTexts((GUI_ONEOFMANY *)lo,texts,7);
    GuiObject_SetTitleText(lo,sid);
    SetNumOfMenuItem(lo,7);
    SetCursorToItem(lo,0);
    GUIObject_Softkey_SetAction(lo,ACTION_SELECT1,CM_onEnterPressed); 
    GUIObject_Softkey_SetAction(lo,ACTION_BACK,CM_OnBack);
    ShowWindow(lo);
  }
}
