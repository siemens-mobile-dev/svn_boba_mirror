#include "arm_siemens.h"
#define ramStr(strnum) (0xB0000000|strnum)


#define MAX_ABFILE_SIZE 30000


const char fname[]  @ "DEBUG4" =  "0:\\System\\apo\\addr\\abT9";
const char ABpath[]  @ "DEBUG4" =  "0:\\System\\apo\\addr\\";
const char ABmain[]  @ "DEBUG4" =  "0:\\System\\apo\\addr\\main";
const char ABfout[]  @ "DEBUG4" =  "0:\\System\\apo\\addr\\abT9";

const unsigned short dsRects[4][4]  @ "DEBUG3" =
{
  {4,22,131,44},
  {4,26,131,44},
  {4,27,131,44},
  {4,20,131,44}
};

const unsigned char color[4] @ "DEBUG3" =

{255,255,0x90,255};

typedef struct
{
  char unk[3];
  char itemID;
  int unk1;
  int *pnt;
}ABitem;


typedef struct
{
  CSM_RAM csm;
  char dummy[0xCC-sizeof(CSM_RAM)];
  WSHDR *str1;
  WSHDR *strings[5];
  int myAB;
  char *fileAB;
  char flashSMS;
}IDLECSM;

//Вместо onCreate
void main(IDLECSM *icsm)
{


//  unsigned int err;
//  int f;
  //void (__thumb *OldOnCreate)(void *)=(void(*)(void *))(0xA02A781C+1);
  icsm->str1=AllocWS(256);
  icsm->strings[0]=AllocWS(50);
  icsm->strings[1]=AllocWS(50);
  icsm->strings[2]=AllocWS(50);
  icsm->strings[3]=AllocWS(50);
  icsm->strings[4]=AllocWS(50);
  icsm->flashSMS=0;

  icsm->myAB=0;
/*  icsm->fileAB=malloc(MAX_ABFILE_SIZE+1);
  if ((f=fopen((char *)fname,A_ReadOnly|A_BIN,P_READ|P_WRITE,&err))!=-1)
  {
    icsm->fileAB[fread(f,icsm->fileAB,MAX_ABFILE_SIZE,&err)]=0;
    fclose(f,&err);
  }
  wsprintf(icsm->str1,"num : ");
*/
  IDLECSM_onCreate((CSM_RAM*)icsm);
}


const char keytab[10][10] @ "DEBUG3" =
{
  {'1',0,0,0,0,0,0,0,0,0},
  {'2','А','Б','В','Г','A','B','C',0,0 },
  {'3','Д','Е','Ё','Ж','З','D','E','F',0 },
  {'4','И','Й','К','Л','G','H','I',0,0},
  {'5','М','Н','О','J','K','L',0,0,0},
  {'6','П','Р','С','M','N','O',0,0,0},
  {'7','Т','У','Ф','Х','P','Q','R','S',0},
  {'8','Ц','Ч','Ш','Щ','Ъ','T','U','V',0},
  {'9','Ы','Ь','Э','Ю','Я','W','X','Y','Z'},
  {'0','+',0,0,0,0,0,0,0,0}
};


const char wintranslation[128] @ "DEBUG3" =
{
  0x5F,0x5F,0x27,0x5F,0x22,0x3A,0xC5,0xD8,0x5F,0x25,0x5F,0x3C,0x5F,0x5F,0x5F,0x5F,
  0x5F,0x27,0x27,0x22,0x22,0x07,0x2D,0x2D,0x5F,0x54,0x5F,0x3E,0x5F,0x5F,0x5F,0x5F,
  0xFF,0xF6,0xF7,0x5F,0xFD,0x83,0xB3,0x15,0xF0,0x63,0xF2,0x3C,0xBF,0x2D,0x52,0xF4,
  0xF8,0x2B,'I' ,'i' ,0xA3,0xE7,0x14,0xFA,0xF1,0xFC,0xF3,0x3E,0x5F,0x5F,0x5F,0xF5,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
  0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF
};

