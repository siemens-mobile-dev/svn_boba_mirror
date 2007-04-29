$EXTEND
$NOMOD166
$STDNAMES(reg.def)
$SEGMENTED
$CASE
$NOWARNING(120)
$NOWARNING(122)
$MODEL(LARGE)
	NAME	MAIN_C
	ASSUME	DPP3:SYSTEM
;	ASSUME	DPP2:C166_DGROUP

@DEFINE	EQUP	(pname,padr) @LOCAL(sss)
PUBLIC @pname
@sss	SECTION CODE AT @padr
@pname PROC FAR
@pname ENDP
@sss ENDS
@ENDD

@DEFINE	EQUB (bname,badr) @LOCAL(sss)
PUBLIC @bname
@sss	SECTION DATA AT @badr & 0FFFFFEh
	ORG	@badr
@bname LABEL BYTE
@sss ENDS
@ENDD

@DEFINE	EQUW (bname,badr) @LOCAL(sss)
PUBLIC @bname
@sss	SECTION DATA AT @badr
	ORG	@badr
@bname LABEL WORD
@sss ENDS
@ENDD
;------------------------------------------------
; Функции сишной библиотеки
;------------------------------------------------
@EQUP(__mul,0C7EE8Eh)
@EQUP(__udil,0C7EFCCh)
@EQUP(__umol,0C7F03Eh)
@EQUP(__icall,0C7EE88h)

;---------------------------------------------------------------
;---------------------------------------------------------------
@EQUP(_doIDLE,0EE1CACh)
@EQUW(_IDLEDIALOG_ID,0DF290h)

@EQUP(_GetKeyLockState1,0A34092h)
@EQUP(_turnLightON,0C0AF24h)

@EQUP(_pShowDialog,0D60340h)
@EQUP(_doBack0A,0d63120h)

@EQUP(_CreateDialogWithSoftKeys,0EE1112h)
@EQUP(_CloseDialogWithSoftKeys,0EE13D0h)
;@EQUP(_GetBotDialogWSK_ID,0EE10CEh)
@EQUP(_GetBotDialogWSK_ID,0EE10DEh) ; С пропуском текущего диалога

@EQUP(_FindDialogWithSoftKey_by_ID,0EE12CEh)

@EQUP(_DrawString,0D6A358h)
@EQUP(_FillRect,0C0E5D0h)
@EQUP(_OnTimer,0D6026Ch)
@EQUP(_OnTimer1,0F07FACh)
@EQUP(_OnTimer2,0F07EC0h)
@EQUP(_DisableTimer,0D60260h)

@EQUP(_StartTimer2,0C1CA86h)
@EQUP(_DisableTimerX,0C1C6DAh)

@EQUP(_drawImage,0D6A45Ch)

@EQUP(_AcquireGbsLock,0C1978Eh)
@EQUP(_FreeGbsLock,0C19798h)

@EQUP(_WriteEEPROMData,0C22F92h)
@EQUP(_ReadEEPROMData,0C22F3Eh)


@EQUP(_ReceiveMessage,0C1B2C6h)
@EQUP(_SendMessage,0C1AA30h)

PUBLIC _SendMessage4
_SendMessage4 EQU _SendMessage

@EQUP(_GBSS_po_open,0E49486h)
@EQUP(_GBSS_po_read,0E4952Eh)
@EQUP(_GBSS_po_close,0E497E0h)
@EQUP(_GBSS_po_write,0E49672h)

@EQUP(_FileOpen,0DFA73Eh)
@EQUP(_FileRead,0DFABAEh)
@EQUP(_FileWrite,0DFB0FEh)
@EQUP(_FileClose,0DFC570h)
@EQUP(_FileSeek,0DFB868h)
@EQUP(_FileErase,0DFE5D0h)

@EQUP(_FilesysICall,0E47FFCh)

PUBLIC _FilesysICall_I
PUBLIC _FilesysICall_UI
PUBLIC _FilesysICall_L
PUBLIC _FilesysICall_UL
PUBLIC _FilesysICall_FP

_FilesysICall_I EQU _FilesysICall
_FilesysICall_UI EQU _FilesysICall
_FilesysICall_L EQU _FilesysICall
_FilesysICall_UL EQU _FilesysICall
_FilesysICall_FP EQU _FilesysICall

@EQUP(_xsprintf,0A4884Eh)
@EQUP(_strlen,0C78536h)
@EQUP(_memset,0C78416h)
@EQUP(_memcpy,0C783B4h)
@EQUP(_memcpyW,0AABF1Ah)
@EQUP(_memcmp,0C78388h)
@EQUP(_memmove,0C783DCh)
@EQUP(_strcpy,0C78516h)
@EQUP(_strcmp,0C784EEh)
@EQUP(_strncmp,0C78578h)

