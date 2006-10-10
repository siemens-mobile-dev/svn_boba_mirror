#include "..\inc\swilib.h"

const int minus11=-11; // стремная константа
char *minigps_path = "0:\\Misc\\gps";

unsigned short maincsm_name_body[140];
int filePassed=0; // передано ли имя файла
int EditorCalled = 0; //редактор вызван
int Terminate = 0; // флаг необходимости завершения работы

unsigned int MAINCSM_ID = 0;
unsigned int MAINGUI_ID = 0;

WSHDR *ws_eddata;     // хранилище списка файлов
char filename[128];     // глобальная переменная для имени файла


RAMNET GetNetParams()
{
  RAMNET Result;
  zeromem(&Result, sizeof(RAMNET));
  Result = *RamNet();
  return Result;
}


typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  WSHDR *ws1;
  WSHDR *ws2;
  int i1;
}MAIN_GUI;


void OnRedraw(MAIN_GUI *data) // OnRedraw
{
}

void onCreate(MAIN_GUI *data, void *(*malloc_adr)(int)) //Create
{
  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  data->gui.state=1;
}

void onClose(MAIN_GUI *data, void (*mfree_adr)(void *)) //Close
{
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void onFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))//Focus
{
  data->gui.state=2;
  if(!EditorCalled) // Если не запускался редактор...
  {
    EditorCalled = 1;
    void LaunchEditor(void); 
    LaunchEditor(); // запускаем редактор :)
  }
  else
  {
    GeneralFuncF1(1); // рубим верхний гуй. И приложение закрывается.
  }
}

void onUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *)) //Unfocus
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}

int OnKey(MAIN_GUI *data, GUI_MSG *msg) //OnKey
{
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    case RIGHT_SOFT:
      return(1); //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI
    }
  }
  //  method0(data);
  return(0);
}

void onDestroy(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  mfree_adr(data);
}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)OnRedraw,	//Redraw
  (void *)onCreate,	//Create
  (void *)onClose,	//Close
  (void *)onFocus,	//Focus
  (void *)onUnfocus,	//Unfocus
  (void *)OnKey,	//OnKey
  0,
  (void *)onDestroy,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,131,175};

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
  MAINGUI_ID=csm->gui_id;
}

void maincsm_onclose(CSM_RAM *csm)
{
  extern void *ELF_BEGIN;
  FreeWS(ws_eddata);
  ws_eddata=NULL;
  ((void (*)(void *))(mfree_adr()))(&ELF_BEGIN);
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if ((msg->msg==MSG_GUI_DESTROYED)&&((int)msg->data0==csm->gui_id))
  {
    csm->csm.state=-3;
  }
  return(1);
}

/*
const CSM_DESC maincsm=
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
};
*/

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
  WSHDR *ws=AllocWS(256);
  char *sh_fname = strrchr(filename,'\\');
  str_2ws(ws,sh_fname,126);
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"TMOEdit: %w",ws);
  FreeWS(ws);
}

int main(char *exename, char *fname)
{
  char dummy[sizeof(MAIN_CSM)];
  if (fname)
  {
    if (strlen(fname)<128)
    {
      strcpy(filename,fname);     // сохраняем в глоб.перем.переданное имя
      filePassed = 1;
    }
  }
  ws_eddata=AllocWS(256);
  MAINCSM_ID = CreateCSM(&MAINCSM.maincsm,dummy,0);
  return 0;
}

//---------------------------------------------------------------------------
// Сохранение текста в файл
//---------------------------------------------------------------------------

void GetCurFileName()
{
  RAMNET Res;
  Res = GetNetParams(); 
  sprintf(filename,"%s\\%u\\%u-%u.tmo",minigps_path,Res.lac,Res.ci,Res.lac);  
}

int SaveText(void)
{
  unsigned int errcode;
  volatile int hFile = -1;
  volatile int flags =A_WriteOnly+A_Truncate+A_BIN;
  RAMNET Res;
  Res = GetNetParams(); 
  
  if(!filePassed)
  { 
    GetCurFileName();
    flags+=A_Create;
  }

  // Сохранение
    unsigned int len = wstrlen(ws_eddata);
    hFile = fopen(filename,flags,P_READ+P_WRITE,&errcode);
    if(hFile==-1)
    {
      // Скорее всего потому, что нет директории
      char dirname[128];
      sprintf(dirname,"%s\\%u",minigps_path,Res.lac);
      int result = mkdir(dirname, &errcode);
      if(result==-1)
      {
        ShowMSG(2,(int)"Запись файла невозможна :(");
        return 0;
      }
      hFile = fopen(filename,flags,P_READ+P_WRITE,&errcode);
    }
    lseek(hFile,0,0,&errcode,&errcode);
    fwrite(hFile,ws_eddata->wsbody,len*2+2,&errcode);
    //char q[20];
    //sprintf(q,"Written %d +2 b",len*2+2);
    //ShowMSG(1,(int)q);
    // CRC
    unsigned int crc = (int)ws_eddata->wsbody[0];
    void *temp;
    char *arr;
    temp = ws_eddata->wsbody;
    arr = temp;
    for(int i=2;i<(len+1)*2;i++)
    {
      crc = crc ^ arr[i];
    }
    fwrite(hFile,&crc,2,&errcode);
    fclose(hFile,&errcode);
    //ShowMSG(1,(int)"Сохранено"); // ибо потом выход НАХ
    return 1;
}

void menup2(void)  // Покинуть редактор (вызов из меню)
{
  Terminate =1 ;
  GeneralFuncF1(1);
}

