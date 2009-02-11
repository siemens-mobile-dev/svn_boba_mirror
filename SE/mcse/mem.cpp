#ifdef NDEBUG

#include "..\include\Lib_Clara.h"

void* operator new(size_t sz){return malloc(sz);};
void* operator new[](size_t sz){return malloc(sz);};
void operator delete(void * p){mfree(p);};

#else

#include "mem.h"
#include "..\include\dir.h"

static char* leaktypes[]={
  "memory/book/gui",
  "strid",
  "iconid",
  "timer",
  "file"
};

static bool started=false;

static LIST* buffers[trace_typescount];

#define LISTDATACOUNT 3

void *trace_alloc(trace_types mt, void* invalid, void* p,char* file,int line)
{
  if(started && p!=invalid)
  {
    ListElement_Add(buffers[mt],p);
    ListElement_Add(buffers[mt],file);
    ListElement_Add(buffers[mt],(void*)line);
  }
  return p;
}
int trace_alloc_i(trace_types mt, int invalid, int i, char* file, int line)
{
  if(started && i!=invalid)
  {
    ListElement_Add(buffers[mt],(void*)i);
    ListElement_Add(buffers[mt],file);
    ListElement_Add(buffers[mt],(void*)line);
  }
  return i;
}

void *trace_free(trace_types mt,void* p)
{
  if(started)
  {
    for(int i=0;i<buffers[mt]->FirstFree;i+=LISTDATACOUNT)
    {
      if(ListElement_GetByIndex(buffers[mt],i)==p)
      {
        for(int j=0;j<LISTDATACOUNT;j++)
          ListElement_Remove(buffers[mt],i);
        break;
      }
    }
  }
  return p;
}



/*
#ifdef __cplusplus
GUI* trace_free(trace_types mt,GUI* p){ return trace_free(mt, p); }
GUI_LIST* trace_free(trace_types mt,GUI_LIST* p){ return trace_free(mt, p); }
GUI_ONEOFMANY* trace_free(trace_types mt,GUI_ONEOFMANY* p){ return trace_free(mt, p); }
GUI_NOFMANY* trace_free(trace_types mt,GUI_NOFMANY* p){ return trace_free(mt, p); }
GUI_FEEDBACK* trace_free(trace_types mt,GUI_FEEDBACK* p){ return trace_free(mt, p); }
#endif
*/

int trace_free_i(trace_types mt,int p)
{
  if(started)
  {
    for(int i=0;i<buffers[mt]->FirstFree;i+=LISTDATACOUNT)
    {
      if(ListElement_GetByIndex(buffers[mt],i)==(void*)p)
      {
        for(int j=0;j<LISTDATACOUNT;j++)
          ListElement_Remove(buffers[mt],i);
        break;
      }
    }
  }
  return p;
}


void mfree_(void* p)
{
  mfree(p);
}

void *mfree_adr_()
{
  return (void*)mfree_;
}

void trace_init()
{
  for(int i=0;i<trace_typescount;i++)
    buffers[i]=List_New();
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
        w_chdir(GetDir(DIR_OTHER|MEM_INTERNAL));
        f=w_fopen(L"memory.txt",0x4A,0x1FF,0);
      }
      
      char tmp[256];
      
      w_fwrite(f,tmp,sprintf(tmp,"leak type \"%s\"\n",leaktypes[memtype]));
      
      for(int j=0;j<buffers[memtype]->FirstFree;j+=LISTDATACOUNT)
      {
        w_fwrite(f,tmp,
                 sprintf(tmp,"- %s:%d\n",
                         ListElement_GetByIndex(buffers[memtype],j+1),//file
                         ListElement_GetByIndex(buffers[memtype],j+2)//line
                           )
                   );
      }
    }
    List_Free(buffers[memtype]);
  }
  if(f!=-1)
    w_fclose(f);
}

#endif
