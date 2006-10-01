; UMM. Usefull Messages Menu
PATCHUMM1 SECTION CODE WORD AT 0B8A8CAh
 dw sof(0D90A62h), seg(0D90A62h) ; MenuProc_36_Write_message
 dw sof(_PassPredef_message), seg(_PassPredef_message) ; MenuProc_36_Predef_message
 dw sof(0D90B02h), seg(0D90B02h) ; MenuProc_36_Archive
 dw sof(0D90DF2h), seg(0D90DF2h) ; MenuProc_37_Voice_message
 dw sof(0D90DFCh), seg(0D90DFCh) ; MenuProc_37_Cell_broadcast
 dw sof(0D90B2Ch), seg(0D90B2Ch) ; MenuProc_36_Setup
 dw sof(0EBFFDEh), seg(0EBFFDEh) ; MenuProc_82__Help_
PATCHUMM1 ENDS

PATCHUMM2 SECTION CODE WORD AT 0B8A9AEh
 dw sof(0D90A62h), seg(0D90A62h) ;MenuProc_36_Write_message
 dw sof(0D90AEEh), seg(0D90AEEh) ;MenuProc_36_Incoming_
 dw sof(0D90AF8h), seg(0D90AF8h) ;MenuProc_36_Outgoing_
 dw sof(0D90D5Ah), seg(0D90D5Ah) ;MenuProc_37_SMS
PATCHUMM2 ENDS

@PATCH1(0B8A90Eh,"dw 03DFh, 03DFh") ;Archive
@PATCH1(0B8A91Ah,"dw 0C9h")
@PATCH1(0B8A920h,"dw 0260h, 025Fh") ;Voice message
@PATCH1(0B8A92Ch,"dw 0E1h")
@PATCH1(0B8A932h,"dw 00A2h, 00A1h") ;Cell broadcast
@PATCH1(0B8A93Eh,"dw 0DEh")
@PATCH1(0B8A980h,"dw seg(0), sof(0)") ;old 0D90D30h

@PATCH1(0B8A9C2h,"dw 029Fh, 029Eh") ;"Write Message"
@PATCH1(0B8A9CEh,"dw 0CBh")
@PATCH1(0B8A9D4h,"dw 048Dh, 048Dh") ;"Incoming \ "
@PATCH1(0B8A9E0h,"dw 0CBh")
@PATCH1(0B8A9E6h,"dw 048Bh, 048Bh") ;"Outgoing \ "
@PATCH1(0B8A9F8h,"dw 0240h, 0240h") ;"Others"
@PATCH1(0B8AA22h,"dw sof(0D90D30h),seg(0D90D30h)") ;old 0
@PATCH1(0D90B8Ah,"cmp r14, #1")
@PATCH1(0D90B92h,"cmp r14, #2")
@PATCH1(0D90D34h,"sub r1, #1")
@PATCH1(0D90D3Eh,"sub r1, #2")

;-----------------------------------------------------------------
;VDL. Voice Dial/Command Light
PATCHVDL SECTION CODE WORD AT 0E6AB86h
PROCVDL  PROC
         calls   0C1h,0FBA8h
         mov     r12, #0
         mov     [r0+#2Eh], r12
         mov     [r0+#30h], r12
         mov     r15, #0
         mov     [r0+#32h], r15
PROCVDL  ENDP
PATCHVDL ENDS
