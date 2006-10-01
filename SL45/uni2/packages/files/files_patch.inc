;---------------------------------------------------------------------------------------
;*** FAM3.2 File Access Modification v3.2 *** (c)Rst7/CBSIE
;Вызов функции R12/R13 в контексте MMC_FILE_SYSTEM
PATCHFAM1 SECTION CODE WORD AT 0E47FFCh
PROCFAM1  PROC
__FilesysICall:
          jmps    MMC_func_call
PROCFAM1  ENDP
PATCHFAM1 ENDS
; Переход на проверку доп. функций.
PATCHFAM2 SECTION CODE WORD AT 0E49EDAh
PROCFAM2  PROC
          jmps    MMC_func_add
MMC_func_add_ret:
PROCFAM2  ENDP
PATCHFAM2 ENDS
;---------------------------------------------------------------------------------------
;*** FNC. Full Name Change v2 *** (c) Viraptor 
PATCHFNC  SECTION CODE WORD AT 0D7FD14h
PROCFNC   PROC
          calls   0C7h, 085EAh ; return 0:0
PROCFNC   ENDP
PATCHFNC  ENDS
;*** Unhide Send Via IrDA Options for JAR file *** (c) RizaPN 
PATCHJAR1 SECTION CODE WORD AT 0D88624h
PROCJAR1  PROC
          jmpr cc_UC, 0D88648h
          nop
          nop
          nop
          nop
          nop
          nop
PROCJAR1  ENDP
PATCHJAR1 ENDS
;---------------------------------------------------------------------------------------
;*** EXR. Explorer Rightsoftkey (v2) *** 
PATCHEXR1 SECTION CODE WORD AT 0B89F98h ;Mark -> Open
        dw 01E8h, 2Dh
PATCHEXR1 ENDS
PATCHEXR2 SECTION CODE WORD AT 0B89F9Eh ;Unmark -> Open 
        dw 01E8h
PATCHEXR2 ENDS

@define exr_doOpenFile  0D8h, 7398h @endd
@define exr_exit        069CEh   @endd
@define cardExpOptions  06436h   @endd
@define cardExpMark     06978h   @endd
@define cardExpMarkAll  064E4h   @endd

PATCHEXR3 SECTION CODE WORD AT 0D863C4h ;rsk
PROCEXR3  PROC
          jmpa    cc_Z, @cardExpOptions
PROCEXR3  ENDP
PATCHEXR3 ENDS
PATCHEXR4 SECTION CODE WORD AT 0D863E4h ;'*'
PROCEXR4  PROC
          jmpa    cc_Z, @cardExpMarkAll
PROCEXR4  ENDP
PATCHEXR4 ENDS
PATCHEXR5 SECTION CODE WORD AT 0D863ECh ;'#'
PROCEXR5  PROC
          jmpa    cc_Z, @cardExpMark
PROCEXR5  ENDP
PATCHEXR5 ENDS

PATCHEXR6 SECTION CODE WORD AT 0D86482h ; end of CardExpOptions ;mov r7, #2Fh
PROCEXR6  PROC
          jmpa    cc_UC, sof(exr_main)
 exr_optExit:
PROCEXR6  ENDP
PATCHEXR6 ENDS

