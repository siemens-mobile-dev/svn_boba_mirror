#include "..\..\..\\include\Types.h"
#include "..\classes.h"

#define GetSignalQuality_Send_SignalID 0x12C00080
#define GetSignalQuality_Receive_SignalID 0x12C007C0
#define MainInput_data 0x29C8480C
#define MainInput_CurPos_Offset 0x3

void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x1165AFC9;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x115A4949;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x115A4AD9;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x10E9BCE9;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x10F2A791;
int (*GetImageHeight_int)(wchar_t ImageID)=(int(*)(wchar_t ImageID))0x11657099;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x115DBE8D;
STRID (*int2strID_int)(int num,char * filename,int line_num)=(STRID(*)(int num,char * filename,int line_num))0x1163AB51;
STRID (*Str2ID_int)(const void* wstr,int flag,int len,char * filename,int line_num)=(STRID(*)(const void* wstr,int flag,int len,char * filename,int line_num))0x1163AD11;
int (*TextID2wstr_int)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num)=(int(*)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num))0x1163B17D;
int (*TextGetLength_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x1163B07D;
void (*TextFree_int)(STRID strid,char * filename,int line_num)=(void(*)(STRID strid,char * filename,int line_num))0x1163AFA9;
STRID (*TextCopyId_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x1163AEB5;
int (*Request_SIM_IMSI_Read)(const int* sync,const int unk,char * imsi,char * error_cause)=(int(*)(const int* sync,const int unk,char * imsi,char * error_cause))0x107A12B1;
int (*get_imsi_const)()=(int(*)())0x1070007D;
int (*Request_SL_GetRadioState)(const int* sync,char * level,char * flag,char * error_cause)=(int(*)(const int* sync,char * level,char * flag,char * error_cause))0x29C06015;
int (*GetChipID_int)()=(int(*)())0x10002A01;