const unsigned short dos2unicode[128] @"DEBUG3"=
{
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
  0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
  0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
  0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
  0x002D,0x002D,0x002D,0x00A6,0x002B,0x00A6,0x00A6,0x00AC,
  0x00AC,0x00A6,0x00A6,0x00AC,0x002D,0x002D,0x002D,0x00AC,
  0x004C,0x002B,0x0054,0x002B,0x002D,0x002B,0x00A6,0x00A6,
  0x004C,0x0433,0x00A6,0x0054,0x00A6,0x003D,0x002B,0x00A6,
  0x00A6,0x0054,0x0054,0x004C,0x004C,0x002D,0x0433,0x002B,
  0x002B,0x002D,0x002D,0x002D,0x002D,0x00A6,0x00A6,0x002D,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
  0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F,
  0x0401,0x0451,0x0404,0x0454,0x0407,0x0457,0x040E,0x045E,
  0x00B0,0x2022,0x00B7,0x0076,0x2116,0x00A4,0x00A6,0x00A0
};
//=======================================================================
void menup1(void)
{
}
void menup2(void)
{
}
void menup3(void)
{
}
void menup4(void)
{
}
char uni2cp1251(unsigned short c)
{
  if (c>0x40F)
  {
    if (c==0x451) return(0xB8);
    return(c-0x350);
  }
  return(c);
}

void ws2asciz( char * d ,WSHDR * s, int len)
{
  int i=1;
  do
  {
    *d++=uni2cp1251(s->wsbody[i++]);
  }
  while(--len);
  *d=0;
}

char cnv(char c)
{
  int i,j;
  char cc;
  if ((unsigned char)c>0x40)
    cc=c&~0x20;
  else
    cc=c;

  if (c==' ') return(' ');
  for (i=0;i<10;i++)
    for (j=0;j<10;j++)
      if  (cc==keytab[i][j]) return(keytab[i][0]);

  return(' ');
}

void UnPackAB()
{
  unsigned int err;
  int f;
  int numrec=0;
  int rnum=-1;
  char ABrecname[40];
  char st[20];
  char *record=malloc(2048);
  char *main=malloc(132);
  char *result=malloc(MAX_ABFILE_SIZE);
  int rp=0;
  zeromem(result,sizeof(result));

  if ((f=fopen((char *)ABmain,A_ReadOnly|A_BIN,P_READ|P_WRITE,&err))!=-1)
  {
    fread(f,main,132,&err);
    fclose(f,&err);

    memcpy(main,&main[4],128);
    do
    {
      if(main[numrec>>3] & (1<<(numrec &7)))
      {
        memcpy(ABrecname,ABpath,23);
        sprintf(st,"%02X\\%02X",numrec>>5,numrec&0x1F);
        strcat(ABrecname,st);

        if ((f=fopen((char *)ABrecname,A_ReadOnly|A_BIN,P_READ|P_WRITE,&err))!=-1)
        {
          char ttype[6];
          char str [80];
          char flag;
          fread(f,record,1024,&err);
          fclose(f,&err);
          int * struc1=malloc(0x1C);
          zeromem(struc1,0x1C);
          UnPackABrecord(struc1,record+4,100,0x28);
          {
            rnum++;
            flag=0;
            char itemcount = ((short*)struc1)[1];
            ABitem * items = (ABitem*)struc1[3];
            char num[40];
            str[0]=0;
            zeromem(ttype,sizeof(ttype));
            do
            {
              switch(items->itemID)
              {
              case 0x23:
              case 0x24:

                strcat(str," ");
                ws2asciz(str+strlen(str),(WSHDR*)items->pnt,wstrlen((WSHDR*)items->pnt));
                break;
              case 0x2D:

              case 0x2A:
              case 0x2C:
              case 0x2B:
                if (!flag)
                {

                  {char j=0;do{result[rp++]=str[j++];}while(j<strlen(str));result[rp++]=0;}
                  char i=0;
                  do
                  {
                    result[rp++]=cnv(str[i++]);
                  }
                  while(i<strlen(str));
                }
                ttype[flag++]=items->itemID;
                ABnumber2str(items->pnt,num,0x2A,0);
                result[rp++]=' ';
                {char j=0;do{result[rp++]=num[j++];}while(j<strlen(num));}
                break;
              }
              items++;
            }
            while(--itemcount);
            result[rp++]=0;
            result[rp++]=(char)rnum;
            result[rp++]=(char)(rnum >>8)|((flag-1)<<2);
            memcpy(&result[rp],ttype,sizeof(ttype));
            rp+=sizeof(ttype);
          }
          FreeUnPackABrecord(struc1,mfree);
          mfree(struc1);
        }
      }
    }while(++numrec<1024);

  }

  if ((f=fopen((char *)ABfout,A_ReadWrite+A_BIN+A_Create+A_Truncate,P_READ+P_WRITE,&err))!=-1)
  {
    fwrite(f,result,rp-1,&err);
    fclose(f,&err);
  }


  mfree(record);
  mfree(main);
  mfree(result);
}