PUBLIC _FAM3_sig
_FAM3_sig_sss	SECTION CODE AT 0E47FFCh
_FAM3_sig LABEL WORD
_FAM3_sig_sss ENDS

PUBLIC __binfile
__binfile_sss	SECTION CODE AT 080000h
__binfile LABEL WORD
__binfile_sss ENDS

;@EQUB(_buf8k,082000h)

@EQUB(_keybQueneIdx,03B9B5h)
@EQUB(_keybQueneBuf,03B9B6h)
@EQUW(_GbsLock_ifNZ,03AF9Ch)
@EQUW(_GbsLock_ifZ,03AF96h)
@EQUW(_PidAct,03B1dch)

@EQUW(_AllHeapsPointers,039DCEh)
@EQUP(_FarMemAlloc,0C1A214h)
@EQUP(_FarMemFree,0C1A572h)
@EQUP(_HeapMalloc,0C1BCD8h)
@EQUP(_HeapFree,0C1BE78h)

@EQUP(_EX_heap_malloc_with_lock,0D7D374h)
@EQUP(_EX_heap_free_with_lock,0D7D3B0h)

;@EQUP(_GetVars,0E47D10h)
;@EQUP(_FreeVars,0E47D14h)


@EQUP(_GeneralFunc,0F08350h)
@EQUP(_DoEscape,0E6F64Eh)
;@EQUP(_print,0C47000h)
@EQUP(_CreateMenu02,0E6EC90h)
@EQUP(_SetMenuItemIcon,0E76048h)
@EQUP(_RefreshMenuItem,0E74A24h)
@EQUP(_GetCurrentMenuItem,0E761AEh)

@EQUP(_CopyDate,0A4AC06h)
@EQUP(_CopyTime,0A4AC7Ch)

@EQUW(__hour,0365DEh)
@EQUW(__minute,0365DCh)
@EQUW(__day,0365E0h)
@EQUW(__month,0365E2h)
@EQUW(__year,0365E4h)

@EQUW(_NETMON_LAC,039D84h)
@EQUW(_NETMON_CI,039D86h)
@EQUW(_CELL6_PNT,0C9B96h)
@EQUW(_CURCELL_PNT,0C9B3Ah)
@EQUW(_NETMON7_TAB,039D8Ch)
@EQUP(_SetBAND,0A2C7FCh)
@EQUP(_GetRXLVL_by_CH,0CF3094h)

@EQUP(_STRtoWSTRP,0F19EA0h)
@EQUP(_DrawObject,0C1400Ah)
@EQUP(_InvalidateScreenRect,0C0C866h)
@EQUP(_PrepDrawObj_type02,0C14B14h)
@EQUP(_PrepDrawObj_type01,0C14A4Ch)

@EQUP(_DisplayMessageBoxV,0E6F138h)
@EQUP(_ResetStringID,0E65B5Eh)
@EQUP(_StoreTextAsAStringID,0E6599Ch)

@EQUP(_playToneifEna,0C300B6h)

@EQUP(_IsCallActive,0A300B2h)

;===================================================
; TCP/IP
;====================================================
@EQUP(_recv,0D07D82h)
@EQUP(_send,0D07ACEh)
@EQUP(_shutdown,0D077D2h)
@EQUP(_closesocket,0D0756Ch)
@EQUP(_CreateSocketAndConnect,0D1B4ACh)
@EQUP(_LoadJavaNETprofile,0C8AF54h)
@EQUP(_UpNETCORE,0D0F726h)
@EQUP(_Send7_to_NETCORE_PROCESS,0D0F65Ah)
@EQUP(_Send8_to_NETCORE_PROCESS,0D0F69Ah)
@EQUP(_Send9_to_NETCORE_PROCESS,0D0F6E0h)
@EQUP(_gethostbyname,0D08582h)
@EQUW(_socklasterr,0CAFD4h)

NAME	XCSTART			; module name.
	
EXTERN	_main:FAR

PUBLIC __CSTART
__CSTART_MY	SECTION CODE WORD PUBLIC 'CMYINIT'
__CSTART 	PROC  FAR
	jmps	_main
__CSTART	ENDP

PUBLIC _setExtSIDTable
_setExtSIDTable PROC FAR
	MOV		R4,R12
	MOV		R5,R13
	jmps	0E4h, 07EC0h
_setExtSIDTable ENDP

PUBLIC __exit
__exit PROC FAR
	mov		r12,#sof(_FreeGbsLock)
	mov		r13,#seg(_FreeGbsLock)
	push	r13
	push	r12
	calls	_AcquireGbsLock
	mov		r12,#01EDAh
	mov		r13,#00Eh
	mov		r14,#pof(__CSTART)
	mov		r15,#pag(__CSTART)
	jmps	_HeapFree
__exit	ENDP

__CSTART_MY	ENDS
	REGDEF	R0-R15

END