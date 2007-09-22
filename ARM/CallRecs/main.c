/*
TODO:
парсинг базы событий - пока обойдемся думаю мало у кого больше 512  слбытий
*/
#define DEBUG

#ifdef NEWSGOLD

  #define DEFAULT_DISK "4"

#else
  #define DEFAULT_DISK "0"
#endif



#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "rect_patcher.h"
#include "conf_loader.h"
#include "language.h" //потом сгодится
static int prev_clmenu_itemcount;
int curlist=0;
//#include "const.c"




extern void kill_data(void *p, void (*func_p)(void *));

typedef struct 
{
  short year;
  short unk;
  char month;
  char day;
  short unk2;
  char hour;
  char min;
  char sec;
  char unk3; 
}dates;

//константа APO
#define APO_EN "calrec"
#define APO_TYPE 0x37
/*
typedef struct cals{
  dates *datetime;
  WSHDR  *number;
  WSHDR  *name;  
  unsigned int duration;
  unsigned char type;
  
};
*/
int S_ICONS[3];

int CountRecord(void);

char clm_hdr_text[48]="Calls Records";

void UpdateCLheader(void)
{
  char str[3][15]={"Calls-Dial","Calls-Recv.","Calls-Miss"};
  sprintf(clm_hdr_text,"%s :%d",str[curlist],CountRecord());
}

volatile int contactlist_menu_id=0;
static const HEADER_DESC contactlist_menuhdr = {0, 0, 0, 0, S_ICONS+0, (int)clm_hdr_text, LGP_NULL};
static const int menusoftkeys[] = {0,1,2};

void contactlist_menu_ghook(void *data, int cmd);
int contactlist_menu_onkey(void *data, GUI_MSG *msg);
void contactlist_menu_iconhndl(void *data, int curitem, void *unk);


char clmenu_sk_r[16]="Right";

typedef  struct {
  void* next;
  void* next_g;  //указатель на звонки с таким же номером
  dates datetime;
  char number[16];
  char name[32];  
  unsigned int duration;
  unsigned char type; //битовое поле флагов
  unsigned int recid;  //ид записи, мож пригодится
  char cnt;  //если группа то число субитемов -1
  char isGroup;  //признак группы
}CRECS;


int FindContactByContact(CRECS *req);

void RecountMenu(CRECS  *req){
  LockSched();  
  int i;
  int j;
  void *data;
  if (!contactlist_menu_id) return; //Нечего считать

  data=FindGUIbyId(contactlist_menu_id,NULL);
 if (req==NULL)
  {
    j=0;
  }
  else
  {
    j=FindContactByContact(req);
  }
  i=CountRecord();
  if (j>=i) j=i-1;
  if (j<0) j=0;
  
  if (i!=prev_clmenu_itemcount)
  {
    prev_clmenu_itemcount=i;
    Menu_SetItemCountDyn(data,i);
  }
  UpdateCLheader();
  SetCursorToMenuItem(data,j);
  
  
  if (IsGuiOnTop(contactlist_menu_id)) RefreshGUI();
  UnlockSched();
  return;
}

static const SOFTKEY_DESC clmenu_sk[]=
{
  {0x0018, 0x0000, (int)LG_OPTIONS},
  {0x0001, 0x0000, (int)clmenu_sk_r},
  {0x003D, 0x0000, (int)LGP_DOIT_PIC}
};


static const SOFTKEYSTAB clmenu_skt =
{
  clmenu_sk, 0
};

static const ML_MENU_DESC contactlist_menu=
{
  8,contactlist_menu_onkey,contactlist_menu_ghook,NULL,
  menusoftkeys,
  &clmenu_skt,
  0x11, //+0x400
  contactlist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0,   //n
  2 //Добавочных строк  
};


