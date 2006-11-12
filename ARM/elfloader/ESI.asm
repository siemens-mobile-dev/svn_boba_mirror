        RSEG    CODE:CODE(2)
        PUBLIC  BXR1
BXR1:
        BX      R1

#ifdef NEWSGOLD

        THUMB
        RSEG    PATCH_NO_BROWSE_TXT:CODE:ROOT(1)
        ADDS    R0,R0,#8+0x28

	RSEG	PATCH_ESI1:CODE:ROOT(2)
	ARM
	LDR	PC,pESI
	RSEG	PATCH_ESI2:DATA(2)
pESI
	DCD	callESI

	EXTERN	ESI

	RSEG	CODE:CODE(2)
	ARM
callESI:
	BL	ESI
	MOV	R0, #1
	LDMFD	SP!, {R4-R8,PC}

#else

	RSEG	PATCH_ESI:CODE:ROOT
	CODE16
	LDR	R3,=ESI_PATCH
	BX	R3

	RSEG	CODE:CODE:NOROOT
	CODE32
	EXTERN	ESI
        EXTERN  ESIOld
ESI_PATCH:
	MOV	R3,R0,LSR #+0x1C
	CMP	R3,#0x0A
        CMPNE	R3,#0x0B
	BNE	DO_LGP
	BLX	ESI
	LDMFD	SP!,{R4,PC}
DO_LGP:
        LDR	R3,ESIOld
	MOV	R4,R1
	MOV	R1,R2
	LDR	R2,=0x2710
	CMP	R0,R2
	BX	R3


 	RSEG	PATCH_PROPERTY_WINDOW:CODE:ROOT
	CODE32


        ADD     LR, PC, #4
        LDR     PC, =PATCH_EXT

        RSEG	CODE:CODE:NOROOT
	CODE32
        EXTERN  PropertyPatch

PATCH_EXT:
        STMFD   SP!,{R1-R3, LR}
        LDR     R2, [SP, #0x20+0xC4+20]
        CMP     R2, #0xA0000000
        BLCC    PropertyPatch
        LDMFD   SP!,{R5-R7, PC}

#endif

	END




