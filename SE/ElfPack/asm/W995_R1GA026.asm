//W995 R1GA026
        RSEG   CODE
#ifdef W995_R1GA026
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
        defadr   STANDBY_RET,0x14E2EA78+1
        defadr   DB_PATCH_RET,0x142DBACC+1
        defadr   DB_EXT_RET,0x142DB6D4+1
        defadr   DB_PATCH3_RET,0x14FEDCBC+1
        defadr   DB_PATCH4_RET,0x14FC5180+1
        defadr   MESS_HOOK_RET,0x101368CC+1
        defadr   PAGE_ACTION_RET,0x14104358+1

        defadr  memalloc,0x15454548+1
        defadr  memfree,0x15454580+1
// --- CreateLists ---

        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:

        STR     R0, [R7,#0xC]
        STR     R0, [R7,#0x10]
        STR     R0, [R7,#0x14]
        STR     R0, [R7,#0x18]
	PUSH    {R0-R7}
        BLX     CreateLists
	POP     {R0-R7}
        LDR     R5,=STANDBY_RET
        BX      R5

        RSEG  PATCH_STANDBY_CALL
        CODE16
        LDR     R5,=PATCH_STANDBY
        BX      R5

// --- PageAction_Hook ---
        EXTERN  PageAction_Hook
        EXTERN  List_RemoveAt
        RSEG  CODE
        CODE16
_PageAction:
	LDR     R0, =0x4BA27B6C
        LDR     R0, [R0, #0]
        MOV     R1, #0
        BLX     List_RemoveAt
        BL      PageAction_Hook
        LDR     R1, =PAGE_ACTION_RET
        BX      R1



        RSEG    PATCH_PageAction:CODE(1)
        CODE16
        LDR     R1, =_PageAction
        BX      R1



// --- ParseHelperMessage ---
        EXTERN  ParseHelperMessage
        RSEG    CODE
        CODE16
MESS_HOOK:
	MOV	R0, R6
        BLX     ParseHelperMessage
	LDR     R1, =0x4BAFDED0
	LDR     R2, =0x4BAFDED4
        LDR     R3, =MESS_HOOK_RET
        BX      R3

        RSEG   PATCH_MMI_MESSAGE_HOOK
        CODE16
        LDR     R3,=MESS_HOOK
        BX      R3

// --- Data Browser ---

        EXTERN  GetExtTable
        RSEG   CODE
        CODE16
DB_PATCH:
        BLX     GetExtTable
	ADD	R1, R0, #0
        LSL     R0, R7, #2
        LDR     R0, [R1,R0]
	LDR	R1, =0x15C77680
        LDR     R3, =DB_PATCH_RET
        BX      R3


        RSEG   CODE
        CODE16

DBEXT:
        BLX     GetExtTable
	MOV	R1, R0
        LSL     R0, R4, #2
        LDR     R0, [R1,R0]
	LDR	R1, =0x15C77680
        LDR     R5, =DB_EXT_RET
        BX      R5

        RSEG   CODE
        CODE16

DB_PATCH3:
        BLX     GetExtTable
	MOV	R7, R0
	LSL     R0, R5, #2
	LDR     R0, [R7,R0]
	LDR	R1, =0x15C77680
	STR     R0, [SP,#0]
        LDR     R7, =DB_PATCH3_RET
        BX      R7


        RSEG   CODE
        CODE16

DB_PATCH4:
        BLX     GetExtTable
        ADD     R6, R0, #0
        LSLS    R0, R5, #2
	LDR     R7, [R6,R0]
	LDR	R0, =0x15C77680
        LDR     R6, =DB_PATCH4_RET
        BX      R6



        RSEG   PATCH_DB1(2)
        CODE16
        LDR    R3, =DB_PATCH
        BX     R3

        RSEG   PATCH_DB2(2)
        CODE16
        LDR    R5, =DBEXT
        BX     R5

        RSEG   PATCH_DB3(2)
        CODE16
        LDR    R7, =DB_PATCH3
        BX     R7

        RSEG   PATCH_DB4(2)
        CODE16
        LDR    R6, =DB_PATCH4
        BX     R6


#endif
        END
