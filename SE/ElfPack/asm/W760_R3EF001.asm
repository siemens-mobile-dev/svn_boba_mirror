//W760_R3EF001
        RSEG   CODE
        CODE32

defadr  MACRO   a,b
        PUBLIC  a
a       EQU     b
        ENDM

        RSEG  CODE
        defadr   STANDBY_RET,0x10E2891C+1
        defadr   DB_PATCH_RET,0x11883E34+1
        defadr   DB_EXT_RET,0x11883C3C+1
        defadr   DB_PATCH3_RET,0x11883AE4+1
        defadr   DB_PATCH4_RET,0x118844B2+1
        defadr   DB_PATCH5_RET,0x11883C88+1
        defadr   DB_PATCH6_RET,0x11883CBC+1
        defadr   MESS_HOOK_RET,0x112AD4C0+1

        defadr  memalloc,0x4BB00584
        defadr  memfree,0x4BB005AC

// --- Patch Keyhandler ---
	EXTERN Keyhandler_Hook
	RSEG  PATCH_KEYHANDLER1
        RSEG  CODE
        CODE16
NEW_KEYHANDLER1:

	PUSH	{R0,R1}
	LDRH	R0, [R4,#0]
	BLX	Keyhandler_Hook
	STRH	R0, [R4,#0]
	MOV	R1, R0
	MOV	R0, R7
	LDR	R2, =SFE(PATCH_KEYHANDLER1)+1
	MOV	R12, R2
	POP	{R2,R3}
	BX	R12

	RSEG  PATCH_KEYHANDLER1
        CODE16
        LDR     R3,=NEW_KEYHANDLER1
        BX      R3


	RSEG  PATCH_KEYHANDLER2
        RSEG  CODE
        CODE16
NEW_KEYHANDLER2:

	MOV	R7, R0
	MOV	R1, #0x0
	MOV	R0, R4
	BLX	Keyhandler_Hook
	MOV	R4, R0
	MOV	R2, R7
	MOV	R3, #0x0
	MOV	R1, R4
	MOV	R0, R6
	LDR	R7, =SFE(PATCH_KEYHANDLER2)+1
	BX	R7

	RSEG  PATCH_KEYHANDLER2
        CODE16
        LDR     R2,=NEW_KEYHANDLER2
        BX      R2


	RSEG  PATCH_KEYHANDLER3
        RSEG  CODE
        CODE16
NEW_KEYHANDLER3:

	PUSH	{R0,R1}
	LDRH	R0, [R7,#0x4]
	BLX	Keyhandler_Hook
	POP	{R2,R3}
	STRH	R0, [R7,#0x4]
	MOV	R1, R0
	LDR	R0, =SFE(PATCH_KEYHANDLER3)+1
	BX	R0

	RSEG  PATCH_KEYHANDLER3
        CODE16
        LDR     R3,=NEW_KEYHANDLER3
        BX      R3


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
        EXTERN  PageAction_Hook2
        RSEG    PATCH_PageActionImpl_All
        RSEG   CODE
        CODE16
PG_ACTION:
	MOV	R2, R6
	MOV	R1, R5
	MOV	R0, R4
        BLX     PageAction_Hook2
	CMP	R0, #0x0
	BNE	SKIP_ORIGINAL
	MOV	R1, R6
        MOV	R0, R5
	LDR	R2, [R4,#0]
	BLX	R2

SKIP_ORIGINAL:
        LDR     R4,=SFE(PATCH_PageActionImpl_All)+1
        BX      R4



        RSEG    PATCH_PageActionImpl_All
        CODE16
        LDR     R2, =PG_ACTION
        BX      R2


        EXTERN  PageAction_Hook2
        RSEG    PATCH_PageActionImpl_EnterExit
        RSEG   CODE
        CODE16
PG_ACTION2:
	LDR	R2, [SP,#0x1C]
	LDR	R1, [SP,#0x20]
	MOV	R0, R6
        BLX     PageAction_Hook2
	LDR	R1, [SP,#0x1C]
        LDR	R0, [SP,#0x20]
	LDR	R2, [R6,#0]
	BLX	R2
        LDR     R7,=SFE(PATCH_PageActionImpl_EnterExit)+1
        BX      R7



        RSEG    PATCH_PageActionImpl_EnterExit
        CODE16
        LDR     R2, =PG_ACTION2
        BX      R2

// --- Data Browser ---

        EXTERN  GetExtTable
        RSEG   CODE
        CODE16
DB_PATCH:
        BLX     GetExtTable
        LSL     R1, R7, #2
        LDR     R0, [R0,R1]
        LDR     R1, =0x11EF9A90
        LDR     R3, =DB_PATCH_RET
        BX      R3


        RSEG   CODE
        CODE16

DBEXT:

        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R0, [R0,R1]
	LDR	R1, =0x11EF9A90
	STR	R0, [SP,#0]
        LDR     R3, =DB_EXT_RET
        BX      R3

        RSEG   CODE
        CODE16

DB_PATCH3:
        BLX     GetExtTable
	LSL	R1, R5, #2
	LDR	R0, [R0,R1]
	LDR	R1, =0x11EF9A90
	STR	R0, [SP,#0]
        LDR     R3, =DB_PATCH3_RET
        BX      R3


        RSEG   CODE
        CODE16

DB_PATCH4:
        BLX     GetExtTable
	LSL	R1, R6, #2
	LDR	R7, [R0,R1]
	LDR	R0, =0x11EF9A90
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

        END
