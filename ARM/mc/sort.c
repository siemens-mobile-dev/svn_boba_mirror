#include "..\inc\swilib.h"
#include "mc.h"

FILEINF * _MovMin(FILEINF * list, int sort)
{
  FILEINF *cur = list;
  FILEINF *prev, *minprev;
  FILEINF *min = list;
  char* szmin;
  char* szcur;

  int srt=sort & STV_MASK;
  int neadstrbuf=(srt==ST_NAME || srt==ST_EXT);
  if(neadstrbuf)
  {
    szmin=malloc(MAX_PATH);
    szcur=malloc(MAX_PATH);
    strtolower(min->sname, szmin, -1);
  }  
  while(cur)
  {
    if(neadstrbuf)
     strtolower(cur->sname, szcur, -1);
    if(min!=cur)
    {
     int st=0;
     if(srt==ST_NAME)st=strcmp(szmin, szcur);else
     if(srt==ST_EXT) st=strcmp(GetFileExt(szmin), GetFileExt(szcur));else
     if(srt==ST_SIZE)st=(min->size)>(cur->size)?-1:+1;
     if(srt==ST_DATE)st=(min->time)>(cur->time)?-1:+1;
   
     int ds=((st<0) && !(sort & ST_REVERS)) || 
            ((st>0) &&  (sort & ST_REVERS));
     if(ds)
     {
       min = cur;
       if(neadstrbuf)
         strtolower(min->sname, szmin, -1);
       minprev = prev;
     }
    } 

    prev=cur;
    cur=cur->next;
  }
  if(neadstrbuf)
  {
   mfree(szcur);
   mfree(szmin);
  } 

  cur=list;
  if (min!=cur)
  {
    minprev->next=min->next;
    min->next=cur;
    cur=min;
  }
  return cur;
}

FILEINF * SortList(FILEINF * list, int sort)
{
  FILEINF *base=list;
  FILEINF *cur=list;
  FILEINF *prev=0;
  while(cur)
  {
    cur = _MovMin(cur, sort);
    if(prev) 
      prev->next = cur;
    else
      base = cur;
    
    prev=cur;
    cur=cur->next;
  }
  return base;
}

void SortFiles(int tab)
{
 if(tabs[tab]->ccFiles>1) 
 {
  LockSched();
  FILEINF *files=0;
  FILEINF *dirs=0;
  
  FILEINF *cur = FileListBase[tab]->next;
  FILEINF *next;
  while(cur!=FileListBase[tab])
  {
    next = cur->next;
    if(cur->attr & FA_DIRECTORY)
    {
      cur->next=dirs;
      dirs=cur;
    }
    else
    {
      cur->next=files;
      files=cur;
    }
    cur = next;
  }
  FileListBase[tab]->next=FileListBase[tab];
  
  int srt = tabs[tab]->sort;
  
  dirs  = SortList(dirs,  srt & STD_MASK);
  files = SortList(files, srt & STD_MASK);
  if(srt & STV_MASK)
  {
   if((srt & STV_MASK)!=ST_SIZE)
    dirs  = SortList(dirs,  srt);
   files = SortList(files, srt);
  }
  
  while(files)
  {
    next = files->next;
    files->next = FileListBase[tab]->next;
    FileListBase[tab]->next=files;
    files=next;
  }
  while(dirs)
  {
    next = dirs->next;
    dirs->next = FileListBase[tab]->next;
    FileListBase[tab]->next=dirs;
    dirs=next;
  }
  
  UnlockSched();
 } 
}
