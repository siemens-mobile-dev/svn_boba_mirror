#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"

void CopyNumFilesSet(EXPL_DATA far* expl_data)
{
 int i, n=expl_data->total_folders+expl_data->total_files;
 CopyNumFiles=CopyFileSz=0;
 for (i=0; i<n; i++) {
  if (expl_data->filelist[i].marked==0) continue;
  CopyNumFiles++;
 }
 if (CopyNumFiles) return;
 CopyNumFiles=1;
}

int My_Rename(char far* source, char far* dest)
{
 char far* buff;
 int fs, fd, sz;
 if (CopyNumFiles==0) return(Rename(source,dest));
 if (!strcmp(source,dest)) return(0);
 fs=FileOpen(source,_O_RDONLY,0);
 if (fs==-1) return(0);//Error
 fd=FileOpen(dest,_O_CREAT+_O_WRONLY+_O_TRUNC,_S_IREAD);
 if (fd==-1) { FileClose(fs); return(0); }//Error
 buff=EX_heap_malloc_with_lock(0x2000);
 if (buff==0) { FileClose(fs); FileClose(fd); return(0); }//Error
 do {
  sz=FileRead(fs,buff,0x2000); //8kb buff
  FileWrite(fd,buff,sz);
  CopyCopiedSz+=sz;
 } while (sz!=0);
 FileClose(fs);
 FileClose(fd);
 EX_heap_free_with_lock(buff);
 CopyNumFiles--; //+one file copied
 return(1); //Success
}


static const char far extfile[]="A:\\zbin\\execute.ext";

int search_and_open(STR fname)
{
 char s[256];
 int f;
 unsigned int i;
 unsigned int mi;
 unsigned char c;
 unsigned int sm=0;
 char far *execname=0;

 i=strlen(fname);
 if (!i) return -1;
 if (i>127) return -1;
 fname=fname+strlen(fname); //Указатель на посл. байт
 while(fname[-1]!='.') {
  i--;
  if (i==0) {
   fname="";
   break;
  }
  fname--;
 }
 //Теперь fname указывает на расширение
 if ((f=FileOpen((STR)extfile,_O_RDONLY,0))!=-1) {
  i=FileRead(f,s,128);
  mi=128;
  if (i<128) s[i]=0;
   else {
 L1:
   i=FileRead(f,s+128,128);
   if (i<128) {
    (s+128)[i]=0;
    mi=256;
   }
  }
  i=0;
  for(;;) {
   if (i==mi) {
    memcpy(s,s+128,128);
    goto L1;
   }
   c=s[i++];
   if (!c) break; //Конец файла
   switch(sm) {
        case 0xFFFF:
         //Ждем окончания строки
         if (c==':') {
          execname=s+i; //Последний файл - универсальный
          sm--; //Только первый вход
         }
    case 0xFFFE:
         if ((c==0x0D)||(c==0x0A)) sm=0; break;
    default:
         if ((c==0x0D)||(c==0x0A)) {
          //Неожиданный конец строки
          sm=0;
          break;
         }
         if ((c==':')&&(!fname[sm])) {
          //Полное совпадение
          execname=s+i;
          goto L_EOF;
         }
         if (toupper(c)==toupper(fname[sm])) sm++; else sm=0xFFFF;
         break;
   }
  }

  if (!execname) {
   DisplayMessageBoxV(1,lng_nothingrun);
   return -1; //Нет сопоставлений
  }
  L_EOF:
  FileClose(f);
  i=0;
  while(execname[i]>31) i++;
  execname[i]=0;
  f=FileOpen(execname,_O_RDONLY,0);
  if (f==-1) {
   DisplayMessageBoxV(1,lng_nobinfile);
  }
  return(f);
 } else {
  DisplayMessageBoxV(1,lng_noextfile);
  return -1;
 }
}

void FTA2_main(char far* fname)
{
 int f;
 if ((f=search_and_open(fname))==-1) return;
 FileRead(f,(void *)0x80000,0x3FFF);
 FileRead(f,(void *)0x84000,0x3FFF);
 FileRead(f,(void *)0x88000,0x3FFF);
 FileRead(f,(void *)0x8C000,0x3FFF);
 FileClose(f);
 _binfile(fname);
 SendMessage(pid_MMI,0x85,0);
}


void CreatePlsFile(WSHDR far* pPlsName)
{
 char s[255];
 char path[128];
 char name[128];
 unsigned int far* wstr=pPlsName->wstr;
 int i,f, n=pEXPL_DATA->total_folders+pEXPL_DATA->total_files;
 for (i=0;i<wstr[0];i++)
  s[i]=wstr[i+1];
 s[wstr[0]]=0;
 sprintf(name,"A:\\MP3\\%s.mpl",s);
 MMIHeapFree(pPlsName);
 f=FileOpen(name,_O_CREAT+_O_WRONLY+_O_TRUNC,_S_IREAD);
 if (f!=-1) {
  for (i=0; i<n; i++) {
   if (pEXPL_DATA->filelist[i].type!=0x20) continue;
   if (pEXPL_DATA->filelist[i].marked==0) continue;
   if (strlen(pEXPL_DATA->path)<=6) path[0]=0;//"a:\mp3"
    else sprintf(path,"%s\\",(STR)&pEXPL_DATA->path[7]);
   sprintf(s,"%s%s\r\n",path,pEXPL_DATA->filelist[i].filename);
   FileWrite(f,s,strlen(s));
  }
  FileClose(f);
 }
 DisplayMessageBoxV(1,0x55A);
 CloseCSM(id_dia);
}

void CreatePlsDiaOnCreate(void)
{
 StartAndCloseDummyDialog();
 CreatePlaylistName(NULL);
}

int  CreatePlsDiaOnKey(CSM_RAM far* data, GBS_MSG far* Msg)
{
 if (!IsFocus(data)) return(1);
 if (Msg->Msg==0xAC) 
  if (Msg->Param[0]==0x17) {
   FilesysICall_FP(CreatePlsFile,*((void far* far*)(Msg->Param+2)));
   PleaseWait(1);
   return(0);
  }
 CloseCSM(id_dia);
 return(0);
}

void CreatePlsDiaOnClose(void){}

static const unsigned int md_tab[]={0xFFF5};
static const CSM_DESC CreatePlsDiaHNDL=
{
 CreatePlsDiaOnKey,
 CreatePlsDiaOnCreate,
 CreatePlsDiaOnClose,
 0x2C,
 sizeof(md_tab)/sizeof(unsigned int),
 md_tab
};


void CardExCreatePls(EXPL_DATA far* expl_data)
{
 char s[129];
 char Buff[0x2C]; 
 strcpy((STR)&s,expl_data->path);
 s[6]=0;
 if (strcmp_nocase((STR)&s,"A:\\MP3")) return;
 pEXPL_DATA=expl_data;
 id_dia=CreateCSM(&CreatePlsDiaHNDL,Buff,0);
}
