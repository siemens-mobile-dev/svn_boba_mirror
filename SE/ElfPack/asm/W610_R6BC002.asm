//W610 R6BC002
        RSEG   CODE
#ifdef W610_R6BC002
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
        defadr   STANDBY_RET,0x452B89C4+1
        defadr   DB_PATCH_RET,0x44EB7864+1
        defadr   DB_EXT_RET,0x44EB7650+1
        defadr   DB_PATCH3_RET,0x44EB74F0+1
        defadr   DB_PATCH4_RET,0x44EB7EB8+1
        defadr   MESS_HOOK_RET,0x447AC644+1
        defadr   PAGE_ACTION_RET,0x452A47D4+1

// --- CreateLists ---

        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:

        STR     R0, [R5,#8]
        STR     R0, [R5,#0]
        STR     R0, [R5,#0xC]
        STR     R0, [R5,#0x10]
        BLX     CreateLists
        MOV     R0, #0
        LDR     R1,=STANDBY_RET
        BX      R1

        RSEG  PATCH_STANDBY_CALL
        CODE16
        LDR     R1,=PATCH_STANDBY
        BX      R1

// --- PageAction_Hook ---
        EXTERN  PageAction_Hook
        EXTERN  ListElement_Remove
        RSEG  CODE
        CODE16
_PageAction:
        LDR     R0, [R0, #0]
        MOV     R1, #0
        BLX     ListElement_Remove
        BL      PageAction_Hook
        MOV     R6, R0
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
        LDR     R3, [R0, #0]
        STR     R3, [SP,#0x1C]
        BLX     ParseHelperMessage
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
        LSL     R7, R1, #2
        BLX     GetExtTable
        LDR     R7, [R0,R7]
        LDR     R1, =0x45967D64
        LDR     R3, =DB_PATCH_RET
        BX      R3


        RSEG   CODE
        CODE16

DBEXT:

        ADD     R2, R2, #1
        ADD     R1, R5, #0
        ADD     R0, R4, #0
        PUSH    {R0-R2}
        BLX     GetExtTable
        ADD     R5, R0, #0
        POP     {R0-R2}
        LDR     R3, =DB_EXT_RET
        BX      R3

        RSEG   CODE
        CODE16

DB_PATCH3:
        MOV     R6, #0
        ADD     R5, R0, #0
        BEQ     L_DB3EX
        BLX     GetExtTable
        ADD     R4, R0, #0
        LDR     R1, =DB_PATCH3_RET
        BX      R1

L_DB3EX
        ADD     R0, R6, #0
        ADD     SP, #8
        POP     {R4-R7,PC}

        RSEG   CODE
        CODE16

DB_PATCH4:
        ADD     R2, SP, #4
        PUSH    {R2}
        LDR     R0, [R0, #0]
        MOV     R3, #2
        PUSH    {R0-R3}
        BLX     GetExtTable
        ADD     R7, R0, #0
        POP     {R0-R3}
        LDR     R1, =DB_PATCH4_RET
        BX      R1



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


        RSEG   DATA_N
        RSEG   CUT_PRINT_BUF1(2)
        DATA
        DCD    SFB(DATA_N)+0x1000

        RSEG   CUT_PRINT_BUF2(2)
        DATA
        DCD    SFB(DATA_N)+0x1000

        RSEG   CUT_PRINT_BUF_SIZE1(2)
        DATA
        DCD    0xC350-0x1000

        RSEG   CUT_PRINT_BUF_SIZE2(2)
        DATA
        DCD    0xC350-0x1000

        RSEG   CUT_PRINT_BUF_SIZE3(2)
        DATA
        DCD    0xC351-0x1000
#endif
        END
