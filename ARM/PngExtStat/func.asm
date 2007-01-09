	PUBLIC	ELF_BEGIN
	RSEG	ELFBEGIN:DATA
ELF_BEGIN



	RSEG	CODE:CODE
	PUBLIC	kill_data
	CODE32
kill_data
	BX	R1
        
/*	RSEG	CODE:CODE
	PUBLIC	get_top_pl
	CODE32
get_top_pl
	LDR     R0, =0xA8D7AC70 //0xA86894C0
        LDR     R0, [R0,#0] 
        BX      LR
*/        

defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM

//        defadr  rama,0xA86894C0
//        defadr  RegisterCepidForSocketWork,0xA09915E5
//        defadr  RequestLMANConnect,0xA09926F5
//        defadr  RefreshGUI,0xA097851D
//        defadr  EDIT_SetFocus,0xA0A118D9

	END
	