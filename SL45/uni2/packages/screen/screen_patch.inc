;---------------------------------------------------------------------------------------
;IdleKeyFlag
@PATCH1(0DAC464h,"calls lol_100") ; orig=> "mov r6, 3F76h
@PATCH1(0DB0362h,"calls lol_1rst_main") ; orig=> cmp r12, #22h
@PATCH1(0DB0C08h,"calls lol_1set") ; orig=> "mov DPP0, #35h
;---------------------------------------------------------------------------------------
@PATCH1(0A2D600h,"mov r12, #1");Всегда показывать\пускать Clear logo
;---------------------------------------------------------------------------------------
;StartDrawMainscreen - Antenna
PATCHANT1 SECTION CODE WORD AT 0DB36C6h
PROCANT1 PROC
        mov     [-r0], r12 
        calls   _FullScreenEvent
        mov     r12, [r0+]
        cmp     r4, #0
        jmpa    cc_NZ, 03A58h ;exit other draw
        mov     [-r0], r12 
        calls   _SelectableOpsosLogo
        mov     r12, [r0+]
        calls   _SignalLevel
        jmpr    cc_UC, 0DB36EEh
PROCANT1 ENDP
PATCHANT1 ENDS
;EndDrawMainscreen - Battery
PATCHBAT1 SECTION CODE WORD AT 0DB3A1Eh
PROCBAT1 PROC
        mov     r12, r8
        calls   _BatteryLevel
        calls   _ExtIndicators
        calls   _AutoStart
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
PROCBAT1 ENDP
PATCHBAT1 ENDS
;---------------------------------------------------------------------------------------
;Indicators
@PATCH2(0DB38C4h,"calls _IndicatorsFill","jmpr cc_UC, 0DB38DAh")
@PATCH2(0DB38E4h,"mov r12, #0","calls _IndicatorsGetPos")
@PATCH2(0DB391Eh,"mov r12, #1","calls _IndicatorsGetPos")
@PATCH2(0DB392Ah,"mov r12, #2","calls _IndicatorsGetPos")
@PATCH2(0DB3940h,"mov r12, #1","calls _IndicatorsGetPos")
@PATCH2(0DB394Ch,"mov r12, #2","calls _IndicatorsGetPos")
@PATCH2(0DB397Eh,"mov r12, #1","calls _IndicatorsGetPos")
@PATCH2(0DB398Ch,"mov r12, #2","calls _IndicatorsGetPos")
@PATCH2(0DB39A0h,"mov r12, #1","calls _IndicatorsGetPos")
@PATCH2(0DB39AEh,"mov r12, #2","calls _IndicatorsGetPos")
@PATCH2(0DB39C2h,"mov r12, #1","calls _IndicatorsGetPos")
@PATCH2(0DB39D0h,"mov r12, #2","calls _IndicatorsGetPos")
@PATCH1(0DB3708h,"calls _AddVibraIndicator");Vibra On\Off
;---------------------------------------------------------------------------------------
;hook getpredefinedstring for button
;for change IdleKeyFlag if menu string
;move->copy if coping
@PATCH1(0C0E424h,"calls _hook_buttontext")
;---------------------------------------------------------------------------------------
;invert\flip screen
@PATCH1(0E40028h,"calls _jaf_1")
@PATCH1(0E4004Ch,"calls _jaf_2")
@PATCH1(0E40168h,"calls _jaf_3")
@PATCH1(0E40072h,"add r11, r6")
@PATCH1(0E40096h,"add r11, r6")
@PATCH1(0E400BAh,"add r11, r6")
@PATCH1(0E400DEh,"add r11, r6")
@PATCH1(0E40102h,"add r11, r6")
@PATCH1(0E40126h,"add r11, r6")
@PATCH1(0E4014Ah,"add r11, r6")
;-----------------------------------------------------------------
;Date\time changes
@PATCH1(0DB0000h,"jmps _DoDateTime")
;-----------------------------------------------------------------
;Date\time\opname\cbmess positions
@PATCH1(0B8E7FEh,"dw pof(_buff4SDM+00h), pag(_buff4SDM)")
@PATCH1(0B8E74Eh,"dw pof(_buff4SDM+20h), pag(_buff4SDM)")
@PATCH1(0B8E77Ah,"dw pof(_buff4SDM+30h), pag(_buff4SDM)")
@PATCH1(0B8E6F6h,"dw pof(_buff4SDM+40h), pag(_buff4SDM)")
@PATCH1(0B8E722h,"dw pof(_buff4SDM+40h), pag(_buff4SDM)")
@PATCH1(0B8E7A6h,"dw pof(_buff4SDM+40h), pag(_buff4SDM)")
@PATCH1(0B8E7D2h,"dw pof(_buff4SDM+58h), pag(_buff4SDM)")
@PATCH1(0B8E82Ah,"dw pof(_buff4SDM+70h), pag(_buff4SDM)")
@PATCH1(0B8E856h,"dw pof(_buff4SDM+70h), pag(_buff4SDM)")
@PATCH1(0B8E882h,"dw pof(_buff4SDM+70h), pag(_buff4SDM)")
@PATCH1(0B8E8AEh,"dw pof(_buff4SDM+80h), pag(_buff4SDM)")
@PATCH1(0DB1012h,"calls _Generate_buff4SDM")
;-----------------------------------------------------------------
;Draw Softs patches
@PATCH1(0C0DD1Ch,"calls _SoftButtonWidth") ;mov r4, #2Fh
@PATCH1(0C0E2B0h,"calls _My_DrawAllSofts") ;hook drawallsofts
@PATCH2(0C0DFDCh,"calls _My_DrawTextSoft","jmpr cc_UC, 0C0E004h") ;hook drawtextsoft
@PATCH1(0C0E2C2h,"calls _HookAfterDrawSofts") ;hook after draw softs
;-----------------------------------------------------------------
@PATCH1(0DB02B2h,"calls _ScreenSaverStartAuto") ;"calls _LockKBD when autokeylocked
@PATCH1(0DAD2B8h,"calls _ScreenSaverStartManual") ;"calls _LockKBD when keylocked manual
