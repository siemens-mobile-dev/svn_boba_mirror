#ifndef _PARSE_OMS_H_
#define _PARSE_OMS_H_

#include "view.h"

void OMS_DataArrived(VIEWDATA *vd, const char *buf, int len);

enum IOMS_PARSESTATE
{
  OMS_BEGIN=0,
  OMS_HDR_COMMON,
  OMS_HDR,
  OMS_GZIPHDR,
  OMS_PAGEURL_SIZE,
  OMS_PAGEURL_DATA,
  OMS_TAG_NAME,
  OMS_TAG_DATA,
  OMS_TAGA_STAGE3,
  OMS_TAGT_STAGE3,
  OMS_TAGI_STAGE3,
  OMS_TAGX_STAGE3,
  OMS_TAGh_STAGE3,
  OMS_TAGh_STAGE4,
  OMS_TAGe_STAGE3,
  OMS_TAGe_STAGE4,
  OMS_TAGp_STAGE3,
  OMS_TAGp_STAGE4,
  OMS_TAGu_STAGE3,
  OMS_TAGu_STAGE4,
  OMS_TAGx_STAGE3,
  OMS_TAGx_STAGE4,
  OMS_TAGc_STAGE3,
  OMS_TAGc_STAGE4,
  OMS_TAGr_STAGE3,
  OMS_TAGr_STAGE4,
  OMS_TAGs_STAGE3,
  OMS_TAGo_STAGE3,
  OMS_TAGo_STAGE4,
  OMS_TAGi_STAGE3,
  OMS_TAGi_STAGE4,
  OMS_TAGk_STAGE3,
  OMS_TAGL_STAGE3,
  OMS_TAGP_STAGE3,
  OMS_TAGZ_STAGE3,
  OMS_TAGx5E_STAGE3,
  OMS_TAGx40_STAGE3,
  OMS_STOP
};

#endif /* _PARSE_OMS_H_ */
