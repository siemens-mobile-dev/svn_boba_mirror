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
OldOnClose:
	DCD	0xA17143F0+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA171422C+1
        
       
        
        RSEG	PATCH_GET_PIT:CODE:ROOT
	CODE16
        LDR      R1, =Patch_PIT
        BX       R1
        
        EXTERN	PatchGetPIT
        RSEG	CODE:CODE:ROOT  
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
        
OldPit: MOV     R1, #0x600
        ADD     R1, R1, #0x40
        ADD     R0, R0, R1
        LDR     R0, [R0,#0xC]
        LDRH    R0, [R0,#0x14]
        BX      LR
        
     defadr  png_set_gray_to_rgb,0xA15EDB40+1

        
	END
	