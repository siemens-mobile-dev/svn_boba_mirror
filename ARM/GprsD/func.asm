	PUBLIC	ELF_BEGIN
	RSEG	ELFBEGIN:DATA
ELF_BEGIN

	RSEG	CODE:CODE
	PUBLIC	kill_data
	CODE32
kill_data
	BX	R1
;
;        STMFD   SP!,{LR}    ; Адрес возврата - в стек
;        LDR     LR,[R1]     ; Адрес следующего обработчика - в LR, будет выполнен переход из mfree
;        BX      R3





;
defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM

;        defadr  GBS_WaitForMsg,0xA00925C4
;        defadr  RegisterCepidForSocketWork,0xA09915E5
;        defadr  RequestLMANConnect,0xA09926F5

	END
	