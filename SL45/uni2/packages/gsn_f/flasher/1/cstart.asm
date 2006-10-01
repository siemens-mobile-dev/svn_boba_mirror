;@" @(#)cstart.asm	1.94 02/01/17
;*****************************************************************************
;*
;* MODULE	: cstart.asm
;*
;* DESCRIPTION	: C startup code for non-extended C166 architecture CPU's
;*		  (e.g. SAB 80C166, GOLD)
;*
;*		- Processor initialization.
;*		- Initialization of static variables in internal or external
;*		  ram is done in '_c_init.asm'.
;*		- Call the user program: main().
;*		- On exit cpu is set in idle mode.
;*
;* Copyright 1991-2002 Altium BV
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

ERASE_ACK EQU 0202h

WRITE_ACK EQU 0303h

OK_ACK EQU 04B4Fh

CRC_ERROR_NAK EQU 0BBBBh

BLOCK_NOT_FOUND_NAK EQU 0DDDDh

RAM_ERROR EQU 0FFFFh

ADDRESS_TOO_FAR_NAK EQU 0EEEEh

UNKNOWN_FLASH_NAK EQU 0CCCCh

LOADER_VERSION EQU 0100h

BLOCK67_SIZE EQU 20

MCU_START_ADDRESS EQU 0A0h ;A00000

DEFAULT_EEPROM_BASE EQU 0FFh

DEFAULT_FIRST_FLASH_ID EQU 280h

DEFAULT_SECOND_FLASH_ID EQU 300h

IMEI_5008_SIZE EQU 0E0h
                       
IMEI_5077_SIZE EQU 0E8h

	
NAME	XCSTART			; module name.
	
EXTERN	_main:FAR			; start label user program.
EXTERN	_SRVWDT:FAR			; wdt
PUBLIC _cmd_F

;*****************************************************************************
;* __CSTART
;*****************************************************************************
__CSTART_MY	SECTION CODE WORD PUBLIC 'CMYINIT'
__CSTART 	PROC  FAR 
    jmp    _main
	retv 
__CSTART 	ENDP
__CSTART_MY	ENDS

__FREIA	SECTION CODE WORD PUBLIC 'CROM'
			assume dpp0:__FREIA

;_SRVWDT PROC    NEAR
;			JNB     IEN,_8
;			JMPA    cc_UC,_7
;_8:
;
;			JB      T3IR,_9
;			JMPA    cc_UC,_7
;_9:
;			bclr   T3IR   
;			bmovn  P4.1, P4.1
;			mov    T3, #0C62h
;_7:
;			RET
;_SRVWDT ENDP

RxByte PROC	NEAR
		call _SRVWDT
		jnb	S0RIR, RxByte
		bclr	S0RIR
		movbz	r3, S0RBUF
		ret
RxByte ENDP