int onKeyGUI(void * GUI, GUI_MSG * msg)
{
  int item=GetCurrMenuItem(GUI);
  IDLECSM *icsm=FindCSMbyID(CSM_root.idle_id);

  if (msg->unk2==0x018)
  {
    char num[20];
    WSHDR * dummystr=AllocWS(1);
    icsm->flashSMS=0;
    ws2ascii(num,icsm->strings[item+1],wstrlen(icsm->strings[item+1]));
    Open_NewSMS_Dialog(dummystr,(char*)num,0x4209,0x00C1,0x0001);
    return(1);
  }

  if (msg->unk2==0x031)
  {

    char num[20];
    WSHDR * dummystr=AllocWS(1);
    icsm->flashSMS=1;
    ws2ascii(num,icsm->strings[item+1],wstrlen(icsm->strings[item+1]));
    Open_NewSMS_Dialog(dummystr,(char*)num,0x4209,0x00C1,0x0001);
    return(1);
  }

  if (msg->msg->submess==GREEN_BUTTON)
  {
    char pnum[41];
    ws2ascii(pnum,icsm->strings[item+1],wstrlen(icsm->strings[item+1]) );
    MakeVoiceCallTo(pnum,0x10,0x20C1);
    return(1);
  }
  return(0);
}

int gmproc(void * GUI, int cmd)
{
  //  SetNMenuItems(GUI,2);
  if (cmd==2)
  {
    //    SetMenuItemIcon(GUI,0,3);
    //    SetNMenuItems(GUI,1);
  }
  if (cmd==3)
  {
    MENU_DESC *menu=((MENU_DESC**)GUI)[2];
    mfree(menu->items);
    mfree(menu);
  }
  return((int)GUI);
}


const int icons[] @"DEBUG3"={0x43A,0x39B,0x397,0x39A,0x399,0};

const HEADER_DESC menuheader @"DEBUG3" ={3,24,126,41,(int *)icons,ramStr(0),0x7FFF};

const SOFTKEY_DESC skeys[] @"DEBUG3" =
{
  {0x018,0x018,(int)"SMS"},
  {0x031,0x031,(int)"Flash"},
  {0x030,0x030,(int)""}
};

const int menuskey[4][3] @"DEBUG3" =
{
  {0,1,2},
  {1,2,0},
  {2,0,1},
  {0,1,9}
}
;

const SOFTKEYSTAB skey @"DEBUG3"={ (SOFTKEY_DESC*)skeys,3 };

const MENUITEM_DESC MenuItems[] @"DEBUG3" =
{
  {(int*)icons+1,ramStr(1),0x0,0x000,(int*)menuskey,0x003,0x578},
  {(int*)icons+2,ramStr(2),0x0,0x000,(int*)menuskey+1,0x003,0x578},
  {(int*)icons+3,ramStr(3),0x0,0x000,(int*)menuskey+2,0x003,0x578},
  {(int*)icons+3,ramStr(4),0x0,0x000,(int*)menuskey+3,0x003,0x578}

};

const void * const menuprocs[4] @"DEBUG3" ={(void *)menup1,(void *)menup2,(void *)menup3,(void *)menup4};

