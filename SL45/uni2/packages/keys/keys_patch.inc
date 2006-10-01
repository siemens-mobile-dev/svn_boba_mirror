@PATCH1(0DE2DCAh,"calls _CLST1_green") ;calls getCurrMnItem
@PATCH1(0DB0502h,"calls _CLST_12") ;idle
@PATCH1(0DABF6Ch,"calls _CLST_12") ;call
@PATCH5(0DE3150h,"mov r12, #1","calls _CLST_34_sel","nop","mov r12, r8","mov r13, r9")
@PATCH5(0DE3182h,"mov r12, #2","calls _CLST_34_sel","nop","mov r12, r8","mov r13, r9")
@PATCH5(0DE31B4h,"mov r12, #3","calls _CLST_34_sel","nop","mov r12, r8","mov r13, r9")
;-----------------------------------------------------------------
@PATCH1(0B8E6DEh,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E70Ah,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E736h,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E762h,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E78Eh,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E7BAh,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E7E6h,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E812h,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E83Eh,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E86Ah,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
@PATCH1(0B8E896h,"dw sof(_My_MainScreenDialog_onKey), seg(_My_MainScreenDialog_onKey)")
;-----------------------------------------------------------------
@PATCH2(0DB02D0h,"nop","nop") ;delete keylock handler proc
@PATCH1(0C1EAC0h,"calls _GetKeyPatch")