TxDword PROC NEAR
		movb	rl3, [R10+#0]
		call	cc_UC, TxByte
		movb	rl3, [R10+#1]
		call	cc_UC, TxByte
		movb	rl3, [R10+#2]
		call	cc_UC, TxByte
		movb	rl3, [R10+#3]
		jmp	cc_UC, TxByte
		retv
TxDword ENDP

TxByte PROC	NEAR
		bclr	S0TIR
		movbz	S0TBUF,	rl3

loc_7D2:				
		call _SRVWDT
		jnb	S0TIR, loc_7D2
		ret
TxByte ENDP

TxWord PROC	NEAR
       push r3
       call cc_UC, TxByte
       mov rl3,rh3
       call cc_UC, TxByte
       pop r3
       ret
TxWord ENDP

WriteSixBytes PROC NEAR
        mov     r3, U16flashbase
        call   cc_UC, TxWord
        mov     r3, r5
        call   cc_UC, TxWord
        mov     r3, r1
        call   cc_UC, TxWord
        xor     rh3, rh3
		movb	rl3, rl6
		call	cc_UC, TxWord
		mov	r3, r7
		call	cc_UC, TxWord
		mov	r3, r2
		call	cc_UC, TxWord
        mov     r3, #0FFFFh
		call	cc_UC, TxWord
		ret
WriteSixBytes ENDP

SetupAddress PROC NEAR
		ret
;		mov	r7, #0E009h
;		cmp	r1, #200h
;		jmp	cc_C, loc_318
;		sub	r1, #200h
;		mov	r7, #8009h
;loc_318:
;		mov	ADDRSEL4, r7
;		ret
SetupAddress ENDP

CalculateAddress PROC NEAR ;R1:R5 is the adjusted address, R2 is the blocksize
                      ;R5, R2, R1 is ruined
			push   r0
			push   r3
			push   r4
			push   r7
			push   r8

;			extp   #0, #1
;			mov    r2, SAVED_ADDRSEL4 ;set-up the default address config
;			mov    ADDRSEL4, r2

			mov	r2, #20h ;default 128k blocks
			mov    r5, #0

			extp #0, #1
			movb   rl7, BC35_type
			cmpb   rl7, #1
			jmp    cc_Z, loc_334
			call   cc_UC, SetupAddress
loc_334:
			mov    r0, U16flashsize
			cmp	r1, r0  ; TARGET_ADDR < U16flashsize
			jmp	cc_C, loc_3AA
			mov	r5, #0FFh
			jmp    cc_UC, loc_ret
loc_3AA:		
			mov	r8, U16endof128kblocks
			cmp	r8, #0
			jmp    cc_Z, loc_3AB
			cmp	r1, r8   ; TARGET_ADDR < U16endof128kblocks
			jmp	cc_C, loc_38E 
loc_3AB:
			mov    r2, #10h ;64k
			mov	r8, U16endof64kblocks
			cmp	r8, #0
			jmp    cc_Z, loc_3AC
			cmp	r1, r8   ; TARGET_ADDR < U16endof64kblocks
			jmp	cc_C, loc_38E 
loc_3AC:
			mov	r2, #8h  ;32k
			mov	r7, U16endof32kblocks
			cmp	r7, #0
			jmp    cc_Z, loc_3AD
			cmp	r1, r7   ; TARGET_ADDR < U16endof32kblocks
			jmp	cc_C, loc_386
loc_3AD:
			mov	r2, #2h  ;4k
			call   cc_UC, CheckOldAMDFlash
loc_386:				
			sub	r1, r8  ;
			shl	r1, #12 ;
			mov	r5, r1  ; R5 = (TARGET_ADDR << 12)
			mov	r1, r8
loc_38E:
			mov	r4, r5
			shr	r4, #14
			shr	r1, #2
			and	r5, #3FFFh

			movb   rl3,WhichFlashICToUse ;it's been already set by DetectWhichFlashICToUse
			cmpb   rl3, #0 ;use the first 
			jmp    cc_NZ, loc_3bc_non_zero
			mov    r3, #DEFAULT_FIRST_FLASH_ID ;use the "first" flash IC
			jmp    cc_UC, loc_3bc_next
loc_3bc_non_zero:

            mov    r3, #DEFAULT_SECOND_FLASH_ID ;otherwise use the second flash IC
loc_3bc_next:
			add    r4, r3
			add	r1, r4
			mov    U16flashbase, r3
loc_ret:
			pop    r8
			pop    r7
			pop    r4
			pop    r3
			pop    r0
			ret
CalculateAddress ENDP

CheckOldAMDFlash PROC NEAR  ;r9 must still be the flashid
                 cmp r9, #0F201h
                 jmp cc_NZ, locret_38A
                 cmp r1, #0FCh
                 jmp cc_C, locret_38A
                 mov  r2, #4h ;bloody shitty blocksize
locret_38A:
                 ret
CheckOldAMDFlash ENDP

ReadBlockAndChecksum PROC NEAR
			push r1
			push R11
			push DPP2

			mov R11, R2
			shr R11, #2 ;R11 now equals to the number of 16k blocks
			jmp cc_NZ, loc_37c_good_R11a
			mov R11,#1 ;let it be at least 16K, if zero is found
loc_37c_good_R11a:
			mov r3, #10h
			sub r3, R11
			mov r11, r3
			mov DPP2, R11
			nop

			mov	r6, r2		; r6 is	the number of 4k blocks to read
			movb	rl7, #0
			mov	r8, #8000h
			mov	r1, #4096
loc_6A4:				
			call	cc_UC, RxByte
			;extp    R11, #4
			movb	[r8], rl3
			nop
			nop
			movb    rh3, [r8]        ; it's a kinda sanity check
			cmpb    rl3,rh3
			jmp cc_Z, loc_equal
			mov r3, #RAM_ERROR
			call cc_UC, TxWord
			movb rl7, #00
			movb rl4, #0FFh
			jmp cc_UC, loc_6a5a
loc_equal:
			xorb	rl7, rl3
			add	r8, #1
			jnb	r8.14, loc_6A5
			mov	r8, #8000h
			;add	R11, #1
			add     DPP2, #1
loc_6A5:
			sub	r1, #1
			jmpr	cc_NZ, loc_6A4
			mov	r1, #4096
			sub	r6, #1		; read R6 number of 4Kbytes
			jmpr	cc_NZ, loc_6A4
			call	cc_UC, RxByte	; RX checksum
			movb    rl4 , rl3
loc_6a5a:
			pop DPP2
			pop R11
			pop r1
			ret
ReadBlockAndChecksum ENDP

WaitWhileBusy PROC NEAR
            push r0

loc_864:
			call _SRVWDT
			movb rl0, [r5]
			jnb  r0.7, loc_864  

			pop r0
			ret 
WaitWhileBusy ENDP

EraseIntelNonBlocked PROC NEAR ;block id in DPP0/R5
			movb rl3, #050h ; clear
			movb [r5], rl3

			movb rl3, #60h ;unlock 1
			movb [r5],rl3

			movb rl3, #0D0h ;unlock 2
			movb [r5],rl3

			nop
			nop

			movb rl3, #050h ; clear
			movb [r5], rl3

			movb rl3, #0FFh ; reset
			movb [r5], rl3

			movb rl3, #020h ;setup erase mode
			movb [r5],rl3

			movb rl3, #0D0h
			movb [r5],rl3

			nop
			nop

			call cc_UC, WaitWhileBusy

			movb rl3, #050h ; clear
			movb [r5], rl3

			movb rl3, #0FFh ; reset
			movb [r5], rl3
			ret
EraseIntelNonBlocked ENDP

EraseIntel PROC NEAR ;block id in DPP0/R5
           movb rl3, #050h ; clear
           movb [r5], rl3

           movb rl3, #60h ;unlock 1
           movb [r5],rl3

           movb rl3, #0D0h ;unlock 2
           movb [r5],rl3

           nop
           nop

           call cc_UC, WaitWhileBusy

           movb rl3, #050h ; clear
           movb [r5], rl3
   
           movb rl3, #0FFh ; reset
           movb [r5], rl3

           movb rl3, #020h ;setup erase mode
           movb [r5],rl3

           movb rl3, #0D0h
           movb [r5],rl3

           nop
           nop

           call cc_UC, WaitWhileBusy

           movb rl3, #050h ; clear
           movb [r5], rl3
   
           movb rl3, #0FFh ; reset
           movb [r5], rl3
           ret
EraseIntel ENDP

WriteIntel PROC NEAR ;block id in DPP0/R5 word in R4
           movb rl3, #40h ; write mode
           movb [r5],rl3

           mov [r5], r4

           call cc_UC, WaitWhileBusy

           movb rl3, #050h ; clear
           movb [r5],rl3

           movb rl3, #0FFh ; reset
           movb [r5],rl3
           ret
WriteIntel ENDP

WriteWord PROC NEAR ;in DPP0/R5 - R4, DPP1 base of flash
			push R0
			push r3
			push R6
			push R7

			mov r0, #Flash_HC_RAM_loc

			extp    #0, #2
			movb rl7, [r0+#0]
			movb rh7, [r0+#1]

			extp    #0, #2
			movb rl6, [r0+#2]
			movb rh6, [r0+#3]

            call cc_UC,WriteIntel
loc_ret1:
			pop R7
			pop r6
			pop r3
			pop R0
			ret
WriteWord ENDP

EraseSector PROC NEAR ;in R1/R5 offset DPP1 base of address
            push r0
            push r3
            push r6
            push R7
            push DPP0

            mov r0, #Flash_HC_RAM_loc

            extp    #0, #2
            movb rl7, [r0+#0]
            movb rh7, [r0+#1]

            extp    #0, #2
            movb rl6, [r0+#2]
            movb rh6, [r0+#3]

            mov DPP0, r1

            call cc_UC,EraseIntel

loc_ret2:
            pop DPP0
            pop R7
            pop r6
            pop r3
            pop R0
            ret
EraseSector ENDP

_cmd_F PROC FAR
        mov     DPP0, #PAG _cmd_F
		call 	Get_Flash_HC

		call	cc_UC, RxByte
		movb	rh1, rl3
		call	cc_UC, RxByte
		movb	rl1, rl3

        call    cc_UC, DetectWhichFlashICToUse
                ;R9 is ruined
		call	cc_UC, CalculateAddress
                ;R5, R2, R1 is ruined
		mov	r3, #ADDRESS_TOO_FAR_NAK
		cmp	r5, #0FFh
		jmp	cc_Z, loc_348
		call	cc_UC, WriteSixBytes
		call	cc_UC, ReadBlockAndChecksum

		mov	r3, #CRC_ERROR_NAK
		cmpb	rl7, rl4
		jmp	cc_NZ, loc_348

        mov     DPP1, U16flashbase ;it's updated in CalculateAddress

        mov     r3, DPP1
        call    cc_UC, TxWord
        mov     r3, r1
        call    cc_UC, TxWord
        mov     r3, r5
        call    cc_UC, TxWord

		call	cc_UC, EraseSector

        mov     r3, #ERASE_ACK
        call    cc_UC, TxWord

        mov     DPP1, U16flashbase ;it's updated in CalculateAddress

        mov     r3, DPP1
        call    cc_UC, TxWord
        mov     r3, r1
        call    cc_UC, TxWord
        mov     r3, r5
        call    cc_UC, TxWord

        call	cc_UC, WriteFlashBlock

        mov     r3, #WRITE_ACK
        call    cc_UC, TxWord

		call	cc_UC, WriteResult
		mov	r3, #OK_ACK	; "OK"
loc_348:		
		call	cc_UC, TxWord
cmd_Q:
		ret
_cmd_F ENDP

WriteResult PROC NEAR ;using DPP0:R5 and R2 as size
			push DPP0
			push r3
			push r4

			mov DPP0, r1
			mov	r0, #0
			mov     r3, #4096
loc_4B6:
			mov     r4, [r5+]
			add	r0, r4
			jnb	r5.14, loc_4D4
			mov	r5, #0
			add	DPP0, #1
loc_4D4:	
			sub	r3, #2
			jmpr	cc_NZ, loc_4B6
			mov	r3, #4096
			sub	r2, #1		; read R2 number of 4Kbytes
			jmpr	cc_NZ, loc_4B6
			mov	r3, r0
			call	cc_UC, TxWord

			pop r4
			pop r3
			pop DPP0
			ret
WriteResult ENDP

WriteFlashBlock PROC NEAR ;R1:R5 flashbase DPP1, R2 number of 4K blocks
			push	r2
			push    r3
			push	r5
			push    r11
			push    DPP0
			push    DPP2

			mov     DPP0, r1

			mov R11, R2
			shr R11, #2 ;R11 now equals to the number of 16k blocks
			jmp cc_NZ, loc_37c_good_R11b
			mov R11,#1 ;let it be at least 16K, if zero is found
loc_37c_good_R11b:
			mov r3, #10h
			sub r3, R11
			mov r11, r3
			mov DPP2, R11
			nop

			;R11:R8 is the flash block
			mov	r8, #8000h
			mov     r3, #4096
loc_500a:	
			;extp    r11, #1	
			mov	r4, [r8+]
			call	cc_UC, WriteWord
			add	r5, #2
			jnb	r8.14, loc_514a
			mov	r8, #8000h
			;add	R11, #1
			add     DPP2, #1
loc_514a:				
			jnb	r5.14, loc_51Ea
			mov	r5, #0
			add	DPP0, #1
loc_51Ea:			
			sub	r3, #2
			jmpr	cc_NZ, loc_500a
			mov	r3, #4096
			sub	r2, #1		; read R2 number of 4Kbytes
			jmpr	cc_NZ, loc_500a

			movb	rl2, #0F0h ;return to read mode?
			movb	[r5], rl2
			movb	rl2, #0FFh
			movb	[r5], rl2

			pop     DPP2
			pop     DPP0
			pop     R11
			pop	r5
			pop     r3
			pop	r2
			ret
WriteFlashBlock ENDP

Get_Flash_HC PROC NEAR
		mov	DPP1, #DEFAULT_FIRST_FLASH_ID
		mov	r12, #4000h
		mov	r11, #4AAAh
		mov	r10, #4555h
		mov	r4, #55AAh
		mov	r3, #90FFh
		movb	[r11], rl4
		movb	[r10], rh4
		movb	[r11], rh3
		mov	r0, #Flash_HC_RAM_loc
		mov	r5, [r12+#0]
		mov	r6, [r12+#2]
		movb	[r0+#0],	rl5
		movb	[r0+#1],	rh5
		movb	[r0+#2],	rl6
		movb	[r0+#3],	rh6
		movb	[r11], rl3
		movb	rh5, rl6

        mov r9,r5 ; R9 is used during flashtype detection
                         ; although it will be overridden later

		mov	DPP1, #DEFAULT_SECOND_FLASH_ID
		mov	r0, #Flash_HC_RAM_loc2
		movb	[r11], rl4
		movb	[r10], rh4
		movb	[r11], rh3
		mov	r5, [r12+#0]
		mov	r6, [r12+#2]
		movb	[r0+#0],	rl5
		movb	[r0+#1],	rh5
		movb	[r0+#2],	rl6
		movb	[r0+#3],	rh6
		movb	[r11], rl3
		movb	rh5, rl6

		ret
Get_Flash_HC ENDP

DetectWhichFlashICToUse PROC NEAR ;R9 is ruined (R9 is the flashtype)
			push r0
			push r2
			push r6
			push r7

			movb rl6, #0

			mov r0, #Flash_HC_RAM_loc
			movb rl7,[r0+#0]
			movb rh7,[r0+#2]

			; if we do not have anymore flash, we'll surely
			; use the only one available

			; need to detect, which Flash IC is mapped first
			; i.e. it might turn out that the 2nd flash IC is mapped
			; before the 1st one.
			mov r2, #DEFAULT_FIRST_FLASH_ID
			cmp r2, #DEFAULT_SECOND_FLASH_ID
			jmp cc_C, loc_smaller ;1st is before second, "normal" case
			;other case: the second is before the first flash IC, so it IS the 1st
			mov r0, #Flash_HC_RAM_loc2
			movb rl7,[r0+#0]
			movb rh7,[r0+#2]
			mov r9,r7
			call cc_UC, SetupFlashBoundaries
			movb rl6, #1
			mov r2, U16flashsize
			cmp r1, r2 ;we're in the "2nd" flash IC
			jmp cc_C, loc_within_1st_flash
			movb rl6, #0
			;otherwise we MIGHT be in the first one
			sub r1, r2 ; remove size of 2nd flash
			mov r0, #Flash_HC_RAM_loc
			jmp cc_UC, loc_again
loc_smaller:
			mov r9,r7
			call cc_UC, SetupFlashBoundaries
			mov    r2, U16flashsize
			cmp	r1, r2
			jmp	cc_C, loc_within_1st_flash
			sub	r1, r2 ; remove size of 1st flash
			mov r0, #Flash_HC_RAM_loc2
			movb rl6, #1
loc_again:
			movb rl7,[r0+#0]
			movb rh7,[r0+#2]
			mov r9,r7
			call cc_UC, SetupFlashBoundaries
			mov r2, U16flashsize
			cmp r1, r2
			jmp cc_C, loc_within_1st_flash
			mov r1,#0FFFFh ;invalid address we have

loc_within_1st_flash:

			movb WhichFlashICToUse, rl6
			mov r9,r7

			pop r7
			pop r6
			pop r2
			pop r0
			call	cc_UC, SetupFlashBoundaries
			ret
DetectWhichFlashICToUse ENDP

SetupFlashBoundaries PROC NEAR ;R9 is the flashtype
			push r0
			push r1
			push r2
			push r3

			;1Mbyte flashes
			mov	r0, #100h
			mov     r1, #0h
			mov	r2, #0F0h
			mov	r3, #0F8h

			cmp	r9, #0F201h  ;some pretty old AMD flash
			jmp	cc_Z, loc_34A

			;1Mbyte flashes
			mov	r0, #100h
			mov     r1, #0h
			mov	r2, #0F0h
			mov	r3, #0F0h

			cmp	r9, #0A220h  ;ST ?
			jmp	cc_Z, loc_34A
			cmp	r9, #0C089h  ;Intel 8Mbit
			jmp	cc_Z, loc_34A

			;2Mbyte flashes
			mov	r0, #200h
			mov     r1, #0h
			mov	r2, #1F0h
			mov	r3, #1F0h

			cmp	r9, #0C289h  ;Intel 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #09089h  ;Intel 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0F389h  ;Intel 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #09189h  ;Intel 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0F489h  ;Intel 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0C401h  ;Amd 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0C01Fh  ;Atmel 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0C21Fh  ;Atmel 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #48B0h   ;Sharp 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #68B0h   ;Sharp 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0D0B0h  ;Sharp 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0E0B0h  ;Sharp 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0CE20h  ;ST 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #09020h  ;ST 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #09120h  ;ST 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0E401h  ;AMD 16Mbit
			jmp	cc_Z, loc_34A
			cmp	r9, #0E404h  ;Fujitsu 16Mbit
			jmp	cc_Z, loc_34A

			;4Mbyte flashes
			mov	r0, #400h
			mov     r1, #0h
			mov	r2, #3F0h
			mov	r3, #3F0h

			cmp	r9, #0C489h ;Intel 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0C589h ;Intel 32Mb - bottom boot!
			jmp	cc_Z, loc_34A
			cmp	r9, #09289h ;Intel 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #9689h ;Intel 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0C11Fh ;Atmel 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0C81Fh ;Atmel 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0C91Fh ;Atmel 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0D41Fh ;Atmel 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0BA20h ;ST 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0BC20h ;ST 32Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0B701h ;AMD 32Mb
			jmp	cc_Z, loc_34A

			;4Mbyte flashes
			mov	r0, #400h
			mov     r1, #400h
			mov	r2, #0h ;we don't have 64k blocks
			mov	r3, #0h ;we don't have 32k blocks

			cmp	r9, #01689h ;Intel 32Mb
			jmp	cc_Z, loc_34A

			;8Mbyte flashes - 64k blocks
			mov	r0, #800h ;"usual" 64Mbit, last 64k are 8k blocks
			mov     r1, #0h
			mov	r2, #7F0h 
			mov	r3, #7F0h

			cmp	r9, #5489h ;Intel 64Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #04820h ;ST 64Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #01020h ;ST 64Mb
			jmp	cc_Z, loc_34A

			;8Mbyte flashes - 128k Blocks
			mov	r0, #800h
			mov     r1, #800h ;yes, we do have 128k blocks until the end flash
			mov	r2, #800h ;shall be the last 64k different (from blocksize), 
			mov	r3, #0    ;but it's not the case for the 1789

			cmp	r9, #01789h ;Intel 64Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #01720h ;ST 64Mb
			jmp	cc_Z, loc_34A
			cmp	r9, #0172Ch ;?? 64Mb
			jmp	cc_Z, loc_34A

			;unsupported flash
			mov r1, #UNKNOWN_FLASH_NAK
			call cc_UC, TxWord
			mov r1, r9
			call cc_UC, TxWord
			jmp cc_UC, cmd_Q

loc_34A:
			mov U16endof128kblocks,r1
			mov U16endof64kblocks,r2
			mov U16endof32kblocks,r3
			mov U16flashsize,r0

			pop r3
			pop r2
			pop r1
			pop r0
			ret
SetupFlashBoundaries ENDP
CURRENT_LOCATION:

decryptkey db 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

BC35_type db 1 ;number of flashes (but with a C35 "style")

WhichFlashICToUse db 0 ;which flash to use out of the 2

SAVED_ADDRSEL4 dw 0

U16endof128kblocks dw 0

U16endof64kblocks dw 0

U16endof32kblocks dw 0

U16flashsize dw 0

U16flashbase dw 0

num_eeprom_bases dw 1
eeprom_bases dw DEFAULT_EEPROM_BASE

imei_block_sizes dw 10, 10, IMEI_5008_SIZE, IMEI_5077_SIZE


RAM_IMEI_BLK0_Addr_Ofs dw 0

RAM_IMEI_BLK0_Addr_Seg dw 0

RAM_IMEI_BLK1_Addr_Ofs dw 0

RAM_IMEI_BLK1_Addr_Seg dw 0

RAM_IMEI_BLK2_Addr_Ofs dw 0

RAM_IMEI_BLK2_Addr_Seg dw 0

RAM_IMEI_BLK3_Addr_Ofs dw 0

RAM_IMEI_BLK3_Addr_Seg dw 0

dw 0

Flash_HC_RAM_loc dw 0,0

Flash_HC_RAM_loc2 dw 0,0

HC_From_Flash_IN_RAM dw 0

T3IRQ_Counter dw 0

PhoneID_IN_RAM_1 dw 0, 0, 0, 0, 0, 0, 0, 0 ; just to make sure that nothing gets
                               ; overwritten

PhoneID_2nd_AT_RAM dw 0, 0, 0, 0, 0, 0, 0, 0 ; just to make sure that nothing gets
                                 ; overwritten

__FREIA	ENDS

CSTART_RBANK    REGDEF R0-R15
			
	END
