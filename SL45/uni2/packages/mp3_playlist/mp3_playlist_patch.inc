;MP3 Playlist Patch v1.0
; reduce playlist size to 1 song on allocate
@PATCH2(0DBB82Eh,"sub r12, #1","nop") ;sub r12, #30
; reduce playlist size to 1 song on free
@PATCH1(0DBB27Ch,"nop") ;jmpr cc_C, 0DBB236h
; increase playlist file size to 8190 bytes
@PATCH1(0DBBA30h,"mov r15, #8190") ;mov r15, #1800
; increase filename length to 126 chars
@PATCH1(0DBB75Ah,"mov r12, #127") ;sub r12, r6 ;add r12, #1
; Disable standart repeat 
@PATCH1(0DBB338h,"jmps 0DBh, 0B3D0h") ;jmpa cc_NZ, loc_DBB3D0
; Hook "Analyze&play mp3 file" function
@PATCH1(0DBB916h,"calls _My_Org_Mp3File")
@PATCH1(0DBBC68h,"calls _My_Org_Mp3File")
@PATCH1(0DBBDCEh,"calls _My_Org_Mp3File")
@PATCH1(0DBC394h,"calls _My_Org_Mp3File")
; hook playlist memory alloc
@PATCH1(0DBCAB6h,"calls __PLSALLOCHOOK") ;calls 0D8h, MMIMalloc(1802)
@PATCH1(0DBCB8Ah,"calls __PLSALLOCHOOK") ;calls 0D8h, MMIMalloc(1805)
; hook mem free
@PATCH1(0D8F5A2h,"jmps __PLSFREEHOOK") ;jmps 0C1h, HeapFree
@PATCH1(0DBC76Ch,"calls __jPLSMalloc") ;DialogMP3GUI_onCreate
@PATCH1(0DBCBDAh,"calls __jPLSHeapFree") ;DialogMP3GUI_onClose
@PATCH1(0DBA14Eh,"calls _MP3UpKey") ;mov r12, #pid_MMI ;KeyPressMP3 Up Key
@PATCH1(0DBA136h,"calls _MP3DownKey") ;mov r12, #pid_MMI ;KeyPressMP3 Down Key

@PATCH1(0D8E1C6h,"jmps _MP3_From_Menu") ;jmps 0DBh, MP3Player
@PATCH1(0D6293Eh,"calls _MP3_From_Menu")
@PATCH1(0DAE3F4h,"calls _MP3_From_Menu")
@PATCH1(0DB06B8h,"calls _MP3_From_Menu")
@PATCH1(0DB3E7Eh,"jmps  _MP3_From_Menu")

@PATCH1(0D875A4h,"calls _MP3_From_Explorer") ;calls 0DBh, PlayMP3file

