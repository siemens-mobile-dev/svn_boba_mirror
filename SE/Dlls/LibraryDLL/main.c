#include "..\..\\include\Lib_Clara.h"
#include "..\..\\include\Types.h"
#include "..\..\\include\dll.h"
//#include "classes.h"


#define DLL_PRIVATE_AREA  // использовать ТОЛЬКО!! при компиляции самой DLL

#include "export\LibraryDLL.h"

void elf_exit(void)
{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}

// -----------------  Dll_System area  -----------------
// данные самой DLL. их копии не создаются.

volatile int usage_count;
#ifdef A2
IFont * pFont;
FONT_DESC font_desc;
int font_count;
#endif


// -------------------  Public area  -------------------
// Свойсива и методы доступные снаружи.
// Внутри методов доступна Private area.
// Для читабельности имена методов переопределены в h-файле.
// Чтобы не было проблем при компиляции с несколькими DLL, имена - DLLFILENAME_FunctionName.
//

/*void remove_tail_call()
{
}*/

#ifdef A2
void dll_GC_PutChar_0( GC* gc, int x, int y, int width, int height, wchar_t imageID )
{
  IImageManager * pImageManager=0;
  IIconImage * pIconImage=0;
  IUnknown * pGC =0;
  
  CoCreateInstance(CImageManagerCreator_guid,IImageManager_guid,&pImageManager);
  pImageManager->CreateIconImage_Internal(imageID,&pIconImage);

  DisplayGC_AddRef(gc,&pGC);
  
  pImageManager->DrawIconEx(pIconImage,pGC,x,y,width,height);
  
  if (pImageManager) pImageManager->Release();
  if (pIconImage) pIconImage->Release();
  if (pGC) pGC->Release();
}
#endif

