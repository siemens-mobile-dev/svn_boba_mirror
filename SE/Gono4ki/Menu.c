#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"
#include "..\\include\cfg_items.h"
#include "StructBook.h"
#include "Menu.h"
#include "MyFUNC.h"
#include "Game.h"
#include "ErrorIAR.h"
#include "Rec_Check.h"


const PAGE_MSG bk_msglst_gm[] @ "DYN_PAGE"  = 
{
  PAGE_ENTER_EVENT_TAG,    CreateGame,
  NIL_EVENT_TAG,           NULL
};

const PAGE_DESC bk_game = {"Game_GuiMenu_Page",0,bk_msglst_gm};

void ShowResults(TABL*t)
{
  int text[60];
  int i, j;
  j=0;
  for(i=0; i<10; i++)
  {
    text[j]=int2strID(i+1);
    j++;
    text[j]=STR(". ");
    j++;
    text[j]=Str2ID(t->name[i], 0, SID_ANY_LEN);
    j++;
    text[j]=STR(" - ");
    j++;
    text[j]=int2strID(t->rezultat[i]);
    j++;
    text[j]=STR("\n");
    j++;
  }
  int ID=Str2ID(text, 5, 60);
  MessageBox(STR("Результаты"), ID, 0, 2, 0, 0);
}

void DestroyInputI()
{
  GUI_Free(MyBK()->gui1);
  MyBK()->gui1=0;
  ShowWindow(MyBK()->gui);
}


void OnEntInputI(BOOK * bk,wchar_t * string,int len)
{
  wstrcpy(((MyBook*)bk)->nast->im->names[ListMenu_GetSelectedItem(((MyBook*)bk)->gui)], string);
  GUI_Free(((MyBook*)bk)->gui1);
  ((MyBook*)bk)->gui1=0;
  ImagesDel(bk);
  CheckResurs(MyBK());
  ShowWindow(((MyBook*)bk)->gui);
}

void CreateInputI(int x, IMGN*i, MyBook*bk)
{
  if(bk->gui1) GUI_Free(bk->gui1);
  bk->gui1=0;
  bk->gui1=CreateStringInput(0,
                                       VAR_PREV_ACTION_PROC(DestroyInputI),
                                       VAR_LONG_BACK_PROC(DestroyInputI),
                                       VAR_BOOK((BOOK*)bk),
                                       VAR_STRINP_FIXED_TEXT(Str2ID(L"Изменить имя:",0, SID_ANY_LEN)),
                                       VAR_STRINP_TEXT(Str2ID(i->names[x], 0, SID_ANY_LEN)),
                                       VAR_STRINP_MAX_LEN(127),
                                       VAR_STRINP_MIN_LEN(0),
                                       VAR_OK_PROC(OnEntInputI),
                                       0); 
}

void OnEntINM( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  if(item<3)
  {
    CreateInputI(item, ((MyBook*)bk)->nast->im, (MyBook*)bk);
  }
  else
  {
    if(MyBK()->nast->im->on) MyBK()->nast->im->on=0;
    else MyBK()->nast->im->on=1;
    GUI_Free(((MyBook*)bk)->gui);
    ((MyBook*)bk)->gui=0;
    CreateINM((MyBook*)bk, 3);
  }      
}

