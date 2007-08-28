#include "..\..\inc\swilib.h"

#ifdef NEWSGOLD
#define DEFAULT_DISK "4"
#else
#define DEFAULT_DISK "0"
#endif

void check();
void start_ring();
void load_settings();

unsigned int hour[6];
unsigned int min[6];
unsigned int status[6];
unsigned int weekdays[6][8];
int show_icon;
int X;
int Y;
int imgw;
int imgh;
char icon[]=DEFAULT_DISK":\\Zbin\\alarm\\icon.gpf";

GBSTMR mytmr;
TDate date;
TTime time;
TDate week;

#define idlegui_id (((int *)data)[DISPLACE_OF_IDLEGUI_ID/4])
int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
CSM_DESC icsmd;

extern void kill_data(void *p, void (*func_p)(void *));

const int minus11=-11;

typedef struct
{
  CSM_RAM csm;
}MAIN_CSM;

int IDLECSM_ID=-1;

typedef struct
{
  char signature[16];
  unsigned short picnum;
  unsigned short unk1;
  char w; 
  char h; 
  char Compr_Bits;  
}PICHDR;

DrwImg(IMGHDR *img, int x, int y, char *pen, char *brush)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc,x,y,img->w,img->h);
  SetPropTo_Obj5(&drwobj,&rc,0,img);
  SetColor(&drwobj,pen,brush);
  DrawObject(&drwobj);
}

void DrawGPF(char *fname, int x, int y)
{
  volatile int hFile;
  PICHDR Pic_Header;
  unsigned int io_error = 0;
  hFile = fopen(fname, A_ReadOnly + A_BIN, P_READ, &io_error);
  if(!io_error)
  {
    fread(hFile, &Pic_Header, sizeof(Pic_Header), &io_error);
    unsigned int buf_size = lseek(hFile, 0, S_END, &io_error, &io_error) - sizeof(PICHDR);
    lseek(hFile, sizeof(PICHDR), S_SET, &io_error, &io_error); 
    char *pic_buffer = malloc((buf_size+3)&(~3));
    fread(hFile, pic_buffer, buf_size, &io_error);    
    fclose(hFile, &io_error);
    IMGHDR img;
    img.w = Pic_Header.w;
    img.h = Pic_Header.h;
    img.bpnum = Pic_Header.Compr_Bits;
    //img.zero = 0;
    img.bitmap = pic_buffer;
    DrwImg(&img, x, y, GetPaletteAdrByColorIndex(1), GetPaletteAdrByColorIndex(0));
    mfree(pic_buffer);
  }
}

void getimgwh()
{
  volatile int hFile;
  PICHDR Pic_Header;
  unsigned int io_error = 0;
  hFile = fopen(icon, A_ReadOnly + A_BIN, P_READ, &io_error);
  if(!io_error)
  {
    fread(hFile, &Pic_Header, sizeof(Pic_Header), &io_error);
    fclose(hFile, &io_error);
    imgw=Pic_Header.w;
    imgh=Pic_Header.h;
  }
}

void load_settings(void)
{
char *fname=DEFAULT_DISK":\\Zbin\\alarm\\alarm.cfg";
char *data; 
unsigned int err; 
int handle; 
handle=fopen(fname, A_ReadOnly, P_READ,&err); 
if(handle!=-1)
{
  data=malloc(256);
  if(data!=0)
    {
      fread(handle,data,10000,&err);
      
status[1]=data[2];
hour[1]=data[3];
min[1]=data[4];
weekdays[1][1]=data[5];
weekdays[1][2]=data[6];
weekdays[1][3]=data[7];
weekdays[1][4]=data[8];
weekdays[1][5]=data[9];
weekdays[1][6]=data[10];
weekdays[1][7]=data[11];
status[2]=data[12];
hour[2]=data[13];
min[2]=data[14];
weekdays[2][1]=data[15];
weekdays[2][2]=data[16];
weekdays[2][3]=data[17];
weekdays[2][4]=data[18];
weekdays[2][5]=data[19];
weekdays[2][6]=data[20];
weekdays[2][7]=data[21];
status[3]=data[22];
hour[3]=data[23];
min[3]=data[24];
weekdays[3][1]=data[25];
weekdays[3][2]=data[26];
weekdays[3][3]=data[27];
weekdays[3][4]=data[28];
weekdays[3][5]=data[29];
weekdays[3][6]=data[30];
weekdays[3][7]=data[31];
status[4]=data[32];
hour[4]=data[33];
min[4]=data[34];
weekdays[4][1]=data[35];
weekdays[4][2]=data[36];
weekdays[4][3]=data[37];
weekdays[4][4]=data[38];
weekdays[4][5]=data[39];
weekdays[4][6]=data[40];
weekdays[4][7]=data[41];
status[5]=data[42];
hour[5]=data[43];
min[5]=data[44];
weekdays[5][1]=data[45];
weekdays[5][2]=data[46];
weekdays[5][3]=data[47];
weekdays[5][4]=data[48];
weekdays[5][5]=data[49];
weekdays[5][6]=data[50];
weekdays[5][7]=data[51];

show_icon=data[53];
X=data[54];
Y=data[55];

mfree(data);
    }
  fclose(handle,&err);
}
/////////////
  getimgwh();
/////////////
}

