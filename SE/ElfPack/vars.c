#include "..\include\types.h"

__no_init int (*pKBD) (int,int,int) @ "OLD_KEY_HANDLER";
__no_init EP_DATA * elfpackdata @ "EPD";
