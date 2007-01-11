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
	
	THUMB
	RSEG	PATCH_GET_PIT:CODE(1)
//MAINCODE:A0975D34 10 B5                       PUSH    {R4,LR}
//MAINCODE:A0975D36 04 1C                       ADD     R4, R0, #0
//MAINCODE:A0975D38 B1 F0 F2 EC                 BLX     sub_A0A27720
	LDR	R1,JJ_PIT
	BX	R1
	
	RSEG	PATCH_GET_PITJ:DATA(2)
JJ_PIT	DC32	J_PIT

	RSEG	PIT_PNG_EXTENSION:CODE(2)
	EXTERN	PITgetN
	EXTERN	PatchGetPIT
	EXTERN	PITret
	ARM
J_PIT:
	STMFD	SP!,{R4,LR}
	MOV	R4,R0
	BL	PatchGetPIT
        CMP     R0, #0
	LDMNEFD	SP!,{R4,PC}
	MOV	R0,R4
	LDR	R12,PITgetN
	BLX	R12
	CMP	R0,R4
        LDR	R12,PITret
	BX	R12
#else
        THUMB
        RSEG    PATCH_NO_BROWSE_TXT:CODE:ROOT(1)
        EXTERN  OldTxtReg
        B       0xA0BE4CC4+1
        
; ==============================================   
	RSEG	PATCH_ESI:CODE:ROOT
	CODE16
	LDR	R3,=ESI_PATCH
	BX	R3
        
; ----------------------------------------------    
	RSEG	CODE:CODE:NOROOT
	CODE32
	EXTERN	ESI
        EXTERN  ESIOld
ESI_PATCH:
	MOV	R3,R0,LSR #28
	CMP	R3,#0x0A
        BNE     DO_LGP
	BL 	ESI
	LDMFD	SP!,{R4,PC}
DO_LGP:
        LDR	R3,ESIOld
	MOV	R4,R1
	MOV	R1,R2
	LDR	R2,=0x2710
	CMP	R0,R2
	BX	R3
        
; ==============================================       
 	RSEG	PATCH_PROPERTY_WINDOW:CODE:ROOT
	CODE32


        ADD     LR, PC, #4
        LDR     PC, =PATCH_EXT
        
; ----------------------------------------------   
        RSEG	CODE:CODE:NOROOT
	CODE32
        EXTERN  PropertyPatch

PATCH_EXT:
        STMFD   SP!,{R1-R3, LR}
        LDR     R2, [SP, #0x20+0xC4+20]
        CMP     R2, #0xA0000000
        BLCC    PropertyPatch
        LDMFD   SP!,{R5-R7, PC}
        
; ==============================================               
	THUMB
	RSEG	PATCH_GET_PIT:CODE:ROOT(2)
//MAINCODE:A0975D34 10 B5                       PUSH    {R4,LR}
//MAINCODE:A0975D36 04 1C                       ADD     R4, R0, #0
//MAINCODE:A0975D38 B1 F0 F2 EC                 BLX     sub_A0A27720
	LDR	R1,JJ_PIT
	BX	R1
        
        RSEG	PATCH_GET_PITJ:DATA(2)
JJ_PIT	DC32	J_PIT
        
             
; ----------------------------------------------       
	RSEG	PIT_PNG_EXTENSION:CODE(2)
	EXTERN	PITgetN
	EXTERN	PatchGetPIT
	EXTERN	PITret
	ARM
J_PIT:
	STMFD	SP!,{R4,LR}
	MOV	R4,R0
	BL	PatchGetPIT
        CMP     R0, #0
	LDMNEFD	SP!,{R4,PC}
	MOV	R0,R4
	LDR	R12,PITgetN
	BLX	R12
        CMP	R0,R4
	LDR	R12,PITret
	BX	R12

        

#endif

	END




