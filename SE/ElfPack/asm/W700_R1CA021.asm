//W700_R1CA021
        RSEG   CODE
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        defadr  memalloc,0x44F89D7C+1
        defadr  memfree,0x44F8AE98+1
// --- CreateLists ---

        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:

        MOV     R1,LR
        ADD     R1,R1,#4
        PUSH    {R1}
        STR     R0, [R5,#0x04]
        STR     R0, [R5,#0x08]
        STR     R0, [R5,#0x0C]
        STR     R0, [R5,#0x10]
        BLX     CreateLists
        MOV     R0,#0x00
        POP     {PC}

        RSEG  PATCH_STANDBY_CALL
        CODE16
        LDR     R1,=PATCH_STANDBY
        BLX     R1


// --- PageAction_Hook ---
        EXTERN  PageAction_Hook
        RSEG  CODE
        CODE16
_PageAction:
        PUSH    {LR}
        LDR     R3,=PageAction_Hook
        BLX     R3
        MOV     R4,R0
        LDRB    R0,[R0,#0]
        POP     {PC}


        RSEG    PATCH_PageAction:CODE(1)
        CODE16
        BL    _PageAction



// --- ParseHelperMessage ---
        EXTERN  ParseHelperMessage
        RSEG   CODE
        CODE16
MESS_HOOK:
        LDR     R7, [SP,#0]
        LDR     R2, [R7,#0]
        STR     R2, [SP,#0x18]
        MOV     R3,LR
        ADD     R3,R3,#6
        PUSH    {R3}
        BLX     ParseHelperMessage
        POP     {PC}


        RSEG   PATCH_MMI_MESSAGE_HOOK
        CODE16
        LDR     R3,=MESS_HOOK
        BLX     R3

// --- PageAction1 ---
        EXTERN  PageAction_Hook1
        RSEG    PATCH_PageActionImpl
        RSEG   CODE
        CODE16
PG_ACTION:
        ADD     R5, R1, #0
        BL      PageAction_Hook1
        MOV     R6, #0
        MOV     R2, #0
        ADD     R1, R4, #0
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
        PUSH    {R2,LR}
        LSL     R4, R3, #2
        BLX     GetExtTable
        LDR     R6, [R0,R4]
        POP     {R1,PC}


        RSEG   CODE
        CODE16
DBEXT:
        PUSH    {LR}

        BLX     GetExtTable
        SUB     R5,R0,#4
        POP     {PC}


        RSEG   PATCH_DB1(1)
        CODE16
        BL       DB_PATCH

        RSEG   PATCH_DB2
        CODE16
        BL       DBEXT
        END
