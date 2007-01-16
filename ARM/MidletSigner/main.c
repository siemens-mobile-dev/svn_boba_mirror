
#include "..\inc\swilib.h"

#include "objects.h"
#include "zipstruct.h"

extern void InitConfig();
extern const char WORKFOLDER[];
extern const char DER_FILE[];
extern const char KEY_FILE[];

extern const char FOLDER_ICON[];
extern const char JAR_ICON[];

extern const int FREAD;
extern const int FWRITE;
extern const int INTERNET;
extern const int COMM;
extern const int BLUETOOTH;
extern const int SMS;
extern const int MMS;
extern const int MEDIA;

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  void *next;
  WSHDR *fullname;
  WSHDR *filename;
  int is_dir;
}FLIST;

int filelist_menu_id;
volatile int request_remake_filelist;
volatile int request_close_filelist;
int options_menu_id;
volatile FLIST *fltop;
WSHDR* ews;
WSHDR* jar_file;
int S_ICONS[3];

#ifdef NEWSGOLD
  const char folder[129]="0:\\Applications\\";  
#else
  const char folder[129]="0:\\Java\\jam\\";    
#endif
  
const int minus11=-11;
unsigned short maincsm_name_body[140];
extern void kill_data(void *p, void (*func_p)(void *));
extern char * base64_encode(const void *data, int *size);

const char midlet_cert[]="MIDlet-Certificate-1-1:";
const char midlet_jar_rsa[]="MIDlet-Jar-RSA-SHA1:";
const char midlet_permissions[]="MIDlet-Permissions:";
const char eol[]="\r\n";

#pragma inline
void patch_input(INPUTDIA_DESC* inp,int x,int y,int x2,int y2)
{
  inp->rc.x=x;
  inp->rc.y=y;
  inp->rc.x2=x2;
  inp->rc.y2=y2;
}

#pragma inline
void patch_header(HEADER_DESC* head,int x,int y,int x2,int y2)
{
  head->rc.x=x;
  head->rc.y=y;
  head->rc.x2=x2;
  head->rc.y2=y2;
}
void ed1_locret(void){}

void* zcalloc(int unk,size_t nelem, size_t elsize)
{
  return (malloc(nelem*elsize));
}

void zcfree(int unk, void* ptr)
{
  mfree(ptr);
}


#define DONE 0
#define CANT_OPEN_JAD 1
#define BAD_JAR 2
#define FILE_ALREADY_SIGNED 3
#define CANT_READ_CERT 4
#define CANT_CREATE_HASH 5
#define CANT_SIGN_HASH 6
#define CANT_APPEND 7
#define JAD_ALREDY_EXISTS 8
#define CANT_OPEN_JAR 9
#define CANT_FIND_MANIFEST 10
#define CANT_CREATE_JAD 11
#define BAD_KEY_FILE 12
#define SIZE_OF_JAD_NULL 13
#define FILE_WAS_NOT_SIGNED 14

const char* errors[15]=
{  
  "Done!",
  "Can't open *.jad!",
  "Bad *.jar!",
  "*.jad already signed!",
  "Can't read cerificate!",
  "Can't create SHA1 hash!",
  "Can't sign hash!",
  "Can't open *.jad for append!",
  "*.jad file already exists!",
  "Can't open *.jar!",
  "Can't find MANIFEST.MF!",
  "Can't create *.jad!",
  "Bad *.key file!",
  "Size of jad file is NULL!",
  "File was not signed!"
};

void ErrMsg(int num)
{
  ShowMSG(1,(int)errors[num]);
}

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

int get_file_size(char* fname)
{
  int f;
  unsigned int err;
  FSTATS fs;
  if ((f=GetFileStats(fname,&fs,&err))==-1)
    return f;
  return (fs.size);
} 

