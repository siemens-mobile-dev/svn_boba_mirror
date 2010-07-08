typedef struct LIBRARY_DLL_DATA _LIBRARY_DLL_DATA;

typedef struct LIBRARY_DLL_DATA
{
  wchar_t * dll_name; // об€зательное поле.не перемещать!!! необходимо дл€ UnLoadDLL
  void (*dll_GC_PutChar)( LIBRARY_DLL_DATA * data, GC* gc, int x, int y, int _zero, int zero1, wchar_t wchar );
  int (*dll_AB_READSTRING)( LIBRARY_DLL_DATA * data, AB_STR_ITEM*, int rec_num, int field_ID );
  void (*dll_DrawString)( LIBRARY_DLL_DATA * data, STRID, int align, int x1, int y1, int x2, int y2, int unk, int unk1, int pen_color, int brush_color );
  int (*dll_SetFont)( LIBRARY_DLL_DATA * data, int );
  int (*dll_GetImageWidth)( LIBRARY_DLL_DATA * data, wchar_t ImageID );
  int (*dll_GetImageHeight)( LIBRARY_DLL_DATA * data, wchar_t ImageID );
  void (*dll_OrangeLED_Control)( LIBRARY_DLL_DATA * data, int _free_val, int or_LED_ID, int level, int fade_time );
  void* (*dll_MetaData_Desc_Create)( LIBRARY_DLL_DATA * data, wchar_t* path, wchar_t* name );
  void (*dll_MetaData_Desc_Destroy)( LIBRARY_DLL_DATA * data, void* MetaData_Desc );
  wchar_t* (*dll_MetaData_Desc_GetTags)( LIBRARY_DLL_DATA * data, void* MetaData_Desc, int tagID );
  int (*dll_MetaData_Desc_GetTrackNum)( LIBRARY_DLL_DATA * data, void* MetaData_Desc, int __NULL );
  int (*dll_MetaData_Desc_GetCoverInfo)( LIBRARY_DLL_DATA * data, void* MetaData_Desc, char* cover_type, int* size, int* cover_offset );
  void (*dll_RedLED_On)( LIBRARY_DLL_DATA * data, int __NULL );
  void (*dll_RedLED_Off)( LIBRARY_DLL_DATA * data, int __NULL );
  void (*dll_Display_SetBrightness)( LIBRARY_DLL_DATA * data, int display, int brightness );
  int (*dll_Disp_GetStrIdWidth)( LIBRARY_DLL_DATA * data, STRID strid, int len );
  FONT_DESC* (*dll_GetFontDesc)( LIBRARY_DLL_DATA * data );
  int* (*dll_GetFontCount)( LIBRARY_DLL_DATA * data );


#ifdef DLL_PRIVATE_AREA
#include "LibraryDLL_private.h"
#endif

}_LIBRARY_DLL_DATA;


#define LIBRARY_DLL_GC_PutChar(__this__,...) __this__->dll_GC_PutChar(__this__,__VA_ARGS__)
#define LIBRARY_DLL_AB_READSTRING(__this__,...) __this__->dll_AB_READSTRING(__this__,__VA_ARGS__)
#define LIBRARY_DLL_DrawString(__this__,...) __this__->dll_DrawString(__this__,__VA_ARGS__)
#define LIBRARY_DLL_SetFont(__this__,...) __this__->dll_SetFont(__this__,__VA_ARGS__)
#define LIBRARY_DLL_GetImageWidth(__this__,...) __this__->dll_GetImageWidth(__this__,__VA_ARGS__)
#define LIBRARY_DLL_GetImageHeight(__this__,...) __this__->dll_GetImageHeight(__this__,__VA_ARGS__)
#define LIBRARY_DLL_OrangeLED_Control(__this__,...) __this__->dll_OrangeLED_Control(__this__,__VA_ARGS__)
#define LIBRARY_DLL_MetaData_Desc_Create(__this__,...) __this__->dll_MetaData_Desc_Create(__this__,__VA_ARGS__)
#define LIBRARY_DLL_MetaData_Desc_Destroy(__this__,...) __this__->dll_MetaData_Desc_Destroy(__this__,__VA_ARGS__)
#define LIBRARY_DLL_MetaData_Desc_GetTags(__this__,...) __this__->dll_MetaData_Desc_GetTags(__this__,__VA_ARGS__)
#define LIBRARY_DLL_MetaData_Desc_GetTrackNum(__this__,...) __this__->dll_MetaData_Desc_GetTrackNum(__this__,__
#define LIBRARY_DLL_MetaData_Desc_GetCoverInfo(__this__,...) __this__->dll_MetaData_Desc_GetCoverInfo(__this__,__VA_ARGS__)
#define LIBRARY_DLL_RedLED_On(__this__,...) __this__->dll_RedLED_On(__this__,__VA_ARGS__)
#define LIBRARY_DLL_RedLED_Off(__this__,...) __this__->dll_RedLED_Off(__this__,__VA_ARGS__)
#define LIBRARY_DLL_Display_SetBrightness(__this__,...) __this__->dll_Display_SetBrightness(__this__,__VA_ARGS__)
#define LIBRARY_DLL_Disp_GetStrIdWidth(__this__,...) __this__->dll_Disp_GetStrIdWidth(__this__,__VA_ARGS__)
#define LIBRARY_DLL_GetFontDesc(__this__) __this__->dll_GetFontDesc(__this__)
#define LIBRARY_DLL_GetFontCount(__this__) __this__->dll_GetFontCount(__this__)
