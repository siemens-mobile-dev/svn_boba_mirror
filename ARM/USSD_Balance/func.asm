	PUBLIC	ELF_BEGIN
	RSEG	ELFBEGIN:DATA
ELF_BEGIN

	RSEG	CODE:CODE
	PUBLIC	kill_data
	CODE32
kill_data
	BX	R1
        PUBLIC  seqkill
seqkill
        BX      R3



;
defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM

//        defadr  GetFunctionPointer,0xA08DAB57
//        defadr  ExecuteElf,0xA08DAB57

	END
	