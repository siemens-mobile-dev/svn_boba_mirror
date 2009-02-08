#include "inc\exts.h"
#include "inc\mc.h"

wchar_t GetIcon(wchar_t *name, int fattr)
{
  wchar_t icon;
  unsigned short d;
  if (fattr&FA_DIRECTORY) return folder_icn;
  SUB_EXECUTE *data;
  FILEITEM *fi=FILEITEM_Create();
  FILEITEM_SetFname(fi,name);
  data=DataBrowser_CreateSubExecute(-1, fi);
  DataBrowser_ExecuteSubroutine(data,0x3B,&d);
  DataBrowser_ExecuteSubroutine(data,0x2D,0);
  FILEITEM_Destroy(fi);
  icon=d;
  return icon;
}

