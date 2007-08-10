#include "..\inc\swilib.h"
#include "mc.h"

void fn_zero(FN_LIST *list)
{
 zeromem(list, sizeof(FN_LIST));
}

void fn_free(FN_LIST *list)
{
 if(list==NULL) return;
 if(list->count)
 {
  LockSched();
  while(list->items)
  {
    FN_ITM *itm = list->items;
    list->items = itm->next;
    {
     if(itm->full) mfree(itm->full);
     mfree(itm);
    } 
    list->count--;
  }
  UnlockSched();
 }
}

void fn_rev(FN_LIST *list)
{
 if(list==NULL) return;

 if(list->count)
 {
  LockSched();
  FN_ITM *itm = list->items;
  FN_ITM *next = itm->next;
  while(next)
  {
    itm->next=next->next;
    next->next=list->items;
    list->items = next;
    next = itm->next;
  }
  UnlockSched();
 }
}

int fn_inlist(FN_LIST *list, char *full)
{
  if(list==NULL || !list->count) return 0;

  FN_ITM *itm = list->items;
  while(itm)
  {
    if(!strcmp(itm->full, full)) return 1;  
    itm=itm->next;
  }
  return 0;
}

void fn_add(FN_LIST *list, int type, int isfile, int pname, char *full)
{
  if(list==NULL) return;
  
  if(type!=list->type)
  { 
    if(list->count) fn_free(list);
    list->type = type;
  }

  if(!fn_inlist(list, full))
  {
    FN_ITM *itm = malloc(sizeof(FN_ITM));

    itm->isfile = isfile;
    itm->pname  = pname;
    itm->full = malloc(strlen(full)+1);
    strcpy(itm->full, full);

    LockSched();
    itm->next  = list->items;
    list->items = itm;
    list->count++;
    UnlockSched();
  }  
}

int _fn_fill(DIR_ENTRY *de, int param)
{
  if(pathbuf_fn)
  {
    int isfile = de->file_attr & FA_DIRECTORY?0:1;
    FN_LIST *list = (FN_LIST *) param;
    sprintf(pathbuf_fn, _s_s, de->folder_name, de->file_name);
    fn_add(list, FNT_NONE, isfile, 0, pathbuf_fn);
    return 1;
  }  
 return 0;
}
void fn_fill(FN_LIST *list, char *path)
{
  if(list==NULL) return;

  int idir=isdir(path, &err);
  if(idir)
    EnumFiles(path, _fn_fill, (int)list);
  fn_add(list, FNT_NONE, !idir, 0, path);
}

#ifdef LOG
void fn_log(FN_LIST *list)
{
  FN_ITM *itm = list->items;
  while(itm)
  {
    _WriteLog(itm->full);
    itm=itm->next;
  }
}
#endif