const MENU_DESC menu4 @"DEBUG3" =
{
  0,				//0x00
  (void*)onKeyGUI,		//0x04
  (void*)gmproc,		//0x08
  NULL,				//0x0C
  (int *)menuskey,		//0x10
  (SOFTKEYSTAB *)(&skey),	//0x14
  1,				//0x18
  NULL,				//0x1C
  (MENUITEM_DESC *)MenuItems,	//0x20
  (void **)menuprocs,		//0x24
  4				//0x28
};

//=======================================================================

unsigned short char8to16(char c) @"DEBUG1"
{
  if (c<32) return('_');
  if (c>=128)
  {
    c=wintranslation[c-128];
    return(dos2unicode[c-128]);
  }
  return(c);
}

void OpenABRecord(char * s,char sname[],IDLECSM *icsm, char *p)
{
  char i=0;
  char *s1=sname;
  do
  {
    if (*s!=32)
    {
      *s1++=*s;
    }
    else
    {
      *s1=0;
      i++;
      wsprintf(icsm->strings[i],"%t",(int*)sname);
      s1=sname;
    }
  }
  while(*s++);
  i++;
  wsprintf(icsm->strings[i],"%t",(int*)sname);
  sname[0]=0;


  MENUITEM_DESC * items_desc = malloc(sizeof(MenuItems));
  MENU_DESC * menu_desc = malloc(sizeof(menu4));


  memcpy(items_desc,MenuItems,sizeof(MenuItems));
  memcpy(menu_desc,&menu4,sizeof(menu4));
  menu_desc->n_items=i;
  menu_desc->items=items_desc;

  menu_desc->items[0].icon=(int *)(icons+((*p++)-0x29));
  menu_desc->items[1].icon=(int *)(icons+((*p++)-0x29));
  menu_desc->items[2].icon=(int *)(icons+((*p++)-0x29));
  menu_desc->items[3].icon=(int *)(icons+((*p)-0x29));
  CreateMenu(1,0,menu_desc,&menuheader,0,1,0,0);
}




int doit(void *gui, GUI_MSG *msg) @ "DEBUG1"
{
  int (*OldMethod)(void *, GUI_MSG *)=(int (*)(void *, GUI_MSG *))(0xA02CB28E+1);
  int i,j,key;
  char dstr[41];
  unsigned short index[6][2];
  unsigned short tmp;
  WSHDR *str;
  void *adr=((void ****)gui)[0x50/4][0x4C/4][0x28/4];
  dstr[0]=' ';
  ws2ascii(&dstr[1],getDialString(adr),wstrlen(getDialString(adr)));

  IDLECSM *icsm=FindCSMbyID(CSM_root.idle_id);
  char sname[80];
  char ss[200];
  char *s=icsm->fileAB;

  key=msg->msg->submess;
  if ((key==UP_BUTTON)||(key==DOWN_BUTTON))
  {
    i=0;
    msg->unk2=0;
  }
  else
  {
    if (key!=GREEN_BUTTON)  i=(*OldMethod)(gui, msg);
  }
#define cp (icsm->myAB)
  str=AllocWS(20);
  cutwstr(icsm->str1,0);


  j=0;
  do
  {
    strcpy(sname,s);
    s+=strlen(sname)+1;
    strcpy(ss,s);
    s+=strlen(ss)+1;
    tmp=(*s++);
    tmp|=((unsigned short)(*s++))<<8;
    s+=6;
    if (strstr(ss,dstr))
    {
      // номер записи в файле.
      index[j][0]=tmp&0x03FF;
      // количество номеров в записи.
      index[j][1]=tmp>>10;
      j++;
    }
  }
  while(((*s)&&(j<6)));
  FreeWS(str);

  //--------------------------------------------------

  if (msg->msg->msg== KEY_DOWN)
  {
    switch (key)
    {
    case UP_BUTTON:
      if ((cp)&&(cp!=-1)) cp--; else cp=j-1;
      break;
    case DOWN_BUTTON:
      if (cp<(j-1)) cp++; else cp=0;
    		break;
    case LEFT_SOFT: cp=-1;
    break;
    }
    if ((key >= 0x30)&&(key <= 0x39))
    {
      //      wsaddwchar(icsm->str1,key);
      cp=-1;
    }
    PendedRedrawGUI();
  }


  if ((key == ENTER_BUTTON))
  {
    if ((unsigned int)cp<6)
    {
      //      WSHDR *str=getDialString(adr);
      char *p;
      s=icsm->fileAB;
      tmp=index[cp][0]+1;
      do
      {
        strcpy(sname,s);
        s+=strlen(sname)+1;
        strcpy(ss,s);
        s+=strlen(ss)+3+6;
      }
      while(--tmp);
      p=s-6;
      s=ss+strlen(sname)+1;
      j=1;

      wsprintf(icsm->strings[0],"%t",(int*)sname);
      OpenABRecord(s,sname,icsm,p);
      i=0;
      msg->unk2=0;
    }
  }

  if ((key == GREEN_BUTTON))
  {
    if ((unsigned int)cp<6)
    {
      WSHDR *str=getDialString(adr);
      char *p;
      s=icsm->fileAB;
      tmp=index[cp][0]+1;
      do
      {
        strcpy(sname,s);
        s+=strlen(sname)+1;
        strcpy(ss,s);
        s+=strlen(ss)+3+6;
      }
      while(--tmp);
      p=s-6;
      s=ss+strlen(sname)+1;
      j=1;

      wsprintf(icsm->strings[0],"%t",(int*)sname);

      if (index[cp][1])
      {
        OpenABRecord(s,sname,icsm,p);
        i=0;
        msg->unk2=0;
      }
      else
      {
        while(*s)
        {
          str->wsbody[j]=*s++;
          j++;
        }
        str->wsbody[0]=j-1;
        i=(*OldMethod)(gui, msg);
      }
    }
    else
      i=(*OldMethod)(gui, msg);
  }

  //  FreeWS(str);
  //--------  --------------------------------------------------------


  return(i);
#undef cp
}

