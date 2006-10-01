;*****************************************************************************
;*
;* MODULE	: cstart.asm
;*
;*
;* (c) mamaich, mamaich@uymail.com
;*
;*****************************************************************************

$CASE
$GENONLY
$DEBUG
$NOLOCALS
$CHECKCPU16
$CHECKBUS18
$NOMOD166			; disable the internal set of SAB 80C166 SFRs
@IF( ! @DEFINED(__STDNAMES ) )	
	$STDNAMES(reg166.def)	; use non-extended set of SFR's by default
@ELSE
	$STDNAMES(@__STDNAMES)	; use processor specific register definition 
@ENDI

	
NAME	XCSTART			; module name.
	
EXTERN	_main:FAR		; start label user program.


;*****************************************************************************
;* __CSTART
;*****************************************************************************
__CSTART_MY	SECTION CODE WORD PUBLIC 'CMYINIT'
__CSTART 	PROC  FAR 
    jmp    _main
	retv
__CSTART_MY	ENDS

CSTART_RBANK    REGDEF R0-R15
			
	END
