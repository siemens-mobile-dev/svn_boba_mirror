        THUMB
        CODE16
        
	RSEG	PATCH_NET_ICONS:DATA:ROOT(1)
        BL net_icons
	RSEG	PATCH_Battery_ICONS:DATA:ROOT(1)
        BL acc_icons
        
	RSEG	PATCH_Iconbar_x:DATA:ROOT(1)
        MOV     R7,R0
        BL iconbar_x
	RSEG	PATCH_Iconbar_y:DATA:ROOT(1)
        BL iconbar_y
        
	RSEG	PATCH_TimeDate
        LSL     R1, R0, #0x18
        LSR     R1, R1, #0x18
        BL timedate
                
	RSEG	GO_MAIN
net_icons:
        LDR     R0, =net_icons2
        BX      R0
acc_icons:
        LDR     R0, =acc_icons2
        BX      R0
iconbar_x:
        LDR     R0, =iconbar_x2
        BX      R0
iconbar_y:
        LDR     R0, =iconbar_y2
        BX      R0
timedate:
        LDR     R0, =timedate2
        BX      R0
        
        EXTERN get_icons
        EXTERN plus
        
        CODE16
        RSEG	CODE
net_icons2:
        PUSH    {R2-R7,LR}
        MOV     R0, #1
        BL get_icons
        MOV     R7, R0
        MOV     R0, #0
        BL get_icons
        MOV     R1, R7
        POP     {R2-R7,PC}
acc_icons2:
        PUSH    {R2-R7,LR}
        MOV     R0, #3
        BL get_icons
        MOV     R7, R0
        MOV     R0, #2
        BL get_icons
        MOV     R1, R7
        POP     {R2-R7,PC}
        
iconbar_x2:
        PUSH    {R1-R6,LR}
        MOV     R0, #4
        BL get_icons
        ADD     R0, R0, R7
        LSL     R7, R0, #0x10
        ASR     R7, R7, #0x10
        POP     {R1-R6,PC}
        
iconbar_y2:
        PUSH    {R2-R7,LR}
        MOV     R0, #5
        BL get_icons
        MOV     R1, R0
        MOV     R0, R7
        POP     {R2-R7,PC}
        
timedate2:
        PUSH    {R1,R3-R7,LR}
        MOV     R0, #6
        BL get_icons
        MOV     R2, R0
        MOV     R0, R4
        POP     {R1,R3-R7,PC}
        
        
	RSEG	PATCH_NET_CANVAS:DATA:ROOT(1)
        NOP
        NOP
	RSEG	PATCH_Battery_CANVAS:DATA:ROOT(1)
        NOP
        NOP
	RSEG	PATCH_Iconbar_CANVAS
        NOP
        NOP
        
	END