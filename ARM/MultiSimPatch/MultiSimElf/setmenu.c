#pragma once
#include "..\..\inc\swilib.h"
#include "..\..\inc\cfg_items.h"
//#include "pnglist.h"
#include "language.h"
#include "mainmenu.h"

//==============================================================================
// ELKA Compatibility
#pragma inline
static void patch_header(const HEADER_DESC* head)
{
  ((HEADER_DESC*)head)->rc.x=0;
  ((HEADER_DESC*)head)->rc.y=YDISP;
  ((HEADER_DESC*)head)->rc.x2=ScreenW()-1;
  ((HEADER_DESC*)head)->rc.y2=HeaderH()-1+YDISP;
}
/*
#pragma inline
static void patch_input(const INPUTDIA_DESC* inp)
{
  ((INPUTDIA_DESC*)inp)->rc.x=0;
  ((INPUTDIA_DESC*)inp)->rc.y=HeaderH()+1+YDISP;
  ((INPUTDIA_DESC*)inp)->rc.x2=ScreenW()-1;
  ((INPUTDIA_DESC*)inp)->rc.y2=ScreenH()-SoftkeyH()-1;
}
*/
//==============================================================================
int SetMenu_ID;

extern  int curblock;
#define __SVN_REVISION__  1007
int sS_ICONS[3];
static const SOFTKEY_DESC smenu_sk[] =
{
  {0x0018, 0x0000, (int)LG_SELECT},
  {0x0001, 0x0000, (int)LG_CLOSE},
  {0x003D, 0x0000, (int)LGP_DOIT_PIC}
};
const SOFTKEYSTAB smenu_skt =
{
  smenu_sk, 0
};




/*
static void EditConfig(void)
{
  extern const char *successed_config_filename;
  WSHDR *ws;
  ws=AllocWS(150);
  str_2ws(ws,successed_config_filename,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
  GeneralFuncF1(1);
}
*/
extern IPC_REQ gipc;
MSIM_IPC_MSG_EEPROM msg;
unsigned char bp[1600];
extern const char f5400s[];
extern const char f5401s[];

int WriteFile(char *name,int size)
{

  int f;
  unsigned char *filebuf;
  char *buf=NULL,*bufs;
  filebuf=bp;
  const int c=3*size+size/16;
  buf=malloc(c);
  if (!buf)return 0;
  memset(buf,0,c);
  bufs=buf;
  
  unsigned int ul;  

//  char Block5400[1024];
//  if ((f=fopen("0:\\Misc\\5400s.txt\0",0x8301,0x100,&ul))!=-1){
  if ((f=fopen(name,0x8301,0x100,&ul))!=-1){
    for (int i=0;i<(size/16);i++){
      for (int j=0;j<15;j++){
        sprintf(buf,"%02X ",*filebuf++);
        buf+=strlen(buf);
      }
      sprintf(buf,"%02X\r\n",*filebuf++);
       buf+=strlen(buf);
    }
    fwrite(f,bufs,c,&ul);
        fclose(f,&ul);    
  }
  
//  SetEEFULLBlock(5400, Block5400, 0, 1024);
//  ShowMSG(1,(int)"Block 5400 updated!");
  mfree(bufs);
  return 1;
}



static void SaveBlock(void)
{
  curblock=0;
  //add hash checking
      gipc.name_to=IPC_MSIM_NAME;
      gipc.name_from=ELF_MSIM_NAME;

      gipc.data=&msg;
      msg.type=PRC_MSIM_READBLOCK2FILE;
      msg.block=5400;
      msg.buf=bp;
      msg.offset=0;
      msg.size=0x30*20;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_MSIM_EEPROM_BACKDOOR,&gipc);
	  //поидее надо дождаться callback
   //    WriteFile();
}

static void SaveBlock1(void)
{
    curblock=1;
  //add hash checking
      gipc.name_to=IPC_MSIM_NAME;
      gipc.name_from=ELF_MSIM_NAME;

      gipc.data=&msg;
      msg.type=PRC_MSIM_READBLOCK2FILE;
      msg.block=5401;
      msg.buf=bp;
      msg.offset=0;
      msg.size=0x50*20;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_MSIM_EEPROM_BACKDOOR,&gipc);
	  //поидее надо дождаться callback
   //    WriteFile();
}


unsigned char htoi(char c)
{
  if ((c >= '0') && (c <='9'))
    return c - '0';
  if ((c >= 'A') && (c <='F'))
    return c - ('A' - 0xA) ;
  return 0;
}



int ReadFile(char *name)
{
  int size;
  int i, cnt;
  char *filebuf=NULL;

//  char Block5400[1024];

  int f;
  unsigned ul;
  if ((f=fopen(name,A_BIN,P_READ,&ul))!=-1){
    size=lseek(f,0,S_END,&ul,&ul);
    filebuf=malloc(size+1);
    if (filebuf){
      lseek(f,0,S_SET,&ul,&ul);   
      fread(f,filebuf,size,&ul);
      fclose(f,&ul);

  // parse file
      cnt = 0;

      for (i=0;i<size;i++)
        if (filebuf[i] > 32)
          bp[cnt++] = (htoi(filebuf[i++])<<4) + htoi(filebuf[i++]) ;
      mfree(filebuf);
      return cnt;
    };
   
  }
  return 0;
 
}



