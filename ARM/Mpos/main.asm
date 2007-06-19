        THUMB
        CODE16
        
	RSEG	PATCH_NET_ICONS
        BL net_icons
	RSEG	PATCH_Battery_ICONS
        BL acc_icons
        ADD     R2, R4, #0
        
	RSEG	PATCH_Iconbar_x
        BL iconbar_x
        LSL     R7, R0, #0x10
        ASR     R7, R7, #0x10
	RSEG	PATCH_Iconbar_y
        LDRH    R2, [R0,R6]
        BL iconbar_y
        
	RSEG	PATCH_TimeDate
        LSL     R1, R0, #0x18
        LSR     R1, R1, #0x18
        BL timedate
                
        EXTERN get_icons
        EXTERN plus
        
        CODE16
        RSEG	CODE
net_icons:
        PUSH    {R2-R7,LR}
        MOV     R0, #1
        BL get_icons
        MOV     R7, R0
        MOV     R0, #0
        BL get_icons
        MOV     R1, R7
        POP     {R2-R7,PC}
acc_icons:
        PUSH    {R2-R7,LR}
        MOV     R0, #3
        BL get_icons
        MOV     R7, R0
        MOV     R0, #2
        BL get_icons
        MOV     R1, R7
        POP     {R2-R7,PC}
        
iconbar_x:
        PUSH    {R1-R4,R6-R7,LR}
        MOV     R7, R0
        MOV     R0, #4
        BL get_icons
        ADD     R0, R0, R7
        MOV     R5, #0
        POP     {R1-R4,R6-R7,PC}
iconbar_y:
        PUSH    {R2-R7,LR}
        MOV     R0, #5
        BL get_icons
        MOV     R1, R0
        ADD     R0, R7, #0
        POP     {R2-R7,PC}
        
timedate:
        PUSH    {R1,R3-R7,LR}
        ADD     R0, R4, #0
        MOV     R7, R0
        MOV     R0, #6
        BL get_icons
        MOV     R2, R0
        MOV     R0, R7
        POP     {R1,R3-R7,PC}
        
        
	RSEG	PATCH_NET_CANVAS
        CMP R0,#0
        CMP R0,#0
	RSEG	PATCH_Battery_CANVAS
        CMP R0,#0
        CMP R0,#0
	RSEG	PATCH_Iconbar_CANVAS
        CMP R0,#0
        CMP R0,#0
        
	END