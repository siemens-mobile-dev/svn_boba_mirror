//K600_R2BB001
        RSEG   CODE
#ifdef K600_R2BB001
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        defadr  memalloc,0x20363C0C+1
        defadr  memfree,0x20364D24+1
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
        MOV     R1,LR
        ADD     R1,R1,#4
        PUSH    {R1}
        LDR     R1,=0x20231FBD
        BLX     R1
        LDR     R3,=PageAction_Hook
        BLX     R3
//        BL    PageAction_Hook
        MOV     R4,R0
        LDRB    R0,[R0,#0]
        POP     {PC}

        RSEG    PATCH_PageAction
        CODE16
        LDR     R1,=_PageAction
        BLX     R1


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
        MOV     R6,LR
        ADD     R6,#4
        ADD     R2,#0xEC
        PUSH    {R2,R6}
        LSL     R4, R3, #2
        BLX     GetExtTable
        LDR     R6, [R0,R4]
        POP     {R1,PC}

        RSEG   CODE
        CODE16
DBEXT:
        LDR     R4,=0x20F257E8
        BEQ     DBEXT_EXIT
        MOV     R5,LR
        ADD     R5,#6
        PUSH    {R5}

        BLX     GetExtTable
        MOV     R5,R0
        POP     {PC}
DBEXT_EXIT:
        MOV     R0,R4
        ADD     SP, #0x14
        POP     {R4-R7,PC}

        RSEG   PATCH_DB1
        CODE16
//        BL       DB_PATCH
        LDR     R6,=DB_PATCH
        BLX     R6

        RSEG   PATCH_DB2
        CODE16
//        BL       DBEXT
        LDR     R4,=DBEXT
        BLX     R4
#endif
        END
