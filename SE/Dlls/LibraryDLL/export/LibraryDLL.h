
typedef struct LIBRARY_DLL_DATA
{
  wchar_t * dll_name; // об€зательное поле.не перемещать!!! необходимо дл€ UnLoadDLL
  int num1;
  void (*dll_GC_PutChar)( GC* gc, int x, int y, int _zero, int zero1, wchar_t wchar );
  int num2;
  int (*dll_AB_READSTRING)( AB_STR_ITEM*, int rec_num, int field_ID );
  int num3;
  void (*dll_DrawString)( STRID, int align, int x1, int y1, int x2, int y2, int unk, int unk1, int pen_color, int brush_color );
  int num4;
  int (*dll_SetFont)( int );
  int num5;
  int (*dll_GetImageWidth)( wchar_t ImageID );
  int num6;
  int (*dll_GetImageHeight)( wchar_t ImageID );
  int num7;
  void (*dll_OrangeLED_Control)( int _free_val, int or_LED_ID, int level, int fade_time );
  int num8;
  void* (*dll_MetaData_Desc_Create)( wchar_t* path, wchar_t* name );
  int num9;
  void (*dll_MetaData_Desc_Destroy)( void* MetaData_Desc );
  int num10;
  wchar_t* (*dll_MetaData_Desc_GetTags)( void* MetaData_Desc, int tagID );
  int num11;
  int (*dll_MetaData_Desc_GetTrackNum)( void* MetaData_Desc, int __NULL );
  int num12;
  int (*dll_MetaData_Desc_GetCoverInfo)( void* MetaData_Desc, char* cover_type, int* size, int* cover_offset );
  int num13;
  void (*dll_RedLED_On)( int __NULL );
  int num14;
  void (*dll_RedLED_Off)( int __NULL );
  int num15;
  void (*dll_Display_SetBrightness)( int display, int brightness );
  int num16;
  int (*dll_Disp_GetStrIdWidth)( STRID strid, int len );
  int num17;
  FONT_DESC* (*dll_GetFontDesc)(  );
  int num18;
  int* (*dll_GetFontCount)(  );
  int end_marker;


#ifdef DLL_PRIVATE_AREA
#include "LibraryDLL_private.h"
#endif

}_LIBRARY_DLL_DATA;
