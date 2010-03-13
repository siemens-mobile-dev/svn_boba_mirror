#ifdef NDEBUG

#include "..\include\Lib_Clara.h"

void* operator new(size_t sz){return malloc(sz);};
void* operator new[](size_t sz){return malloc(sz);};
void operator delete(void * p){mfree(p);};

#else

#include "mem2.h"
#include "..\include\dir.h"
#include "lib_clara_original.h"


enum trace_types
{
  trace_memory,
  trace_strid,
  trace_iconid,
  trace_timer,
  trace_file,
  trace_hook,

  trace_unallocated,

  trace_typescount
};

static char* leaktypes[]={
  "memory/book/gui",
  "strid",
  "iconid",
  "timer",
  "file",
  "hook",

  "unallocated"
};

static bool started=false;

static LIST* buffers[trace_typescount];

#define LISTDATACOUNT 3


void trace_init()
{
  for(int i=0;i<trace_typescount;i++)
    buffers[i]=__original_List_Create();
  started=true;
}

void trace_done()
{
  started=false;
  int f=-1;
  for(int memtype=0;memtype<trace_typescount;memtype++)
  {
    if(buffers[memtype]->FirstFree)
    {
      if(f==-1)
      {
        __original_w_chdir(GetDir(DIR_OTHER|MEM_INTERNAL));
        f=__original_w_fopen(L"memory.txt",WA_Write+WA_Create+WA_Truncate,0x1FF,0);
      }

      char tmp[256];

      __original_w_fwrite(f,tmp,__original_sprintf(tmp,"leak type \"%s\"\n",leaktypes[memtype]));

      for(int j=0;j<buffers[memtype]->FirstFree;j+=LISTDATACOUNT)
      {
        __original_w_fwrite(f,tmp,
                 sprintf(tmp,"- %s:%d (%x)\n",
                         __original_List_Get(buffers[memtype],j+1),//file
                         __original_List_Get(buffers[memtype],j+2),//line
                         __original_List_Get(buffers[memtype],j)//value
                           )
                   );
      }
    }
    __original_List_Destroy(buffers[memtype]);
  }
  if(f!=-1)
    __original_w_fclose(f);
}

void trace_alloc(int mt, void* ptr,char *file, int line)
{
    __original_List_InsertLast(buffers[mt],ptr);
    __original_List_InsertLast(buffers[mt],file);
    __original_List_InsertLast(buffers[mt],(void*)line);
}

void trace_free(int mt,void* p, char* file, int line)
{
  if(started)
  {
    bool found=false;
    for(int i=0;i<buffers[mt]->FirstFree;i+=LISTDATACOUNT)
    {
      if(__original_List_Get(buffers[mt],i)==p)
      {
        for(int j=0;j<LISTDATACOUNT;j++)
          __original_List_RemoveAt(buffers[mt],i);
        found=true;
        break;
      }
    }
    if(!found)
    {
      __original_List_InsertLast(buffers[trace_unallocated],(void*)p);
      __original_List_InsertLast(buffers[trace_unallocated],file);
      __original_List_InsertLast(buffers[trace_unallocated],(void*)line);
    }
  }
}


bool isallocatedstrid(int strid)
{
  return strid!=0x6fffffff && (strid&0xFFFF0000)!=0;
}

bool isallocatediconid(int strid)
{
//  return strid!=0xffff && (strid&0xFFFF0000)!=0;
  return false;//!!!
}

//---------------------------------------------------------------------------

