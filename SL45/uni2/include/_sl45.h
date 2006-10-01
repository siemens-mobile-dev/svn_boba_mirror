#define WIDTH                   104 //for analog clock
#define HEIGHT                  80
#define sgn(a)                  ((a)>0?1:((a)<0?-1:0))
#define abs(a)                  ((a)>0?(a):(-a))

#define _NULL                   0
#define NULL                    _NULL

#define END_DATA                0x7FFF

#ifndef _STR_DEF
typedef char far*               STR;
typedef unsigned int far*       WSTR;
#define _STR_DEF
#endif

#ifndef _SIZE_T_DEF
typedef unsigned int    size_t;
#define _SIZE_T_DEF
#endif

typedef unsigned short WORD;

typedef void far* LPVOID;
typedef LPVOID far* LPLPVOID;

#include "pit.h"
#include "heaps.h"
#include "strings.h"
#include "GBS.h"
#include "sound.h"
#include "eeprom.h"
#include "files.h"
#include "time.h"
#include "gfx.h"
#include "gui.h"
#include "network.h"
#include "stdlib.h"
#include "c166.h"

//mp3
extern void MP3Player(int autoplay);
void PlayMP3file(char far* data1, char far* data2);
extern void CreatePlaylistName(struct WSHDR far* plsname);
//afre hook mp3_playlist
void Org_Mp3File(char far* Mp3Struct, long Pos);

//keyboard
extern int GetKeyLockState1(void); //1 - if unlocked
extern void LockKBD(void);
extern void UnLockKBD(void);

//light
extern void TurnLightON(void);
extern void TurnLightIfON(void);

//profiles
extern char GetProfileNum(void);


//serial port
extern int SendStringToHost(const char far *);
extern char far* GetATCmdLine(void);

//addrbook/calllist
extern void CopyNumberToScreen(const char far * number);
extern void CallDialList(void);
extern void CallRecvList(void);
extern void CallMissedList(void);


//after hooks keys
extern int MainScreenDialog_onKey(void far* data, char far* key_data);
extern int SmsEdit_onKey(void far* data, char far* key_data);
extern void DialogConfirmUnlock(void);

//Dictaphone\Voice
extern void VoiceDialing(void);
extern void DictaphoneRecording(void);
extern void DictaphoneStop(void);



//System
extern int PowerOff(void);
extern char Is26MHz;
extern void Set13MHz(void);
extern void Set26MHz(void);

//SMS
typedef struct{
  unsigned int type;  //2- with icon, 6 - without
  unsigned int end;  //0x7FFF
  unsigned int zero;
  unsigned int wstr[50];
}StructUSSDStr;
extern void AddNewCBMess(StructUSSDStr*);
extern void ClearFlashSms(void);
//after hooks
extern void WriteMessageDialog(int Zero1, int Zero2, int Zero2, char far* data);
extern void MenuPredef_message(void);


extern void return_hookSetIdleBackTimer(long ms);
extern void SetTimerAutoKeyLock(char far*data, long ms,void huge* proc ,int _1);


extern unsigned int NumFreeSMS;
extern unsigned int NumMissedSMS;
extern int IsHaveNoFreeSMS(void);
extern int IsHaveMissedSMS(void);
extern int IsHaveMissedAlarm(void);
extern long GetNumMissedCalls(void);



