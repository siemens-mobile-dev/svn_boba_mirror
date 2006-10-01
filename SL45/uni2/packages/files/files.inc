;----------------------------------------------
MMC_func_add:
        calls   0D0h,05E82h   ; А не MP3 ли сейчас плейит?
        cmp     r4, #0
        jmpr    cc_Z,NoMP3
        mov     r12,#sof(MP3_unpause) ;when rets - start unpause
        mov     r13,#seg(MP3_unpause)
        push    r13
        push    r12
        mov     r12,#pof(_pid_MMI)
        mov     r13,#pag(_pid_MMI)
        mov     r14,#0ACh
        mov     r15,#024h
        calls   _SendMessage ; Пауза MP3
WaitMP3stop:
        calls   0D0h,05E82h   ; А не MP3 ли сейчас плейит?
        cmp     r4, #0
        jmpr    cc_NZ,WaitMP3stop
NoMP3:
;---std
        cmp     r8,#40h ;FileSys_icall_func_number
        jmpr    cc_Z,MMC_func_icall
        cmp     r8,#22h
        jmps    MMC_func_add_ret
MMC_func_icall:
        mov     r4,[r0+#0ah]
        mov     r5,[r0+#0ch]
        mov     r12, [r0+#0Eh]
        mov     r13, [r0+#10h]
        calls   __icall
        jmps    0E4h, 0AB48h;MMC_FILE_SYSTEM_proc_exit
;---addon v3.3
MP3_unpause:
        mov     r12,#pof(_pid_MMI)
        mov     r13,#pag(_pid_MMI)
        mov     r14,#0ACh
        mov     r15,#026h
        calls   _SendMessage     ; Play MP3
WaitMP3play:
        calls   0D0h,05E82h   ; А не MP3 ли сейчас плейит?
        cmp     r4, #0
        jmpr    cc_Z,WaitMP3play
        rets

;------------------------
MMC_func_call:
        mov     [-r0],r15
        mov     [-r0],r14
        mov     [-r0],r13
        mov     [-r0],r12
        mov     r12,#pof(_pid_MMC_FILE_SYSTEM)
        mov     r13,#pag(_pid_MMC_FILE_SYSTEM)
        mov     r14,#40h ;FileSys_icall_func_number
        mov     r15,#0
        calls   _SendMessage
        add     r0,#8
        rets
;----------------------------------------------
_bfa_redir:
        mov     r14, r8
        mov     r15, r9
        add     r14, #0B2h
_ebin_load:
        mov     r12, #sof(_bin_load)
        mov     r13, #seg(_bin_load)
        jmps    __FilesysICall
; ---------------------------------------------------------------------------
_bin_load:
        mov       r14, #0
        mov       r15, #0
        calls     _FileOpen
        mov       r8, r4
        cmp       r8, #0FFFFh
        jmpr      cc_Z, _bin_open_error
        mov       r12, r8
        mov       r13, #0
        mov       r14, #20h
        mov       r15, #3FFFh
        calls     _FileRead
        mov       r12, r8
        mov       r13, #0
        mov       r14, #21h
        mov       r15, #3FFFh
        calls     _FileRead
        mov       r12, r8
        mov       r13, #0
        mov       r14, #22h
        mov       r15, #3FFFh
        calls     _FileRead
        mov       r12, r8
        mov       r13, #0
        mov       r14, #23h
        mov       r15, #3FFFh
        calls     _FileRead
        mov       r12, r8
        calls     _FileClose
        calls     8, 0 ;20:0000
        mov       r12, #pof(_pid_MMI)
        mov       r13, #pag(_pid_MMI)
        mov       r14, #085h
        mov       r15, #0
        calls     _SendMessage
 _bin_open_error:
        rets
;----------------------------------------------
_fta2:
        jmpr    cc_UGT, _fta2_run
        jmps    0D8h, 074D2h ;return to std proc
 _fta2_run:
        mov     r12, #sof(_FTA2_main)
        mov     r13, #seg(_FTA2_main)
        mov     r14, r8
        mov     r15, r9
        add     r14, #0B2h
        calls   _FilesysICall
        jmps    0D8h, 07618h ;exit proc
;----------------------------------------------
_bfl_menu:
        cmp     r12, #14h
        jmpr    cc_Z, _pressDUR
        cmp     r12, #5
        jmpr    cc_NZ, _not_press_GREEN
        extp    #20h, #1
        mov     r4, 0
        jmpr    cc_Z, _pressDUR
        jmps    8, 0                    ;start last bin

_not_press_GREEN:
        jmps    0DBh, 40B4h ;return

_pressDUR:
        mov     r14, #pof(_zbin_utilities)
        mov     r15, #pag(_zbin_utilities)
_bfl:
        sub     r0, #22Ch
        mov     r12, r0
        and     r12, #3FFFh
        mov     r13, DPP1
        mov     [-r0], r15
        mov     [-r0], r14
        mov     [-r0], r12
        calls   0D8h, 0E060h
        mov     r12, [r0+]
        add     r12, #31h
        mov     r13, DPP1
        mov     r14, [r0+]
        mov     r15, [r0+]
        calls   _strcpy
;        calls   _doIDLE
        jmps    0D8h, 0E192h ;create CardExplorerDialog

_bfl_games:
        mov     r14, #pof(_zbin_games)
        mov     r15, #pag(_zbin_games)
        jmpr    cc_UC, _bfl

_zbin_utilities:
        db 'A:\zbin\Utilities',0;
_zbin_games:
        db 'A:\zbin\Games',0
;----------------------------------------------
Menu_32_Items:
        dw 0, 0,1422h,1422h, 0, 0, 0, 3, 53Ah;String #1422        
        dw 0, 0, 7C7h, 7C7h, 0, 0, 0, 3, 53Ah;Clear Bitmap        
        dw 0, 0, 189h, 189h, 0, 0, 0, 3, 53Ah;Mark                
        dw 0, 0, 182h, 182h, 0, 0, 0, 3, 53Ah;Unmark              
        dw 0, 0, 17Ch, 17Ch, 0, 0, 0, 3, 53Ah;Delete              
        dw 0, 0, 036h, 036h, 0, 0, 0, 3, 53Ah;Delete all          
        dw 0, 0, 2C4h, 2C4h, 0, 0, 0, 3, 53Ah;Move                
        dw 0, 0, 1DBh, 1DBh, 0, 0, 0, 3, 53Ah;New folder          
        dw 0, 0, 2B9h, 2B9h, 0, 0, 0, 3, 53Ah;Rename              
        dw 0, 0, 221h, 221h, 0, 0, 0, 3, 53Ah;Send via IrDA       
        dw 0, 0, 242h, 242h, 0, 0, 0, 3, 53Ah;Sort                
        dw 0, 0, 7C7h, 7C7h, 0, 0, 0, 3, 53Ah;Clear Bitmap        
        dw 0, 0, 0C8h, 0C8h, 0, 0, 0, 3, 53Ah;Info                
        dw 0, 0, 1B0h, 1B0h, 0, 0, 0, 3, 53Ah;Drive info          
        dw 0, 0, 162h, 162h, 0, 0, 0, 3, 53Ah;!NEW Copy
        dw 0, 0, 7C2h, 7C2h, 0, 0, 0, 3, 53Ah;!NEW Create playlist
        dw 0, 0, 128h, 128h, 0, 0, 0, 3, 53Ah;--------Help--------
        dw 0, 0, 128h, 128h, 0, 0, 0, 3, 53Ah;!NEW Copy to other


Menu_32_Entries:
        dw sof(0D8738Ah), seg(0D8738Ah)         ;""                    
        dw sof(0D87F0Ch), seg(0D87F0Ch)         ;"Clear Bitmap"        
        dw sof(0D88122h), seg(0D88122h)         ;"Mark"                
        dw sof(0D8818Ah), seg(0D8818Ah)         ;"Unmark"              
        dw sof(0D87620h), seg(0D87620h)         ;"Delete"              
        dw sof(0D877E4h), seg(0D877E4h)         ;"Delete all"          
        dw sof(_menu_move),  seg(_menu_move)    ;"Move"                
        dw sof(0D87B00h), seg(0D87B00h)         ;"New folder"          
        dw sof(_menu_rename),  seg(_menu_rename);"Rename"              
        dw sof(0D87DA4h), seg(0D87DA4h)         ;"Send via IrDA"       
        dw sof(0D87F00h), seg(0D87F00h)         ;"Sort"                
        dw sof(0D87F0Ch), seg(0D87F0Ch)         ;"Clear Bitmap"        
        dw sof(0D87F42h), seg(0D87F42h)         ;"Info"                
        dw sof(0D880A0h), seg(0D880A0h)         ;"Drive info"          
        dw sof(_menu_copy), seg(_menu_copy)
        dw sof(_menu_create_pls), seg(_menu_create_pls)
        dw sof(0D8811Ah), seg(0D8811Ah)         ;"--------Help--------"
        dw sof(_menu_copy_to_other), seg(_menu_copy_to_other)

_menu_move:
        extp    #pag(_CopyNumFiles),#2
        mov     pof(_CopyNumFiles), ZEROS ;std
        mov     pof(_CopyNumFiles)+2, ZEROS ;std
        jmps    0D8h,0786Ah ;Move dialog
_menu_rename:
        extp    #pag(_CopyNumFiles),#2
        mov     pof(_CopyNumFiles), ZEROS ;std
        mov     pof(_CopyNumFiles)+2, ZEROS ;std
        jmps    0D8h,07BD0h ;Rename
_menu_copy:
        mov     [-r0], r12
        mov     [-r0], r13
        calls   _GetCSMPointer
        mov     r12, r4
        mov     r13, r5
        calls   _CopyNumFilesSet
        mov     r13, [r0+]
        mov     r12, [r0+]
        jmps    0D8h,0786Ah ;Move dialog
_menu_copy_to_other:
        rets
_menu_create_pls:
        calls   _GetCSMPointer
        mov     r12, r4
        mov     r13, r5
        jmps    _CardExCreatePls
;----------------------------------------------

