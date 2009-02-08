#include "inc\mc.h"

void fn_zero(FN_LIST *list)
{
  memset(list, 0, sizeof(FN_LIST));
}

void fn_free(FN_LIST *list)
{
  if (list==NULL) return;
  if (list->count)
  {
    while(list->items)
    {
      FN_ITM *itm = list->items;
      list->items = (FN_ITM *)itm->next;
      
      if (itm->full) delete(itm->full);
      if (itm->zipPath) delete(itm->zipPath);
      delete(itm);
      list->count--;
    }
  }
}

int fn_inlist(FN_LIST *list, wchar_t *full)
{
  if (list==NULL || !list->count) return 0;
  
  FN_ITM *itm = list->items;
  while(itm)
  {
    if (!wstrcmp(itm->full, full)) return 1;  
    itm=(FN_ITM *)itm->next;
  }
  return 0;
}

void fn_add(FN_LIST* list, int type, int ftype, int pname, wchar_t* full, wchar_t* zipPath)
{
  if (list==NULL) return;
  
  if (type!=list->type)
  {
    if (list->count) fn_free(list);
    list->type = type;
  }
  
  if (!fn_inlist(list, full))
  {
    FN_ITM *itm = new FN_ITM;
    
    itm->ftype = ftype;
    itm->pname = pname;
    itm->full = new wchar_t[wstrlen(full)+1];
    wstrcpy(itm->full, full);
    if (zipPath != NULL)
    {
      itm->zipPath = new wchar_t[wstrlen(zipPath)+1];
      wstrcpy(itm->zipPath, zipPath);
    }
    else
    {
      itm->zipPath = NULL;
    }
    itm->next  = list->items;
    list->items = itm;
    list->count++;
  }
}
