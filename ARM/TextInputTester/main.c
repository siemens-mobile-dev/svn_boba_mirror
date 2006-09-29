#include "..\inc\swilib.h"

int icon[]={0x58,0};
const int minus11=-11;

SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Лев"},
  {0x0001,0x0000,(int)"Прав"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

WSHDR *ews;

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

void ed1_locret(void){}

int ed1_onkey(GUI *data, GUI_MSG *msg)
{
  //-1 - do redraw
  return(0); //Do standart keys
  //1: close
}

void ed1_ghook(GUI *data, int cmd)
{
  static SOFTKEY_DESC sk={0x0018,0x0000,(int)"Menu"};
  EDITCONTROL ec;
  if (cmd==2)
  {
    //Create
  }
  if (cmd==7)
  {
    //OnRun
//    SetSoftKey(data,&sk,1);
    ExtractEditControl(data,3,&ec);
    wstrcpy(ews,ec.pWS);
  }
}

HEADER_DESC ed1_hdr={0,0,131,21,NULL,(int)"Редактор",0x7FFFFFFF};

INPUTDIA_DESC ed1_desc=
{
  1,
  ed1_onkey,
  ed1_ghook,
  (void *)ed1_locret,
  0,
  &menu_skt,
  {0,22,131,153},
  4,
  100,
  101,
  0,

//  0x00000001 - Выровнять по правому краю
//  0x00000002 - Выровнять по центру
//  0x00000004 - Инверсия знакомест
//  0x00000008 - UnderLine
//  0x00000020 - Не переносить слова
//  0x00000200 - bold
  0,

//  0x00000002 - ReadOnly
//  0x00000004 - Не двигается курсор
//  0x40000000 - Поменять местами софт-кнопки
  0
};

int create_ed(void)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;

  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());

  wsprintf(ews,"%08X ",ed1_desc._0x40000000);
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"%t","Обычная строка:");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"%t","Превед!");
  ConstructEditControl(&ec,3,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"%t","Строка с номером телефона:");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"123456789*#");
  ConstructEditControl(&ec,2,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"%t","Число (фикс. кол. цифр):");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"3");
  ConstructEditControl(&ec,5,0x40,ews,5);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"%t","Число (произв. кол. цифр):");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"123");
  ConstructEditControl(&ec,6,0x40,ews,5);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"%t","ComboBox:");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(ews,"item1");
  ConstructEditControl(&ec,7,0x40,ews,5);
  AddEditControlToEditQend(eq,&ec,ma);

  return CreateInputTextDialog(&ed1_desc,&ed1_hdr,eq,1,0);
}


void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  ews=AllocWS(256);
  csm->gui_id=create_ed();
}

void Killer(void)
{
  extern void *ELF_BEGIN;
  extern void kill_data(void *p, void (*func_p)(void *));
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

void maincsm_onclose(CSM_RAM *csm)
{
  FreeWS(ews);
  SUBPROC((void *)Killer);
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      /*if (ed1_desc._0x40000000)
      {
	ed1_desc._0x40000000<<=1;
	csm->gui_id=create_ed();
      }
      else*/
      {
	csm->csm.state=-3;
      }
    }
  }
  return(1);
}

unsigned short maincsm_name_body[140];

const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM =
{
  {
  maincsm_onmessage,
  maincsm_oncreate,
  0,
  0,
  0,
  0,
  maincsm_onclose,
  sizeof(MAIN_CSM),
  1,
  &minus11
  },
  {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};

void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"TextInputTester");
}

int main()
{
  char dummy[sizeof(MAIN_CSM)];
  LockSched();
  UpdateCSMname();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  return 0;
}
