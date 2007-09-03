#include "..\..\inc\swilib.h"
//#include "..\..\inc\xtask_ipc.h"
#include "mainmenu.h"
#include "..\multisim_ipc.h"
//#include "..\inc\swilib.h"
#include "conf_loader.h"

extern void kill_data(void *p, void (*func_p)(void *));
extern const unsigned int onc_ena;
extern const unsigned int onc_sim;
extern const unsigned int onc_sim2;\
  int curblock=0;;
int maincsm;
MSIM_ELF_RESP simblk;
//unsigned char bp[1024];
     IPC_REQ gipc;

const int minus11=-11;

//__root int xxx=123;
//volatile static int yyy;
IPC_REQ gipc2;
void SendRequest(int submess, void *data){
      gipc2.name_to=IPC_MSIM_NAME;
      gipc2.name_from=ELF_MSIM_NAME;
      gipc2.data=data;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,submess,&gipc2); 

  
}

 void ChangeSim( int num)
{
  SendRequest(IPC_MSIM_SWITCH,(void*)num);
//  RefreshGUI();
  
}


typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;


inline void ReadConfig(){
  InitConfig();
}  


void maincsm_oncreate(CSM_RAM *data)
{
//  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
//  zeromem(main_gui,sizeof(MAIN_GUI));
  /*
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
 */
  csm->csm.state=0;
  csm->csm.unk1=0;
///  csm->gui_id=CreateGUI(main_gui);
//  csm->gui_id=    ShowMainMenu();  
//  maincsm=
}

void ELF_KILLER(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

void maincsm_onclose(CSM_RAM *csm)
{
  SUBPROC((void *)ELF_KILLER);
}

extern int simnum;
extern int simcnt;
extern int WriteFile(char*,int);
extern const char f5400s[];
extern const char f5401s[];

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    extern const char *successed_config_filename;
    if (strcmp(successed_config_filename,(char *)msg->data0)==0)
//    if (strcmp_nocase(successed_config_filename,(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"MultisimElf config updated!");
      ReadConfig();
    }
    
  }

  if (msg->msg==MSG_IPC)
  {
        //  ShowMSG(1,(int)"test-ipc");
    IPC_REQ *ipc;
    if ((ipc=(IPC_REQ*)msg->data0))
    {
      if (strcmp(ipc->name_from,IPC_MSIM_NAME)==0)
      {

	switch (msg->submess)
	{
	case IPC_MSIM_PROCESSED: 
            simnum=simblk.CurSim;
            simcnt=simblk.SimCnt;
          if (!onc_ena){
            csm->gui_id=    ShowMainMenu();            
            RefreshGUI();
          }else{
            if (simnum==onc_sim)ChangeSim(onc_sim2);else
              ChangeSim(onc_sim);
          }
            
        break;    
	case IPC_MSIM_PROCESSED_READBLOCK2FILE:
          if (curblock==0)
            WriteFile((char*)f5400s,0x30*20);
          else
            WriteFile((char*)f5401s,0x50*20);            
            GeneralFuncF1(1);                      
        break;  
        case IPC_MSIM_PROCESSED_WRITEBLOCK4FILE:
            GeneralFuncF1(1);                                
         // }else{

        //  }
	  break;
        
          
	case IPC_MSIM_SWITCHED:
          if (onc_ena){
              CloseCSM(maincsm);
          }else
            GeneralFuncF1(1);
	  break;
	}
        
      }
    }
  }
  if ((msg->msg==MSG_GUI_DESTROYED)&&((int)msg->data0==csm->gui_id))
  {
    csm->csm.state=-3;
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
#ifdef NEWSGOLD
0,
0,
0,
0,
#endif
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),ELF_MSIM_NAME);
}

//unsigned char Block5400[1024];
     

  
//MSIM_IPC_MSG_EEPROM msg;
CSM_DESC icsmd;

int main()
{
  ReadConfig();
  
  /*
              CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
      memcpy(&icsmd,icsm->constr,sizeof(icsmd));

      onmsg=icsmd.onMessage;

      icsmd.onMessage=MyIDLECSM_onMessage;
      icsm->constr=&icsmd;
*/
  
      gipc.name_to=IPC_MSIM_NAME;
      gipc.name_from=ELF_MSIM_NAME;
/*
      gipc.data=&msg;
     msg.type=0;
      msg.block=5400;
      msg.buf=bp;
      msg.offset=0;
      msg.size=1024;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_MSIM_EEPROM_BACKDOOR,&gipc);
      
      */
      gipc.data=&simblk;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_MSIM_GET_RAM5400,&gipc);      
//       GBS_SendMessage(MMI_CEPID, MSG_IPC,IPC_MSIM_PROCESSED,&gipc);
/*
      for (int x=0x0000;x<0xFFFF;x++       )
    if (GetPicNByUnicodeSymbol(x)==0x53d){
      char s[256];
      sprintf(s,"%x\n%d ",x,GetPicNByUnicodeSymbol(CBOX_CHECKED));
        ShowMSG(1,(int)s);
    };
      
  */

// 	void TestMenu(void);
	//TestMenu();


  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  LockSched();
  maincsm=CreateCSM(&MAINCSM.maincsm,dummy,0);
  
  UnlockSched();
 
  return 0;
        
}

//---------------------------------------------------------------------------
// Test menu
//---------------------------------------------------------------------------

