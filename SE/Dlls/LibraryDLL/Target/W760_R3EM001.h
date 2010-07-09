#include "..\..\..\\include\Types.h"
#include "..\classes.h"

void (*CoCreateInstance)(char * guid1,char * guid2,void *pInterface)=(void(*)(char * guid1,char * guid2,void *pInterface))0x107BC051;
void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x10A6DFAD;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x10C6827D;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x10C6847D;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x10AEC2D5;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x10C08709;
int (*GetImageHeight_int)(wchar_t ImageID)=(int(*)(wchar_t ImageID))0x10A6AEB9;
