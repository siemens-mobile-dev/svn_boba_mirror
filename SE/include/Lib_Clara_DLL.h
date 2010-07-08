
#include "..\\include\Lib_Clara.h"
#include "..\\include\Types.h"
#include "..\\Dlls\LibraryDLL\export\LibraryDLL.h"


#define lib_data ((LIBRARY_DLL_DATA*)((EP_DATA *)get_envp(get_bid(current_process()), "elfpackdata"))->LibraryDLL)


#define GC_PutChar( ... ) LIBRARY_DLL_GC_PutChar(lib_data, __VA_ARGS__ )
#define AB_READSTRING( ... ) LIBRARY_DLL_AB_READSTRING(lib_data, __VA_ARGS__ )
#define DrawString( ... ) LIBRARY_DLL_DrawString(lib_data, __VA_ARGS__ )
#define SetFont( ... ) LIBRARY_DLL_SetFont(lib_data, __VA_ARGS__ )
#define GetImageWidth( ... ) LIBRARY_DLL_GetImageWidth(lib_data, __VA_ARGS__ )
#define GetImageHeight( ... ) LIBRARY_DLL_GetImageHeight(lib_data, __VA_ARGS__ )
#define OrangeLED_Control( ... ) LIBRARY_DLL_OrangeLED_Control(lib_data, __VA_ARGS__ )
#define MetaData_Desc_Create( ... ) LIBRARY_DLL_MetaData_Desc_Create(lib_data, __VA_ARGS__ )
#define MetaData_Desc_Destroy( ... ) LIBRARY_DLL_MetaData_Desc_Destroy(lib_data, __VA_ARGS__ )
#define MetaData_Desc_GetTags( ... ) LIBRARY_DLL_MetaData_Desc_GetTags(lib_data, __VA_ARGS__ )
#define MetaData_Desc_GetTrackNum( ... ) LIBRARY_DLL_MetaData_Desc_GetTrackNum(lib_data, __VA_ARGS__ )
#define MetaData_Desc_GetCoverInfo( ... ) LIBRARY_DLL_MetaData_Desc_GetCoverInfo(lib_data, __VA_ARGS__ )
#define RedLED_On( ... ) LIBRARY_DLL_RedLED_On(lib_data, __VA_ARGS__ )
#define RedLED_Off( ... ) LIBRARY_DLL_RedLED_Off(lib_data, __VA_ARGS__ )
#define Display_SetBrightness( ... ) LIBRARY_DLL_Display_SetBrightness(lib_data, __VA_ARGS__ )
#define Disp_GetStrIdWidth( ... ) LIBRARY_DLL_Disp_GetStrIdWidth(lib_data, __VA_ARGS__ )
#define GetFontDesc(  ) LIBRARY_DLL_GetFontDesc(lib_data)
#define GetFontCount(  ) LIBRARY_DLL_GetFontCount(lib_data)
