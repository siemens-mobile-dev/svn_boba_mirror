	RSEG	CODE:CODE
        CODE32
	EXTERN	UnPackAB
path1:
	BL	UnPackAB
        LDR     R1, =0x427
	MOV     R0, #1
	BLX     0xA07C2D32+1
	STR     R0, [R4,#0x5C]
	LDR	R2,=0xA060A540+1
	BX	R2
	
;	LDR	R1,[R1]
;	BX	R1
;	MOV	R1,#0x40000
	

	RSEG	PATCH_ABCHANGED:CODE
        CODE16
	EXTERN	unpack

	LDR	R2,=path1
	BX	R2





	RSEG	PATCH_RESIZE:CODE
        CODE16
	EXTERN	resize

	LDR	R1,=resize
	BLX	R1
        MOV	R1,R0
        B	resize_ret

	RSEG	PATCH_RESIZE_RET:CODE
        CODE16
resize_ret:

	



        END