char * ReadCertificate(int* size)
{
  char fname[256];
  int f;
  unsigned int err;
  unsigned int fsize;
  char* cert;
  sprintf(fname,"2:\\Certificate store\\%s",DER_FILE);
  if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&err))==-1)
  {
    sprintf(fname,"%s%s",WORKFOLDER,DER_FILE);
    if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&err))==-1)
      return 0;
  }
  *size=fsize=lseek(f,0,S_END,&err,&err);
  if (!fsize)
    return 0;
  lseek(f,0,S_SET,&err,&err);
  cert=malloc(fsize);
  fread(f,cert,fsize,&err);
  fclose(f,&err);
  return (cert); 
}
  
int CreateSHA1Hash(const char* file,char * hash)
{
  SHA_CTX ctx;
  int f;
  unsigned int err;
  unsigned int fsize;
  char* jar_data;
  if ((f=fopen(file,A_ReadOnly+A_BIN,P_READ,&err))==-1)
    return (-1);
  fsize=lseek(f,0,S_END,&err,&err);
  if (!fsize)
    return (-2);
  lseek(f,0,S_SET,&err,&err);
  jar_data=malloc(fsize+1);
  fread(f,jar_data,fsize,&err);
  jar_data[fsize]=0;
  fclose(f,&err);
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, jar_data, fsize);
  SHA1_Final(hash,&ctx);
  mfree(jar_data);
  return (0);
}
  
    
#pragma optimize=z 9
BIGNUM *BN_bin2bn_mod(int f,unsigned int len,BIGNUM *ret)
{
  char*s=malloc(len);
  unsigned int err;
  fread(f,s,len,&err);
  unsigned int ch;
  unsigned int k=len/2;
  for (unsigned int i=0; i<k; i++)
  {
    ch=s[i];
    s[i]=s[len-i-1];
    s[len-i-1]=ch;
  }
  mfree(s);
  return (BN_bin2bn(s,len,ret));
}



int SignHash(char * hash,char*sign)
{
  int n;
  int f;
  unsigned int err;
  unsigned int rsa_sha1_len=1024;
  char buf[128];
  sprintf(buf,"%s%s",WORKFOLDER,KEY_FILE);
  if (get_file_size(buf)!=596)
    return(-1);

  if ((f=fopen(buf,A_ReadOnly+A_BIN,P_READ,&err))==-1)
    return(-1);
  
  RSA *rsa;
  
  if ((rsa=RSA_new_method(0))==NULL)
    return (-2);
  
  lseek(f,0x10,S_SET,&err,&err);
  
  if (!(rsa->e=BN_bin2bn_mod(f,4,rsa->e)))
    goto L_BN_ERROR;

  if (!(rsa->n=BN_bin2bn_mod(f,1024/8,rsa->n)))
    goto L_BN_ERROR;
  
  if (!(rsa->p=BN_bin2bn_mod(f,1024/16,rsa->p)))
    goto L_BN_ERROR;
  
  if (!(rsa->q=BN_bin2bn_mod(f,1024/16,rsa->q)))
    goto L_BN_ERROR;
  
  if (!(rsa->dmp1=BN_bin2bn_mod(f,1024/16,rsa->dmp1)))
    goto L_BN_ERROR;
  
  if (!(rsa->dmq1=BN_bin2bn_mod(f,1024/16,rsa->dmq1)))
    goto L_BN_ERROR;
  
  if (!(rsa->iqmp=BN_bin2bn_mod(f,1024/16,rsa->iqmp)))
    goto L_BN_ERROR;
  
  if (!(rsa->d=BN_bin2bn_mod(f,1024/8,rsa->d)))
    goto L_BN_ERROR;
       
 
  fclose(f,&err);
  zeromem(sign,128);
  rsa->meth->flags=RSA_FLAG_SIGN_VER;
  rsa->flags=RSA_FLAG_CACHE_PRIVATE;
  n=RSA_sign(NID_sha1, hash, 20, sign, &rsa_sha1_len, rsa);
  RSA_free(rsa);
  
  if (n<=0)
    return (-4);
  return (0);
  
L_BN_ERROR:
  RSA_free(rsa);
  return(-3);
  
}
  

  
int is_file_signed(char*jad, int size)
{
  int f;
  unsigned int err;
  char* buf;
  
  if ((f=fopen(jad,A_ReadOnly+A_BIN,P_READ,&err))==-1)
    return 1;
  buf=malloc(size+1);
  buf[size]=0;
  fread(f,buf,size,&err);
  fclose(f,&err);
  
  if (strstr(buf,midlet_cert))
    goto L_SIGNED;
  
  if (strstr(buf,midlet_jar_rsa))
    goto L_SIGNED;
  
  if (strstr(buf,midlet_permissions))
    goto L_SIGNED;
  
  mfree(buf);
  return (0);
  
L_SIGNED:
  mfree(buf);
  return (1);
}
  
