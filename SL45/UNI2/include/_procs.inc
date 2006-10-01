@DEFINE PATCH1    (padr,command1) @LOCAL(sss, sssproc)
@sss    SECTION CODE AT @padr
@sssproc PROC FAR
@command1
@sssproc ENDP
@sss ENDS
@ENDD

@DEFINE PATCH2    (padr,command1,command2) @LOCAL(sss, sssproc)
@sss    SECTION CODE AT @padr
@sssproc PROC FAR
@command1
@command2
@sssproc ENDP
@sss ENDS
@ENDD

@DEFINE PATCH3    (padr,command1,command2,command3) @LOCAL(sss, sssproc)
@sss    SECTION CODE AT @padr
@sssproc PROC FAR
@command1
@command2
@command3
@sssproc ENDP
@sss ENDS
@ENDD

@DEFINE PATCH4    (padr,command1,command2,command3,command4) @LOCAL(sss, sssproc)
@sss    SECTION CODE AT @padr
@sssproc PROC FAR
@command1
@command2
@command3
@command4
@sssproc ENDP
@sss ENDS
@ENDD

@DEFINE PATCH5    (padr,command1,command2,command3,command4,command5) @LOCAL(sss, sssproc)
@sss    SECTION CODE AT @padr
@sssproc PROC FAR
@command1
@command2
@command3
@command4
@command5
@sssproc ENDP
@sss ENDS
@ENDD

@DEFINE EQUP    (pname,padr) @LOCAL(sss)
PUBLIC @pname
@sss    SECTION CODE AT @padr
@pname PROC FAR
@pname ENDP
@sss ENDS
@ENDD

@DEFINE EQUB (bname,badr) @LOCAL(sss)
PUBLIC @bname
@sss    SECTION DATA AT @badr & 0FFFFFEh
        ORG     @badr
@bname LABEL BYTE
@sss ENDS
@ENDD

@DEFINE EQUW (bname,badr) @LOCAL(sss)
PUBLIC @bname
@sss    SECTION DATA AT @badr
        ORG     @badr
@bname LABEL WORD
@sss ENDS
@ENDD

;------------------------------------------------
; Ôóíêöèè ñèøíîé áèáëèîòåêè
;------------------------------------------------
@EQUP(__mul,0C7EE8Eh)
@EQUP(__udil,0C7EFCCh)
@EQUP(__umol,0C7F03Eh)
@EQUP(__smol,0C7F03Eh)
@EQUP(__sdil,0C7EEAEh)

;Graphical 
@EQUW(_ScreenBufferA,0960Ch)
@EQUW(_ScreenBufferB,0AC0Ch)

@EQUP(_AM_QueryDisplayOfThisProcess,0D4BE86h)
@EQUP(_GetBitmapResource,0C1296Eh)
@EQUP(_StoreImageAsImageID,0C13F54h)
@EQUP(_DrawImage,0D6A45Ch)
@EQUP(_DrawPredefinedImage,0C122ECh)
@EQUP(_FillRect,0C0E5D0h)
@EQUP(_ClearRect,0D6A20Ah)
@EQUP(_DrawString,0D6A358h)
@EQUP(_STRtoWSTRP,0F19EA0h)
@EQUP(_STRtoWSTR,0C7CE5Ch)
@EQUP(_XYWHtoRectXYXY,0C17CF4h)
@EQUP(_CreateColorBitmap,0C125AEh)
@EQUP(_DrawObject,0C1400Ah)
@EQUP(_PrepDrawObj_type02,0C14B14h)
@EQUP(_PrepDrawObj_type01,0C14A4Ch)
@EQUP(_InvalidateScreenRect,0C0C866h)

@EQUP(_doIDLE,0EE1CACh)

@EQUP(_GetKeyLockState1,0A34092h)
@EQUP(_UnLockKBD,0A3407Ah)
@EQUP(_LockKBD,0A34074h)
@EQUP(_TurnLightON,0C0AF24h)
@EQUP(_TurnLightIfON,0C1FBB6h)


@EQUP(_CreateCSM,0EE1112h)
@EQUP(_CloseCSM,0EE13D0h)
@EQUP(_GetBottomCSM_ID,0EE10CEh) ; ‘ ¯à®¯ãáª®¬ â¥ªãé¥£® ¤¨ «®£ 
@EQUP(_FindCSMbyID,0EE12CEh)

@EQUW(_IDLEDIALOG_ID,0DF290h)
@EQUP(_CreateInputTextDialog,0D64ADEh)

@EQUP(_CreateESTR_Dialog,0D64B26h)
@EQUP(_DisplayMessageBoxV,0E6F138h)