int SetTitlePunktsINM(GUI_MESSAGE * msg)
{
  int item, ID;
  switch( GUIonMessage_GetMsg(msg) )
  {
  case 1:
    item=GUIonMessage_GetCreatedItemIndex(msg);
    for(int i=0; i<3; i++)
    {
      if(item==i)
      {
        SetMenuItemText0(msg,Str2ID(GetNamesOfIMN(i, 0),0,SID_ANY_LEN));
        SetMenuItemText1(msg,Str2ID(((MyBook*)isBookX(NameMyElf, 0))->nast->im->names[i],0,SID_ANY_LEN));
        iconidname2id(L"DB_LIST_PICTURE_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
    }
    if(item==3)
    {
      SetMenuItemText0(msg,Str2ID(L"Картинки:",0,SID_ANY_LEN));
      if(MyBK()->nast->im->on)
      {
        SetMenuItemText1(msg,Str2ID(L"Свои",0,SID_ANY_LEN));
        iconidname2id(L"CALLS_INCOMING_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      else
      {
        SetMenuItemText1(msg,Str2ID(L"Из эльфа",0,SID_ANY_LEN));
        iconidname2id(L"CALLS_OUTGOING_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
    }
  }
  return 0;
}

void ExitINM( BOOK* bk, GUI* )
{
  GUI_Free(((MyBook*)bk)->gui);
  ((MyBook*)bk)->gui=0;
  CreateNM((MyBook*)bk);
}

void CreateINM(MyBook*bk, int x)
{
  if(bk->gui) GUI_Free(bk->gui);
  bk->gui=0;
  GUI_LIST*INM=CreateListObject((BOOK*)bk,0);
  ((MyBook*)bk)->gui=INM;
  GuiObject_SetTitleText(INM, Str2ID(L"Картинки",0,SID_ANY_LEN));
  SetNumOfMenuItem(INM, 4);
  OneOfMany_SetonMessage(INM,SetTitlePunktsINM);
  SetCursorToItem(INM,x);
  SetMenuItemStyle(INM,3);
  GUIObject_Softkey_SetAction(INM,ACTION_BACK,ExitINM);
  GUIObject_Softkey_SetAction(INM,ACTION_LONG_BACK,ExitINM);
  GUIObject_Softkey_SetAction(INM,ACTION_SELECT1,OnEntINM);
  GUIObject_Softkey_SetText(INM,ACTION_SELECT1,STR("Изменить"));
  ShowWindow(INM);
}

void ExitMNM( BOOK* bk, GUI* )
{
  GUI_Free(((MyBook*)bk)->gui);
  ((MyBook*)bk)->gui=0;
  CreateNM((MyBook*)bk);
}

void OnMNM1( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  for(int i=0; i<6; i++)
  {
    if(item==i+1)
    {
      if(((MyBook*)bk)->nast->sound->status[i]==0) ((MyBook*)bk)->nast->sound->status[i]=1;
      else ((MyBook*)bk)->nast->sound->status[i]=0;
    }
  }  
  GUI_Free(((MyBook*)bk)->gui);
  ((MyBook*)bk)->gui=0;
  CreateMNM(((MyBook*)bk), item);
}


void DestroyInput()
{
  GUI_Free(MyBK()->gui1);
  MyBK()->gui1=0;
  ShowWindow(MyBK()->gui);
}


void OnEntInput(BOOK * bk,wchar_t * string,int len)
{
  int x=ListMenu_GetSelectedItem(((MyBook*)bk)->gui)-1;
  wstrcpy(((MyBook*)bk)->nast->sound->names[x], string);
  GUI_Free(((MyBook*)bk)->gui1);
  ((MyBook*)bk)->gui1=0;
  ShowWindow(((MyBook*)bk)->gui);
}




void CreateInput(int x, SOUND*s, MyBook*bk)
{
  if(bk->gui1) GUI_Free(bk->gui1);
  bk->gui1=0;
  bk->gui1=CreateStringInput(0,
                                       VAR_PREV_ACTION_PROC(DestroyInput),
                                       VAR_LONG_BACK_PROC(DestroyInput),
                                       VAR_BOOK((BOOK*)bk),
                                       VAR_STRINP_FIXED_TEXT(Str2ID(L"Изменить:",0, SID_ANY_LEN)),
                                       VAR_STRINP_TEXT(Str2ID(s->names[x-1], 0, SID_ANY_LEN)),
                                       VAR_STRINP_MAX_LEN(255),
                                       VAR_STRINP_MIN_LEN(0),
                                       VAR_OK_PROC(OnEntInput),
                                       0); 
}



void OnEntMNM( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  if(item==0)
  {
    if(((MyBook*)bk)->nast->sound->sound==0) ((MyBook*)bk)->nast->sound->sound=1;
    else ((MyBook*)bk)->nast->sound->sound=0;
    GUI_Free(((MyBook*)bk)->gui);
    ((MyBook*)bk)->gui=0;
    CreateMNM(((MyBook*)bk), item);
  }
  if(item>0)
  {
    CreateInput(item, ((MyBook*)bk)->nast->sound, (MyBook*)bk);
  }
}

int SetTitlePunktsMNM(GUI_MESSAGE * msg)
{
  int item, ID;
  switch( GUIonMessage_GetMsg(msg) )
  {
    case 1:
      item=GUIonMessage_GetCreatedItemIndex(msg);
      if(item==0)
      {
        iconidname2id(L"DB_LIST_SOUND_IMELODY_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
        SetMenuItemText0(msg,Str2ID(L"Звук",0,SID_ANY_LEN));
        SetMenuItemText1(msg,GetStrokaIMG(StatusSettings(((MyBook*)isBookX(NameMyElf, 0))->nast->sound->sound),((MyBook*)isBookX(NameMyElf, 0))->nast->sound->sound));
      }
      for(int i=0; i<6; i++)
      {
        if(item==i+1)
        {
          SetMenuItemText0(msg,Str2ID(NamesOfPunkts(i),0,SID_ANY_LEN));
          SetMenuItemText1(msg,GetStrokaIMG(((MyBook*)isBookX(NameMyElf, 0))->nast->sound->names[i],((MyBook*)isBookX(NameMyElf, 0))->nast->sound->status[i]));
          iconidname2id(L"DB_LIST_SOUND_EMELODY_ICN", -1, &ID);
          SetListObjectItemIcon(msg, 0, ID);
        }
      }      
  }
  return(1);        
}

void Set(int item)
{
  if(item==0)
  {
    GUIObject_Softkey_SetAction(((MyBook*)isBookX(NameMyElf, 0))->gui,1,0);
    GUIObject_Softkey_SetText(((MyBook*)isBookX(NameMyElf, 0))->gui, 1, Str2ID(L" ",0,SID_ANY_LEN));
    GUIObject_Softkey_SetText(((MyBook*)isBookX(NameMyElf, 0))->gui, ACTION_SELECT1, Str2ID(StatusSettings1(((MyBook*)isBookX(NameMyElf, 0))->nast->sound->sound),0,SID_ANY_LEN));
    GUIObject_Softkey_SetAction(((MyBook*)isBookX(NameMyElf, 0))->gui, ACTION_SELECT1, OnEntMNM);
    //SetListObjectItemIcon(msg, 0, IconID->I1);
  }
  for(int i=0; i<6; i++)
  {
    if(item==i+1)
    {
      GUIObject_Softkey_SetAction(((MyBook*)isBookX(NameMyElf, 0))->gui,1,OnMNM1);
      GUIObject_Softkey_SetText(((MyBook*)isBookX(NameMyElf, 0))->gui, 1, Str2ID(StatusSettings1(((MyBook*)isBookX(NameMyElf, 0))->nast->sound->status[i]),0,SID_ANY_LEN));
      GUIObject_Softkey_SetText(((MyBook*)isBookX(NameMyElf, 0))->gui, ACTION_SELECT1, Str2ID(L"Изменить",0,SID_ANY_LEN));
      GUIObject_Softkey_SetAction(((MyBook*)isBookX(NameMyElf, 0))->gui, ACTION_SELECT1, OnEntMNM);
      //SetListObjectItemIcon(msg, 0, IconID->I2);
    }
  }
}

void NewKey1(DISP_OBJ* p, int i1, int i2, int i3, int i4)
{
  ((MyBook*)isBookX(NameMyElf, 0))->OldKey1(p, i1, i2, i3, i4);
  int item=ListMenu_GetSelectedItem(MyBK()->gui);
  Set(item);
}

void CreateMNM(MyBook*bk, int x)
{
  if(bk->gui) GUI_Free(bk->gui);
  bk->gui=0;
  GUI_LIST*MNM=CreateListObject((BOOK*)bk,0);
  ((MyBook*)bk)->gui=MNM;
  GuiObject_SetTitleText(MNM, Str2ID(L"Музыка",0,SID_ANY_LEN));
  SetNumOfMenuItem(MNM, 7);
  OneOfMany_SetonMessage(MNM,SetTitlePunktsMNM);
  SetCursorToItem(MNM,x);
  SetMenuItemStyle(MNM,3);
  GUIObject_Softkey_SetAction(MNM,ACTION_BACK,ExitMNM);
  GUIObject_Softkey_SetAction(MNM,ACTION_LONG_BACK,ExitMNM);
  Set(x);
  //GUIObject_Softkey_SetAction(MNM,ACTION_SELECT1,OnEntMNM);
  bk->OldKey1 = DISP_OBJ_GetOnKey( GUIObj_GetDISPObj(MNM) );
  DISP_DESC_SetOnKey( DISP_OBJ_GetDESC ( GUIObj_GetDISPObj(MNM) ), NewKey1);
  ShowWindow(MNM);
}

void ExitNM( BOOK* bk, GUI* )
{
  GUI_Free(((MyBook*)bk)->gui);
  ((MyBook*)bk)->gui=0;
  if(!((MyBook*)bk)->pause) CreateGM(0, bk);
  //CreatePM(bk);
}

void OnEntGameM( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  ((MyBook*)bk)->speed=item+1;
  ((MyBook*)bk)->point=0;
  ((MyBook*)bk)->lvl=0;
  BookObj_GotoPage(bk,&bk_game);
  GUI_Free(((MyBook*)bk)->gui);
  ((MyBook*)bk)->gui=0;  
}

void OnEntMR( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  if(item==0)
  {
    ShowResults(((MyBook*)bk)->nast->rez->te);
  }
  if(item==1)
  {
    ShowResults(((MyBook*)bk)->nast->rez->tn);  
  }
  if(item==2)
  {
    ShowResults(((MyBook*)bk)->nast->rez->th);  
  }
}

void OnEntNM( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  if(item==0)
  {
    CreateMNM((MyBook*)bk, 0);
  }
  if(item==1)
  {
    CreateINM((MyBook*)bk, 0);   
  }
}

int SetTitlePunktsNM(GUI_MESSAGE * msg)
{
  int item, ID;
  switch( GUIonMessage_GetMsg(msg) )
  {
    case 1:
      item=GUIonMessage_GetCreatedItemIndex(msg);
      if(item==0)
      {
        SetMenuItemText0(msg,Str2ID(L"Музыка",0,SID_ANY_LEN));
        iconidname2id(L"ENTERTAINMENT_MENU_MIDI_COMP_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      if(item==1)
      {
        SetMenuItemText0(msg,Str2ID(L"Картинки",0,SID_ANY_LEN));
        iconidname2id(L"ENTERTAINMENT_MENU_PICTUREEDITOR_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
  }
  return(1);        
}

void DeleteRes( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  if(item==0)
  {
    DELTABL(((MyBook*)bk)->nast->rez->te);
  }
  if(item==1)
  {
    DELTABL(((MyBook*)bk)->nast->rez->tn);  
  }
  if(item==2)
  {
    DELTABL(((MyBook*)bk)->nast->rez->th);  
  }
}

int SetTitlePunktsMR(GUI_MESSAGE * msg)
{
  int item;
  int ID;
  switch( GUIonMessage_GetMsg(msg) )
  {
    case 1:
      item=GUIonMessage_GetCreatedItemIndex(msg);
      if(item==0)
      {
        SetMenuItemText0(msg,Str2ID(L"Легко",0,SID_ANY_LEN));
        iconidname2id(L"WV_BUDDYLIST_CONTACT_AVAILABLE_ASHAMED_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      if(item==1)
      {
        SetMenuItemText0(msg,Str2ID(L"Средне",0,SID_ANY_LEN));
        iconidname2id(L"WV_BUDDYLIST_CONTACT_AVAILABLE_ANXIOUS_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      if(item==2)
      {
        SetMenuItemText0(msg,Str2ID(L"Тяжело",0,SID_ANY_LEN));
        iconidname2id(L"WV_BUDDYLIST_CONTACT_AVAILABLE_ANGRY_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
  }
  return(1);        
}

void MenuResultat(MyBook*bk)
{
  if(bk->gui) GUI_Free(bk->gui);
  bk->gui=0;
  GUI_LIST*MR=CreateListObject((BOOK*)bk,0);
  ((MyBook*)bk)->gui=MR;
  GuiObject_SetTitleText(MR, Str2ID(L"Результаты",0,SID_ANY_LEN));
  SetNumOfMenuItem(MR, 3);
  OneOfMany_SetonMessage(MR,SetTitlePunktsMR);
  SetCursorToItem(MR,0);
  SetMenuItemStyle(MR,0);
  GUIObject_Softkey_SetAction(MR,ACTION_BACK,ExitNM);
  GUIObject_Softkey_SetAction(MR,ACTION_LONG_BACK,ExitNM);
  GUIObject_Softkey_SetAction(MR,ACTION_SELECT1,OnEntMR);
  GUIObject_Softkey_SetAction(MR,1,DeleteRes);
  GUIObject_Softkey_SetText(MR,1,STR("Сброс"));
  GUIObject_Softkey_SetText(MR,ACTION_SELECT1,STR("Просм."));
  ShowWindow(MR);
}

void CreateNM(MyBook*bk)
{
  if(bk->gui) GUI_Free(bk->gui);
  bk->gui=0;
  GUI_LIST*NM=CreateListObject((BOOK*)bk,0);
  ((MyBook*)bk)->gui=NM;
  GuiObject_SetTitleText(NM, Str2ID(L"Настройки",0,SID_ANY_LEN));
  SetNumOfMenuItem(NM, 2);
  OneOfMany_SetonMessage(NM,SetTitlePunktsNM);
  SetCursorToItem(NM,0);
  SetMenuItemStyle(NM,0);
  GUIObject_Softkey_SetAction(NM,ACTION_BACK,ExitNM);
  GUIObject_Softkey_SetAction(NM,ACTION_LONG_BACK,ExitNM);
  GUIObject_Softkey_SetAction(NM,ACTION_SELECT1,OnEntNM);
  ShowWindow(NM);
}
  

void YesPress( BOOK* bk, GUI* )
{
  FreeBook(bk);
}

void NoPress( BOOK* bk, GUI* )
{
  GUI_Free(((MyBook*)bk)->gui1);
  ((MyBook*)bk)->gui1=0;
  ShowWindow(((MyBook*)bk)->gui);
}

void YesNo(MyBook*bk)
{
  bk->gui1=CreateYesNoQuestionVA(0,
                                    VAR_BOOK(bk),
                                    VAR_YESNO_QUESTION(Str2ID(L"Выйти?", 0, SID_ANY_LEN)),
                                    0);
  GUIObject_Softkey_SetAction(bk->gui1, ACTION_YES, YesPress);
  GUIObject_Softkey_SetAction(bk->gui1, ACTION_NO,  NoPress);
  GUIObject_Softkey_SetAction(bk->gui1, ACTION_BACK, NoPress);
  GUIObject_Softkey_SetAction(bk->gui1, ACTION_LONG_BACK, NoPress);
  ShowWindow(((MyBook*)bk)->gui1);
}

void ExitGM( BOOK* bk, GUI* )
{
  YesNo((MyBook*)bk);
}



void CreateGameM(MyBook*bk)
{
  if(bk->gui) GUI_Free(bk->gui);
  bk->gui=0;
  GUI_LIST*GameM=CreateListObject((BOOK*)bk,0);
  ((MyBook*)bk)->gui=GameM;
  GuiObject_SetTitleText(GameM, Str2ID(L"Выбор уровня",0,SID_ANY_LEN));
  SetNumOfMenuItem(GameM, 3);
  OneOfMany_SetonMessage(GameM,SetTitlePunktsMR);
  SetCursorToItem(GameM,0);
  SetMenuItemStyle(GameM,0);
  GUIObject_Softkey_SetAction(GameM,ACTION_BACK,ExitNM);
  GUIObject_Softkey_SetAction(GameM,ACTION_LONG_BACK,ExitNM);
  GUIObject_Softkey_SetAction(GameM,ACTION_SELECT1,OnEntGameM);
  ShowWindow(GameM);
}

void OnEntGM( BOOK* bk, GUI* )
{
  int item=ListMenu_GetSelectedItem(((MyBook*)bk)->gui);
  if(item==0)
  {
    CreateGameM((MyBook*)bk);
  }
  if(item==1)
  {
    CreateNM((MyBook*)bk);    
  }
  if(item==2)
  {
    MenuResultat((MyBook*)bk);
  }
  if(item==3)
  {
    ShowInfo();
  }
  if(item==4)
  {
    ExitGM(bk,0);
  }
}

int SetTitlePunktsGM(GUI_MESSAGE * msg)
{
  int item;
  int ID;
  switch( GUIonMessage_GetMsg(msg) )
  {
    case 1:
      item=GUIonMessage_GetCreatedItemIndex(msg);
      if(item==0)
      {
        ID=Str2ID(L"Новая игра",0,SID_ANY_LEN);
        SetMenuItemText0(msg,ID);
        ID=Str2ID(L"Начать игру!",0,SID_ANY_LEN);
        SetMenuItemText1(msg,ID);
        iconidname2id(L"PROFILE_INCAR_LIST_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      if(item==1)
      {
        ID=Str2ID(L"Настройки",0,SID_ANY_LEN);
        SetMenuItemText0(msg,ID);
        ID=Str2ID(L"Поковыряца",0,SID_ANY_LEN);
        SetMenuItemText1(msg,ID);
        iconidname2id(L"MENU_SETTINGS_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      if(item==2)
      {
        ID=Str2ID(L"Результаты",0,SID_ANY_LEN);
        SetMenuItemText0(msg,ID);
        ID=Str2ID(L"Просмотр результатов",0,SID_ANY_LEN);
        SetMenuItemText1(msg,ID);
        iconidname2id(L"MISCELLANEOUS_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      if(item==3)
      {
        ID=Str2ID(L"Инфо",0,SID_ANY_LEN);
        SetMenuItemText0(msg,ID);
        ID=Str2ID(L"Информация",0,SID_ANY_LEN);
        SetMenuItemText1(msg,ID);
        iconidname2id(L"HELP_TEXT_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
      if(item==4)
      {
        ID=Str2ID(L"Выход",0,SID_ANY_LEN);
        SetMenuItemText0(msg,ID);
        ID=Str2ID(L"Выйти из игры",0,SID_ANY_LEN);
        SetMenuItemText1(msg,ID);
        iconidname2id(L"FORBIDDEN_NETWORK_ICN", -1, &ID);
        SetListObjectItemIcon(msg, 0, ID);
      }
  }
  return(1);        
}

int CreateGM(void*, BOOK*bk)
{
  int ID;
  GUI_LIST*GM=CreateListObject((BOOK*)bk,0);
  if(((MyBook*)bk)->gui) GUI_Free(((MyBook*)bk)->gui);
  ((MyBook*)bk)->gui=0;
  ((MyBook*)bk)->gui=GM;
  ID=Str2ID(L"Gono4ki",0,SID_ANY_LEN);
  GuiObject_SetTitleText(GM, ID);
  SetNumOfMenuItem(GM, 5);
  OneOfMany_SetonMessage(GM,SetTitlePunktsGM);
  SetCursorToItem(GM,0);
  SetMenuItemStyle(GM,3);
  GUIObject_Softkey_SetAction(GM,ACTION_BACK,ExitGM);
  GUIObject_Softkey_SetAction(GM,ACTION_LONG_BACK,ExitGM);
  GUIObject_Softkey_SetAction(GM,ACTION_SELECT1,OnEntGM);
  ShowWindow(GM);
  return 0;
}