int create_contactlist_menu(void)
{
  int i;
  i=0;//CountContacts(); /
  i=0;
  prev_clmenu_itemcount=i;
  UpdateCLheader();
  patch_header(&contactlist_menuhdr);
  contactlist_menu_id=CreateMultiLinesMenu(0,0,&contactlist_menu,&contactlist_menuhdr,0,i);
  return contactlist_menu_id;
}

typedef struct {
  CRECS *top;
  CRECS *end;  
  CRECS *grp;  
} CRECL;

CRECL list[4]={0}; //3 списка вызовов, 4-ый прозапас)

CRECS *FindNum(char *num,CRECL* cl){
  CRECS *ct=cl->top;
  while (ct){
    if (!strcmp(ct->number,num))return ct;
    ct=ct->next;
  }
  return NULL;
}

int PutRecSub(CRECS* in,CRECL *cl){  //хитрожопые списки
  if (!(cl->top)){cl->top=in;
    cl->end=in;    
  }
  else{
    CRECS *ex;
    if ((ex=FindNum(in->number,cl))){
      cl->grp=ex;
      ex->cnt++;      
       while(ex->next_g){
        ex=ex->next_g;
      }
      ex->next_g=in;
      in->next=cl->grp;
      in->isGroup=1;
      return 0;
    }else{
      cl->end->next=in;
      cl->end=in;    
    }
  }
  return 1;
};


int PutRec(CRECS* in){ 
  if ((in->type&0x06)==0x06) //missed
       return PutRecSub(in,&list[2]);
  else
    if ((in->type&0x02)==0x02)//incoming
       return PutRecSub(in,&list[1]);
  else  //others - dialed
       return PutRecSub(in,&list[0]);
};
void contactlist_menu_ghook(void *data, int cmd)
{
 // PNGTOP_DESC *pltop=PNG_TOP();
  if (cmd==9)  ///unfocus ?
  {
//  pltop->dyn_pltop=NULL;
  }
  if (cmd==0x0A)
  {
  //pltop->dyn_pltop=XStatusesImgList;
    DisableIDLETMR();
  }
}

CRECS* GetNext(CRECS *ct){  //хитрожопое выдергивание из списка
    if (ct->cnt){ //группа
      if (!(ct->cnt&0x80)){ //группа и свернута
        ct=ct->next;
      }else{
        ct=ct->next_g;
      }
    }else{ //хз че
      if (ct->isGroup){ //если ссылка  внутри группы
       if (ct->next_g)  //если есть еще внутри группы то берем
         ct=ct->next_g;
         else{  //иначе откатываемся на след элемент 
          ct=ct->next;
          ct=ct->next;        
         };
       }
       else  //не группа
       ct=ct->next;       
    }
  
  return ct;
}

CRECS *FindRecordByN(int curitem){
  CRECS *ct=list[curlist].top;

  for (int j=0;ct&&(j<curitem);j++){
    ct=GetNext(ct);
  }
  return ct;
}
int CountRecord(void){
  CRECS *ct=list[curlist].top;

  int j;
  for ( j=0;ct;j++){
    ct=GetNext(ct);
  }
  return j;
}


int FindContactByContact(CRECS *req){
  CRECS *ct=list[curlist].top;

  int j;
  for ( j=0;ct&&ct!=req;j++){
    ct=GetNext(ct);
  }
  return j;
}

int contactlist_menu_onkey(void *data, GUI_MSG *msg)
{
  CRECS *t=NULL;
  int i;
  i=GetCurMenuItem(data);
  t=FindRecordByN(i);
/*
  if (msg->keys==0x18)
  {
    ShowMainMenu();
    return(-1);
  }
  */
  if (msg->keys==0x3D)
  {
    if (t)
    {
     
      if (t->cnt)
      {
	t->cnt^=0x80;
	RecountMenu(t);
	return(-1);
      }
    }
    return(-1);
  }else;
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    int key=msg->gbsmsg->submess;
    //меняем списки
    if (key==RIGHT_BUTTON){curlist++; 
      if (curlist>2)curlist=0;
      RecountMenu(NULL);
    }else
    if (key==LEFT_BUTTON){curlist--;
      if (curlist<0)curlist=2;
      RecountMenu(NULL);
    }else;
      
  }
