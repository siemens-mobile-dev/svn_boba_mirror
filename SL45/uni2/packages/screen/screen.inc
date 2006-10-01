;----------------------------------------------
lol_1set:
        mov     DPP0, #35h
        mov     r2, #1
        extp    #pag(_IdleKeyFlag), #1
        movb    pof(_IdleKeyFlag), rl2
        rets
lol_1rst_main:
        cmp     r12, #1
        jmpr    cc_ULE, lol_1ret
lol_1reset:
        mov     r4, #0
        extp    #pag(_IdleKeyFlag), #1
        movb    pof(_IdleKeyFlag), rl4
lol_1ret:
        cmp     r12, #22h
        rets
lol_100:
        mov     r6, pof(_D7F76)
        jmpr    cc_UC, lol_1reset
_hook_buttontext:
;_hook_menu:
        movb    rl2, #0
        cmp     r12, #1A0h
        jmpr    cc_NZ, _not_menuID
        movb    rl2, #1
 _not_menuID:
        extp    #pag(_IdleKeyFlag), #1
        movb    pof(_IdleKeyFlag), rl2
;_hook_move: 
        cmp     r12, #2C6h
        jmpr    cc_NZ, _not_moveID
        extp    #pag(_CopyNumFiles),#1
        mov     r4, pof(_CopyNumFiles)
        jmpr    cc_Z, _not_moveID
        mov     r12, #162h
 _not_moveID:
        jmps    0E6h, 054F8h ;pGetStringByID
;----------------------------------------------