PATCHEXR7 SECTION CODE WORD AT 0D86496h
PROCEXR7  PROC
 exr_main:
          extp    r1, #1
          mov     r7, [r6]
          cmp     r7, #2Fh
          jmpr    cc_Z, 0D86486h ;exr_optExit
          mov     [-r0], r9
          mov     [-r0], r8
          mov     [-r0], r6
          calls   @exr_doOpenFile
          jmpa    cc_UC, @exr_exit
 exr_100:
          extp    r7, #1
          movb    rl1, [r6+#1]
          xorb    rl1, #0FFh
         rets
PROCEXR7  ENDP
PATCHEXR7 ENDS

PATCHEXR8 SECTION CODE WORD AT 0D869A2h ;movb rl1, #0FFh
PROCEXR8  PROC
          calls   exr_100
PROCEXR8  ENDP
PATCHEXR8 ENDS
PATCHEXR9 SECTION CODE WORD AT 0D8653Ch
PROCEXR9  PROC
          xorb rl6, #0FFh
PROCEXR9  ENDP
PATCHEXR9 ENDS
PATCHEXRA SECTION CODE WORD AT 0D86540h
PROCEXRA  PROC
          jmpr cc_UC, 0D8654Ah
PROCEXRA  ENDP
PATCHEXRA ENDS
;---------------------------------------------------------------------------------------
;*** BFA. Binary File Access v2.2 ***
PATCHBFA1 SECTION DATA AT 0B89E6Eh
          db 'bin'
PATCHBFA1 ENDS
PATCHBFA2 SECTION DATA AT 0B89E96h
          db 'bin'
PATCHBFA2 ENDS
PATCHBFA3 SECTION DATA AT 0B89D78h
          db 'A:\zbin',0
PATCHBFA3 ENDS
PATCHBFA4 SECTION DATA AT 0B8A22Ah
          dw sof(EXPL_ext_bin_action)
PATCHBFA4 ENDS
PATCHBFA5 SECTION CODE WORD AT 0D8750Ch
PROCBFA5  PROC
          jmpr    cc_UC, 0D87606h ;bmp EXPL_ext_bin_action:
 EXPL_ext_bin_action:
          calls   _bfa_redir
          jmps    0D8h, 07618h ;EXPL_ext_unknown_action
PROCBFA5  ENDP
PATCHBFA5 ENDS
;---------------------------------------------------------------------------------------
;*** FTA. File Type Association v2.1 ***
PATCHFTA2  SECTION CODE AT 0D874CEh
PROCFTA2   PROC FAR
           jmps    _fta2
PROCFTA2   ENDP
PATCHFTA2  ENDS
;---------------------------------------------------------------------------------------
@EQUW(_strFN,3C800h)
PATCHEBIN SECTION CODE WORD AT 0E47300h
        db      '** EBIN v1.1 ***'
PROCEBIN  PROC
        mov     r4, #0
        jmpr    cc_UC, ebin_main
        mov     r4, #1
        jmpr    cc_UC, ebin_main
        mov     r4, #2
        jmpr    cc_UC, ebin_main
        mov     r4, #3
        jmpr    cc_UC, ebin_main
        mov     r4, #4
        jmpr    cc_UC, ebin_main
        mov     r4, #5
        jmpr    cc_UC, ebin_main
        mov     r4, #6
        jmpr    cc_UC, ebin_main
        mov     r4, #7
        jmpr    cc_UC, ebin_main
        mov     r4, #8
        jmpr    cc_UC, ebin_main
        mov     r4, #9
        jmpr    cc_UC, ebin_main
        mov     r4, #10
        jmpr    cc_UC, ebin_main
        mov     r4, #11
        jmpr    cc_UC, ebin_main
        mov     r4, #12
        jmpr    cc_UC, ebin_main
        mov     r4, #13
        jmpr    cc_UC, ebin_main
        mov     r4, #14
        jmpr    cc_UC, ebin_main
        mov     r4, #15
        jmpr    cc_UC, ebin_main
ebin_main:
        mov     [-r0], r4
        mov     r12, #pof(_strFN)
        mov     r13, #pag(_strFN)
        mov     r14, #pof(_ebin_fn)
        mov     r15, #pag(_ebin_fn)
        calls   _strcpy
        mov     r4, [r0+]
        add     r4, #6161h
        mov     r14, #pof(_strFN)                        ; filename need to be opened
        mov     r15, #pag(_strFN)
        extp    r15, #2
        movb    [r14+#09], rh4                          ; modify 2 character of filename
        movb    [r14+#10], rl4
        jmps    _ebin_load
PROCEBIN  ENDP
_ebin_fn:
        db      'a:\zbin\zaa.bin',0
PATCHEBIN ENDS
;---------------------------------------------------------------------------------------
PATCHEBFL1 SECTION CODE WORD AT 0DABCC8h ;MainMenu patch1
PROCBFL1   PROC
           jmps    _bfl_menu
PROCBFL1   ENDP
PATCHEBFL1 ENDS
PATCHEBFL2 SECTION CODE WORD AT 0DB40B0h ;MainMenu patch2
PROCBFL2   PROC
           jmps    _bfl_menu
PROCBFL2   ENDP
PATCHEBFL2 ENDS

PATCHEBFL3 SECTION CODE WORD AT 0BA274Ah ;help->other
           dw   240h, 240h
PATCHEBFL3 ENDS
PATCHEBFL4 SECTION CODE WORD AT 0BA2774h 
           dw   sof(_bfl_games), seg(_bfl_games)
PATCHEBFL4 ENDS
;---------------------------------------------------------------------------------------
PATCHEXPRM1 SECTION CODE WORD AT 0B8A21Eh
            dw pof(Menu_32_Items), pag(Menu_32_Items)
            dw pof(Menu_32_Entries), pag(Menu_32_Entries)
            dw 17 ;18
PATCHEXPRM1 ENDS
PATCHRENAME SECTION CODE WORD AT 0E4A9D2h ;rename call patch
PROCRENAME  PROC
            calls   _My_Rename
PROCRENAME  ENDP
PATCHRENAME ENDS

