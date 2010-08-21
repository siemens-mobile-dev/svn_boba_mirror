//C510_R1HA035
        RSEG   CODE
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
        defadr   STANDBY_RET,0x14F738EA+1
        defadr   DB_PATCH1_RET,0x142C7F1C+1
        defadr   DB_PATCH2_RET,0x142C85DC+1
        defadr   DB_PATCH3_RET,0x14F10B3C+1
        defadr   DB_PATCH4_RET,0x14F37810+1
        defadr   MESS_HOOK_RET,0x10129BBC+1
        defadr   PAGE_ACTION_RET,0x1430DFE8+1

        defadr  memalloc,0x4BA32698
        defadr  memfree,0x4BA326C0

// --- CreateLists ---

        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:

        STR     R0, [R7,#0xC]
        STR     R0, [R7,#0x10]
        STR     R0, [R7,#0x14]
        STR     R0, [R7,#0x18]
        BLX     CreateLists
        MOV     R0, #0
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
	LDR     R0, =0x4BA26760
        LDR     R0, [R0, #0]
        MOV     R1, #0
        BLX     List_RemoveAt
        BL      PageAction_Hook
        LDR     R1, =PAGE_ACTION_RET
        BX      R1



        RSEG    PATCH_PageAction:CODE(1)
        CODE16
        LDR     R0, =_PageAction
        BX      R0



// --- ParseHelperMessage ---
        EXTERN  ParseHelperMessage
        RSEG   CODE
        CODE16
MESS_HOOK:
	MOV	R7, #1
        LDR     R5, [R6, #0]

        BLX     ParseHelperMessage
        LDR     R3, =MESS_HOOK_RET
        BX      R3

        RSEG   PATCH_MMI_MESSAGE_HOOK
        CODE16
        LDR     R7,=MESS_HOOK
        BX      R7

// --- PageAction1 ---
        EXTERN  PageAction_Hook1
        RSEG    PATCH_PageActionImpl
        RSEG   CODE
        CODE16
PG_ACTION:
        BL      PageAction_Hook1
        MOV     R0, SP
        MOV     R1, #0
        STRB    R1, [R0,#4]
        ADD     R6, R1, #0
        LDR     R3,=SFE(PATCH_PageActionImpl)+1
        BX      R3



        RSEG    PATCH_PageActionImpl
        CODE16
        LDR     R3, =PG_ACTION
        BX      R3

// --- Data Browser ---

        EXTERN  GetExtTable
        RSEG   CODE
        CODE16
DB_PATCH1:
        BLX     GetExtTable
        LSL     R1, R4, #2
        LDR     R0, [R0,R1]
        LDR     R1, =0x15B5CE04
        LDR     R3, =DB_PATCH1_RET
        BX      R3


        RSEG   CODE
        CODE16

DB_PATCH2:

        BLX     GetExtTable
	LSL	R1, R7, #2
	LDR	R0, [R0,R1]
	LDR	R1, =0x15B5CE04
        LDR     R3, =DB_PATCH2_RET
        BX      R3

        RSEG   CODE
        CODE16

DB_PATCH3:
        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R7, [R0,R1]
	LDR	R0, =0x15B5CE04
        LDR     R3, =DB_PATCH3_RET
        BX      R3


        RSEG   CODE
        CODE16

DB_PATCH4:
        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R0, [R0,R1]
	LDR	R1, =0x15B5CE04
	STR	R0, [SP,#0]
        LDR     R3, =DB_PATCH4_RET
        BX      R3



        RSEG   PATCH_DB1(2)
        CODE16
        LDR    R3, =DB_PATCH1
        BX     R3

        RSEG   PATCH_DB2(2)
        CODE16
        LDR    R3, =DB_PATCH2
        BX     R3

        RSEG   PATCH_DB3(2)
        CODE16
        LDR    R3, =DB_PATCH3
        BX     R3

        RSEG   PATCH_DB4(2)
        CODE16
        LDR    R3, =DB_PATCH4
        BX     R3

        END
