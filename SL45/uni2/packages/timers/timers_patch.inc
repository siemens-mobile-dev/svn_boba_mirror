@PATCH2(0F08822h,"calls _hookSetIdleBackTimer","nop")
@PATCH1(0C1FB5Ah,"jmps  _My_StartTimerLightOff")
@PATCH2(0C1E418h,"calls _My_StartTimerInputSymbol","jmps 0C1h,0E438h") ;скорость регистрации введенного символа (или как еще назвать) 
@PATCH2(0C1E382h,"calls _My_StartTimerReenterSymbol","jmps 0C1h,0E3A2h") ;Timeout between reentering symbol
@PATCH1(0C1E2E0h,"jmps  _My_StartTimerInquirySymbol");Keyboard inquiry acceleration
@PATCH1(0DB0FC2h,"calls _hookSetTimerAutoKeyLock")