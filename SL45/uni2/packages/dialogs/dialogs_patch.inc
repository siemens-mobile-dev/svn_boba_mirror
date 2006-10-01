@PATCH1(0DB2332h,"mov r12, #85Eh") ;do you want start?
@PATCH4(0E522E0h,"calls _GetDialogsState0","cmp r4, #1","jmpr cc_Z, 0E522F4h","jmpr cc_UC, 0E522FEh") ;Power On
;-----------------------------------------------------------------
@PATCH1(0CBE1D2h,"jmps _patch_longred")
@PATCH1(0B8CF8Eh,"dw sof(_My_SmsEdit_onKey), seg(_My_SmsEdit_onKey)")
@PATCH3(0DB0698h,"mov r12, #1","calls _GeneralFunc","calls _ClearFlashSMS_confirm")
@PATCH2(0D51AA0h,"calls _ExitDict_confirm","jmps 0D5h,01BAAh") ; dictRecord
@PATCH2(0D51E96h,"calls _ExitDict_confirm","jmps 0D5h,01BAAh") ; dictRecordPaused
@PATCH2(0D52F94h,"calls _ExitDict_confirm","jmps 0D5h,030A4h") ; dictPaused
@PATCH2(0D529ACh,"calls _ExitDict_confirm","jmps 0D5h,030A4h") ; dictPlayed