@EQUP(_OnTimer,0D6026Ch)
@EQUP(_OnTimer1,0F07FACh)
@EQUP(_OnTimer2,0F07EC0h)
@EQUP(_DisableTimer,0D60260h)
@EQUP(_StartTimer2,0C1CA86h)
@EQUP(_DisableTimerX,0C1C6DAh)

@EQUP(_AcquireGbsLock,0C1978Eh)
@EQUP(_FreeGbsLock,0C19798h)

@EQUP(_GBSS_po_open,0E49486h)
@EQUP(_GBSS_po_read,0E4952Eh)
@EQUP(_GBSS_po_close,0E497E0h)
@EQUP(_GBSS_po_write,0E49672h)

@EQUP(_FileOpen,0DFA73Eh)
@EQUP(_FileRead,0DFABAEh)
@EQUP(_FileWrite,0DFB0FEh)
@EQUP(_FileClose,0DFC570h)
@EQUP(_FileSeek,0DFB868h)
@EQUP(_GetFileStat,0DFF720h)
@EQUP(_FileStat,0DFF6AAh)

@EQUP(_Rename,0DFE040h)

@EQUP(_FilesysICall,0E47FFCh)

PUBLIC _FilesysICall_I
PUBLIC _FilesysICall_UI
PUBLIC _FilesysICall_L
PUBLIC _FilesysICall_UL
PUBLIC _FilesysICall_FP
PUBLIC _FilesysICall_II

_FilesysICall_I EQU _FilesysICall
_FilesysICall_UI EQU _FilesysICall
_FilesysICall_L EQU _FilesysICall
_FilesysICall_UL EQU _FilesysICall
_FilesysICall_FP EQU _FilesysICall
_FilesysICall_II EQU _FilesysICall


@EQUP(_strcmp,0C784EEh)
@EQUP(_strcmp_nocase,0D7FF66h)

@EQUP(_WriteEEPROMData,0C22F92h)
@EQUP(_ReadEEPROMData,0C22F3Eh)


@EQUP(_PlayTone,0C30014h)

@EQUP(_GetProfileNum,0A49866h)

@EQUP(_CreateModalDialog_0_0,0D60340h)
@EQUP(_CreateModalDialog,0D6028Ah)
@EQUP(_DoBack ,0D60562h)
@EQUP(_DoBack0A,0D63120h)

@EQUP(_GeneralFunc,0F08350h)
@EQUP(_DoEscape,0E6F64Eh)

@EQUP(_SendStringToHost,0C47000h)
@EQUP(_GetATCmdLine,0D38A8Eh)


@EQUP(_CreateMenu02,0E6EC90h)
@EQUP(_SetMenuItemIcon,0E76048h)
@EQUP(_RefreshMenuItem,0E74A24h)
@EQUP(_GetCurrentMenuItem,0E761AEh)


@EQUP(_CopyNumberToScreen,0DE7AF2h)

@EQUB(_KeybQueueIdx,03B9B5h)
@EQUB(_KeybQueueBuf,03B9B6h)
@EQUW(_GbsLock_ifNZ,03AF9Ch)
@EQUW(_GbsLock_ifZ,03AF96h)
@EQUW(_PidAct,03B1DCh)

@EQUW(__minute,0365DCh)
@EQUW(__hour,0365DEh)
@EQUW(__day,0365E0h)
@EQUW(__month,0365E2h)
@EQUW(__year,0365E4h)
@EQUP(_CopyDate,0A4AC06h)
@EQUP(_CopyTime,0A4AC7Ch)
@EQUP(_GetWeekDay,0A4B310h)
@EQUP(_GetDateFormat,0DE190Ch);
@EQUP(_DoDate0,0A4B9A2h)
@EQUP(_DoTime0,0A4BAE4h)
@EQUP(_HaveAlarm,0A4AF88h)
;------------------------------
; ïåðåìåííûå NetMonitor-a
@EQUW(_NETMON_LAC,039D84h)
@EQUW(_NETMON_CI,039D86h)
@EQUW(_CELL6_PNT,0C9B96h)
@EQUW(_NETMON7_TAB,039D8Ch)
@EQUW(_CURCELL_PNT,0C9B3Ah)
@EQUP(_SetBAND,0A2C7FCh)
@EQUP(_GetRXLVLbyCH,0CF3094h)

@EQUB(_NETMON_C1,039D8Fh)
@EQUB(_NETMON_C2,039D90h)

@EQUW(_NETMON_mA,03B9FCh)
@EQUW(_NETMON_RX,039D8Eh)
@EQUW(_NETMON_VB,03B9E2h)
@EQUW(_NETMON_LS,03B9EAh)
@EQUW(_NETMON_TB,03B9E6h)
;------------------------------


