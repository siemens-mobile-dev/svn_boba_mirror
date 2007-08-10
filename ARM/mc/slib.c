#include "..\inc\swilib.h"
#include "mc.h"
#include "mui.h"
#include "gui.h"

const char _d[]="%d";
const char _s_stars[]="%s\\*.*";
const char _s_s[]="%s\\%s";
const char _s_s_d[]="%s\\%s.%d";
const char _ss[]="%s: %s";
const char _t[]="%t";
const char _tc[]="%t: ";
const char _d_t[]="%d %t";

char *strtolower(char *src, char *dst, int sz)
{
 int sl = strlen(src)+1;
 int len = sz==-1?sl:sz;
 if(len>sl)len=sl;
   
 for(int ii=0;ii<len-1;ii++)
 {
   int ch = src[ii];
   
   if(ch>='A' && ch<='Z') ch=ch-'A'+'a';
   
   dst[ii]=ch;
 }
 dst[len-1]=0;
 return dst;
}

short wcharlow(short wc)
{
  if(wc>=0x0041 && wc<=0x005a) return wc+(0x0061-0x0041); //A-Z
  if(wc>=0x0410 && wc<=0x042f) return wc+(0x0430-0x0410); //А-Я
  if(wc>=0x0400 && wc<=0x040f) return wc+(0x0450-0x0400);

  if(wc>=0x00c0 && wc<=0x00de && wc!=0x00d7) return wc+(0x00e0-0x00c0);

  if(wc>=0x0100 && wc<=0x0136 && ~(wc & 1)) return wc+1;
  if(wc>=0x0139 && wc<=0x0147 &&  (wc & 1)) return wc+1;
  if(wc>=0x014a && wc<=0x0176 && ~(wc & 1)) return wc+1;
  if(wc>=0x0179 && wc<=0x017d &&  (wc & 1)) return wc+1;
  if(wc>=0x01e4 && wc<=0x01ea && ~(wc & 1)) return wc+1;

  if(wc>=0x01fa && wc<=0x01fe && ~(wc & 1)) return wc+1;
  
  if(wc>=0x0490 && wc<=0x04e8 && ~(wc & 1)) return wc+1;

  if(wc==0x01a0 || wc==0x01af || wc==0x017d || 
     wc==0x01ee || wc==0x0218 || wc==0x021a || wc==0x0228) return wc+1;
     
  if(wc==0x0178) return 0x00ff;
  if(wc==0x018f) return 0x0259;
  if(wc==0x01b7) return 0x0292;
  return wc;
}

int wstrcmpi(WSHDR* ws1, WSHDR* ws2)
{
  int len = ws1->wsbody[0];
  if(len<ws2->wsbody[0]) return -1; else
  if(len>ws2->wsbody[0]) return +1;
  
  short wc1, wc2;
  for(int cc=1;cc<=len;cc++)
  {
    wc1 = wcharlow(ws1->wsbody[cc]);
    wc2 = wcharlow(ws2->wsbody[cc]);
    if(wc1<wc2) return -1; else
    if(wc1>wc2) return +1;
  }
  return 0;
}

char *sz2s(unsigned int size, char *buf)
{
  float s = size;
  char pref[16];
  strcpy(pref, muitxt(ind_byte));
  if(s>=1024)
  {
    s=s/1024;
    strcpy(pref, muitxt(ind_kbyte));
    if(s>=1024)
    {
      s=s/1024;
      strcpy(pref, muitxt(ind_mbyte));
    }
    sprintf(buf, "%.2f %s", s, pref);
  }
  else
   sprintf(buf, "%d %s", size, pref);
  return buf;
}

char *attr2s(int attr, char *buf)
{
  buf[0]=attr & FA_ARCHIVE?'a':'-';
  buf[1]=attr & FA_READONLY?'r':'-';
  buf[2]=attr & FA_HIDDEN?'h':'-';
  buf[3]=attr & FA_SYSTEM?'s':'-';
  buf[4]=attr & FA_DIRECTORY?'d':'-';
  buf[5]=0;
  return buf;
}

char *fdt2s(unsigned int time, char *buf)
{
  short y,M,d,h,m;//,s;
  y=(time>>25) + 80;
  if(y>100)y-=100;
  M=(time>>21) & 0x0f;
  d=(time>>16) & 0x1f;
  
  h=(time>>11) & 0x1f;
  m=(time>>5)  & 0x3f;
//  s=(time&0x1f)<1;
  
  sprintf(buf, "%.2d.%.2d.%.2d %.2d:%.2d", d,M,y,h,m);
  return buf;
}

int getLVC(unsigned short *wsbody, int len)
{
  int width=0;
  for(int ii=0;ii<len;ii++)
  {
    width+=GetSymbolWidth(wsbody[ii], FONT_SMALL);
    if(width>=MAX_TXT_W) return ii;
  }
  return 0;
}
void cutname(WSHDR *fname, WSHDR *sname, int len)
{
  sname->wsbody[0]=len;
  for(int ii=1;ii<len+1;ii++)
  {
    sname->wsbody[ii]=(ii>=len-2)?'.':fname->wsbody[ii];
  }
//  sname->wsbody[len+1]=0;
}

int EnumIni(int local, char *ininame, INIPROC proc)
{
  int res=0;
  char *buf;
  char *cfg;
  int ch;
  int p=0;
  char* name=malloc(MAX_INIBUF);
  char* value=malloc(MAX_INIBUF);
  
  unsigned int size_cfg;
  
  char* fn = local?MCFilePath(ininame):ininame;
  int f;
  if (name && value && fn && (f=fopen(pathbuf,A_ReadOnly,0,&err))!=-1)
  {
    size_cfg=lseek(f,0,S_END,&err,&err);
    lseek(f,0,S_SET,&err,&err);
    cfg=buf=malloc(size_cfg+1);
    if (cfg)
    {
      buf[fread(f,buf,size_cfg,&err)]=0;
      do
      {
      L_NEXT:
        //Камент
        if (*buf==';')
        {
          while((ch=*buf++)>=32);
          if (!ch) goto L_EOF;
        }
        p=0;
        while((ch=*buf++)!='=')
        {
          if (!ch) goto L_EOF;
          if (ch<32) goto L_NEXT;
          if (p<MAX_INIBUF-1) name[p++]=ch;
        }
        name[p]=0;
        p=0;
        while((ch=*buf++)>=32)
        {
          if (p<MAX_INIBUF-1) value[p++]=ch;
        }
        value[p]=0;

        if(proc)
        proc(name, value);        
        
        if (!ch) break; //EOF
      }
      while (1);
     }
    L_EOF:
     mfree(cfg);
     fclose(f,&err);
     res=1;
  }
  if(name)mfree(name);
  if(value)mfree(value);
  return res;
}

void nricp(char* src, char* dst)
{
  *(dst+0)=*(src+0);
  *(dst+1)=*(src+1);
  *(dst+2)=*(src+2);
  *(dst+3)=*(src+3);
}
