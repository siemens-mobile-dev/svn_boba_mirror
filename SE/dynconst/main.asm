#define UNDEFINED 0xFFFFFFFF
#include "include.tmp"
DYNCONST MACRO   val,tag
        org tag*4
        DCD val
        ENDM

#include "..\include\DYN_tag.h"
#include "..\include\DYN_mac.asm"

  END