#ifdef A2
int dll_AB_READSTRING_0( AB_STR_ITEM* ab, int rec_num, int field_ID )
{
  int res = 1;
  char * buf = new char[0x420];
  buf[0xE]=0;
  AB_GET_STRUCT * x = (AB_GET_STRUCT*)rec_num;
  AB_GET_STRUCT * y = (AB_GET_STRUCT*)(buf+0x4);
  y->unk_02=x->unk_02;
  y->rec_num=x->rec_num;
  buf[0x416]=field_ID;
  switch(field_ID)
  {
  default:
    buf[0x12]=2;
    break;
  case 0xC:
  case 0xD:
  case 0xE:
  case 0xF:
  case 0x10:
    buf[0x12]=6;
    break;
  }
  if (!REQUEST_HPHONEBOOK_READSTRING(SYNC,buf)&&buf[0]!=0) AB_STR_ITEM_Copy(ab,(AB_STR_ITEM*)(buf+0x10));
  else res=0;
  delete(buf);
  return(res);
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
void dll_DrawString_0( STRID strid, int align, int x1, int y1, int x2, int y2, int unk, int unk1, int pen_color, int brush_color )
{
  WINDOW_RECT rect;
  
  int right_border = x2-x1;
  
  ITextRenderingManager * pTextRenderingManager=0;
  ITextRenderingFactory * pTextRenderingFactory=0;
  IRichTextLayout * pRichTextLayout=0;
  IRichText * pTextObject=0;
  IUnknown * pGC =0;
  
  CoCreateInstance(CTextRenderingManagerCreator_guid,ITextRenderingManager_guid,&pTextRenderingManager);
  pTextRenderingManager->CreateTextRenderingFactory(&pTextRenderingFactory);
  pTextRenderingFactory->CreateRichText(&pTextObject);
  pTextRenderingFactory->CreateRichTextLayout(pTextObject,0,0,&pRichTextLayout);
  
  if (!pFont) SetFont(10);
  TextObject_SetText(pTextObject,strid);
  TextObject_SetFont(pTextObject,pFont,0x8000000A,0x7FFFFFF5);
  pTextObject->SetForegroundColor(pen_color,0x8000000A,0x7FFFFFF5);
  
  pTextObject->SetTextAlign(align,0x8000000A,0x7FFFFFF5);
  
  pRichTextLayout->SetRightBorder(right_border);
  
  rect.x1=x1;
  rect.y1=y1;
  rect.width=x2-x1;
  rect.height=y2-y1;
  
  DisplayGC_AddRef(get_DisplayGC(),&pGC);
  
  pRichTextLayout->DrawString(pGC,x1,y1,&rect);
  
  if (pTextRenderingManager) pTextRenderingManager->Release();
  if (pTextRenderingFactory) pTextRenderingFactory->Release();
  if (pRichTextLayout) pRichTextLayout->Release();
  if (pTextObject) pTextObject->Release();
  if (pGC) pGC->Release();
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
int dll_SetFont_0( int font_size )
{
  IFontManager * pFontManager=0;
  IFontFactory * pFontFactory=0;
  FONT_DATA pFontData;
  memset(&pFontData,0,sizeof(FONT_DATA));
  
  if (pFont)
  {
    pFont->Release();
    pFont=0;
  }

  CoCreateInstance(CFontManagerCreator_guid,IFontManager_guid,&pFontManager);
  pFontManager->CreateFontFactory(&pFontFactory);
  
  int font_size_without_style = font_size&0xFF;
  
  pFontFactory->GetDefaultFontSettings(large_size,&pFontData);
  pFontData.size=(float)font_size_without_style;
  pFontData.style=font_size>>8;
  pFontFactory->CreateFont(&pFontData,&pFont);
  
  if (pFontManager) pFontManager->Release();
  if (pFontFactory) pFontFactory->Release();
  
  return(1);
}
#endif

#ifdef A2
int dll_GetImageWidth_0( wchar_t imageID )
{
  IIconImage * pIconImage=0;
  IImageManager * pImageManager=0;
  int image_width;
  int image_height;
  
  CoCreateInstance(CImageManagerCreator_guid,IImageManager_guid,&pImageManager);
  pImageManager->CreateIconImage_Internal(imageID,&pIconImage);
  pIconImage->GetImageSize(&image_width,0,&image_height,0);
  
  if (pImageManager) pImageManager->Release();
  if (pIconImage) pIconImage->Release();
  return(image_width);
}
#endif

#ifdef A2
int dll_GetImageHeight_0( wchar_t imageID )
{
  IIconImage * pIconImage=0;
  IImageManager * pImageManager=0;
  int image_width;
  int image_height;
  
  if (imageID<100)
  {
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
    pFont->GetFontSize(&image_height);
#elif defined(DB3150)
    image_height=GetImageHeight( imageID );
#endif
  }
  else
  {
    CoCreateInstance(CImageManagerCreator_guid,IImageManager_guid,&pImageManager);
    pImageManager->CreateIconImage_Internal(imageID,&pIconImage);
    pIconImage->GetImageSize(&image_width,0,&image_height,0);
  }
  
  if (pImageManager) pImageManager->Release();
  if (pIconImage) pIconImage->Release();
  return(image_height);
}
#endif

#if defined(W705_R1GA031) || defined(W715_R1GA030) || defined(W995_R1GA026)
void dll_OrangeLED_Control_0( int _free_val, int or_LED_ID, int level, int fade_time )
{
#ifdef W705_R1GA031
  Illumination_LedID_SetLevel_Gradually(2,level,fade_time);
#elif W715_R1GA030
  Illumination_LedID_SetLevel_Gradually(2,level,fade_time); 
#elif W995_R1GA026
  Illumination_LedID_SetLevel_Gradually(4,level,fade_time);
#endif
}
#endif

#ifdef A2
void* dll_MetaData_Desc_Create_0( wchar_t* path, wchar_t* name )
{
  METADATA_DESC * MetaData_Desc = new METADATA_DESC;
  MetaData_Desc->pMetaData=0;
  MetaData_Desc->artist[0]=0;
  MetaData_Desc->title[0]=0;
  MetaData_Desc->album[0]=0;
  MetaData_Desc->year[0]=0;
  MetaData_Desc->genre[0]=0;
  MetaData_Desc->x6[0]=0;
  MetaData_Desc->x7[0]=0;
  CoCreateInstance(CMetaDataCreator_guid,IMetaData_guid,&MetaData_Desc->pMetaData);
  MetaData_Desc->pMetaData->SetFile(path,name);
  return(MetaData_Desc);
}
#endif

#ifdef A2
void dll_MetaData_Desc_Destroy_0( void* MetaData_Desc )
{
  if (((METADATA_DESC*)MetaData_Desc)->pMetaData) ((METADATA_DESC*)MetaData_Desc)->pMetaData->Release();
  delete(MetaData_Desc);
}
#endif

#ifdef A2
wchar_t* dll_MetaData_Desc_GetTags_0( void* MetaData_Desc, int tagID )
{
  METADATA_DESC * mdd = (METADATA_DESC*)MetaData_Desc;
  wchar_t * buf;
  switch(tagID)
  {
  case 0:
    buf = mdd->artist;
    break;
  case 1:
    buf = mdd->title;
    break;
  case 2:
    buf = mdd->album;
    break;
  case 3:
    buf = mdd->year;
    break;
  case 4:
    buf = mdd->genre;
    break;
  case 5:
    buf = mdd->x6;
    break;
  case 6:
    buf = mdd->x7;
    break;
  }
  mdd->pMetaData->GetTag(tagID,buf);
  return(buf);
}
#endif

#ifdef A2
int dll_MetaData_Desc_GetTrackNum_0( void* MetaData_Desc, int __NULL )
{
  int track_num;
  ((METADATA_DESC*)MetaData_Desc)->pMetaData->GetTrackNum(__NULL,&track_num);
  return(track_num);
}
#endif

#ifdef A2
int dll_MetaData_Desc_GetCoverInfo_0( void* MetaData_Desc, char* cover_type, int* size, int* cover_offset )
{
  COVER_INFO_DESC cover_info;
  ((METADATA_DESC*)MetaData_Desc)->pMetaData->GetCoverInfo(&cover_info);
  *cover_type = cover_info.cover_type;
  *size = cover_info.size;
  *cover_offset = cover_info.cover_offset;
  return(1);
}
#endif

#ifdef A2
void dll_RedLED_On_0( int __NULL )
{
  Illumination_LedID_SetLevel(5,100);
//  remove_tail_call();
}
#endif

#ifdef A2
void dll_RedLED_Off_0( int __NULL )
{
  Illumination_LedID_Off(5);
//  remove_tail_call();
}
#endif

#ifdef A2
void dll_Display_SetBrightness_0( int display, int brightness )
{
  Display_SetBrightness( brightness, 0 );
//  remove_tail_call();
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
int dll_Disp_GetStrIdWidth_0( STRID strid, int len )
{
  ITextRenderingManager * pTextRenderingManager=0;
  ITextRenderingFactory * pTextRenderingFactory=0;
  IRichTextLayout * pRichTextLayout=0;
  IRichText * pTextObject=0;
  
  CoCreateInstance(CTextRenderingManagerCreator_guid,ITextRenderingManager_guid,&pTextRenderingManager);
  pTextRenderingManager->CreateTextRenderingFactory(&pTextRenderingFactory);
  pTextRenderingFactory->CreateRichText(&pTextObject);
  pTextRenderingFactory->CreateRichTextLayout(pTextObject,0,0,&pRichTextLayout);
  
  int width=RichTextLayout_GetTextWidth(strid,pRichTextLayout,pFont);
  
  if (pTextRenderingManager) pTextRenderingManager->Release();
  if (pTextRenderingFactory) pTextRenderingFactory->Release();
  if (pRichTextLayout) pRichTextLayout->Release();
  if (pTextObject) pTextObject->Release();
  
  return(width);
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
FONT_DESC* dll_GetFontDesc_0(  )
{
  return(&font_desc);
}
#endif

#if defined(DB3200) || defined(DB3210) || defined(DB3350)
int* dll_GetFontCount_0(  )
{
  return(&font_count);
}
#endif



// -------------------  Private area  ------------------
// Ввиду того, что нельзя использовать глобальные переменные в Public методах,
// то описываем переменнуй в DLLFILENAME_private.h используем.
// Эта переменная будет видна в пределах DLL ( func2 , func3 , main )
// -----------------------------------------------------

int main ( int Action , LIBRARY_DLL_DATA * data )
{
  int const_minus = 0x100;
  LIBRARY_DLL_DATA * p;
  switch (Action)
  {
  case DLL_INIT:
    // инициалицация Dll_System area. вызывается только один раз, при загрузке в память.
    // если вернуть не ноль, то загрузка прекращается с кодом DLL_ERROR_INIT
    // data == NULL
    debug_printf("\nlibrary.dll: dll init\n");
    usage_count=0;


    return(0);

  case DLL_LOAD:
    debug_printf("\nlibrary.dll: dll load\n");
    // инициалицация Private / Public area. вызывается каждый раз, при вызове LoadDLL.
    // data == NULL

    usage_count++;                      // увеличение счетчика использования этой DLL

    if (!(p = new LIBRARY_DLL_DATA)) return(0);;       // создание this и инициализация свойств и методов

    memset(p,0,sizeof(LIBRARY_DLL_DATA));

    // Public area
    #ifdef A2
    p->num1=0x140-const_minus;  //swi num - 0x100
    p->dll_GC_PutChar=dll_GC_PutChar_0;
    #endif
    #ifdef A2
    p->num2=0x163-const_minus;
    p->dll_AB_READSTRING=dll_AB_READSTRING_0;
    #endif
    #if defined(DB3200) || defined(DB3210) || defined(DB3350)
    p->num3=0x1ED-const_minus;
    p->dll_DrawString=dll_DrawString_0;
    #endif
    #if defined(DB3200) || defined(DB3210) || defined(DB3350)
    p->num4=0x1F1-const_minus;
    p->dll_SetFont=dll_SetFont_0;
    #endif
    #ifdef A2
    p->num5=0x293-const_minus;
    p->dll_GetImageWidth=dll_GetImageWidth_0;
    #endif
    #ifdef A2
    p->num6=0x294-const_minus;
    p->dll_GetImageHeight=dll_GetImageHeight_0;
    #endif
    #if defined(W705_R1GA031) || defined(W715_R1GA030) || defined(W995_R1GA026)
    p->num7=0x2DC-const_minus;
    p->dll_OrangeLED_Control=dll_OrangeLED_Control_0;
    #endif
    #ifdef A2
    p->num8=0x2EF-const_minus;
    p->dll_MetaData_Desc_Create=dll_MetaData_Desc_Create_0;
    #endif
    #ifdef A2
    p->num9=0x2F0-const_minus;
    p->dll_MetaData_Desc_Destroy=dll_MetaData_Desc_Destroy_0;
    #endif
    #ifdef A2
    p->num10=0x2F1-const_minus;
    p->dll_MetaData_Desc_GetTags=dll_MetaData_Desc_GetTags_0;
    #endif
    #ifdef A2
    p->num11=0x2F2-const_minus;
    p->dll_MetaData_Desc_GetTrackNum=dll_MetaData_Desc_GetTrackNum_0;
    #endif
    #ifdef A2
    p->num12=0x301-const_minus;
    p->dll_MetaData_Desc_GetCoverInfo=dll_MetaData_Desc_GetCoverInfo_0;
    #endif
    #ifdef A2
    p->num13=0x307-const_minus;
    p->dll_RedLED_On=dll_RedLED_On_0;
    #endif
    #ifdef A2
    p->num14=0x308-const_minus;
    p->dll_RedLED_Off=dll_RedLED_Off_0;
    #endif
    #ifdef A2
    p->num15=0x31C-const_minus;
    p->dll_Display_SetBrightness=dll_Display_SetBrightness_0;
    #endif
    #if defined(DB3200) || defined(DB3210) || defined(DB3350)
    p->num16=0x300-const_minus;
    p->dll_Disp_GetStrIdWidth=dll_Disp_GetStrIdWidth_0;
    #endif
    #if defined(DB3200) || defined(DB3210) || defined(DB3350)
    p->num17=0x2BB-const_minus;
    p->dll_GetFontDesc=dll_GetFontDesc_0;
    #endif
    #if defined(DB3200) || defined(DB3210) || defined(DB3350)
    p->num18=0x2BC-const_minus;
    p->dll_GetFontCount=dll_GetFontCount_0;
    #endif
    p->end_marker=-1;
    
    
    // Private area
#ifdef A2
    pFont=0;
    font_count=1;
    font_desc.id=20;
    wstrcpy(font_desc.name,L"E_20R");
#endif
    
    return((int)p);

  case DLL_UNLOAD:
    debug_printf("\nlibrary.dll: dll unload\n");
    // освобождение Private / Public area. вызывается каждый раз, при вызове UnLoadDLL.
    // data == this

    //  освобождеем при необходимости data->...


    //  освобождеем всегда data
    mfree(data);
    usage_count--;                      // уменьшение счетчика использования этой DLL

    // если больше никто не использует...
    if(!usage_count)
    {
        //
        //  освобождеем при необходимости Dll_System area
        //
      SUBPROC(elf_exit);
    }
    return(usage_count);

  }
  return(0);
}
