_AllTonesSwitchedOn:
        calls   _GetMessagesByte
        jnb     r4.0,_mess_rets
        jmps    0DBh,01614h
_AllTonesSwitchedOff:
        calls   _GetMessagesByte
        jnb     r4.1,_mess_rets
        jmps    0DBh,01642h
_KeylockedOn:
        calls   _GetMessagesByte
        jnb     r4.2,_mess_rets
        jmps    0DBh,016D2h
_BatteryEmpty:
        mov     [-r0], r12
        calls   _GetMessagesByte
        mov     r12, [r0+] 
        jnb     r4.3,_mess_rets
        jmps    0E6h,0F658h
_NewSMS:
        mov     [-r0], r12
        mov     [-r0], r13
        mov     [-r0], r14
        mov     [-r0], r15
        calls   _GetMessagesByte
        mov     r15, [r0+] 
        mov     r14, [r0+] 
        mov     r13, [r0+] 
        mov     r12, [r0+] 
        jnb     r4.4,_mess_rets
        jmps    0E5h,0DE28h
_SmsDeleted:
        mov     [-r0], r12
        mov     [-r0], r13
        mov     [-r0], r14
        mov     [-r0], r15
        calls   _GetMessagesByte
        mov     r15, [r0+] 
        mov     r14, [r0+] 
        mov     r13, [r0+] 
        mov     r12, [r0+] 
        jnb     r4.5,_mess_rets
        jmps    0E5h,0DE28h
_MessageSaved:
        mov     [-r0], r12
        mov     [-r0], r13
        mov     [-r0], r14
        mov     [-r0], r15
        calls   _GetMessagesByte
        mov     r15, [r0+] 
        mov     r14, [r0+] 
        mov     r13, [r0+] 
        mov     r12, [r0+] 
        jnb     r4.6,_mess_rets
        jmps    0E6h,0F138h
_mess_rets:
        rets
