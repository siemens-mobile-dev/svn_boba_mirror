//W810_R4EA031
        RSEG   CODE
#ifdef W810_R4EA031
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        defadr  memalloc,0x45453AA8+1
        defadr  memfree,0x454547E0+1
// --- CreateLists ---

        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:

        MOV     R5,LR
        ADD     R5,R5,#4
        PUSH    {R5}
        STR     R0, [R4,#0x00]
        STR     R0, [R4,#0x04]
        STR     R0, [R4,#0x08]
        STR     R0, [R4,#0x0C]
        STR     R0, [R4,#0x10]
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


// --- Data Browser ---

        EXTERN  GetExtTable
        RSEG   CODE
        CODE16
DB_PATCH:
        PUSH    {R3,LR}
        LSL     R4, R7, #2
        BLX     GetExtTable
        LDR     R4, [R0,R4]
        POP     {R0,PC}

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
#endif
        END
