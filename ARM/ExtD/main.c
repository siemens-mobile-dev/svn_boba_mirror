#include "../inc/swilib.h"

#define MAX_EXTS 16

#ifdef NEWSGOLD
#define DEFAULT_DISK "4"
#else
#define DEFAULT_DISK "0"
#endif

/*CSM_DESC icsmd;

void (*old_icsm_onClose)(CSM_RAM*);*/

char exts[MAX_EXTS][10];
char elfs[MAX_EXTS][128];
int smallicons[MAX_EXTS][2];
int largeicons[MAX_EXTS][2];

int do_ext(WSHDR *filename, WSHDR *ext, void *param)
{
  int i;
  char s[128];
  WSHDR *elfname=AllocWS(256);
  ws_2str(ext,s,126);
  i=-1;
  do
  {
    i++;
    if (i>=MAX_EXTS) return 0;
  }
  while(strcmp(s,exts[i]));
  str_2ws(elfname,elfs[i],126);
  ws_2str(filename,s,126);
  i=ExecuteFile(elfname,NULL,s);
  FreeWS(elfname);
  return(i);
}


REGEXPLEXT reg=
#ifdef NEWSGOLD
{
  NULL,
  0x55,
  0x59D43FF,
  NULL,
  NULL,
  0x109, //LGP "Открыть"
  0x197, //LGP "Опции"
  0x7FFFC0FB,
  (void *)do_ext,
  0
};
#else
{
  NULL,
  0x55,
  0x57807FF,
  NULL,
  NULL,
  (void *)do_ext,
  0
};
#endif

/*void MyIDLECSM_onClose(CSM_RAM *data)
{
extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
extern void *ELF_BEGIN;
reg.icon1=NULL;
reg.icon2=NULL;
while(reg.unical_id!=0x55)
{
reg.proc=NULL;
RegExplorerExt(&reg);
reg.unical_id--;
  }
seqkill(data,old_icsm_onClose,&ELF_BEGIN,SEQKILLER_ADR());
}*/

static const char extfile[]=DEFAULT_DISK ":\\ZBin\\etc\\extension.cfg";

int main()
{
  int i=0;
  int p=0;
  char *s;
  char *olds;
  int f;
  unsigned int ul;
  unsigned int size_cfg;
  int c;
  
  if ((f=fopen(extfile,A_ReadOnly,0,&ul))!=-1)
  {
    size_cfg=lseek(f,0,S_END,&ul,&ul);
    lseek(f,0,S_SET,&ul,&ul);
    olds=s=malloc(size_cfg+1);
    if (s)
    {
      s[fread(f,s,size_cfg,&ul)]=0;
      i=0;
      do
      {
      L_NEXT:
        p=0;
#ifdef NEWSGOLD
        smallicons[i][0]=1033;
        largeicons[i][0]=1105;
#else
        smallicons[i][0]=0x1F9;
        largeicons[i][0]=0x1FA;
#endif
        reg.icon1=smallicons[i];
        reg.icon2=largeicons[i];
        //Заполняем расширение
        if (*s==';')
        {
          //Камент ;)
          while((c=*s++)>=32);
          if (!c) goto L_EOF;
        }
        while((c=*s++)!=':')
        {
          if (!c) goto L_EOF;
          if (c<32) goto L_NEXT;
          if (c=='/')
          {
            smallicons[i][0]=strtol(s,&s,10);
            c=*s++;
            if (!c) goto L_EOF;
            if (c<32) goto L_NEXT;
            if (c=='/')
            {
              largeicons[i][0]=strtol(s,&s,10);
            }
            else s--;
            if (*s==':') s++;
            break;
          }
          if (p<9) exts[i][p++]=c;
        }
        exts[i][p]=0;
        p=0;
        while((c=*s++)>=32)
        {
          if (p<127) elfs[i][p++]=c;
        }
        elfs[i][p]=0;
        reg.ext=exts[i];
        reg.unical_id=0x56+i;
        LockSched();
        RegExplorerExt(&reg);
        UnlockSched();
        i++;
	if (strcmp(reg.ext,"txt")==0)
	{
	  if (i<MAX_EXTS)
	  {
#ifdef NEWSGOLD
	    smallicons[i][0]=1033;
	    largeicons[i][0]=1105;
#else
	    smallicons[i][0]=0x1F9;
	    largeicons[i][0]=0x1FA;
#endif
	    reg.icon1=smallicons[i];
	    reg.icon2=largeicons[i];
	    reg.unical_id=0;
	    exts[i][0]=0;
	    reg.ext=exts[i];
	    strcpy(elfs[i],elfs[i-1]);
	    LockSched();
	    RegExplorerExt(&reg);
	    UnlockSched();
	    i++;
	  }
	}
        if (!c) break; //EOF
      }
      while(i<MAX_EXTS);
    L_EOF:
      fclose(f,&ul);
    }
    mfree(olds);
  }
  /*  LockSched();
  CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
  memcpy(&icsmd,icsm->constr,sizeof(icsmd));
  old_icsm_onClose=icsmd.onClose;
  icsmd.onClose=MyIDLECSM_onClose;
  icsm->constr=&icsmd;
  //  ShowMSG(1,(int)"ExtD installed!");
  UnlockSched();*/
  return 0;
}