void AboutDlg(void)
{
  char *str = "TMO-редактор v2.8 RC2(0xC604)\r\n(c) 2006 Kibab\r\n(r) Rst7/CBSIE";
  ShowMSG(2,(int)str);
/*
  unsigned short* RamCH  = RamNet();   // 648
  unsigned short* RamCID = RamCH + 1;  // 33798
  unsigned short* RamLAC = RamNet() + 2; // 7737
  sprintf(str,"%u-%u:%u",*RamCID, *RamLAC, *RamCH);
  ShowMSG(2,(int)str);
*/  
}

int icon[]={0x3DB,0};
int about_icon[]={0x4DB,0};
int exit_icon[] = {0x315,0};
int save_icon[] = {0x50E,0};

HEADER_DESC menuhdr={0,0,131,21,icon,(int)"Меню редактора",0x7FFFFFFF};
int menusoftkeys[]={0,1,2};
MENUITEM_DESC menuitems[3]=
{
  {save_icon,(int)"Сохранить",0x7FFFFFFF,0,NULL/*menusoftkeys*/,0,0x59D},
  {exit_icon,(int)"Выход",0x7FFFFFFF,0,NULL/*menusoftkeys*/,0,0x59D},
  {about_icon,(int)"Об эльфе...",0x7FFFFFFF,0,NULL/*menusoftkeys*/,0,0x59D},
};

void *menuprocs[3]={(void *)SaveText,(void *)menup2, (void *) AboutDlg};

SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Выбор"},
  {0x0001,0x0000,(int)"Назад"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};

MENU_DESC tmenu=
{
  8,NULL,NULL,NULL,
  menusoftkeys,
  &menu_skt,
  1,
  NULL,
  menuitems,
  menuprocs,
  3
};

//---------------------------------------------------------------------------
// Test edit dialog
//---------------------------------------------------------------------------
int inp_onkey(GUI *gui, GUI_MSG *msg)
{

//  В этом куске, вообще говоря, нет смысла, ибо при запущенном XTask
// красная кнопка действует как "Назад"
  if(msg->gbsmsg->submess==RED_BUTTON)
  {
    return(1); // выходим без сохранения.
  }

  if(msg->gbsmsg->submess==GREEN_BUTTON)
  {
    if(SaveText()) // сохраняемся
    {
      return(1); // Выходим
    }
  }

  if (msg->keys==0x18)
  {
    CreateMenu(0,0,&tmenu,&menuhdr,0,3,0,0);
//    EDIT_CURSOR_POS(data)=3;
    return(-1); //do redraw
  }
  return(0); //Do standart keys
  //1: close
}


int curpos;

void inp_redraw(void *data)
{
//ws_eddata->wsbody[curpos]='!';
// DrawString(ws_eddata,0,22,131,153,11,0,GetPaletteAdrByColorIndex(1),GetPaletteAdrByColorIndex(0));
}

void inp_ghook(GUI *gui, int cmd)
{
  static SOFTKEY_DESC sk={0x0018, 0x0000,(int)"Меню"};
  EDITCONTROL ec;
  if (cmd==2)
  {
    //Called after onCreate
  }
  if (cmd==7)
  {
    curpos=EDIT_CURSOR_POS(gui);
    SetSoftKey(gui,&sk,0);    
    ExtractEditControl(gui,1,&ec);    
    wstrcpy(ws_eddata,ec.pWS);
  }
 
  if(Terminate) 
 {
   GeneralFuncF1(1); // убиваем текущий гуй.
 }
}

void inp_locret(void){}

INPUTDIA_DESC inp_desc=
{
  1,
  inp_onkey,
  inp_ghook,
  (void *)inp_locret,
  0,
  &menu_skt,
  {0,22,131,153},
  4,
  100,
  101,
  0,
  0,
  0x40000000
};

HEADER_DESC inp_hdr={0,0,131,21,/*icon*/NULL,(int)"TMOEdit",0x7FFFFFFF};

void LaunchEditor(void)
{
  void *ma=malloc_adr();
  void *eq;

  int str_len = 0;
  unsigned int errcode;
  volatile int hFile = -1;
  
  if(!filePassed)
  {
    // Пробуем открыть файл, может быть он такой есть
    GetCurFileName();
    hFile = fopen(filename,A_ReadWrite+A_BIN,P_READ+P_WRITE,&errcode);
    if(hFile!=-1)
    {
      // Файл открыт, значит он есть, нефиг новый делать ;-)
      filePassed = 1;
      fclose(hFile, &errcode);
    }
  }
  
  if(filePassed)
  {
    hFile = fopen(filename,A_ReadWrite+A_BIN,P_READ+P_WRITE,&errcode);
    fread(hFile,&str_len,2,&errcode);
    char *sh_fname = strrchr(filename,'\\');
    inp_hdr.lgp_id = (int)sh_fname;
    WSHDR *tmostr=AllocWS(str_len);
    errcode=0;
    lseek(hFile,0,0,&errcode,&errcode); // позиционирование на начало файла
    fread(hFile,tmostr->wsbody,(str_len+1)*2,&errcode);

    if(errcode)
    {
      wsprintf(ws_eddata,"%t %u","Ошибка I/O", errcode);          
      filePassed = 0;
    }
    else
    {
      CutWSTR(tmostr,str_len);
      wsprintf(ws_eddata,"%w",tmostr);
    }
    FreeWS(tmostr);
    fclose(hFile, &errcode);
  }
  else
  {
    wsprintf(ws_eddata,"%t","0 ");
    sprintf(filename,"new");
  } 
  UpdateCSMname();
  EDITCONTROL ec;
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  ConstructEditControl(&ec,3,0x40,ws_eddata,256);
  AddEditControlToEditQend(eq,&ec,ma);
  CreateInputTextDialog(&inp_desc,&inp_hdr,eq,1,/*ws_eddata*/0);
}
