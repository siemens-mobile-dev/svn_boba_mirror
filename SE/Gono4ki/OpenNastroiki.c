#include "..\\include\Lib_Clara_DLL.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"
#include "..\\include\cfg_items.h"
#include "MyFUNC.h"
#include "StructBook.h"

void SaveSettings(SOUND*s, IMGN*i, TAB*t)
{
  int f=_fopen(GetConfigPath(), L"Settings.gms",0x2,0x180,0);
  fwrite(f, s, sizeof(SOUND));
  fwrite(f, i, sizeof(IMGN));
  fclose(f);
  f=_fopen(GetConfigPath(), L"Resultats.tab",0x2,0x180,0);
  fwrite(f, t->te, sizeof(TABL));
  fwrite(f, t->tn, sizeof(TABL));
  fwrite(f, t->th, sizeof(TABL));
  fclose(f);
}

void OpenSettings(MyBook*bk)
{
  int f;
  if(!FindFile(GetConfigPath(), L"Settings.gms"))
  {
    f=_fopen(GetConfigPath(), L"Settings.gms",0x1,0x180,0);
    fread(f, bk->nast->sound, sizeof(SOUND));
    fread(f, bk->nast->im, sizeof(IMGN));
    fclose(f);
  }
  else
  {    
    f=_fopen(GetConfigPath(), L"Settings.gms",0x2,0x180,0);
    fwrite(f, bk->nast->sound, sizeof(SOUND));
    fwrite(f, bk->nast->im, sizeof(IMGN));
    fclose(f);
  }
  if(!FindFile(GetConfigPath(), L"Resultats.tab"))
  {
    f=_fopen(GetConfigPath(), L"Resultats.tab",0x1,0x180,0);
    fread(f, bk->nast->rez->te, sizeof(TABL));
    fread(f, bk->nast->rez->tn, sizeof(TABL));
    fread(f, bk->nast->rez->th, sizeof(TABL));
    fclose(f);    
  }else{
    f=_fopen(GetConfigPath(), L"Resultats.tab",0x2,0x180,0);
    fwrite(f, bk->nast->rez->te, sizeof(TABL));
    fwrite(f, bk->nast->rez->tn, sizeof(TABL));
    fwrite(f, bk->nast->rez->th, sizeof(TABL));
    fclose(f);
  }
}
