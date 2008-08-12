
#include "..\\include\Target.h"
#include "..\\include\Types.h"

#define NULL    0

#ifndef SWI_HOOK__

#ifndef DB2020
//DB2000/2010
  extern __thumb void *memalloc(int size,int f1,int f2,const char *fname,int fline);
  extern __thumb void memfree(void * mem,const char * fname ,int fline);
#else
//DB2020
  extern __thumb void *memalloc(int zero, int size,int f1,int f2,const char *fname,int fline);
  extern __thumb void memfree(int zero, void * mem,const char * fname ,int fline);
#endif

extern __thumb void memset(void *mem,char chr,int size);
extern __arm memcpy(void *dest,const void *source,int cnt);
extern __thumb int _fopen(const unsigned short *filpath, const unsigned short *filname,unsigned int mode , unsigned int rights, unsigned int __0);
extern __thumb u16* wstrrchr(const u16* wstr,u16 wchar);
extern __thumb int wstrlen(u16 * wstr);
extern __thumb u16* wstrcpy(u16 * dest, u16 * source);
extern __thumb u16* wstrcat(u16 * wstr, u16 * subwstr);
extern __thumb int ListElement_Find(LIST *lst,void *element, int (*cmp_proc)(void *,void *));
extern __thumb void * ListElement_Remove(LIST *lst,int index);
extern __thumb void * ListElement_GetByIndex(LIST *lst,int num_element);
extern __thumb void ListElement_AddtoTop(LIST *lst,void *newElement);
extern __thumb int ListElement_Prt2NumElement(LIST *lst,void *ptr);
extern __thumb LIST *List_New(void);
extern __thumb void * AllocDirHandle(u16 * path);
extern __thumb void *GetFname(void *, void*);
extern __thumb void  DestroyDirHandle(void *handle);

#endif