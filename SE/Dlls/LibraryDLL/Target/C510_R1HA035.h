#include "..\..\..\\include\Types.h"
#include "..\classes.h"

#define GetSignalQuality_Send_SignalID 0x12C00040
#define GetSignalQuality_Receive_SignalID 0x12C007C0
#define MainInput_data 0x4BA2654C
#define MainInput_CurPos_Offset 0x8

void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x144CDAA5;
void (*TextObject_SetText)(IRichText * pTextObject,STRID strid)=(void(*)(IRichText * pTextObject,STRID strid))0x14183165;
void (*TextObject_SetFont)(IRichText * pTextObject,void * pFont,const int _0x8000000A,const int _0x7FFFFFF5)=(void(*)(IRichText * pTextObject,void * pFont,const int _0x8000000A,const int _0x7FFFFFF5))0x1449CD75;
int (*Illumination_LedID_SetLevel_Gradually)(int LED_ID,int level,int fade_time)=(int(*)(int LED_ID,int level,int fade_time))0x143A0421;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x1444D9C5;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x142E2905;
int (*RichTextLayout_GetTextWidth)(STRID strid,IRichTextLayout * pRichTextLayout,IUnknown * pFont)=(int(*)(STRID strid,IRichTextLayout * pRichTextLayout,IUnknown * pFont))0x14335245;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x140B6BD9;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x1527FCAD;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x1541F1C9;
STRID (*int2strID_int)(int num,char * filename,int line_num)=(STRID(*)(int num,char * filename,int line_num))0x1401F5BD;
STRID (*Str2ID_int)(const void* wstr,int flag,int len,char * filename,int line_num)=(STRID(*)(const void* wstr,int flag,int len,char * filename,int line_num))0x141818E5;
int (*TextID2wstr_int)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num)=(int(*)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num))0x1418243D;
int (*TextGetLength_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x14183015;
void (*TextFree_int)(STRID strid,char * filename,int line_num)=(void(*)(STRID strid,char * filename,int line_num))0x14182789;
STRID (*TextCopyId_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x14181E89;
int (*Request_SIM_IMSI_Read)(const int* sync,const int unk,char * imsi,char * error_cause)=(int(*)(const int* sync,const int unk,char * imsi,char * error_cause))0x100DFA0D;
int (*get_imsi_const)()=(int(*)())0x100E9461;
int (*Request_SL_GetRadioState)(const int* sync,char * level,char * flag,char * error_cause)=(int(*)(const int* sync,char * level,char * flag,char * error_cause))0x1055B495;
int (*Network_INetworkConnection_GetConnectionInfo)(CONNECTION_INFO_DESC * desc)=(int(*)(CONNECTION_INFO_DESC * desc))0x141CC76D;
int (*GetChipID_int)()=(int(*)())0x10777BA1;
OSBOOLEAN (*get_mem_int)(int,int,void*)=(OSBOOLEAN(*)(int,int,void*))0x101CFE6C;
