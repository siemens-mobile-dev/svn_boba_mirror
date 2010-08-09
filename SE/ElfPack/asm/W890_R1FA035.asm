//W890 R1FA035
        RSEG   CODE
#ifdef W890_R1FA035
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
        defadr   STANDBY_RET,0x11651410+1
        defadr   MESS_HOOK_RET,0x1077914E+1
        defadr   PAGE_ACTION_RET,0x11644C20+1

        defadr  memalloc,0x1109D078+1
        defadr  memfree,0x1109D088+1
// --- CreateLists ---

        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:

        STR     R0, [R5,#4]
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
        EXTERN  List_RemoveAt
        RSEG  CODE
        CODE16
_PageAction:
        LDR     R0, [R0, #0]
        MOV     R1, #0
        BLX     List_RemoveAt
	MOV     R6, R0
        BL      PageAction_Hook
	MOV     R0, R6
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
        LDR     R0, [SP,#4]
        MOV     R7, #1
        LDR     R6, [R0,#0]
        BLX     ParseHelperMessage
        LDR     R3, =MESS_HOOK_RET
        BX      R3

        RSEG   PATCH_MMI_MESSAGE_HOOK
        CODE16
        LDR     R3,=MESS_HOOK
        BX      R3

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

	RSEG    PATCH_DB1
        RSEG    CODE
        CODE16
DB_PATCH1:
        BLX     GetExtTable
	LSL     R1, R5, #2
	LDR     R0, [R0,R1]
	LDR     R1, =0x11A7F408
        LDR     R3, =SFE(PATCH_DB1)+1
        BX      R3

	RSEG    PATCH_DB2
        RSEG    CODE
        CODE16

DB_PATCH2:
        BLX     GetExtTable
        LSL     R1, R5, #2
	LDR     R1, [R0,R1]
	LSL     R0, R6, #2
        LDR     R7, =SFE(PATCH_DB2)+1
        BX      R7

	RSEG    PATCH_DB3
        RSEG    CODE
        CODE16

DB_PATCH3:
        BLX     GetExtTable
        LSL     R1, R5, #2
	LDR     R1, [R0,R1]
	LSL     R0, R6, #2
        LDR     R2, =SFE(PATCH_DB3)+1
        BX      R2

	RSEG    PATCH_DB4
        RSEG    CODE
        CODE16

DB_PATCH4:
        BLX     GetExtTable
        LSL     R1, R6, #2
	LDR     R0, [R0,R1]
	LDR     R1, =0x11A7F408
        LDR     R3, =SFE(PATCH_DB4)+1
        BX      R3

	RSEG    PATCH_DB5
        RSEG    CODE
        CODE16

DB_PATCH5:
	BLX     GetExtTable
	LSL     R1, R5, #2
	LDR     R0, [R0,R1]
	STR     R0, [SP,#0]
	LDR     R1, =0x11A7F408
	LDR     R7, =SFE(PATCH_DB5)+1
        BX      R7

	RSEG    PATCH_DB6
        RSEG    CODE
        CODE16

DB_PATCH6:
	BLX     GetExtTable
	LSL     R1, R6, #2
	LDR     R7, [R0,R1]
	LDR     R0, =0x11A7F408
	LDR     R3, =SFE(PATCH_DB6)+1
        BX      R3


        RSEG   PATCH_DB1(2)
        CODE16
        LDR    R3, =DB_PATCH1
        BX     R3

        RSEG   PATCH_DB2(2)
        CODE16
        LDR    R7, =DB_PATCH2
        BX     R7

        RSEG   PATCH_DB3(2)
        CODE16
        LDR    R2, =DB_PATCH3
        BX     R2

        RSEG   PATCH_DB4(2)
        CODE16
        LDR    R3, =DB_PATCH4
        BX     R3

	RSEG   PATCH_DB5(2)
        CODE16
        LDR    R7, =DB_PATCH5
        BX     R7
	
	RSEG   PATCH_DB6(2)
        CODE16
        LDR    R3, =DB_PATCH6
        BX     R3

#endif
        END
