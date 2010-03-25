//Z610 R6DA001
        RSEG   CODE
#ifdef Z610_R6DA001
        CODE32

EXT_TABLE EQU 0x45A4E168
TABMENUBAR_NAME EQU 0x45B42900 //getTabMenuBarOnDescCreate

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
STANDBY_RET	equ	PATCH_STANDBY_CALL_start+8
DB_PATCH_RET	equ	PATCH_DB1_start+8
DB_EXT_RET	equ	PATCH_DB2_start+8
DB_PATCH3_RET	equ	PATCH_DB3_start+0x0A
DB_PATCH4_RET	equ	PATCH_DB4_start+8
MESS_HOOK_RET	equ	PATCH_MMI_MESSAGE_HOOK_start+8
PAGE_ACTION_RET	equ	PATCH_PageAction_start+8

	defadr	memalloc,0x44E35A08+1
        defadr  memfree,0x44E35A34+1
// --- CreateLists ---

        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:

        STR     R0, [R5,#0x04]
        STR     R0, [R5,#0x08]
        STR     R0, [R5,#0x0C]
        STR     R0, [R5,#0x10]
        BLX     CreateLists
        MOV     R0, #0
        LDR     R1,=STANDBY_RET
        BX      R1

        RSEG  PATCH_STANDBY_CALL
        CODE16
PATCH_STANDBY_CALL_start:
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



        RSEG    PATCH_PageAction ;:CODE(1)
        CODE16
PATCH_PageAction_start:
        LDR     R1, =_PageAction
        BX      R1

// --- ParseHelperMessage ---
        EXTERN  ParseHelperMessage
        RSEG   CODE
        CODE16
MESS_HOOK:
        LDR     R3, [R0, #0]
        STR     R3, [SP,#0x1C]
        BLX     ParseHelperMessage
        LDR     R3, =MESS_HOOK_RET
        BX      R3

        RSEG   PATCH_MMI_MESSAGE_HOOK
        CODE16
PATCH_MMI_MESSAGE_HOOK_start:
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
        LDR     R1, =EXT_TABLE
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
PATCH_DB1_start:
        LDR    R3, =DB_PATCH
        BX     R3

        RSEG   PATCH_DB2(2)
        CODE16
PATCH_DB2_start:
        LDR    R3, =DBEXT
        BX     R3

        RSEG   PATCH_DB3(2)
        CODE16
PATCH_DB3_start:
        LDR    R3, =DB_PATCH3
        BX     R3

        RSEG   PATCH_DB4(2)
        CODE16
PATCH_DB4_start:
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


        RSEG   CODE
        CODE16
TabMenuCheck:
        PUSH    {LR}
        LDR     R0, [R0, #0] //GUIObject_GetDispObject
        LDR     R0, [R0, #8] //DispObject_GetName ptr1
        LDR     R0, [R0, #0] //DispObject_GetName ptr2
        LDR     R1, =TABMENUBAR_NAME
        CMP     R0, R1
        BNE     TabMenuCheck_false
        MOV     R0, #1
        POP     {PC}
TabMenuCheck_false:                     
        MOV     R0, #0
        POP     {PC}


        RSEG   PATCH_TabMenuCheck
        CODE16
        LDR     R3, =TabMenuCheck
        BX      R3
#endif
        END
