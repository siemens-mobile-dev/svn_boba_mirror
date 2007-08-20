
        NAME    main


	RSEG	ICODE:CODE:ROOT(2)
	ARM
main
	MOV	R0,R4
	BLX	0xA04EAFF8+1
	MOV	R0,R4
	BL	0xA0094948	; mfree
	MOV	R0,R5
	LDMFD   SP!,{R1-R7,PC}
	RSEG	PATCH:CODE:ROOT(2)
	THUMB
	LDR	R0,=main
	BX	R0
        END