@EQUP(_DrawShutdownAnimation,0E5252Ch)

@EQUP(_strlen,0C78536h)
@EQUP(_memset,0C78416h)
@EQUP(_memcpy,0C783B4h)
@EQUP(_memcpyW,0AABF1Ah)
@EQUP(_toupper,0DFCE3Ah)
@EQUP(_strncmp,0C78578h)
@EQUP(_strnicmp,0C787DAh)
@EQUP(_sprintf,0A4884Eh)
@EQUP(_strcpy,0C78516h)
@EQUP(_strncpy,0C7859Eh)

@EQUP(_strcat,0C784A6h)
@EQUP(_strchr,0C784CCh)
@EQUP(_strstr,0C785F0h)
@EQUP(_atol,0C77EBAh)
@EQUP(_atoi,0C782E6h)


@EQUP(_StoreTextAsStringID ,0E6599Ch)
@EQUP(_ReplaceCharInStr ,0CA6C6Eh)
@EQUP(_Word2DigitArray,0C7891Eh)
@EQUP(_FormatNumber,0C78436h)
@EQUP(_GetStringByID1,0E65360h)
@EQUP(_ConcatWSTR,0F1B454h)
@EQUP(_ConvLangSTRToWSHDR,0E65FECh)
@EQUP(_ClearWSTR,0F1B2CCh)
@EQUP(_AddCharToWSTR,0F1B5D0h)
@EQUP(_InsertCharToWSTR,0F1B546h)
@EQUP(_AllocWSTR,0F1B340h)
@EQUP(_CutWSTR,0F1B860h)
@EQUP(_GetStringByID,0E654F8h)
@EQUP(_AsciizToWSTR,0F1B8B6h)
@EQUP(_wstrlen,0F19AA8h)
@EQUP(_ClearInputData,0F063C6h)
@EQUP(_SetInputData,0F0649Ah)
@EQUP(_SetInputType,0F0644Ch)
@EQUP(_AddInputToWSTR,0F07490h)
@EQUP(_sub_E70BAC,0E70BACh)
@EQUP(_sub_E70774,0E70774h)
@EQUP(_sub_F07A66,0F07A66h)
;-----------------------------------
@EQUW(_pid_DIALER_PROCESS,3B788h)
@EQUW(_pid_NETCORE_PROCESS,3B780h)
@EQUW(_pid_INET_TEST_PROCESS,3B778h)
@EQUW(_pid_MMI,3B5E8h)
@EQUW(_pid_MMC_FILE_SYSTEM,3B870h)
@EQUW(_pid_MMC_EXPLORER,3B878h)

@EQUP(_SendMessage,0C1AA30h)
@EQUP(_ReceiveMessage,0C1B2C6h)

@EQUP(__icall,0C7EE88h)

;-----------------------------------
@EQUP(_CallDialList,0DE5616h)
@EQUP(_CallRecvList,0DE563Eh)
@EQUP(_CallMissedList,0DE5704h)
;-----------------------------------
@EQUP(_MainScreenDialog_onKey,0DB02B8h)
@EQUP(_SmsEdit_onKey,0DA7BD2h)
@EQUP(_DialogConfirmUnlock,0DAF6ECh)
 
@EQUP(_ShowMessageWithIconAndNoBorder,0E5DE28h)
@EQUP(_ShowMessageDlg,0E5ECD8h)
@EQUP(_ShowMessageDlgWithIcon,0E5DDCEh)

@EQUP(_MP3Player,0DB86FEh)
@EQUP(_VoiceDialing,0E6AB82h)
@EQUP(_DictaphoneRecording,0D4F314h)
@EQUP(_DictaphoneStop,0C321CEh)

@EQUP(_PowerOff,0CBE1D6h)

@EQUP(_ClearFlashSms,0DB1982h)

@EQUB(_Is26MHz,03B960h)
@EQUP(_Set13MHz,0C1D296h)
@EQUP(_Set26MHz,0C1D2AAh)

@EQUP(_Org_Mp3File,0DBB5AEh)

@EQUP(_Main_Menu_create,0DB40C4h)

@EQUP(_PlayMP3file,0DB8738h)

@EQUP(_SendMessage2toMMCEXPL,0D7D4D8h)
@EQUP(_FreeDataConstructedByEXPL,0D8C726h)

@EQUP(_WriteMessageDialog,0D96322h)

@EQUP(_MenuPredef_message,0D90A78h)

@EQUP(_GetCSMPointer,0E7646Ah)

@EQUP(_CreatePlaylistName,0DB919Eh)

@EQUP(_PleaseWait,0E6F4FAh)

