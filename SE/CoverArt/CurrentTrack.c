#include "..\include\Lib_Clara.h"
#include "..\include\dir.h"
#include "structs.h"

void TrackDesc_Free(TRACK_DESC *track_desc)
{
  if (track_desc)
  {
    if (track_desc->path)
    {
      delete(track_desc->path);
    }
    if (track_desc->name)
    {
      delete(track_desc->name);
    }
    delete(track_desc);
  }
};

bool TrackDesc_Compare(TRACK_DESC *t1, TRACK_DESC *t2)
{
  if (t1 && t2)
  {
    if (wstrcmp(t1->path,t2->path)==0 && wstrcmp(t1->name,t2->name)==0 && t1->fulltime==t2->fulltime)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
};

TRACK_DESC * TrackDesc_Get(BOOK *bk)
{
  if (bk)
  {
    int platform=0;
    switch (GetChipID())
    {
      case 0x7100:
      case 0x8000:
      case 0x8040:
        platform = 2010;
        break;
      case 0x9900:
        platform = 2020;
        break;
    }
    if (platform==2010)
    {
      BOOK_PLAYER_2010 *bkp=(BOOK_PLAYER_2010*)bk;
      TRACK_DESC *track_desc=new TRACK_DESC;
      wchar_t *path=bkp->dsc->path;
      wchar_t *name=bkp->dsc->name;
      track_desc->path=new wchar_t[wstrlen(path)+1];
      wstrcpy(track_desc->path,path);
      track_desc->name=new wchar_t[wstrlen(name)+1];
      wstrcpy(track_desc->name,name);
      track_desc->fulltime=bkp->dsc->fulltime;
      return track_desc;
    }
    else if (platform==2020)
    {
      BOOK_PLAYER_2020 *bkp=(BOOK_PLAYER_2020*)bk;
      TRACK_DESC *track_desc=new TRACK_DESC;
      wchar_t *path=bkp->dsc->path;
      wchar_t *name=bkp->dsc->name;
      track_desc->path=new wchar_t[wstrlen(path)+1];
      wstrcpy(track_desc->path,path);
      track_desc->name=new wchar_t[wstrlen(name)+1];
      wstrcpy(track_desc->name,name);
      track_desc->fulltime=bkp->dsc->fulltime;
      return track_desc;
    }
  }
  return 0;
};

