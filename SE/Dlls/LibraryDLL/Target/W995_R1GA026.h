#include "..\..\..\\include\Types.h"
#include "..\classes.h"

#define OrangeLED_ID 4
#define GetSignalQuality_Send_SignalID 0x12C00040
#define GetSignalQuality_Receive_SignalID 0x12C007C0
#define MainInput_data 0x4BA27944
#define MainInput_CurPos_Offset 0x8

void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x143D0681;
void (*TextObject_SetText)(IRichText * pTextObject,STRID strid)=(void(*)(IRichText * pTextObject,STRID strid))0x14170009;
void (*TextObject_SetFont)(IRichText * pTextObject,void * pFont,const int _0x8000000A,const int _0x7FFFFFF5)=(void(*)(IRichText * pTextObject,void * pFont,const int _0x8000000A,const int _0x7FFFFFF5))0x14172F05;
int (*Illumination_LedID_SetLevel_Gradually)(int LED_ID,int level,int fade_time)=(int(*)(int LED_ID,int level,int fade_time))0x143E9CFD;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x144C7E79;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x14339081;
int (*RichTextLayout_GetTextWidth)(STRID strid,IRichTextLayout * pRichTextLayout,IUnknown * pFont)=(int(*)(STRID strid,IRichTextLayout * pRichTextLayout,IUnknown * pFont))0x144232E1;    
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x141AD49D;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x1536C365;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x15504C79;
STRID (*int2strID_int)(int num,char * filename,int line_num)=(STRID(*)(int num,char * filename,int line_num))0x143D67E5;
STRID (*Str2ID_int)(const void* wstr,int flag,int len,char * filename,int line_num)=(STRID(*)(const void* wstr,int flag,int len,char * filename,int line_num))0x142D8069;
int (*TextID2wstr_int)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num)=(int(*)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num))0x1416F325;
int (*TextGetLength_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x1416FED1;
void (*TextFree_int)(STRID strid,char * filename,int line_num)=(void(*)(STRID strid,char * filename,int line_num))0x1416F6B9;
STRID (*TextCopyId_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x1416EDF1;
int (*Request_SIM_IMSI_Read)(const int* sync,const int unk,char * imsi,char * error_cause)=(int(*)(const int* sync,const int unk,char * imsi,char * error_cause))0x100EBBE9;
int (*get_imsi_const)()=(int(*)())0x100F6175;
int (*Request_SL_GetRadioState)(const int* sync,char * level,char * flag,char * error_cause)=(int(*)(const int* sync,char * level,char * flag,char * error_cause))0x10613B11;
int (*Network_INetworkConnection_GetConnectionInfo)(CONNECTION_INFO_DESC * desc)=(int(*)(CONNECTION_INFO_DESC * desc))0x1423E061;
int (*GetChipID_int)()=(int(*)())0x1091275D;
OSBOOLEAN (*get_mem_int)(int,int,void*)=(OSBOOLEAN(*)(int,int,void*))0x101E4940;
