;----------------------------------------------
_CLST1:
        extp    #pag(_wCLST1), #2
        movb    rl1, pof(_wCLST1)
        movb    pof(_wCLST1)+1, rl1
        addb    rl1, #1
        cmpb    rl1, #3
        jmpr    cc_C, _CLST1_000
        movb    rl1, #0
_CLST1_000:
        extp    #pag(_wCLST1), #1
        movb    pof(_wCLST1), rl1
        jmpr    cc_NZ, _CLST1_001
        jmps    _CallMissedList
_CLST1_001:
        cmpb    rl1, #1
        jmpr    cc_NZ, _CLST1_002
        jmps    _CallDialList
_CLST1_002:
        jmps    _CallRecvList
;----------------------------------------------
_CLST1_green:
        extp    #pag(_wCLST1), #2
        movb    rl1, pof(_wCLST1)+1
        movb    pof(_wCLST1), rl1
        jmps    _GetCurrentMenuItem
;----------------------------------------------
_CLST_34_sel:
        mov     [-r0], r12
        calls   _GetCLST        
        mov     r12, [r0+]
        cmp     r4, #3
        jmpr    cc_ULT, _not_CLST_34
        mov     r13, r4
        calls   _CLST_34
        mov     r14, r4
        mov     r15, r5
        rets
 _not_CLST_34:
        mov     r14, #pof(0BA40EAh)
        mov     r15, #pag(0BA40EAh)
        rets
;----------------------------------------------
_jCLST34_STD_HNDL:
        jmps 0DEh, 2D92h

_CLST34_start:
        mov     [-r0], r9
        mov     [-r0], r8
        mov     r1, r12
        mov     r2, r13
        extp    r15, #1
        mov     r1, [r14+#2]
        rets

_CLST4_end:
        calls   0DCh, 2804h
_CLST3_end:
        mov     r4, #0FFFEh
        mov     r8, [r0+]
        mov     r9, [r0+]
        rets

_CLST3_1_HNDL:
        calls   _CLST34_start
        cmp     r1, #23h
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        calls   _CallMissedList
        jmps    _CLST3_end
_CLST3_2_HNDL:
        calls   _CLST34_start
        cmp     r1, #23h
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        calls   _CallRecvList
        jmps    _CLST3_end
_CLST3_3_HNDL:
        calls   _CLST34_start
        cmp     r1, #23h
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        calls   _CallDialList
        jmps    _CLST3_end
;----------------------------------------------

_CLST4_2_HNDL:
        calls   _CLST34_start
        cmp     r1, #23h
        jmpr    cc_Z, _jCallRecvList
        cmp     r1, #20h
        jmpr    cc_Z, loc_E0D838
        cmp     r1, #0Dh
        jmpr    cc_Z, loc_E0D838
        cmp     r1, #21h
        jmpr    cc_Z, loc_E0D83E
        cmp     r1, #0Eh
        jmpr    cc_Z, loc_E0D83E
        jmpr    cc_UC, _jCLST34_STD_HNDL
loc_E0D838:
        calls   sub_E0D8BE
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        jmpr    cc_UC, _jCallRecvList
loc_E0D83E:
        calls sub_E0D8B0
        jmpr cc_NZ, _jCLST34_STD_HNDL
        jmpr cc_UC, _jCallMissedList
_CLST4_1_HNDL:
        calls   _CLST34_start
        cmp     r1, #23h
        jmpr    cc_Z, _jCallMissedList
        cmp     r1, #20h
        jmpr    cc_Z, loc_E0D866
        cmp     r1, #0Dh
        jmpr    cc_Z, loc_E0D866
        cmp     r1, #21h
        jmpr    cc_Z, loc_E0D86C
        cmp     r1, #0Eh
        jmpr    cc_Z, loc_E0D86C
        jmpr    cc_UC, _jCLST34_STD_HNDL
loc_E0D866:
        calls   sub_E0D8BE
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        jmpr    cc_UC, _jCallMissedList
loc_E0D86C:
        calls   sub_E0D8B0
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        jmpr    cc_UC, _jCallDialList
_CLST4_3_HNDL:
        calls   _CLST34_start
        cmp     r1, #23h
        jmpr    cc_Z, _jCallDialList
        cmp     r1, #20h
        jmpr    cc_Z, loc_E0D894
        cmp     r1, #0Dh
        jmpr    cc_Z, loc_E0D894
        cmp     r1, #21h
        jmpr    cc_Z, loc_E0D89A
        cmp     r1, #0Eh
        jmpr    cc_Z, loc_E0D89A
        jmpr    cc_UC, _jCLST34_STD_HNDL
loc_E0D894:
        calls   sub_E0D8BE
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        jmpr    cc_UC, _jCallDialList
loc_E0D89A:
        calls   sub_E0D8B0
        jmpr    cc_NZ, _jCLST34_STD_HNDL
        jmpr    cc_UC, _jCallRecvList

sub_E0D8B0:
        extp    r13, #2
        mov     r8, [r12+#32h]
        mov     r9, [r12+#34h]
        or      r8, r9
        rets
sub_E0D8BE:
        extp    r13, #2
        mov     r1, [r12+#32h]
        mov     r2, [r12+#34h]
        extp    r13, #2
        mov     r8, [r12+#36h]
        mov     r9, [r12+#38h]
        sub     r8, #1
        subc    r9, #0
        sub     r1, r8
        subc    r2, r9
        rets
_jCallDialList:
        calls   _CallDialList
        jmps    _CLST4_end
_jCallRecvList:
        calls   _CallRecvList
        jmps    _CLST4_end
_jCallMissedList:
        calls   _CallMissedList
        jmps    _CLST4_end
;----------------------------------------------
;----------------------------------------------
;----------------------------------------------
_GetKeyPatch:
        jmpr    cc_NZ, loc_C1EAC6
_rets_0:
        movb    rl4, #0
        rets

_standart:
        jmps   0C1h,0EAC6h

loc_C1EAC6:
        calls   _GetKeyLockState1
        cmp     r4, #0
        jmpr    cc_Z, _standart
        calls   _GetCallMenuState
        cmp     r4, #0
        jmpr    cc_Z, _standart

        calls   _GetCallMenuStr
        mov     r13, r4
        mov     DPP0, #pag(_KeybQueueIdx)
        nop     
        movbz   r12, pof(_KeybQueueIdx)
        calls   _func_1
        jmpr    cc_NZ, _standart
_loop:
        calls   _func_2
        jmpr    cc_NZ,_standart
        extp    #pag(_uni_vars),#1
        movb    rh1,[r13]
        cmpb    rh1,#0
        jmpr    cc_NZ,_loop
        movb    rl2, pof(_KeybQueueIdx)
        addb    rl2, #1
        movb    pof(_KeybQueueIdx), rl2
        calls   _Main_Menu_create
        mov       r12, #pof(_pid_MMI)
        mov       r13, #pag(_pid_MMI)
        mov       r14, #085h
        mov       r15, #0
        calls     _SendMessage
        jmpr    cc_UC, _rets_0

_func_2:
        cmp     r12, #0
        jmpr    cc_NZ, _skip
        mov     r12, #20
 _skip:
        sub     r12, #2
_func_1:
        movb    rl1, [r12+#pof(_KeybQueueBuf+1)]
        extp    #pag(_uni_vars),#1
        movb    rh1, [r13+]
        cmpb    rl1, rh1
        rets
;----------------------------------------------
