#include "..\include\Lib_Clara.h"
#include "..\include\Dir.h"
#include "header\books.h"
#include "header\mybook.h"
#include "header\main.h"
#include "header\config_data.h"
#include "header\conf_loader.h"
#include "..\\include\cfg_items.h"
#include "header\cfg.h"
#include "header\iofunc.h"
#include "header\untar.h"
#include "header\xml.h"

extern wchar_t myelfpath[512];
extern RECT rect;

int NewKey(int key, int r1 , int mode);
int redraw_init();

void elf_exit()
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
};

void Init()
{ 
  if (csofts_fill(GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG), L"AdvSoftkeys.cfg")<0)
  {
    csofts_create_empty(GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG), L"AdvSoftkeys.cfg");
    csofts_fill(GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG), L"AdvSoftkeys.cfg");
  }
  wchar_t *extConfig=GetDir(MEM_EXTERNAL+DIR_ELFS_CONFIG);
  wchar_t *folder=new wchar_t[wstrlen(extConfig)+1+wstrlen(L"AdvSoftkeys/")+1];
  wstrcpy(folder,extConfig);
  wstrcat(folder,L"/");
  wstrcat(folder,L"AdvSoftkeys/");
  if (!cmpfile(L"/tpa/system/settings/",L"current_theme.thm", folder, L"curthm.thm"))
  {
    rmdir(folder, 0);
    mkdir(folder);
    copyfile(L"/tpa/system/settings/",L"current_theme.thm", folder, L"curthm.thm");
    tar_uncompress(folder, L"curthm.thm", folder);
  }
  skin_execute_xml(folder, L"Theme.xml");
  delete(folder);   
  InvalidateRect(DispObject_SoftKeys_Get(),&rect);
};

int isSoftkeysBook_ByName(BOOK *bk);

int main(wchar_t *elfpath)
{
  if (FindBook(isSoftkeysBook_ByName))
  {
    MessageBox(0x6FFFFFFF, Str2ID(L"Already runned",0,SID_ANY_LEN), 0, 1, 5000, 0);
    SUBPROC(elf_exit);
    return 0;
  }
  wstrcpy(myelfpath,elfpath);
  if (MyBook_Create())
  {
    Init();
    ModifyKeyHook(NewKey,1);
    redraw_init();
  }
  else
  {
    SUBPROC(elf_exit);
  }
  return 0;
};