void start_ring()
{
  char elf[]=DEFAULT_DISK":\\Zbin\\alarm\\alarm_ring.elf";
  WSHDR *ws;
  ws=AllocWS(64);
  str_2ws(ws,elf,strlen(elf)+1);
  ExecuteFile(ws,0,0);
  FreeWS(ws);
}

void alarm(int n)
{
  if (status[n])
  {
    TDate date;
    GetDateTime(&date, 0);
    char wd = GetWeek(&date);
    if (weekdays[n][wd+1])
      {
        GetDateTime(&date,&time);
        if (time.hour==hour[n])
          {
            if (time.min==min[n])
              {
                start_ring();
              }
          }
      }
  }
}

void start_check(void)
{
  if (status[1] || status[2] || status[3] || status[4] || status[5])
  {
    show_icon=1;
  }
   else show_icon=0;
  
  for (int i=1;i<6;i++)
  {
    alarm(i);
  }
  
  GBS_StartTimerProc(&mytmr,216*60,start_check);
}

int MyIDLECSM_onMessage(CSM_RAM* data, GBS_MSG* msg)
{
  int csm_result=old_icsm_onMessage(data, msg);
	
  if (IsGuiOnTop(idlegui_id))
  {
    GUI *igui=GetTopGUI();
    if (igui)
    {
      void *idata = GetDataOfItemByID(igui, 2);
      if (idata)
      {
        if((IsUnlocked())&&(show_icon==1))
        {
          void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
          DrawCanvas(canvasdata, X, Y, X + imgh, Y + imgh, 1);
          DrawGPF(icon,X,Y);
        }
    }
    
  }
  }
  return(csm_result);
}


#pragma inline=forced
int toupper(int c)
{
  if ((c>='a')&&(c<='z')) c+='A'-'a';
  return(c);
}
#pragma inline
int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i=(c=toupper(*s1++))-toupper(*s2++))) if (!c) break;
  return(i);
}

int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg)
{
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
    if (strcmp_nocase("alarm",(char *)msg->data0)==0)
    {
      ShowMSG(1,(int)"alarm settings updated!");
      load_settings();
    }
  }

  return(1);
}

static void maincsm_oncreate(CSM_RAM *data){}

static void Killer(void)
{
  extern void *ELF_BEGIN;
  GBS_DelTimer(&mytmr);
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

static void maincsm_onclose(CSM_RAM *csm)
{
  SUBPROC((void *)Killer);
}

static unsigned short maincsm_name_body[140];

static const struct
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

static void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"alarm");
}

int main(void)
{  
  CSM_RAM *save_cmpc;
  char dummy[sizeof(MAIN_CSM)];
  load_settings();
  UpdateCSMname();  
  
  LockSched();
  save_cmpc=CSM_root()->csm_q->current_msg_processing_csm;
  CSM_root()->csm_q->current_msg_processing_csm=CSM_root()->csm_q->csm.first;
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  CSM_root()->csm_q->current_msg_processing_csm=save_cmpc;
  UnlockSched();
  
  start_check();
  
    CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
    memcpy(&icsmd,icsm->constr,sizeof(icsmd));
    old_icsm_onMessage=icsmd.onMessage;
    icsmd.onMessage=MyIDLECSM_onMessage;
    icsm->constr=&icsmd;
    
  return 0;
}

