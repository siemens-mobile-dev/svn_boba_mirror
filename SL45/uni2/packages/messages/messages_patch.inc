@PATCH1(0E52632h,"jmps _AllTonesSwitchedOn") ;jmps 0DBh, pMessageAllTonesSwitchedOn
@PATCH1(0E52636h,"jmps _AllTonesSwitchedOff") ;jmps 0DBh, pMessageAllTonesOff
@PATCH1(0DAD2B4h,"calls _KeylockedOn") ;calls 0DBh, sub_DB16D2 ;keylocked on
@PATCH1(0E52602h,"jmps _BatteryEmpty") ;jmps 0E6h, MessageBoxWithExclamation ;battery empty
@PATCH1(0E5261Ah,"jmps _NewSMS") ;jmps 0E5h, pShowMessageWithIconAndNoBorder ;new sms
@PATCH1(0D9A55Ah,"calls _SmsDeleted") ;calls pShowMessageWithIconAndNoBorder ;sms deleted
@PATCH1(0D95D9Ah,"calls _MessageSaved") ;calls DisplayMessageBoxV ;message saved