static void LoadBlock(void) //from flie
{
//  int m;
  if ((ReadFile((char*)f5400s))==0x30*20){
    
      gipc.name_to=IPC_MSIM_NAME;
      gipc.name_from=ELF_MSIM_NAME;

      gipc.data=&msg;
      msg.type=PRC_MSIM_WRITEBLOCK4FILE;
      msg.block=5400;
      msg.buf=bp;
      msg.offset=0;
      msg.size=0x30*20;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_MSIM_EEPROM_BACKDOOR,&gipc);
  }else {
    
      MsgBoxError(1,(int)LG_MSGERRBLOCK);
      GeneralFuncF1(1);
  }
}

static void LoadBlock1(void) //from flie
{
  if (ReadFile((char*)f5401s)==0x50*20){
    
      gipc.name_to=IPC_MSIM_NAME;
      gipc.name_from=ELF_MSIM_NAME;

      gipc.data=&msg;
      msg.type=PRC_MSIM_WRITEBLOCK4FILE;
      msg.block=5401;
      msg.buf=bp;
      msg.offset=0;
      msg.size=0x50*20;
      GBS_SendMessage(MMI_CEPID,MSG_IPC,IPC_MSIM_EEPROM_BACKDOOR,&gipc);
  }else {
      MsgBoxError(1,(int)LG_MSGERRBLOCK);
      GeneralFuncF1(1);
  }
}

extern const char *successed_config_filename;
static void OpenCfg(void) //from flie
{
  WSHDR *ws;
  ws=AllocWS(150);
  str_2ws(ws,successed_config_filename,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
}

static void OpenBlock(void) //from flie
{
  WSHDR *ws;
  ws=AllocWS(150);
  str_2ws(ws,f5400s,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
}

static void OpenBlock1(void) //from flie
{
  WSHDR *ws;
  ws=AllocWS(150);
  str_2ws(ws,f5401s,128);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
}

static const HEADER_DESC smenuhdr={0,0,0,0,NULL,(int)LG_MENU,LGP_NULL};

static const int smmenusoftkeys[]={0,1,2};

static const char * const smenutexts[]=
{
  LG_MNUASNET,
  LG_MNUCFG,
  LG_MNUSAVEB,
  LG_MNULOADB,  
  LG_MNUOPENB, 
  LG_MNUSAVEB1,
  LG_MNULOADB1,  
  LG_MNUOPENB1, 

  
};

extern void SendRequest(int submess, void *data);
static void AutoNet(void)
{
  SendRequest(IPC_MSIM_EEPROM_AUTOSEARCH,(void*)0);
  GeneralFuncF1(1);
//  GeneralFuncF1(1);
}
static const void *smenuprocs[]=
{
  (void *)AutoNet,
  (void *)OpenCfg,    
  (void *)SaveBlock,  
  (void *)LoadBlock,
  (void *)OpenBlock,
  (void *)SaveBlock1,    
  (void *)LoadBlock1,  
   (void *)OpenBlock1
};

void stmenu_ghook(void *data, int cmd)
{
  if (cmd==9)
  {}
  if (cmd==0x0A)
  {
    DisableIDLETMR();

   Menu_SetItemCountDyn(data,7);

//    if (simnum<3+simcnt)      SetCursorToMenuItem(data,2+simnum);    
  }
}

//static int sicon_array[2];



static void smenuitemhandler(void *data, int curitem, void *unk)
{
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  
  const char percent_t[]="%t";
//  extern const char percent_t[];
  ws=AllocMenuWS(data,strlen(smenutexts[curitem]));
  wsprintf(ws,percent_t,smenutexts[curitem]);

    SetMenuItemText(data, item, ws, curitem);    
  
}

static int stmenu_keyhook(void *data, GUI_MSG *msg)
{
  if ((msg->keys==0x18)||(msg->keys==0x3D))
  {
      ((void (*)(void))(smenuprocs[GetCurMenuItem(data)]))();
  }
  return(0);
}


static const MENU_DESC stmenu=
{
  8,stmenu_keyhook,stmenu_ghook,NULL,
  smmenusoftkeys,
  &smenu_skt,
  0x11,//MENU_FLAG,
  smenuitemhandler,
  NULL,//menuitems,
  NULL,//menuprocs,
  11
};

int ShowSetMenu()
{
  sS_ICONS[0]=(int)"4:\\zbin\\naticq\\img\\online.png";
  sS_ICONS[1]=(int)"4:\\zbin\\naticq\\img\\offline.png";
  sS_ICONS[2]=0;
//  sicon_array[0]=GetPicNByUnicodeSymbol(CBOX_CHECKED);
//  sicon_array[1]=GetPicNByUnicodeSymbol(CBOX_UNCHECKED);
  *((int **)(&smenuhdr.icon))=sS_ICONS;
  patch_header(&smenuhdr);
  SetMenu_ID=CreateMenu(0,0,&stmenu,&smenuhdr,0,11,0,0);
  return SetMenu_ID;
}