void sign_jad()
{
  char jar[256];
  char * base64cert;
  char * base64hash;
  char *cert;
  char sha1jar[20];
  char rsa_sha1[128];
  char jad_name[256];
  char* signature;
  char*p;
  int size;
  int err_n;
  
  ws_2str(jar_file,jar,255);
  if (!(p=strstr(jar,".jar")))
  {
    err_n=BAD_JAR;
    goto L_ERR;
  }
  strcpy(jad_name,jar);
  strcpy(jad_name+(p-jar),".jad");
  
  if ((size=get_file_size(jad_name))==-1)
  {
    err_n=CANT_OPEN_JAD;
    goto L_ERR;
  }
  
  if (is_file_signed(jad_name,size))
  {
    err_n=FILE_ALREADY_SIGNED;
    goto L_ERR;
  }
    
  
  if (!(cert=ReadCertificate(&size)))
  {
    err_n=CANT_READ_CERT;
    goto L_ERR;
  }
  
  base64cert=base64_encode(cert,&size);
  mfree(cert);
  if(!base64cert)
  {
    err_n=CANT_READ_CERT;
    goto L_ERR;
  }
  
  if (CreateSHA1Hash(jar,sha1jar))
  {
    mfree(base64cert);
    err_n=CANT_CREATE_HASH;
    goto L_ERR;
  }
  
  if (SignHash(sha1jar,rsa_sha1))
  {
    err_n=CANT_SIGN_HASH;
    goto L_ERR;
  }
    
  size=128;
  base64hash=base64_encode(rsa_sha1,&size);
  
  signature=malloc(0x2000);
  sprintf(signature,"%s %s\r\n%s %s\r\n%s %s",midlet_cert,base64cert,midlet_jar_rsa,base64hash,midlet_permissions,"javax.microedition.midlet.MIDlet.platformRequest");
  
  if (FREAD>0)
  {
    strcat(signature,",javax.microedition.io.Connector.file.read");
    if (FREAD==1)
      strcat(signature,",com.siemens.mp.io.File.read");
  }
  
  if (FWRITE>0)
  {
    strcat(signature,",javax.microedition.io.Connector.file.write");
    if (FWRITE==1)
      strcat(signature,",com.siemens.mp.io.File.readwrite");
  }
  
  if (INTERNET==1)
  {
    strcat(signature,",javax.microedition.io.Connector.http");
    strcat(signature,",javax.microedition.io.Connector.https");
    strcat(signature,",javax.microedition.io.Connector.ssl");
    strcat(signature,",javax.microedition.io.Connector.socket");   
    strcat(signature,",javax.microedition.io.Connector.datagram");
    strcat(signature,",javax.microedition.io.Connector.serversocket");
    strcat(signature,",javax.microedition.io.Connector.datagramreceiver");
  }
  if (COMM==1)
    strcat(signature,",javax.microedition.io.Connector.comm");
  
  if (BLUETOOTH==1)
  {
    strcat(signature,",javax.microedition.io.Connector.bluetooth.client");
    strcat(signature,",javax.microedition.io.Connector.bluetooth.server");    
  }
  
  if (SMS==1)
  {
    strcat(signature,",javax.microedition.io.Connector.sms");
    strcat(signature,",javax.microedition.io.Connector.sms.send");   
    strcat(signature,",javax.microedition.io.Connector.sms.receive");
    strcat(signature,",javax.wireless.messaging.sms.receive");
    strcat(signature,",javax.wireless.messaging.sms.send");
  }
  
  if (MMS==1)
  {
    strcat(signature,",javax.microedition.io.Connector.mms");   
    strcat(signature,",javax.microedition.io.Connector.mms");
    strcat(signature,",javax.wireless.messaging.mms.receive");
    strcat(signature,",javax.wireless.messaging.mms.send"); 
  }
  
  if (MEDIA==1)
    strcat(signature,",javax.microedition.media.RecordControl.startRecord");  
  else if (MEDIA==2)
  {
    strcat(signature,",javax.microedition.media.control.RecordControl");  
    strcat(signature,",javax.microedition.media.control.VideoControl.getSnapshot");  
  }
  strcat(signature,eol);
  
  int f;
  unsigned int err;
  
  if ((f=fopen(jad_name,A_WriteOnly+A_BIN+A_Append,P_WRITE,&err))==-1)
  {
    err_n=CANT_APPEND;
    goto L_ERR_FREE;
  }
  fwrite(f,signature,strlen(signature),&err);
  fclose(f,&err);
  err_n=DONE;
L_ERR_FREE:  
  mfree(signature);
  mfree(base64cert);
  mfree(base64hash);
  
L_ERR:  
  GeneralFuncF1(1);
  ErrMsg(err_n);
}

