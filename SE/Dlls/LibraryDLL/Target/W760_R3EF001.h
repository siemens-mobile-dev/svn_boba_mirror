#include "..\..\..\\include\Types.h"
#include "..\classes.h"

void (*CoCreateInstance)(char * guid1,char * guid2,void *pInterface)=(void(*)(char * guid1,char * guid2,void *pInterface))0x107B9771;
void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x10A6B809;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x10C67F8D;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x10C68181;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x10AE9B3D;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x10C062B9;
int (*GetImageHeight_int)(wchar_t ImageID)=(int(*)(wchar_t ImageID))0x10A68715;
void (*Display_SetBrightnessLevel)(int level)=(void(*)(int level))0x10E5CACD;
