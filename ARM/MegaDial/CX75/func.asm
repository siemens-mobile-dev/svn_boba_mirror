        RTMODEL "ARMv4M", "USED"
        RTMODEL "ARMv4T", "USED"
        RTMODEL "ARMv5T", "USED"
        RTMODEL "StackAlign4", "USED"
        RTMODEL "__cpu_mode", "__pcs__interwork"
        RTMODEL "__data_model", "absolute"
        RTMODEL "__dlib_file_descriptor", "0"
        RTMODEL "__endian", "little"
        RTMODEL "__rt_version", "6"

        RSEG CODE:CODE:NOROOT(2)

defadr	MACRO	a,b
	PUBLIC	a
a	EQU	b
	ENDM


;-----------
	defadr malloc,0xA0203C24
        defadr mfree,0xA0203C8C
        defadr memcpy,0xA0EF4908
        defadr zeromem,0xA0219F1C

;File library
	defadr fopen,0xA07CBB08
        defadr fclose,0xA07CBA5C
        defadr fread,0xA07CBB48
	defadr fwrite,0xA07CBC30
        defadr fflush,0xA07CBA8C
        defadr lseek,0xA07CBABC

;GBS library
	defadr GBS_SendMessage,0xA020A770


;GSM library

	defadr isCallActive,0xA050B02A+1
        defadr getConvTime,0xA0554602+1

;CSM library
	defadr	CSM_root,0xA84C9964
	defadr	CreateCSM,0xA07BED04+1
	defadr	FindCSMbyID,0xA07BED16+1
        defadr	IDLECSM_onCreate,0xA02A781C+1

;GUI library
	defadr	DirectRedrawGUI,0xA07C3966+1
	defadr	DirectRedrawGUI_ID,0xA07C3980+1
	defadr	PendedRedrawGUI,0xA07C3A60+1
	defadr  DrawFrameInNativeMenu,0xA07F6316+1
	defadr  GetPaletteAdrByColorIndex,0xA07F85E8+1
	defadr  CreateMenu,0xA07C28DC+1
        defadr	GetDataOfItemByID,0xA08037A8+1
	defadr	SetMenuItemIcon,0xA07C786C+1

;Draw objects library
	defadr	DrawObject,0xA07FC5AA+1
	defadr	SetPropTo_Obj1,0xA089BBA8+1
	defadr	StoreXYWHtoRECT,0xA07FD254+1
	defadr	FreeDrawObject_subobj,0xA089941C+1
        defadr  SetColor,0xA0899366+1
	defadr  DrawString,0xA089BBF8+1

;Strings library
	defadr  strcpy,0xA0EF5104+1
        defadr	strlen,0xA0EF5184+1
	defadr	strstr,0xA0EF52E4+1
	defadr	strcat,0xA0EF5024+1
        defadr  sprintf,0xA0EF373C+1

;Wide strings library
	defadr	AllocWS,0xA07BCA5A+1
	defadr	FreeWS,0xA07BCA96+1
	defadr	CreateLocalWS,0xA07BC9F2+1
	defadr	wstrlen,0xA07BC4A6+1
	defadr	wstrcpy,0xA07BC444+1
	defadr	wstrncpy,0xA07BC458+1
	defadr	wstrcpy_safe,0xA07BCB7A+1
	defadr	wstrcat,0xA07BCBC0+1
	defadr	wsprintf,0xA07BC24A+1
        defadr	wsaddwchar,0xA07BCCE8+1
        defadr  cutwstr,0xA07BCE4E+1
        defadr	ws2ascii,0xA07C1E2C+1
        defadr  asciz2wstr,0xA07BD1F0+1

;------------- misc ------------

	defadr	getDialStringLen,0xA049EEE8
        defadr  getDialString,0xA08A1E88+1
        defadr  openAB,0xA032B500+1
        defadr  CreateEditViewCSM,0xA05CDE68+1
	defadr  Open_NewSMS_Dialog,0xA050E118+1
	defadr  GetCurrMenuItem,0xA07C63AA+1
        defadr	SetNMenuItems,0xA07C681A+1
        defadr  MakeVoiceCallTo,0xA050861C+1;
        defadr	StartMidlet,0xA088E3B4
        defadr  UnPackABrecord,0xA05FA6CE+1
        defadr  FreeUnPackABrecord,0xA055A35A+1
        defadr  ABnumber2str,0xA083F6A2+1
        defadr  ShowMsgInR1,0xA060A540+1

;===================================================================

;	RSEG    PATCH1
;       LDR	R0,=CreateCSM
;        BX	R0


	END
