$CASE
$GENONLY
$DEBUG
$NOLOCALS
$CHECKCPU16
$CHECKBUS18
$NOMOD166                       ; disable the internal set of SAB 80C166 SFRs
@IF( ! @DEFINED(__STDNAMES ) )  
        $STDNAMES(reg166.def)   ; use non-extended set of SFR's by default
@ELSE
        $STDNAMES(@__STDNAMES)  ; use processor specific register definition 
@ENDI

$SEGMENTED

$INCLUDE ("include\_procs.inc")

@EQUB(_D7F76,0D7F76h)
@EQUW(_LogoFlag,32842h)

public _buff4SDM
public _IdleKeyFlag
public _ImgIdx
public _ColonIdx
public _hdr_TmpImg
public _PtrScreenSz
public _SSV
public _CopyNumFiles
public _CopyFileSz
public _CopyCopiedSz
public _CopyTimer
public _Started
public _MP3_Started
public _pPls
public _wCLST1
public _dir_req
public _pEXPL_DATA
public _id_dia
public _Event
public _IsBlankScreen
public _uni_vars

CRAM SECTION DATA AT 0AC004h
_buff4SDM       LABEL WORD
                DS    088h
_IdleKeyFlag    LABEL BYTE
                DS    2
_ImgIdx         LABEL BYTE
                DS    2
_ColonIdx       LABEL BYTE
                DS    2
_hdr_TmpImg     LABEL WORD
                DS    8
_PtrScreenSz    LABEL WORD
                DS    4A0h
_SSV            LABEL WORD
                DS    1Ah
_CopyNumFiles   LABEL WORD
                DS    4
_CopyFileSz     LABEL WORD
                DS    4
_CopyCopiedSz   LABEL WORD
                DS    4
_CopyTimer      LABEL WORD
                DS    4
_Started        LABEL WORD
                DS    2
_MP3_Started    LABEL WORD
                DS    2
_pPls           LABEL WORD
                DS    4
_wCLST1         LABEL WORD
                DS    2
_dir_req        LABEL WORD
                DS    22h
_pEXPL_DATA     LABEL WORD
                DS    4
_id_dia         LABEL WORD
                DS    2
_Event          LABEL WORD
                DS    2
_IsBlankScreen  LABEL WORD
                DS    2
_uni_vars       LABEL WORD
                DS    2
CRAM ENDS

NAME    XCSTART
        
EXTERN _initproc:FAR
;EXTERN _shutdown:FAR
EXTERN _timerproc:FAR
EXTERN _AutoStart:FAR
EXTERN _FullScreenEvent:FAR
EXTERN _SelectableOpsosLogo:FAR
EXTERN _SignalLevel:FAR
EXTERN _AddVibraIndicator:FAR
EXTERN _IndicatorsFill:FAR
EXTERN _IndicatorsGetPos:FAR
EXTERN _BatteryLevel:FAR
EXTERN _LightWhenCharge:FAR
EXTERN _ExtIndicators:FAR

EXTERN _jaf_1:FAR
EXTERN _jaf_2:FAR
EXTERN _jaf_3:FAR
EXTERN _Generate_buff4SDM:FAR
EXTERN _DoDateTime:FAR
EXTERN _SoftButtonWidth:FAR
EXTERN _My_DrawTextSoft:FAR
EXTERN _HookAfterDrawSofts:FAR
EXTERN _ScreenSaverStartManual:FAR
EXTERN _ScreenSaverStartAuto:FAR

EXTERN _GetCLST:FAR
EXTERN _CLST_12:FAR
EXTERN _CLST_34:FAR
EXTERN _My_MainScreenDialog_onKey:FAR
EXTERN _GetCallMenuState:FAR
EXTERN _GetCallMenuStr:FAR

EXTERN _GetMessagesByte:FAR
EXTERN _GetDialogsState0:FAR
EXTERN _patch_longred:FAR
EXTERN _My_SmsEdit_onKey:FAR
EXTERN _ClearFlashSMS_confirm:FAR
EXTERN _ExitDict_confirm:FAR