//ТУТ БЫ тоже не плохо было прикрутить поиск по Т9 и дозвон
    
  return(0);
}



const char percent_t[]="%t";
const char percent_d[]="%d";
const char empty_str[]="";

void contactlist_menu_iconhndl(void *data, int curitem, void *unk)
{
  CRECS *t=NULL;
  void *item=AllocMLMenuItem(data);
  
  WSHDR *ws2;
  WSHDR *ws4;
  WSHDR ws1loc, *ws1;
  unsigned short num[128];
  ws1=CreateLocalWS(&ws1loc,num,128);
  WSHDR ws3loc, *ws3;
  unsigned short num3[128];
  ws3=CreateLocalWS(&ws3loc,num3,128);
  
  t=FindRecordByN(curitem);
  
  if (t!=NULL)
  {
    if (t->name[0]){  //если есть в справочнике имя то
      str_2ws(ws1,t->name,120);    
      wsprintf(ws3, "%t(%d)\n%02d.%02d.%02d\t%02d:%02d",t->number,(t->cnt&0x7f),t->datetime.day,t->datetime.month,t->datetime.year%100,t->datetime.hour,t->datetime.min);        
    }
    else{//иначе тока номер
       wsprintf(ws1, "%d (%d) %t",curitem,t->cnt&0x7f,t->number);                   
       wsprintf(ws3, "\n%02d.%02d.%02d\t%02d:%02d",t->datetime.day,t->datetime.month,t->datetime.year%100,t->datetime.hour,t->datetime.min);               
    }
    
  }
  else
  {
    wsprintf(ws1, LG_CLERROR);
    wsprintf(ws3, LG_CLERROR);   
  }
 
  ws2=AllocMenuWS(data,ws1->wsbody[0]);
  wstrcpy(ws2,ws1);
  
  if (t->cnt)      {//группа
      if (t->cnt&0x80)  //развернута -
       SetMenuItemIconArray(data, item, S_ICONS+0);
     else //свернута +
       SetMenuItemIconArray(data, item, S_ICONS+1);
  }
 else
   SetMenuItemIconArray(data, item, S_ICONS+2);
  
  ws4=AllocMenuWS(data,ws3->wsbody[0]);
  wstrcpy(ws4,ws3);
  SetMLMenuItemText(data, item, ws2, ws4, curitem);
}




