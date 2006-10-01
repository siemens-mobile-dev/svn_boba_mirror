;-------------------------------------------------------------------------------
__PLSALLOCHOOK:
        extp    #pag(_pPls),#2
        mov     r4, pof(_pPls)
        mov     r5, pof(_pPls+2)
; clear playlist memory or we'll get garbage at its end
        mov     R12, r4
        mov     R13, r5
        mov     R14, #0
        mov     R15, #8192 ;8kb playlist
        jmps    _memset
;----------------------
__PLSFREEHOOK:
        cmp     r15,#2Bh
        jmpr    cc_NZ,_MMIHeap
        rets
 _MMIHeap:
        jmps    _HeapFree
;----------------------
__jPLSMalloc:
        mov     [-r0], r9
        mov     [-r0], r8
        mov     [-r0], r13
        mov     [-r0], r12
        calls   _PLSMalloc
        mov     r12, [r0+]
        mov     r13, [r0+]
        rets
__jPLSHeapFree:
        mov     [-r0], r9
        mov     [-r0], r8
        mov     [-r0], r13
        mov     [-r0], r12
        calls   _PLSHeapFree
        mov     r12, [r0+]
        mov     r13, [r0+]
        rets
;----------------------
_rand:
        extp    #37h, #1
        movb    rl1, 32ECh        ;DF2EC
        jmpr    cc_NZ, _not_first
        mov     r12, #1
        extp    #37h, #2
        mov     32E8h, r12        ;DF2E8
        mov     32EAh, ZEROS      ;DF2EA
        movb    rl2, #1
        extp    #37h, #1
        movb    32ECh, rl2        ;DF2EC
 _not_first:
        mov     r10, #20077
        extp    #37h, #2
        mov     r4, 32E8h         ;DF2E8
        mov     r5, 32EAh         ;DF2EA
        mov     r11, #16838
        calls   0C7h, 0EE8Eh
        add     r4, #12345
        addc    r5, #0
        extp    #37h, #2
        mov     32E8h, r4
        mov     32EAh, r5
        mov     r10, #0
        mov     r11, #1
        calls   0C7h, 0EFCCh
        mov     r5, #0
        mov     r10, #8000h
        mov     r11, #0
        calls   0C7h, 0EF14h
        extp    #0Dh, #3
        add     r4, 25DCh
        add     r4, 25DEh
        add     r4, 25E0h
        rets
;-------------------------------------------------------------------------------
