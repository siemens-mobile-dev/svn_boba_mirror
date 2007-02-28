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

	RSEG	CODE:CODE(2)
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
	LDR	R12,=PITgetN
        LDR     R12, [R12]
	BLX	R12
	CMP	R0,R4
        LDR	R12,=PITret
        LDR     R12, [R12]
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
        LDMEQFD SP!,{R4,LR}
        BEQ     ESI 
DO_LGP:
        LDR	R3,=ESIOld
        LDR     R3, [R3]
	MOV	R4,R1
	MOV	R1,R2
	LDR	R2,=0x2710
	CMP	R0,R2
	BX	R3
        
; ==============================================       
 	RSEG	PATCH_PROPERTY1:CODE:ROOT
	CODE32

        BL      PropertyHook
        
        RSEG	PATCH_PROPERTY2:CODE:ROOT
	CODE32
        
        BL      PropHandler
        
; ----------------------------------------------   
        RSEG	CODE:CODE:NOROOT
	CODE32
        EXTERN  PropertyPatch

PropertyHook:
        MOV     R0, R4
        MOV     R4, #1
        STR     R4, [R0, #0x18]
        BX      LR
        
PropHandler:
        LDR     R0, [R5, #0x18]
        CMP     R0, #0
        LDMIA   R2, {R0-R2}
        BXEQ    LR
        BL      PropertyPatch
        MOV     R0, #0
        STR     R0, [R5, #0x18]
        ADD     SP, SP, #0x234
        LDMFD   SP!, {R4-R7,PC}

        
        
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
	RSEG	CODE:CODE(2)
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
	LDR	R12,=PITgetN
        LDR     R12, [R12]
	BLX	R12
        CMP	R0,R4
	LDR	R12,=PITret
        LDR     R12, [R12]
	BX	R12
        
; ================================================
/*        THUMB
	RSEG	PATCH_KEYB_KEY_DOWN:CODE:ROOT(1)
        BLX     J_PATCH
        RSEG	PATCH_KEYB_KEY_UP:CODE:ROOT(1)
	BLX     J_PATCH
        RSEG	PATCH_KEYB_KEY_LONG:CODE:ROOT(1)
	BLX     J_PATCH
        
	RSEG	PATCH_KEYB_MSGJ:CODE:NOROOT(2)
        ARM
        EXTERN  PatchKeybMsg
        
J_PATCH        
	LDR     PC, =PatchKeybMsg
        
        
-Z(CODE)PATCH_KEYB_KEY_DOWN=A0CB60D6-A0CB60D9
-Z(CODE)PATCH_KEYB_KEY_UP=A0CB61AC-A0CB61AF
-Z(CODE)PATCH_KEYB_KEY_LONG=A0CB64C6-A0CB64C9
-Z(CODE)PATCH_KEYB_MSGJ=A0CB74A0-A0CB74A7
*/
        THUMB 
        RSEG    PATCH_KEYB_MSG:CODE:ROOT(1)
        LDR     R2, JJ_KEYB
        BX      R2
        
        THUMB 
        RSEG	PATCH_KEYB_MSGJ:DATA(2)
JJ_KEYB DC32    J_KEYB

        
        RSEG    CODE:CODE:NOROOT
        CODE16
        EXTERN  PatchKeybMsg
        EXTERN  KEYBret
J_KEYB: 
        PUSH    {R3-R7,LR}
        ADD     R6, R0, #0
        ADD     R5, R1, #0
        BL      PatchKeybMsg
        CMP     R0, #0
        BEQ     NORMAL_MODE
        MOV     R5, #0
NORMAL_MODE:
        MOV     R4, #0
        MOV     R7, #0
        LDR     R2, =KEYBret
        LDR     R2, [R2]
        BX      R2

#endif

	END