@EQUP(_DrawAllSofts,0C0DFA4h)

@EQUP(_DrawMainScreen,0DB36B8h)

@EQUW(_NumFreeSMS,0369D4h)
@EQUW(_NumMissedSMS,0369D8h)
@EQUP(_IsHaveNoFreeSMS,0A4EEE2h)
@EQUP(_IsHaveMissedSMS,0A4EF00h)
@EQUP(_IsHaveMissedAlarm,0DDD768h)
@EQUP(_GetNumMissedCalls,0A2F9CAh)



@EQUP(_EE_RAW_ClearBlock,0C1FF3Ah)
@EQUP(_EE_RAW_WriteBlock,0C1FF06h)

@EQUP(_AddNewCBMess,0DB1798h)



;---------------------------------------------------------------------------------------------
;HEAPS
;---------------------------------------------------------------------------------------------
@EQUW(_AllHeapsPointers,        039DCEh)
@EQUW(_SIMHeap_mo              ,030E72h);Free 1800 bytes
@EQUW(_DBAL_Heap               ,031A5Ch);Free 7928 bytes
@EQUW(_ImageHeap_mo            ,031DC4h);Free 750 bytes
@EQUW(_OMA_Heap                ,032FBEh);Free 2902 bytes
@EQUW(_DisplayQueue_           ,039D58h);Free 3984 bytes
@EQUW(_SystemFARHeap0          ,039E86h);Free 668 bytes
@EQUW(_SystemFARHeap1          ,039EA2h);Free 13292 bytes
@EQUW(_SystemFARHeap2          ,039EBEh);Free 18 bytes
@EQUW(_SystemFARHeap3          ,039EDAh);Free 16376 bytes
@EQUW(_SystemFARHeap4          ,039F16h);Free 24 bytes
@EQUW(_SystemFARHeap6          ,039F32h);Free 236 bytes
@EQUW(_SystemFARHeap8          ,039F4Eh);Free 16376 bytes
@EQUW(_SystemFARHeap9          ,039F6Ah);Free 30 bytes
@EQUW(_SystemFARHeap10         ,03AF86h);Free 76 bytes
@EQUW(_I2CQueue                ,03B96Ah);Free 496 bytes
@EQUW(_DSM2BFB_my_heap         ,03F106h);Free 746 bytes
@EQUW(_L1Heap                  ,0BF5EEh);Free 0 bytes
@EQUW(_ssHeap                  ,0CAD3Ch);Free 500 bytes
@EQUW(_XRC_my_heap             ,0CBE60h);Free 568 bytes
@EQUW(_EX_Heap                 ,0D3E76h);Free 15718 bytes
@EQUW(_MMIHeap_mo              ,0D7EA0h);Free 14450 bytes
@EQUW(_RAP_Heap_               ,0D9114h);Free 3072 bytes
@EQUW(_FS_Heap                 ,0DB2CEh);Free 1016 bytes
@EQUW(_SSTEXTEHeap_mo          ,036CC2h);Free 90 bytes
@EQUW(_l2farHeap               ,043B6Ch);Free 15200 bytes
@EQUW(_FSS_FarHeap_            ,03951Ch);Free 320 bytes
@EQUW(_MessageMem              ,03B1CCh);Free 1534 bytes
@EQUW(_TimerMem                ,03B940h);Free 406 bytes
@EQUW(_SIZE_1_HEAP             ,043EFEh);Free 126 bytes
@EQUW(_SIZE_2_HEAP             ,043F0Eh);Free 1800 bytes
@EQUW(_SIZE_3_HEAP             ,043F1Eh);Free 0 bytes
@EQUW(_SIZE_4_HEAP             ,043F2Eh);Free 456 bytes
@EQUW(_SIZE_5_HEAP             ,043F3Eh);Free 2736 bytes
@EQUW(_CbsPageHeap             ,0C3548h);Free 450 bytes
@EQUW(_l2Heap                  ,0C951Eh);Free 34 bytes
@EQUW(_rrnbheap                ,0C9BAEh);Free 182 bytes

@EQUP(_FarMemAlloc,0C1A214h)
@EQUP(_FarMemFree,0C1A572h)
@EQUP(_HeapMalloc,0C1BCD8h)
@EQUP(_HeapFree,0C1BE78h)
@EQUP(_EX_heap_malloc_with_lock,0D7D374h)
@EQUP(_EX_heap_free_with_lock,0D7D3B0h)
@EQUP(_MMIMalloc,0D8F55Ah)
@EQUP(_MMIHeapFree,0D8F596h)


@EQUP(_return_hookSetIdleBackTimer,0F08828h)
@EQUP(_SetTimerAutoKeyLock,0F0DABCh)