void ReadCal(void)//собсветнно чтени АПО  
// ВНИМАНИЕ ЧТЕНИЕ НАПИСАНО ПОД ЕЛКУ и наверно НСГ, и идет с последней записи а не с первой, на СГОЛД надо чето придумать
{
//  struct cals tmp_cal;
//  tmp_cal.number=AllocWS(250);
//  tmp_cal.name=AllocWS(250);  
  unsigned int ul;

  WSHDR *tel;
    tel=AllocWS(50);

  int fin;
  
#ifdef NEWSGOLD
#define MAX_RECORDS 5000
#define LEVEL1_RN	(41*41)
#define LEVEL2_RN	(41)
#else 
#define MAX_RECORDS 1024
#define LEVEL1_RN	(0x20)
#endif
  
  
#pragma pack(1)
  struct {
#ifdef NEWSGOLD
    long dummy1; //62 33 dc 05
    short len;  //!!!!! НА СГОЛД ХЗ ЕСТЬ ЭТО ПОЛЕ ИЛИ НЕТ
    char bitmap[MAX_RECORDS/8];
#else
    long dummy1; // ВОЗМОЖНО ОН ВХОДИТ СЮДА и здесь должно быть два шота, иначе придется делать подругому поиск
    char bitmap[MAX_RECORDS/8];    
#endif    
  } ABmain;
#pragma pack()
  
//    unsigned int rec=0;
    unsigned int rec;    
  int fsz;
  char recname[128];  
  
  AB_UNPRES ur;
  void *buffer;
  int m=0;
//start parsing  
    if ((buffer=malloc(65536)))
  {
  zeromem(&ABmain,sizeof(ABmain));

    if ((fin=fopen("0:\\System\\apo\\" APO_EN "\\main",A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
    {
  
#ifdef ELKA
      if (fread(fin,&ABmain,sizeof(ABmain),&ul)>=69)
#else
      if (fread(fin,&ABmain,sizeof(ABmain),&ul)==sizeof(ABmain))
#endif
      {
        rec=ABmain.len;  //СОБСТВЕННО ВОТ ЗДЕСЬ  И НАЧИНАЕМ ЦИКЛ НЕ С НУЛЯ
        fclose(fin,&ul);
	do
	{
          #ifdef NEWSGOLD
	  if (ABmain.bitmap[rec>>3]&(0x80>>(rec&7)))
          #else
          if (ABmain.bitmap[rec>>3]&(1<<(rec&7)))
          #endif   
	  {



            
            #ifdef NEWSGOLD
	    //Запись есть в битмапе
            unsigned int rl1;
	    unsigned int rl2;
	    unsigned int rl3;
	    rl1=rec/LEVEL1_RN;
	    rl2=(rec%LEVEL1_RN)/LEVEL2_RN;
	    rl3=rec%LEVEL2_RN;
//	    snprintf(recname,128,"0:\\System\\apo\\app\\data\\%02d\\%02d\\%02d",rl1,rl2,rl3);
	   snprintf(recname,128,"0:\\System\\apo\\" APO_EN "\\data\\%02d\\%02d\\%02d",rl1,rl2,rl3);
            
            #else
	    unsigned int rl1=rec/LEVEL1_RN;
	    unsigned int r12=rec%LEVEL1_RN;
	    snprintf(recname,128,"0:\\System\\apo\\" APO_EN "\\%02x\\%02x",rl1,r12);            
            #endif             
	    if ((fin=fopen(recname,A_ReadOnly+A_BIN,P_READ,&ul))!=-1)
	    {
	      zeromem(&ur,sizeof(AB_UNPRES));
              fsz=lseek(fin,0,S_END,&ul,&ul);
              lseek(fin,0,S_SET,&ul,&ul);
  	      fread(fin,buffer,fsz,&ul);
	      fclose(fin,&ul);
              #ifdef NEWSGOLD
	      UnpackABentry(&ur,((char *)buffer+8),fsz,APO_TYPE);
              #else
              UnpackABentry(&ur,((char *)buffer+4),fsz-4,APO_TYPE);
              #endif
	      int i=0;
              
                CRECS *loc;
                loc=malloc(sizeof(CRECS));
                zeromem(loc,sizeof(CRECS));
                loc->  recid=rec;
                
                
	      while(i<ur.number_of_records)
	      {
                
		AB_UNPRES_ITEM *r=ur.record_list+i;
		if (r->no_data!=1)
		{
                  int tt=GetTypeOfAB_UNPRES_ITEM(r->item_type);
                    PKT_NUM *p;
                  switch (tt){
                  case 6: //дата
//                  tmp_cal.datetime=(dates*)r->data;
                  memcpy(&loc->datetime,r->data,sizeof(dates));
                    break;
                    case 5: //имя
///                        tmp_cal.name=r->data;
                        ws_2str(r->data,loc->name,31) ;    
  
                    break;    
                    
                  case 1:          //номер          
                      p=(PKT_NUM*)r->data;
                      if (p){
                        unsigned int c;
                         unsigned int c1;
                           int m,j;
			  j=0;
                          m=0;
                        wsprintf(tel,"");
                        if (p->format==0x91) wsAppendChar(tel,'+');
			  while(j<p->data_size)
			  {
                            if (m&1) {c1=c>>4; j++;}
                            else c1=(c=p->data[j])&0x0F;
			    if (c1==0x0F) break;
                            
                            if (c1==0xA) c1='*';
                            else if (c1==0xB) c1='#';
                            else if (c1==0xC) c1='+';
                            else if (c1==0xD) c1='?';
                            else c1+='0';
                            wsAppendChar(tel,c1);
			    m++;
			  }
                          
                         ws_2str(tel,loc->number,15) ;    
                      }
                
                    break;
                  case 3://unsigned short / timne in secs?
                
///                    dr=*((short*)r->data);
                    if (r->item_type==0x52)
                    {
                      //tmp_cal.duration=*((short*)r->data);
                      loc->duration=*((short*)r->data);
                    }
                    break;
                  case 0://unsigned int/ boolean ?
                                        
                                        if (r->item_type==0x54)  loc->type|=0x01*(*((short*)r->data));  //kto polozil trubku (1 -sam)
                                        if (r->item_type==0x55)  loc->type|=0x02*(*((short*)r->data)); //if 1 -incomig call
                                        if (r->item_type==0x56)  loc->type|=0x04*(*((short*)r->data));  //not answered 
                                        if (r->item_type==0x57)  loc->type|=0x08*(*((short*)r->data));  //conected by service  suzh as mts and wrong number 
                                        if (r->item_type==0x58)  loc->type|=0x10*(*((short*)r->data));  // ??? rejected
                                        if (r->item_type==0x59)  loc->type|=0x20*(*((short*)r->data));  //all side connected
                                        if (r->item_type==0x5a)  loc->type|=0x40*(*((short*)r->data));
                                                                                                        //06 -07 missed calls

                    
                    break;
                    
                  default:
                    ;
                  };
                }
		i++;

	      }
             
              
             m+=PutRec(loc);
	      FreeUnpackABentry(&ur,mfree_adr());




              
              
            }
	
            RecountMenu(NULL); // как-то позно оно апдеитится
	}
        
//          rec++;
          rec--;          //читы
        }  
//	while(rec<MAX_RECORDS); 
	while(rec>0);        //читы
      }
      else
      {
	fclose(fin,&ul);
      }
     mfree(buffer) ;
    }
          

    RecountMenu(NULL);
}
///end of parsing

  

    FreeWS(tel)  ;

}


void ELF_KILLER(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

unsigned short maincsm_name_body[140];


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
  csm->gui_id=    create_contactlist_menu();
//  maincsm=
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if(msg->msg == MSG_RECONFIGURE_REQ) 
  {
  }
  if ((msg->msg==MSG_GUI_DESTROYED)&&((int)msg->data0==csm->gui_id))
  {
    csm->csm.state=-3;
  }
  return(1);
}

void maincsm_onclose(CSM_RAM *csm)
{
  ///kill lists
  for (int i=0;i<3;i++){
    curlist=i;
    int cnt=CountRecord();
    for (int j=cnt-1;j>=0;j--){
      CRECS *t=      FindRecordByN(j);
      mfree(t);
    }
      
  }
  
  SUBPROC((void *)ELF_KILLER);
}
const int minus11=-11;
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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"Call Records v0.1a");
}

int maincsm;

// нужны иконки )
char str1[]="4:\\zbin\\naticq\\img\\groupon.png";
char str2[]="4:\\zbin\\naticq\\img\\groupoff.png";
char str3[]="4:\\zbin\\naticq\\img\\work.png";


int main(void)
{  
  S_ICONS[0]=(int)str1;
  S_ICONS[1]=(int)str2;
  S_ICONS[2]=(int)str3;
  SUBPROC((void*)  ReadCal);
  LockSched();
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  LockSched();
  maincsm=CreateCSM(&MAINCSM.maincsm,dummy,0);
  
  UnlockSched();

  return 0;
}