int unzip(char *compr, unsigned int comprLen, char *uncompr, unsigned int uncomprLen)
{
    int err;
    z_stream d_stream;
    d_stream.zalloc = (alloc_func)zcalloc;
    d_stream.zfree = (free_func)zcfree;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = (Byte*)compr;
    d_stream.avail_in = (uInt)comprLen;
    err = inflateInit2(&d_stream,-MAX_WBITS);
     if(err!=Z_OK){ 
   unerr:
      return err;
    }
    d_stream.next_out = (Byte*)uncompr;
    d_stream.avail_out = (uInt)uncomprLen;
    err = inflate(&d_stream, 2);
    if(err<0) goto unerr;
    err = inflateEnd(&d_stream);
    if(err<0) goto unerr;
    return 0;
}

char* find_manifest_mf(const char* jar, unsigned int fsize)
{
  int f;
  unsigned int err;
  char* buf;
  CENTRAL_ZIP* c_zip;
  LOCAL_ZIP* l_zip;
  EXTRA_FIELD* ex_field;
  char* fname;
  
  unsigned int compr_size,uncompr_size,offset;
  char* compr_data;
  char* uncompr_data;
  
  if((f=fopen(jar,A_ReadOnly+A_BIN,P_READ,&err))==-1)
    return (0);
  buf=malloc(fsize+1);
  fread(f,buf,fsize,&err);
  fclose(f,&err);
  for(int n=0; n<fsize-3;n++)
  {
    if (buf[n]==0x50 && buf[n+1]==0x4b && buf[n+2]==0x01 && buf[n+3]==0x02)
    {
      c_zip=(CENTRAL_ZIP*)(buf+n);
      int len=c_zip->fname_len;
      fname=malloc(len+1);
      strncpy(fname,buf+n+sizeof(CENTRAL_ZIP),len);
      fname[len]=0;
      compr_data=strstr(fname,"MANIFEST.MF");
      mfree(fname);
      if (compr_data)
      {
        compr_size=c_zip->compr_size;
        uncompr_size=c_zip->uncompr_size;
        offset=c_zip->relative_offset;
        l_zip=(LOCAL_ZIP*)(buf+offset);
        offset+=sizeof(LOCAL_ZIP)+l_zip->fname_len;
        int ex_len=l_zip->extra_field_len;
        if(ex_len)
        {
          ex_field=(EXTRA_FIELD*)(buf+offset);
          offset+=ex_field->data_size+ex_len+2;
        }
        compr_data=malloc(compr_size);
        memcpy(compr_data,buf+offset,compr_size);
        mfree(buf);
        uncompr_data=malloc(uncompr_size+1);
        uncompr_data[uncompr_size]=0;
        unzip(compr_data,compr_size,uncompr_data,uncompr_size);
        mfree(compr_data);
        return (uncompr_data);
      }     
    }
  }
  mfree(buf);
  return (0);
}

