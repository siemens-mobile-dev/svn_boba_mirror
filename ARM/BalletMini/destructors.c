#include "../inc/swilib.h"
#include "view.h"
#include "string_works.h"
#include "destructors.h"

void FreeREFCACHEentry(REFCACHE *p)
{
  freegstr(&p->form_id1);
  freegstr(&p->form_id2);
  freegstr(&p->id);
  freegstr(&p->value);
  freegstr(&p->id2);
}

void FreeREFCACHEtotal(REFCACHE **t)
{
  REFCACHE *p=*t;
  REFCACHE *next;
  while(p)
  {
    FreeREFCACHEentry(p);
    next=p->next;
    mfree(p);
    p=next;
  }
  *t=NULL;
}

void FreeRawText(VIEWDATA *vd)
{
  mfree(vd->rawtext);
  vd->rawtext=NULL;
  vd->rawtext_size=0;
}

void FreeDynImgList(VIEWDATA *vd)
{
  OMS_DYNPNGLIST *dpl=vd->dynpng_list;
  vd->dynpng_list=NULL;
  while(dpl)
  {
    OMS_DYNPNGLIST *p=dpl;
    dpl=dpl->dp.next;
    if (p->dp.img)
    {
      mfree(p->dp.img->bitmap);
      mfree(p->dp.img);
    }
    mfree(p);
  }
}

void FreeViewData(VIEWDATA *vd)
{
  if (vd->zs)
  {
    inflateEnd(vd->zs);
    mfree(vd->zs);
  }
  if (vd->ws) FreeWS(vd->ws);
  FreeREFCACHEtotal(&vd->ref_cache);
  FreeREFCACHEentry(&vd->work_ref);
  mfree(vd->lines_cache);
  mfree(vd->rawtext);
  mfree(vd->oms);
//  mfree(vd->I_cache);
  mfree(vd->S_cache);
  FreeDynImgList(vd); 
  mfree(vd);
}
