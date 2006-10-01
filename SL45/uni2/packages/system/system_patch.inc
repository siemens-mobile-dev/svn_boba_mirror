@PATCH1(0EEFFC0h,"calls _initproc");init from ISS
;@PATCH1(0EEFFE0h,"calls _shutdown");shut from ISS autosave settings
@PATCH1(0E52E40h,"calls _timerproc");in timer 0xB8 ~2 sec 
@PATCH2(0E6541Ah,"jmps _ExtendedSID","nop");*** ESI. Extended firmware String ID ***
@PATCH1(0E47EC0h,"jmps __SetExtSIDTable");old proc place

;This fix allows you to write patches that disable interrupts for a very
;long period of time. Of cause you should also use T3/P4.1 trick to serve WDT.
;Additionally this patch fixes several traps that occure after applying 
;some other patches (like the phone switching off in "Prnce of Persia").
;You should not commend these lines as they would cause no harm when the
;firmware operates in a normal way (the patched code is never executed).
@PATCH1(0C41FE4h,"nop")
@PATCH1(0C41FF2h,"nop")
@PATCH1(0C42006h,"nop")
@PATCH1(0C4200Ch,"nop")
@PATCH1(0C41F28h,"nop")
@PATCH1(0C41F44h,"nop")
@PATCH1(0C0C20Ah,"nop")
@PATCH1(0C2B9DAh,"jmpr cc_UC, 0C2B9E2h")
@PATCH1(0C2D116h,"jmpr cc_UC, 0C2D11Eh")
@PATCH1(0CB0A9Eh,"nop")
@PATCH2(0C15E12h,"mov r12, #0Fh","mov [r0+#392h], r12");drawobj_txt
@PATCH1(0CDCA7Eh,"jmpa cc_UC, 0CB30h")
@PATCH1(0CB0BF4h,"nop")
@PATCH1(0C14784h,"nop");on more then 80h draw objects

;*** OpenBFB для SL45v56 *** (c) Sinclair =) 
@PATCH1(0C00290h,"db 00")
