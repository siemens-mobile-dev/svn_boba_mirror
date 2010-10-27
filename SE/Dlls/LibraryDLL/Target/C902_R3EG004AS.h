#define C902_R3EG004AS
#define A2
#define DB3150

#include "..\..\include\Types.h"
#include "classes.h"

#define GetSignalQuality_Send_SignalID 0x12C00080
#define GetSignalQuality_Receive_SignalID 0x12C007C0
#define MainInput_data 0x4BBAA3E4
#define MainInput_CurPos_Offset 0x3

void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x109B1315;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x10BAE5C5;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x10BAE7C5;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x10A2F641;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x10B4C52D;
int (*GetImageHeight_int)(wchar_t ImageID)=(int(*)(wchar_t ImageID))0x109AE221;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x10DA5331;
STRID (*int2strID_int)(int num,char * filename,int line_num)=(STRID(*)(int num,char * filename,int line_num))0x10F45F6D;
STRID (*Str2ID_int)(const void* wstr,int flag,int len,char * filename,int line_num)=(STRID(*)(const void* wstr,int flag,int len,char * filename,int line_num))0x10F46141;
int (*TextID2wstr_int)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num)=(int(*)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num))0x10F466D5;
int (*TextGetLength_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x10F46585;
void (*TextFree_int)(STRID strid,char * filename,int line_num)=(void(*)(STRID strid,char * filename,int line_num))0x10F4647D;
STRID (*TextCopyId_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x10F46321;
int (*Request_SIM_IMSI_Read)(const int* sync,const int unk,char * imsi,char * error_cause)=(int(*)(const int* sync,const int unk,char * imsi,char * error_cause))0x1128D4A5;
int (*get_imsi_const)()=(int(*)())0x111E4E61;
int (*Request_SL_GetRadioState)(const int* sync,char * level,char * flag,char * error_cause)=(int(*)(const int* sync,char * level,char * flag,char * error_cause))0x4BB44415;
int (*GetChipID_int)()=(int(*)())0x125DDDD5;
int (*ConnectionManager_Connection_GetState_int)(char * buf)=(int(*)(char * buf))0x10C0631D;
