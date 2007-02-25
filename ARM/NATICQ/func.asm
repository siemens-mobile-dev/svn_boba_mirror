	PUBLIC	ELF_BEGIN
	RSEG	ELFBEGIN:DATA
ELF_BEGIN

	RSEG	CODE:CODE
	PUBLIC	kill_data
	CODE32
kill_data
	BX	R1


defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM

        PUBLIC GetParamPlayFile
GetParamPlayFile:
 swi 0x804A
 ldr r0,[r0,#8]
 lsl r0,r0,#24
 lsr r0,r0,#24
 bx lr

//        defadr  GBS_WaitForMsg,0xA00925C4
//        defadr  RegisterCepidForSocketWork,0xA09915E5
//        defadr  RequestLMANConnect,0xA09926F5
//        defadr  RefreshGUI,0xA097851D
//        defadr  EDIT_SetFocus,0xA0A118D9

	defadr	async_gethostbyname,0xA0A4AA20+1


	END
	