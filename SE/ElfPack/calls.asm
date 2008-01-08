//#include "..\\include\DevDef.h"
	RTMODEL "ARMv4M", "USED"
        RTMODEL "ARMv4T", "USED"
        RTMODEL "ARMv5T", "USED"
        RTMODEL "StackAlign4", "USED"
        RTMODEL "__cpu_mode", "__pcs__interwork"
        RTMODEL "__data_model", "absolute"
        RTMODEL "__dlib_file_descriptor", "0"
        RTMODEL "__endian", "little"
        RTMODEL "__rt_version", "6"

        EXTERN  Library

        RSEG CODE:CODE:NOROOT(2)
	


call    MACRO   a,b
        PUBLIC  a
a:
	CODE32
        LDR     R12,=Library-0x400
        LDR     R12,[R12,#b*4]
        BX      R12
        ENDM

defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM

        call    ListElement_Prt2NumElement,0x17B
        call    AllocDirHandle,0x11D
        call    GetFname,0x11E
        call    memset,0x112
        call    memcpy,0x113
	call    List_New,0x169
        call    ListElement_Find,0x1AC
        call    ListElement_GetByIndex,0x1AD
        call    ListElement_AddtoTop,0x16B
        call    ListElement_Remove,0x179
        call    wstrrchr,0x1AE

        call    _fopen,0x116
        call    BookObj_CallSubroutine,0x1AF
        call    wstrlen,0x157
        call    wstrcpy,0x152
        call    wstrcat,0x154
        call    BookObj_ReturnPage,0x12E
        call    debug_printf,0x1B1
        call    wstr2str,0x15B
	
	call	int2strID,0x15C
	call 	StatusIndication_ShowNotes,0x143


#ifdef K750_R1CA021
        defadr  memalloc,0x44F7B500+1
        defadr  memfree,0x44F7C61C+1
#endif	

#ifdef W700_R1CA021
        defadr  memalloc,0x44F89D7C+1
        defadr  memfree,0x44F8AE98+1
#endif	

#ifdef W810_R4EA031
        defadr  memalloc,0x45453AA8+1
        defadr  memfree,0x454547E0+1
#endif	

#ifdef W810_R4DB005
        defadr  memalloc,0x454499A4+1
        defadr  memfree,0x4544A6DC+1
#endif	

#ifdef Z550_R6CA009
        defadr  memalloc,0x45403710+1
        defadr  memfree,0x45404440+1
#endif	

#ifdef W800_R1BC002
        defadr  memalloc,0x44F814DC+1
        defadr  memfree,0x44F825F8+1
#endif	

#ifdef W800_R1BD001
        defadr  memalloc,0x44F8145C+1
        defadr  memfree,0x44F82578+1
#endif	

#ifdef K600_R2BB001
        defadr  memalloc,0x20363C0C+1
        defadr  memfree,0x20364D24+1
#endif	

   END