/*
#define OnlyVIPFilter   0A4A466h
#define addrBookFilter  0A4A456h
#define sendMessage3    0C1B164h

#define moveWord        0C7EE0Ah
;       mov     r4, #pof(src)
;       mov     r5, #pag(src)
;       mov     r10, #pof(dest)
;       mov     r11, #pag(dest)
;       mov     r3, #wordCount

#define memcmp          0C78388h
;       mov     r12, #length
;       mov     [-r0], r12
;       mov     r12, #pof(dest)
;       mov     r13, #pag(dest)
;       mov     r14, #pof(src)
;       mov     r15, #pag(src)


#define memset2         0f1b2cch

#define pokeW           0C78484h
#define pokeDW          0C78490h




#define upCase          0DFCE3Ah
;       mov     r12, #char              ; r4=result

#define playVMOFile     0D536B2h

#define storeASCIIZ     0D3B1B2h
#define sendInt2        0D3B120h

#define readEEP         0C22F3Eh
#define writeEEP        0C22F92h

#define playTone        0C30014h
#define playToneifEna   0C300B6h
#define setSoundFlag    0C300D4h
#define getSoundFlag    0C300E0h
#define playDTMF        0C300ECh

#define dspMsgBoxV      0E6F138h
#define dspMsgBoxI      0E6F198h
#define dspString       0CA8B7Ah

#define callR5R4        0C7EE88h
#define runJava         0C808DEh



#define getSignalLvl    0CF1616h
#define getSignalPct    0E54B02h
#define setContrast     0C0CACAh
#define getContrast     0C0CBBCh
#define cekLight        0DE1710h
#define isKeylocked     0A34080h
#define turnLightDimm   0C0B14Eh
#define getIllumination 0C0B13Ch
#define setIllumination 0C0B19Eh
#define turnLightON     0c0af24h
#define turnLightOFF    0c0afc0h
#define turnLightIfON   0c1fbb6h


#define getIMEIasText   0CD9D2Ch
#define _IMEI_DATA      0321344h

#define showDlgI        0e5dd76h
#define showMsgIcon2    0e5ddceh
#define showMsgNoBox    0e5ecd8h
#define sendMsg         0c1aa30h

#define convText2ID     0e6599ch
#define resetTextID     0e65b5eh
#define convWStr2ID     0e65c2eh

#define cekVibraFlag    0de17a6h
#define doVibraOnce     0c3026ch

#define dspAlert        0d60ed4h


#define doDate          0a4b9a2h
;       mov     r4, #1
;       mov     [-r0], r4
;       mov     r4, #7
;       mov     [-r0], r4
;       mov     r12, #pof(dateBuff)
;       mov     r13, #pag(dateBuff)
;       mov     r14, #pof(dstBuff)
;       mov     r15, #pag(dstBuff)

#define doTime          0a4bae4h
;       mov     r4, #1
;       mov     [-r0], r4
;       mov     r4, #24
;       mov     [-r0], r4
;       mov     r12, #pof(timeBuff)
;       mov     r13, #pag(timeBuff)
;       mov     r14, #pof(dstBuff)
;       mov     r15, #pag(dstBuff)

#define cpTime          0a4ac7ch
#define cpDate          0a4ac06h
#define doDateTime      0a4f856h
;       mov     r4, #size
;       mov     [-r0], r4
;       mov     r12, #pof(destBuff)
;       mov     r13, #pag(destBuff)
;       mov     r14, #pof(datTimBuff)
;       mov     r15, #pag(datTimBuff)


#define convertBMP      0dc6068h
;       mov     [-r0], #pag(ptrSize)    -> r1
;       mov     [-r0], #pof(ptrSize)    -> r14
;       mov     [-r0], #pag(dst)        -> r6
;       mov     [-r0], #pof(dst)        -> r9
;       mov     [-r0], #imgheight
;       mov     r13, #pag(src)
;       mov     r12, #pof(src)
;       mov     r15, #imgwidth


#define drawSomeImages  0e6fab2h

#define getOprName      0a2cd5ch
;       mov     r12, #pof(dstBuff)
;       mov     r13, #pag(dstBuff)

#define getCallStatus   0ccc468h
;       mov     r12, #1

#define isIncoming      0ccc4c2h
;       mov     r12, #1

#define fmtNumber       0c78436h
;       mov     r12, #number
;       mov     r13, #pof(dstBuff)
;       mov     r14, #pag(dstBuff)
;       mov     r15, #noOfDigit

#define TimeBuff        0Dh:25DCh       ; DC=mm DE=hh E0=dd E2=mn E4=yy

#define dspDialogCenter 0d78a54h
;       mov     r12, #pof(strList)
;       mov     r13, #pag(strList)

#define _dspAlert       0e5d420h
;       mov     r12, #pof(strList)
;       mov     r13, #pag(strList)
;       mov     [-r0], r13
;       mov     [-r0], r12
;       mov     r12, #pof(wndClass)
;       mov     r13, #pag(wndClass)
;       mov     r14, #0
;       mov     r15, #0

#define dspAlert00      0dab442h
;       mov     r12, #pof(strList)
;       mov     r13, #pag(strList)
;       mov     [-r0], r13
;       mov     [-r0], r12
;       mov     r12, #pof(wndClass)
;       mov     r13, #pag(wndClass)
;-- no need to add r0, #4 ... already done !

#define _dspScroll      0e6f2a6h
;       mov     r12, #0
;       mov     [-r0], r12
;       mov     r12, #pof(par1menu)
;       mov     r13, #pag(par1menu)
;       mov     r14, #pof(strList)
;       mov     r15, #pag(strList)


#define setMicOFF       0xC247E6
#define setMicON        0xC247D6

#define isMultiline     0xA015AC

#define runDelay        0dafc68h
;       mov     r12, #miliseconds
;       mov     r13, #miliseconds
;       mov     r14, #ofs(func)
;       mov     r15, #seg(func)

*/

