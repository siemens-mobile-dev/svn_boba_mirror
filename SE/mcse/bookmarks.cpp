#include "inc\mc.h"
#include "inc\mui.h"

const wchar_t mcbm_path[] = L"Bookmarks";
const wchar_t mcbm_ext[] = L"mcbm";


void UseBM(wchar_t* filename)
{
  int f;
  if ((f=w_fopen(filename, WA_Read, 0x1FF, NULL)) >=0)
  {
    if (w_fread(f, pathbuf, MAX_PATH*sizeof(wchar_t)))
    {
      if (curtab >= MAX_TABS)
        curtab = back_tab < MAX_TABS ? back_tab : 0;
        
      if (isdir(pathbuf))
        cd(curtab, pathbuf);
      else
        //if (wsbuf)
        {
          //str_2ws(wsbuf,pathbuf,MAX_PATH);
          //ExecuteFile(wsbuf,0,0);
        }
    }
    w_fclose(f);
  }
}
