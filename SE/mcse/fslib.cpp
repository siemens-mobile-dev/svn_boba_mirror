#include "inc\mc.h"
#include "inc\mui.h"


//File Name
wchar_t* GetFileExt(wchar_t* fname)
{
  wchar_t *s1, *s2;
  s1=wstrrchr(fname,'.');
  if (s1)
  {
    int i=0;
    s2=++s1;       
    while(*s2++) i++;
    if (i>MAX_EXT) s1=0;
  }
  return (s1);
}

wchar_t* GetFileName(wchar_t* fname)
{
  int len = wstrlen(fname);
  int ii;
  for(ii = len-1; ii >= 0; ii--) {
    if (fname[ii] == '\\' || fname[ii] == '/') break;
  }
  if (ii>=0) return fname+ii+1;
  return fname;
}


wchar_t* GetFileDir(wchar_t* fname, wchar_t* buf)
{
  int c;
  wchar_t *s=fname;
  int len=0;
  int i=0;
  while((c=*s++))
  {
    if ((c=='\\' || c=='/') && *s!=0)  len=i;
    i++;
  }
  if (buf)
  {
    if (len)
    {
      wstrncpy(buf,fname,len);      
    }
    buf[len]=0;    
  }
  return (buf);
}

int fexists(wchar_t* fname)
{
  W_FSTAT fs;
  return (w_fstat(fname,&fs)!=-1);
}

int fsrm(wchar_t* path, int ip)
{
  int err;
  int res;
  if (isdir(path))
    res;// = rmtree(path, ip);
  else
  {
    wchar_t *r;
    wchar_t *w=wstrrchr(path,'/');
    if (w)
    {
      r=w+1;
      *w=0;
      FileDelete(path, r, &err);
    }
    if (ip) incprogr(1);
  }
  return res;
}

#define BUF_SIZE 0x4000
int fcopy(wchar_t* src, wchar_t* dst)
{
  int fi=-1, fo=-1;
  char* buff=0;
  int cb, left;
  int res = 0;
  //int attr=0;
  
 // if (CONFIG_CONFIRM_REPLACE && fexists(dst))
 // {
 //   if (MsgBoxYesNoWithParam(ind_pmt_exists, 0) != IDYES)
 //     return 1; // Все хорошо, если не стали перезаписывать
 // }
  fi = w_fopen(src, WA_Read, 0x1FF, 0);
  if (fi >=0) 
  {
    fo = w_fopen(dst, WA_Read+WA_Write+WA_Create+WA_Truncate, 0x1FF, 0);
    if (fo >=0) 
    {
      left = w_lseek(fi, 0, WSEEK_END);
      w_lseek(fi, 0, WSEEK_SET);
      if (left)
      {
        buff = new char[BUF_SIZE];
        if (!buff) goto L_EXIT;
        progrsp_max = left;
        incprogrsp(0);
      }
      while (left) 
      {
        cb = left < BUF_SIZE ? left : BUF_SIZE;
        left -= cb;
        incprogrsp(cb);
        if (w_fread(fi, buff, cb) != cb) goto L_EXIT;
        if (w_fwrite(fo, buff, cb) != cb) goto L_EXIT;
      }
      endprogrsp();
      res = 1;
      WriteLog("fcopy. OK");
    }
  }
L_EXIT:
  if (buff) delete(buff);
  if (fo != -1) w_fclose(fo);
  if (fi !=- 1) w_fclose(fi);
  return res;
}


int EnumFiles(wchar_t* dname, ENUM_FILES_PROC enumproc, unsigned int param)
{
  // Рекурсивно пробегаем и по подкаталогам тоже
  //return EnumFilesInDir( dname, enumproc, param, 1, 1);
  return 0;
}

int GetFilesCnt(wchar_t* path)
{
  int res=1;
  if (isdir(path))
  {
    int sub = EnumFiles(path, NULL, 0);
    res += (sub>>16) + (sub & 0xffff);
  }
  return res;
}

typedef struct
{
	int number;
	wchar_t cleanName[MAX_PATH];
	wchar_t* cleanExt;
} ParseFileNameStruct;

void parse_name(wchar_t* fname, ParseFileNameStruct* pfns)
{
  pfns->number = 0;
  wstrcpy(pfns->cleanName, fname);
  int fname_len = wstrlen(fname);
  wchar_t* ps = GetFileExt(pfns->cleanName); // Указатель на расширение
  
  if (ps != 0)
  {
    pfns->cleanExt = ps;
    *(--ps) = '\0';	// Разделяем имя и расширение
  }
  else
  {
    // Расширения нет вовсе
    pfns->cleanExt = (wchar_t *)str_empty;
    ps = pfns->cleanName + fname_len; // Ссылаемся на конец имени
  }
  if (ps - pfns->cleanName >= 3) // Должно быть 3 символа как минимум - скобки и число
  {
    if (*--ps == ')')  // Закрывающая скобка есть
    {
      int i = 0;
      int p10 = 1;
      while (--ps >= fname && *ps >= '0' && *ps <= '9' && p10 < 100000)
      {
        i += (*ps - '0') * p10;
        p10 *= 10;
      }
      if (ps >= fname && *ps == '(' && i != 0)
      {
        pfns->number = i;
        *ps = '\0'; // обрубаем чистое имя
      }
    }
  }
}

int find_next_name(ParseFileNameStruct* pfns, wchar_t *buf)
{
  for (pfns->number++; pfns->number < 100; pfns->number++)
  {
    snwprintf(buf, MAX_PATH-1,_ls_i_ls, pfns->cleanName, pfns->number, pfns->cleanExt);
    if (!fexists(buf))
      return 1; 
  }
  return 0;
}

int fscp(wchar_t* src, wchar_t* dst, int ip)
{
  progrsp_start = 1;
  int res;
  int isSame = (wstrcmpi(src, dst) == 0);
  if (isdir(src))
  {
    //res = cptree(src, dst, ip);
  }
  else
  {
    if (isSame)
    {
      wchar_t buf[MAX_PATH];
      // Генерим новое имя и копируем...
      ParseFileNameStruct fns;
      parse_name(src, &fns);
      // Ищем следующее незанятое имя
      if (find_next_name(&fns,buf))
      {
        res = fcopy(src, buf);
      }
      else res = fcopy(src, dst);
    }
    else res = fcopy(src, dst);
    if (ip) incprogr(1);
  }
  progrsp_start = 0;
  return res;
}

int fsmv(wchar_t* src, wchar_t* dst)
{
  int res = 0;
  /*
  if (src[0] != dst[0])
  {
    if (wstrcmpi(src, dst) != 0) // Только для неодинаковых файлов
      res = fmove(src, dst, &err);
    incprogr(GetFilesCnt(dst));
  }
  else*/
    if (fscp(src, dst, 1)) 
      res = fsrm(src, 0);
  return res;
}

