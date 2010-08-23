#define W705_R1GA031
#define A2
#define DB3210

#include "..\..\include\Types.h"
#include "classes.h"

#define OrangeLED_ID 2
#define GetSignalQuality_Send_SignalID 0x12C00040
#define GetSignalQuality_Receive_SignalID 0x12C007C0
#define MainInput_data 0x4BA26C7C
#define MainInput_CurPos_Offset 0x8

void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x144D5BD1;
void (*TextObject_SetText)(IRichText * pTextObject,STRID strid)=(void(*)(IRichText * pTextObject,STRID strid))0x141864ED;
void (*TextObject_SetFont)(IRichText * pTextObject,void * pFont,const int _0x8000000A,const int _0x7FFFFFF5)=(void(*)(IRichText * pTextObject,void * pFont,const int _0x8000000A,const int _0x7FFFFFF5))0x144A4C81;
int (*Illumination_LedID_SetLevel_Gradually)(int LED_ID,int level,int fade_time)=(int(*)(int LED_ID,int level,int fade_time))0x143A6905;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x14455469;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x142E70AD;
int (*RichTextLayout_GetTextWidth)(STRID strid,IRichTextLayout * pRichTextLayout,IFont * pFont)=(int(*)(STRID strid,IRichTextLayout * pRichTextLayout,IFont * pFont))0x14339FF1;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x140B7D2D;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x152E86FD;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x15488EE9;
STRID (*int2strID_int)(int num,char * filename,int line_num)=(STRID(*)(int num,char * filename,int line_num))0x1401F995;
STRID (*Str2ID_int)(const void* wstr,int flag,int len,char * filename,int line_num)=(STRID(*)(const void* wstr,int flag,int len,char * filename,int line_num))0x14184D2D;
int (*TextID2wstr_int)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num)=(int(*)(STRID strid,wchar_t * dest,int maxlen,char * filename,int line_num))0x14185859;
int (*TextGetLength_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x14186395;
void (*TextFree_int)(STRID strid,char * filename,int line_num)=(void(*)(STRID strid,char * filename,int line_num))0x14185B91;
STRID (*TextCopyId_int)(STRID strid,char * filename,int line_num)=(int(*)(STRID strid,char * filename,int line_num))0x141852DD;
int (*Request_SIM_IMSI_Read)(const int* sync,const int unk,char * imsi,char * error_cause)=(int(*)(const int* sync,const int unk,char * imsi,char * error_cause))0x100DF411;
int (*get_imsi_const)()=(int(*)())0x100E8E55;
int (*Request_SL_GetRadioState)(const int* sync,char * level,char * flag,char * error_cause)=(int(*)(const int* sync,char * level,char * flag,char * error_cause))0x10579985;
int (*Network_INetworkConnection_GetConnectionInfo)(CONNECTION_INFO_DESC * desc)=(int(*)(CONNECTION_INFO_DESC * desc))0x141CFF29;
int (*GetChipID_int)()=(int(*)())0x107B7685;
OSBOOLEAN (*get_mem_int)(int,int,void*)=(OSBOOLEAN(*)(int,int,void*))0x101CEC74;
int (*ConnectionManager_Connection_GetState_int)(char * buf)=(int(*)(char * buf))0x14417AB9;
