#define W902_R3EG004
#define A2
#define DB3150

#include "..\..\include\Types.h"
#include "classes.h"

#define GetSignalQuality_Send_SignalID 0x12C00080
#define GetSignalQuality_Receive_SignalID 0x12C007C0
#define MainInput_data 0x4BBB61B4
#define MainInput_CurPos_Offset 0x3

void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x109D8A89;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x10BD56FD;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x10BD58FD;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x10A56DC5;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x10B73661;
int (*GetImageHeight_int)(wchar_t ImageID)=(int(*)(wchar_t ImageID))0x109D5995;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x10DC9571;
STRID (*int2strID_int)(int num,char * filename,int line_num)=(STRID(*)(int num,char * filename,int line_num))0x10F699E9;
STRID (*Str2ID_int)(const void* wstr,int flag,int len,char * filename,int line_num)=(STRID(*)(const void* wstr,int flag,int len,char * filename,int line_num))0x10F69BBD;
int (*TextID2wstr_int)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num)=(int(*)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num))0x10F6A151;
int (*TextGetLength_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x10F6A001;
void (*TextFree_int)(STRID strid,char * filename,int line_num)=(void(*)(STRID strid,char * filename,int line_num))0x10F69EF9;
STRID (*TextCopyId_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x10F69D9D;
int (*Request_SIM_IMSI_Read)(const int* sync,const int unk,char * imsi,char * error_cause)=(int(*)(const int* sync,const int unk,char * imsi,char * error_cause))0x112BD9BD;
int (*get_imsi_const)()=(int(*)())0x112190FD;
int (*Request_SL_GetRadioState)(const int* sync,char * level,char * flag,char * error_cause)=(int(*)(const int* sync,char * level,char * flag,char * error_cause))0x4BB4C9F5;
int (*GetChipID_int)()=(int(*)())0x12700A31;
int (*ConnectionManager_Connection_GetState_int)(char * buf)=(int(*)(char * buf))0x10C2D455;