EXTERN _My_Rename:FAR
EXTERN _CopyNumFilesSet:FAR
EXTERN _CardExCreatePls:FAR

EXTERN _FTA2_main:FAR
EXTERN _MenuUNISetup:FAR

EXTERN _My_Org_Mp3File:FAR
EXTERN _PLSMalloc:FAR
EXTERN _PLSHeapFree:FAR
EXTERN _MP3UpKey:FAR
EXTERN _MP3DownKey:FAR
EXTERN _MP3_From_Menu:FAR
EXTERN _MP3_From_Explorer:FAR
EXTERN _GSN_F:FAR

EXTERN _PassPredef_message:FAR
EXTERN _PassWrite_message:FAR
EXTERN _My_DrawAllSofts:FAR

EXTERN _hookSetIdleBackTimer:FAR
EXTERN _My_StartTimerLightOff:FAR
EXTERN _My_StartTimerInputSymbol:FAR
EXTERN _My_StartTimerReenterSymbol:FAR
EXTERN _My_StartTimerInquirySymbol:FAR
EXTERN _hookSetTimerAutoKeyLock:FAR

public _rand
public _rets
public _SetExtSIDTable
public __binfile

public _CLST1
public _CLST3_1_HNDL
public _CLST3_2_HNDL
public _CLST3_3_HNDL
public _CLST4_1_HNDL
public _CLST4_2_HNDL
public _CLST4_3_HNDL


SECTASM SECTION CODE WORD PUBLIC 'CASM'
assume DPP3:SYSTEM
PROCASM PROC FAR
_rets:
        rets
_jMenuUNISetup:
        jmps    _MenuUNISetup
__binfile:
        jmps    8, 0 ;20:0000
;----------------------------------------------
_SetExtSIDTable:
        mov     r4, r12
        mov     r5, r13
__SetExtSIDTable:
        extp    #2Fh, #2
        mov     0BF514h, r4     ; (002F:3514)
        mov     0BF516h, r5     ; (002F:3516)
        rets

_ExtendedSID:
        cmp     r12, #0E27h
        jmpr    cc_NC, _ExtendedSID_proc
        jmps    0E6h, 5420h     ;return std langpack proc

_ExtendedSID_proc:
        extp    #2Fh, #2
        mov     r4, 0BF514h     ; (002F:3514)
        mov     r5, 0BF516h     ; (002F:3516)
        sub     r12, #0E27h
        shl     r12, #2
        add     r4, r12
        extp    r5, #2
        mov     r14, [r4+]
        mov     r15, [r4]
        mov     r12, r8
        mov     r13, r9
        jmps    0E6h, 543Ah     ; strcpy() & rets
;----------------------------------------------
;----------------------------------------------
;----------------------------------------------
;----------------------------------------------
;----------------------------------------------
;----------------------------------------------
$INCLUDE ("packages\gsn_f\gsn_f.inc")
$INCLUDE ("packages\screen\screen.inc")
$INCLUDE ("packages\files\files.inc")
$INCLUDE ("packages\keys\keys.inc")
$INCLUDE ("packages\messages\messages.inc")
$INCLUDE ("packages\mp3_playlist\mp3_playlist.inc")
$INCLUDE ("packages\timers\timers.inc")

;----------------------------------------------
;----------------------------------------------
;----------------------------------------------
;----------------------------------------------

PROCASM ENDP
SECTASM ENDS

$INCLUDE ("packages\dialogs\dialogs_patch.inc")
$INCLUDE ("packages\files\files_patch.inc")
$INCLUDE ("packages\gsn_f\gsn_f_patch.inc")
$INCLUDE ("packages\keys\keys_patch.inc")
$INCLUDE ("packages\messages\messages_patch.inc")
$INCLUDE ("packages\mp3_playlist\mp3_playlist_patch.inc")
$INCLUDE ("packages\other\other_patch.inc")
$INCLUDE ("packages\passwords\passwords_patch.inc")
$INCLUDE ("packages\screen\screen_patch.inc")
$INCLUDE ("packages\setup\setup_patch.inc")
$INCLUDE ("packages\system\system_patch.inc")
$INCLUDE ("packages\timers\timers_patch.inc")
