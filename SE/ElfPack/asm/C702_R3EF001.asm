//C702_R3EF001
        RSEG   CODE
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
        defadr   STANDBY_RET,0x10DFE608+1
        defadr   DB_PATCH_RET,0x11856648+1
        defadr   DB_EXT_RET,0x11856450+1
        defadr   DB_PATCH3_RET,0x118562F8+1
        defadr   DB_PATCH4_RET,0x11856CC6+1
        defadr   MESS_HOOK_RET,0x11281A54+1
        defadr   PAGE_ACTION_RET,0x10E60588+1
        defadr   DB_PATCH5_RET,_PATCH_DB5+8
        defadr   DB_PATCH6_RET,_PATCH_DB6+8

        defadr  memalloc,0x2AF004F8
        defadr  memfree,0x2AF00520

LastExtDB EQU 0x11EFBBF0

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
        LDR     R0, =0x2AFA9FCC
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
        MOV     R7, #1
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
DB_PATCH:
        BLX     GetExtTable
        LSL     R1, R7, #2
        LDR     R0, [R0,R1]
        LDR     R1, =LastExtDB
        LDR     R3, =DB_PATCH_RET
        BX      R3


        RSEG   CODE
        CODE16

DBEXT:
        BLX     GetExtTable
        LSL     R1, R5, #2
        LDR     R0, [R0,R1]
        LDR     R1, =LastExtDB
        STR     R0, [SP,#0]
        LDR     R3, =DB_EXT_RET
        BX      R3

        RSEG   CODE
        CODE16

DB_PATCH3:
        BLX     GetExtTable
        LSL     R1, R5, #2
        LDR     R0, [R0,R1]
        LDR     R1, =LastExtDB
        STR     R0, [SP,#0]
        LDR     R3, =DB_PATCH3_RET
        BX      R3


        RSEG   CODE
        CODE16

DB_PATCH4:
        BLX     GetExtTable
        LSL     R1, R6, #2
        LDR     R7, [R0,R1]
        LDR     R0, =LastExtDB
        LDR     R1, =DB_PATCH4_RET
        BX      R1

DB_PATCH5:
        BLX     GetExtTable
        LSL     R1, R5, #2
        LDR     R1, [R0,R1]
        LSL     R0, R6, #2
        LDR     R7, =DB_PATCH5_RET
        BX      R7

DB_PATCH6:
        BLX     GetExtTable
        LSL     R1, R5, #2
        LDR     R1, [R0,R1]
        LSL     R0, R6, #2
        LDR     R2, =DB_PATCH6_RET
        BX      R2



        RSEG   PATCH_DB1(2)
        CODE16
        LDR    R3, =DB_PATCH
        BX     R3

        RSEG   PATCH_DB2(2)
        CODE16
        LDR    R3, =DBEXT
        BX     R3

        RSEG   PATCH_DB3(2)
        CODE16
        LDR    R3, =DB_PATCH3
        BX     R3

        RSEG   PATCH_DB4(2)
        CODE16
        LDR    R3, =DB_PATCH4
        BX     R3


        RSEG   PATCH_DB5(2)
        CODE16
_PATCH_DB5:
        LDR    R2, =DB_PATCH5
        BX     R2

        RSEG   PATCH_DB6(2)
        CODE16
_PATCH_DB6:
        LDR    R2, =DB_PATCH6
        BX     R2

        END
