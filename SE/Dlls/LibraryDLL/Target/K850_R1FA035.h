#include "..\..\..\\include\Types.h"
#include "..\classes.h"

void (*CoCreateInstance)(char * guid1,char * guid2,void *pInterface)=(void(*)(char * guid1,char * guid2,void *pInterface))0x116BD0BD;
void (*DisplayGC_AddRef)(GC*,void *)=(void(*)(GC*,void *))0x1165AFC9;
int (*Illumination_LedID_SetLevel)(int LED_ID,int level)=(int(*)(int LED_ID,int level))0x115A4949;
int (*Illumination_LedID_Off)(int LED_ID)=(int(*)(int LED_ID))0x115A4AD9;
int (*REQUEST_HPHONEBOOK_READSTRING)(const int* sync,void * buf)=(int(*)(const int* sync,void * buf))0x10E9BCE9;
void (*AB_STR_ITEM_Copy)(AB_STR_ITEM * dest,AB_STR_ITEM * source)=(void(*)(AB_STR_ITEM * dest,AB_STR_ITEM * source))0x10F2A791;