void ed_inp_redraw(void *data)
{
  ((void(*)(void *))(0xA07FE476+1))(data);
  //Вставлять свое
  IDLECSM *icsm=FindCSMbyID(CSM_root.idle_id);
  DrawFrameInNativeMenu(8,47,130,154,0,0,0,
                        GetPaletteAdrByColorIndex(1),
                        (int)&color[0]/*GetPaletteAdrByColorIndex(7)*/);
  DrawString(icsm->str1,   12,49,128,152,   3,0x0,    GetPaletteAdrByColorIndex(1),
             (int)&color[0]/*GetPaletteAdrByColorIndex(7)*/);
}

__thumb __root int redraw (void * data, int cmd)
{
  int i;
  int (*OldMethod)(void *,int)=(int (*)(void *,int))(0xA02CB7E4+1);
  void * * methods;
  WSHDR *str;
  void *adr=((void ****)data)[0x50/4][0x4C/4][0x28/4];
  str=getDialString(adr);
  IDLECSM *icsm=FindCSMbyID(CSM_root.idle_id);

  if (cmd==2)
  {
    //Called after onCreate
    unsigned int err;
    int f;
    methods=malloc(64+4);
    void **m=GetDataOfItemByID(data,4);
    memcpy(methods,m[1],64);
    *methods=(void *)ed_inp_redraw;
    methods[16]=m[1];
    m[1]=methods;
    icsm->fileAB=malloc(MAX_ABFILE_SIZE+1);
    zeromem(icsm->fileAB,MAX_ABFILE_SIZE);
//    UnPackAB(icsm->fileAB);

    if ((f=fopen((char *)fname,A_ReadOnly|A_BIN,P_READ|P_WRITE,&err))!=-1)
    {
      icsm->fileAB[fread(f,icsm->fileAB,MAX_ABFILE_SIZE,&err)]=0;
      fclose(f,&err);
    }



  }
  if (cmd==3)
  {
    void **m=GetDataOfItemByID(data,4);
    void **mm=m[1];
    m[1]=mm[16];
    mfree(mm);
    cutwstr(icsm->str1,0);
    icsm->flashSMS=0;
    mfree(icsm->fileAB);
  }
  if ((cmd==7)&&(wstrlen(str)))
  {
    char dstr[41];
    char j,k;
    dstr[0]=' ';
    ws2ascii(&dstr[1],str,wstrlen(str));
    char sname[80];
    char ss[200];
    char *s=icsm->fileAB;
#define cp (icsm->myAB)

    str=AllocWS(20);
    cutwstr(icsm->str1,0);
    j=0;
    do
    {
      strcpy(sname,s);
      s+=strlen(sname)+1;
      strcpy(ss,s);
      s+=strlen(ss)+1;
      s+=2+6;
      if (strstr(ss,dstr))
      {
        char *ss1;
        k=(strstr(ss,dstr)-ss);
        cutwstr(str,0);
        if (j==cp) wsaddwchar(str,0xE003);
        ss1=sname+1;
        if (strlen(sname)>16) {sname[14]='.';sname[15]='.';sname[16]=0;}
        strcat(sname," ");
        do
        {
          if ((j!=cp)&&(k<(strlen(sname)))&&(cp==-1))
          {
            if (k==wstrlen(str)) wsaddwchar(str,0xE003);
            if ((wstrlen(str)==(k+(strlen(dstr))))) {wsaddwchar(str,0xE004);}
          }
          wsaddwchar(str,char8to16(*ss1));
          ss1++;
        }
        while(*ss1);
        if ((wstrlen(str)<=(k+(strlen(dstr))))) {wsaddwchar(str,0xE004);}
        if (j==cp) wsaddwchar(str,0xE004);
        wsaddwchar(str,'\n');
        wstrcat(icsm->str1,str);
        j++;
      }
    }
    while(((*s)&&(j<6)));
    FreeWS(str);

#undef cp
  }


  i=OldMethod(data,cmd);

  /*  if (cmd==7)
  {
}*/

  return(i);
}