void generate_jad()
{
  char jar_f[256];
  char jad_f[256];
  int f; 
  int fsize;
  char* p;
  char* manifest;
  int err_n;
  char* jad;
  
  unsigned int err;
  ws_2str(jar_file,jar_f,255);
  
  if ((p=strstr(jar_f,".jar"))==NULL)
  {
    err_n=BAD_JAR;
    goto L_ERR;
  }
  strcpy(jad_f,jar_f);
  strcpy(jad_f+(p-jar_f),".jad");
  
  if ((f=fopen(jad_f,A_ReadOnly+A_BIN,P_READ,&err))!=-1)
  {
    fclose(f,&err);
    err_n=JAD_ALREDY_EXISTS;
    goto L_ERR;
  }
  
  if ((fsize=get_file_size(jar_f))==-1)
  {
    err_n=CANT_OPEN_JAR;
    goto L_ERR;
  }  
    
  if (!(manifest=find_manifest_mf(jar_f,fsize)))
  {
    err_n=CANT_FIND_MANIFEST;
    goto L_ERR;
  }
  
  jad=malloc(strlen(manifest)+512);
  sprintf(jad,"MIDlet-Jar-Size: %u\r\nMIDlet-Jar-URL: %s\r\n",fsize,jar_f);
  strcat(jad,manifest);
  mfree(manifest);
  
  if ((f=fopen(jad_f,A_ReadWrite+A_Create+A_Truncate+A_BIN,P_READ+P_WRITE,&err))==-1)
  {
    mfree(jad);
    err_n=CANT_CREATE_JAD;
    goto L_ERR;
  }
  
  fwrite(f,jad,strlen(jad),&err);
  fclose(f,&err);
  mfree(jad);
  err_n=DONE;
L_ERR:
  GeneralFuncF1(1);
  ErrMsg(err_n);
}

void remove_sign_from_jad()
{
  int f;
  unsigned int err;
  char* p;
  char* first;
  char* end;
  char fname[256];
  unsigned int fsize,fs;
  char* buf;
  ws_2str(jar_file,fname,255);
  int err_n;
  
  if ((p=strstr(fname,".jar"))==NULL)
  {
    err_n=BAD_JAR;
    goto L_ERR;
  }
  strcpy(p,".jad");
  if ((f=fopen(fname,A_ReadWrite+A_BIN,P_READ,&err))==-1)
  {
    err_n=CANT_OPEN_JAD;
    goto L_ERR;
  }
  fs=fsize=lseek(f,0,S_END,&err,&err);
  if (!fsize)
  {
    err_n=SIZE_OF_JAD_NULL;
    goto L_ERR;
  }
  lseek(f,0,S_SET,&err,&err);
  buf=malloc(fsize+1);
  buf[fsize]=0;
  fread(f,buf,fsize,&err);
  
  if ((first=strstr(buf,midlet_cert)))
  {
    end=strstr(first,eol)+2;
    memcpy(first,end,buf+fsize-end+1);
    fsize-=end-first;
  }
  
  if ((first=strstr(buf,midlet_jar_rsa)))
  {
    end=strstr(first,eol)+2;
    memcpy(first,end,buf+fsize-end+1);
    fsize-=end-first;
  }
    
  if ((first=strstr(buf,midlet_permissions)))
  {
    end=strstr(first,eol)+2;
    memcpy(first,end,buf+fsize-end+1);
    fsize-=end-first;
  } 
  fclose(f,&err);
  
  if(fs==fsize)
  {
    mfree(buf);
    err_n=FILE_WAS_NOT_SIGNED;
    goto L_ERR;
  }
  
  f=fopen(fname,A_ReadWrite+A_Truncate+A_BIN,P_READ+P_WRITE,&err);
  fwrite(f,buf,fsize,&err);
  fclose(f,&err); 
  mfree(buf);
  err_n=DONE;
L_ERR:
  GeneralFuncF1(1);
  ErrMsg(err_n);
}

