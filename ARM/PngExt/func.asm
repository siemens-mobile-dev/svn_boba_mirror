

defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM
        
        
	RSEG	CODE:CODE
	PUBLIC	kill_data
	CODE32
kill_data
	BX	R1      
        
        
        RSEG	CODE:CODE

	PUBLIC	OldOnClose
        PUBLIC	OldOnCreate
       
#ifdef CX70
OldOnClose:
	DCD	0xA0FD8698
OldOnCreate:
	DCD	0xA0FD8754
#endif       
#ifdef SL65
OldOnClose:
	DCD	0xA0FD8698
OldOnCreate:
	DCD	0xA0FD8754        
#endif
#ifdef M75
OldOnClose:
	DCD	0xA0FD8698
OldOnCreate:
	DCD	0xA0FD8754        
#endif


        
        
        RSEG	PATCH_GET_PIT:CODE
	CODE16
        LDR      R1, =Patch_PIT
        BX       R1
        
        EXTERN	PatchGetPIT
        RSEG	CODE:CODE 
        CODE32
Patch_PIT:

        STMFD   SP!,{R0,LR}
        MOV     R0, R4
        BL      PatchGetPIT
        CMP     R0, #0
        LDMEQFD SP!,{R0,LR}
        BEQ     OldPit
        ADD     SP,SP,#8
        LDMFD   SP!,{R4,PC}
#ifndef M75        
OldPit: ADD     R0, R0, #0x640
        LDR     R0, [R0,#0xC]
        LDRH    R0, [R0,#0x14]
        BX      LR
#else
OldPit: ADD     R0, R0, #0x640
        LDR     R0, [R0,#0x30]
        LDRH    R0, [R0,#0x14]
        BX      LR
#endif

        

        
	END
	