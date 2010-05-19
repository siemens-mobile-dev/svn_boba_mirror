//W910_R1FA035
        RSEG   CODE
#ifdef W910_R1FA035
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
        defadr   STANDBY_RET,0x115DD10C+1
        defadr   DB_PATCH_RET,0x10FE2EDC+1
        defadr   DB_EXT_RET,0x10FE2CE8+1
        defadr   DB_PATCH3_RET,0x10FE2BA8+1
        defadr   DB_PATCH4_RET,0x10FE36B8+1
        defadr   DB_PATCH5_RET,0x10FE2D34+1
        defadr   DB_PATCH6_RET,0x10FE2D68+1
        defadr   MESS_HOOK_RET,0x107202AA+1
        defadr   PAGE_ACTION_RET,0x115D0956+1

        defadr  memalloc,0x28B001C4
        defadr  memfree,0x28B001D4
LastExtDB EQU 0x11A0A5C8
// --- CreateLists ---
        EXTERN  CreateLists
        RSEG  CODE
        CODE16
PATCH_STANDBY:
        STR     R0, [R5,#0xC]
	STR     R0, [R5,#0x10]
	STR     R0, [R5,#0x14]
	STR     R0, [R5,#0x18]
        BLX     CreateLists
        MOV     R0, #0
        LDR     R6,=STANDBY_RET
        BX      R6

        RSEG  PATCH_STANDBY_CALL
        CODE16
        LDR     R6,=PATCH_STANDBY
        BX      R6
// --- PageAction_Hook ---
        EXTERN  PageAction_Hook
        EXTERN  List_RemoveAt
        RSEG  CODE
        CODE16
_PageAction:
	ADD	R7, R0, #0
	LDR     R0, =0x2A27BE04
	LDR     R0, [R0,#0]
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
        RSEG   CODE
        CODE16
MESS_HOOK:
	LDR     R6, [SP,#4]
	MOV     R7, #1
	LDR     R6, [R6,#0]
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
        LSL     R1, R6, #2
        LDR     R0, [R0,R1]
        LDR     R1, =LastExtDB
        LDR     R3, =DB_PATCH_RET
        BX      R3

        RSEG   CODE
        CODE16
DBEXT:
        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R0, [R0,R1]
	LDR	R1, =LastExtDB
	STR	R0, [SP,#0]
        LDR     R3, =DB_EXT_RET
        BX      R3

        RSEG   CODE
        CODE16
DB_PATCH3:
        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R0, [R0,R1]
	LDR	R1, =LastExtDB
	STR	R0, [SP,#0]
        LDR     R3, =DB_PATCH3_RET
        BX      R3

        RSEG   CODE
        CODE16
DB_PATCH4:
        BLX     GetExtTable
	LSL	R1, R6, #2
	LDR	R7, [R0,R1]
	LDR	R0, =LastExtDB
        LDR     R1, =DB_PATCH4_RET
        BX      R1

        RSEG   CODE
        CODE16
DB_PATCH5:
        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R1, [R0,R1]
	LSL	R0, R6, #2
        LDR     R7, =DB_PATCH5_RET
        BX      R7

        RSEG   CODE
        CODE16
DB_PATCH6:
        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R1, [R0,R1]
	LSL	R0, R6, #2
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
        LDR    R2, =DB_PATCH5
        BX     R2

        RSEG   PATCH_DB6(2)
        CODE16
        LDR    R2, =DB_PATCH6
        BX     R2

#endif
        END