int resize(void *GUI) @ "DEBUG2"
{
  unsigned int *curSize=((unsigned int ****)GUI)[0x50/4][0x4C/4][0x28/4]+(0x14/4);
  unsigned short *fontSize=((unsigned short ******)GUI)[0x50/4][0x4C/4][0x28/4][0x68/4][0x08/4]+(0x0E/2);
  unsigned int dsLen=getDialStringLen(GUI);


  int i;
  if (dsLen>13)
  {
    if (dsLen>17)
    {
      *curSize=7;
      *fontSize=2;
      if (dsLen>20) i=24; else i=16;
    }
    else
    {
      *curSize=2;
      *fontSize=4;
      i=8;
    }
  }
  else
  {
    *curSize=1;
    *fontSize=5;
    i=0;
  }
  return((int)(&dsRects)+i);
}

void ESI(char *s, WSHDR *ws)
{
  int c;
  if ((((unsigned int)s)>>28)==0x0B)
  {
    IDLECSM *icsm=FindCSMbyID(CSM_root.idle_id);
    wstrcpy(ws,icsm->strings[((unsigned int)s)&0x0F]);
    return;
  }
  if ((((unsigned int)s)>>28)!=0x0A) return;
  while((c=*s++))
  {
    wsaddwchar(ws,char8to16(c));
  }
}

__thumb void FlashSMS(char * dest, char * source)
{
  IDLECSM *icsm=FindCSMbyID(CSM_root.idle_id);

  memcpy(dest+0x0A,source,0x0D);

  if (icsm->flashSMS)
  {
    icsm->flashSMS=0;
    dest-=0xB8;
    switch (*dest)
    {
    case 0x08:*dest=0x18;    break;
    case 0x00:*dest=0xF0;    break;
    }
  }
}

__root __arm void unpack(void * data, int cmd)
{
}


//Патчи
#pragma diag_suppress=Pe177
__root static const int NEW_MAKEFLASH @ "PATCH_MAKEFLASH" =(int)FlashSMS;
__root static const int NEW_ONDRAW @ "PATCH_ONDRAW" =(int)redraw;
__root static const int NEW_ONKEY @ "PATCH_ONKEY" =(int)doit;
__root static const int NEW_ONCREATE @ "PATCH_ONCREATE" = (int)main;
__root static const int NEW_CSMSIZE @ "PATCH_CSMSIZE" =sizeof(IDLECSM);