void options_back()
{
  GeneralFuncF1(1);
}

int options_softkeys[]={0,1,2};
SOFTKEY_DESC options_menu_sk[]=
{
  {0x0018,0x0000,(int)""},
  {0x0001,0x0000,(int)"Select"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB options_menu_skt=
{
  options_menu_sk,0
};

HEADER_DESC options_menuhdr={0,0,0,0,NULL,(int)"Опции",LGP_NULL};

MENUITEM_DESC options_menu_ITEMS[4]=
{
  {NULL,(int)"Sign",               LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Generate jad",       LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Remove signature",   LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
  {NULL,(int)"Back",               LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
};

void *options_menu_HNDLS[4]=
{
  (void *)sign_jad,
  (void *)generate_jad,
  (void *)remove_sign_from_jad,
  (void *)options_back,
};

MENU_DESC options_menu_STRUCT=
{
  0,(void *)NULL,(void*)NULL,NULL,
  options_softkeys,
  &options_menu_skt,
  8,
  NULL,
  options_menu_ITEMS,   //Items
  options_menu_HNDLS,   //Procs
  4 //n
};

int CreateOptionsMenu()
{
  char jar_f[256];
  ws_2str(jar_file,jar_f,255);
  if (get_file_size(jar_f)==-1)
  {
    ErrMsg(BAD_JAR);
    return (0);
  }
  options_menuhdr.rc.x=3;
  options_menuhdr.rc.y=0x18;
  options_menuhdr.rc.x2=ScreenW()-6;
  options_menuhdr.rc.y2=0x18+0x13;
  
  return (options_menu_id=CreateMenu(1,0,&options_menu_STRUCT,&options_menuhdr,0,4,0,0)); 
}


void Free_FLIST(void)
{
  FLIST*fl=(FLIST*)fltop;
  FLIST*fl_prev;
  while(fl)
  {
    FreeWS(fl->fullname);
    FreeWS(fl->filename);
    fl_prev=fl->next;
    mfree(fl);
    fl=fl_prev;
  }
  fltop=0;
}


FLIST* Fill_FLIST(FLIST*fl,char* full_name, char* file_name,int is_dir)
{
  FLIST*ftop=(FLIST*)fltop;
  int len;
  WSHDR *ws; 
  
  if (ftop)
    fl=fl->next=malloc(sizeof(FLIST));
  else 
    fltop=fl=malloc(sizeof(FLIST));
  len=strlen(file_name);
  ws=AllocWS(len);
  str_2ws(ws,file_name,len);
  fl->filename=ws;
  
  len=strlen(full_name);
  ws=AllocWS(len);
  str_2ws(ws,full_name,len);
  fl->fullname=ws;  
  
  fl->next=0;
  fl->is_dir=is_dir;
  return fl;
}
  
  
int GetFoldersLevel(const char * fname)
{
  int i=0;
  while(*fname)
  {
    if (*fname++=='\\') i++;
  }
  return i;
}
    
       
void FindFiles(const char *path)
{
  FLIST * fl=(FLIST *)fltop;
  DIR_ENTRY de;
  unsigned int err;
  char name[256];
  char * ext;
  strcpy(name,path);
  strcat(name,"*");
  if (FindFirstFile(&de,name,&err))
  {
    do
    {
      strcpy(name,de.folder_name);
      strcat(name,"\\");
      strcat(name,de.file_name);
      if (de.file_attr&FA_DIRECTORY)
      {
        strcat(name,"\\");
        fl=Fill_FLIST(fl,name,de.file_name,1);
      }
      else
      {
        if((ext=strrchr(name,'.')))
        {
          if(!strncmp(ext+1,"jar",3))
            fl=Fill_FLIST(fl,name,de.file_name,0);
        }
      }
    }
    while(FindNextFile(&de,&err));
  }
  FindClose(&de,&err);
  
  int len;
  WSHDR *ws; 
  if (GetFoldersLevel(path)>=2)  // Добавляем переход на уровень выше
  {
    fl=malloc(sizeof(FLIST));
    fl->next=(FLIST*)fltop;
    ws=AllocWS(2);
    str_2ws(ws,"..",2);
    fl->filename=ws;
    
    strcpy(name,path);
    len=strlen(name);
    len--;
    while(name[len-1]!='\\')
      len--;
    name[len]=0;
    ws=AllocWS(len);
    str_2ws(ws,name,len);
    fl->fullname=ws;  
    fl->is_dir=1;
    fltop=fl;
  }
}


int menusoftkeys[]={0,1,2};
SOFTKEY_DESC menu_sk[]=
{
  {0x0018,0x0000,(int)"Select"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)"+"}
};

SOFTKEYSTAB menu_skt=
{
  menu_sk,0
};


void filelist_menu_iconhndl(void *data, int curitem, int *unk);
int filelist_menu_onkey(void *data, GUI_MSG *msg);
void filelist_menu_ghook(void *data, int cmd);

char header[25];
HEADER_DESC filelist_HDR={0,0,NULL,NULL,NULL,(int)header,LGP_NULL};


MENU_DESC filelist_STRUCT=
{
  8,(void *)filelist_menu_onkey,(void*)filelist_menu_ghook,NULL,
  menusoftkeys,
  &menu_skt,
  0x11,
  (void *)filelist_menu_iconhndl,
  NULL,   //Items
  NULL,   //Procs
  0   //n
};


FLIST *FindFLISTtByN(int n)
{
  FLIST *fl=(FLIST *)fltop;
  int i=0;
  while(fl&&(i!=n))
  {
    fl=fl->next;
    i++;
  }
  return fl;
}

#pragma inline
int GetIconIndex(FLIST *t)
{
  return t->is_dir;
}

void create_menu_folder(void)
{
  Free_FLIST();
  int i=0;
  FLIST* fl;
  FindFiles(folder);
  fl=(FLIST*)fltop;
  while(fl)
  {
    fl=fl->next;
    i++;
  }
  if (!i) return; //Нечего создавать
  filelist_HDR.rc.x2=ScreenW();
  filelist_HDR.rc.y2=HeaderH();
  strncpy(header,folder,24);
  filelist_menu_id=CreateMenu(0,0,&filelist_STRUCT,&filelist_HDR,0,i,0,0);
}
void remake_filelist();
void create_view(int n)
{ 
  FLIST* fl;
  fl=FindFLISTtByN(n);
  if (fl->is_dir)
  {
    char fname[256];
    ws_2str(fl->fullname,fname,wstrlen(fl->fullname));
    strcpy((char*)folder,fname);
    remake_filelist();
  }
  else
  {
    wstrcpy(jar_file,fl->fullname);
    GeneralFunc_flag1(filelist_menu_id,1);
  }
}
  
void filelist_menu_iconhndl(void *data, int curitem, int *unk)
{
  FLIST *t;
  WSHDR *ws;
  void *item=AllocMenuItem(data);
  
  t=FindFLISTtByN(curitem);
  if (t)
  {
    ws=AllocMenuWS(data,wstrlen(t->filename));
    wstrcpy(ws,t->filename);
  }
  else
  {
    ws=AllocMenuWS(data,10);
    wsprintf(ws,"error");
  }
  SetMenuItemIconArray(data,item,S_ICONS);
  SetMenuItemText(data,item,ws,curitem);
  SetMenuItemIcon(data,curitem,GetIconIndex(t));
}

int filelist_menu_onkey(void *data, GUI_MSG *msg)
{
  int keycode=msg->keys;
  int i=GetCurMenuItem(data);
  if (keycode==0x3D||keycode==0x18)
  {
    create_view(i);
    return(-1);
  } 
  return(0);
}

void filelist_menu_ghook(void *data, int cmd)
{
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}


void remake_filelist(void)
{
  if (filelist_menu_id)
  {
    request_remake_filelist=1;
    if (IsGuiOnTop(filelist_menu_id))
    {
      GeneralFunc_flag1(filelist_menu_id,1);
    }
    else
    {
      request_close_filelist=1;
    }
  }
  else
  {
    create_menu_folder();
  }
}

SOFTKEY_DESC edit_sk[]=
{
  {0x0018,0x0000,(int)"Options"},
  {0x0001,0x0000,(int)"Close"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB edit_skt=
{
  edit_sk,0
};  


void ed1_ghook(GUI *data, int cmd)
{
  EDITCONTROL ec;
  int i;
  if (cmd==7)
  {
    SetSoftKey(data,&edit_sk[0],0);
    SetSoftKey(data,&edit_sk[1],1);
    i=EDIT_GetFocus(data);
    ExtractEditControl(data,i,&ec);
    switch(i)
    {
    case 2:
      wstrcpy(ews,jar_file);
      EDIT_SetTextToFocused(data,ews);  
    }
  }
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}


int ed1_onkey(GUI *data, GUI_MSG *msg)
{
  int i;
  int l;
  EDITCONTROL ec;
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    l=msg->gbsmsg->submess;
    switch(l)
    {
    case ENTER_BUTTON:
      i=EDIT_GetFocus(data);
      ExtractEditControl(data,i,&ec);
      switch(i)
      {
      case 2:
        create_menu_folder();
        return (-1);
      }
      
    case LEFT_SOFT:
      CreateOptionsMenu();
      return(-1);
    }
  }
  return(0); //Do standart keys
  //1: close
}


  
HEADER_DESC ed1_hdr={0,0,0,0,NULL,(int)"Midlet Sign",LGP_NULL};

INPUTDIA_DESC ed1_desc=
{
  1,
  ed1_onkey,
  ed1_ghook,
  (void *)ed1_locret,
  0,
  &edit_skt,
  {0,0,0,0},
  4,
  100,
  101,
  0,
  0,
#ifdef NEWSGOLD
  0
#else
  0x40000000
#endif
};

int CreateMainMenu(void)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;
  int scr_w,head_h;
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());

  wsprintf(ews,"%t:","Jar file");
  ConstructEditControl(&ec,1,0x40,ews,256);
  AddEditControlToEditQend(eq,&ec,ma);
  
  wsprintf(jar_file,folder);
  ConstructEditControl(&ec,9,0x40,jar_file,256);
  AddEditControlToEditQend(eq,&ec,ma); 
 
  scr_w=ScreenW();
  head_h=HeaderH();
  patch_header(&ed1_hdr,0,0,scr_w-1,head_h);
  patch_input(&ed1_desc,0,head_h+1,scr_w-1,ScreenH()-SoftkeyH()-1);
  return CreateInputTextDialog(&ed1_desc,&ed1_hdr,eq,1,0);
}



void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  ews=AllocWS(256);
  jar_file=AllocWS(256);
  csm->gui_id=CreateMainMenu();
}

void maincsm_onclose(CSM_RAM *csm)
{
  Free_FLIST();
  FreeWS(ews);
  FreeWS(jar_file);
  SUBPROC((void *)ElfKiller);
}


int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
    if ((int)msg->data0==filelist_menu_id)
    {
      filelist_menu_id=0;
      if (request_remake_filelist)
      {
        request_remake_filelist=0;
        create_menu_folder();
      }
    }    
  }
  return(1);
}


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
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"MidletSigner");
}

int main()
{
  InitConfig();
  S_ICONS[0]=(int)JAR_ICON;
  S_ICONS[1]=(int)FOLDER_ICON;
  S_ICONS[2]=0;
  char dummy[sizeof(MAIN_CSM)];
  UpdateCSMname();
  LockSched();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
}
