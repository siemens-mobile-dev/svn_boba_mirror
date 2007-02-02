#ifndef NEWSGOLD
	EXTERN	KEY_TOP
        EXTERN  KEYBret
        RSEG	SWILIB_FUNC12B_12D:CODE
        DCD     AddKeybMsgHook
        DCD     AddKeybMsgHook_end
        DCD     RemoveKeybMsgHook
        
        CODE16       
        RSEG	KEYB_HOOK:CODE

AddKeybMsgHook:
        PUSH    {R4,R5,LR}
        MOV     R4, R0
        BEQ     CANT_ADD
        MOV     R0, #0xC
        swi     0x14
        MOV     R5, R0
        BEQ     CANT_ADD
        swi     199
        DC16    0x146        
        LDR     R1, =KEY_TOP

_3_for:
        MOV     R3, R1
        LDR     R1, [R3]
        CMP     R1, #0
        BNE     _3_for
        
        STR     R5, [R3]
	STR	R4, [R5, #4]
	MOV     R1, #0
	STR     R1, [R5]
        swi     199
        DC16    0x8015
        STR     R0, [R5, #8]
        swi     199
        DC16    0x147 
        MOV	R0, #1
CANT_ADD:
	POP	{R4,R5,PC}
 

AddKeybMsgHook_end:
        PUSH    {R4,R5,LR}
        MOV     R4, R0
        BEQ     CANT_ADD_END
        MOV     R0, #0xC
        swi     0x14
        MOV     R5, R0
        BEQ     CANT_ADD_END
        swi     199
        DC16    0x146
        LDR     R1, =KEY_TOP
        LDR     R2, [R1]
        STR     R2, [R5]
        STR     R4, [R5, #4]
        STR     R5, [R1]
        swi     199
        DC16    0x8015
        STR     R0, [R5, #8] 
        swi     199
        DC16    0x147       
	MOV	R0, #1
CANT_ADD_END:
	POP	{R4,R5,PC}
       


RemoveKeybMsgHook:
	PUSH    {LR}
        CMP     R0, #0
        BEQ     RKH_ex
	LDR     R2, =KEY_TOP
        
RKH_for:
        MOV     R3, R2
	LDR	R2, [R3]
	CMP     R2, #0
	BEQ	RKH_ex
	LDR	R1, [R2, #4]
	CMP	R1, #0
	BEQ	RKH_ex
	CMP	R1, R0
	BNE     RKH_for
_4:
	LDR	R0, [R2]
	STR	R0, [R3]
	MOV	R0, R2
        LDR     R1, [R2, #0x8]
        BLX     R1	
	MOV	R0, #1
	POP	{PC}
RKH_ex:
	MOV	R0, #0
	POP	{PC}

        
        
PatchKeybMsg:
	PUSH	{R3-R7,LR}
	ADD     R6, R0, #0  //kbd msg
	ADD     R5, R1, #0  //kbd submsg
        LDR     R4, =KEY_TOP
_for:        
	LDR     R4, [R4]
	CMP	R4, #0
	BEQ     NORMAL_EXIT
	LDR	R2, [R4, #4]
	CMP	R2, #0
	BEQ     NORMAL_EXIT
        MOV     R0, R5
        MOV     R1, R6
	BLX     R2
	CMP	R0, #0
	BEQ     _for
EXIT_NOCODE:
        POP	{R3-R7,PC}
NORMAL_EXIT:
        MOV     R4, #0
        MOV     R7, #0

	LDR     R2, =KEYBret
        LDR     R2, [R2]
        BX      R2

	THUMB
	RSEG	PATCH_KEYB_MSG:CODE:ROOT(2)
	LDR	R2,JJ_KEYB
	BX	R2
        
        RSEG	PATCH_KEYB_MSGJ:DATA(2)
JJ_KEYB	DC32	PatchKeybMsg
        
#endif
        END