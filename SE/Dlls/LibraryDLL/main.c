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



// -------------------  Public area  -------------------
// Свойсива и методы доступные снаружи.
// Внутри методов доступна Private area.
// Для читабельности имена методов переопределены в h-файле.
// Чтобы не было проблем при компиляции с несколькими DLL, имена - DLLFILENAME_FunctionName.
//

/*void remove_tail_call()
{
}*/

void dll_GC_PutChar_0( LIBRARY_DLL_DATA * data, GC* gc, int x, int y, int width, int height, wchar_t imageID )
{
#ifdef A2
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
#else
  GC_PutChar( gc, x , y, width, height, imageID );
#endif
}

int dll_AB_READSTRING_0( LIBRARY_DLL_DATA * data, AB_STR_ITEM* ab, int rec_num, int field_ID )
{
#ifdef A2
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
#else
  return(AB_READSTRING( ab, rec_num , field_ID ));
#endif
}

void dll_DrawString_0( LIBRARY_DLL_DATA * data, STRID strid, int align, int x1, int y1, int x2, int y2, int unk, int unk1, int pen_color, int brush_color )
{
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
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
  
  if (!data->pFont) data->dll_SetFont(data,10);
  TextObject_SetText(pTextObject,strid);
  TextObject_SetFont(pTextObject,data->pFont,0x8000000A,0x7FFFFFF5);
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
#else
  DrawString( strid, align, x1, y1, x2, y2, unk, unk1, pen_color, brush_color );
#endif
}

int dll_SetFont_0( LIBRARY_DLL_DATA * data, int font_size )
{
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
  IFontManager * pFontManager=0;
  IFontFactory * pFontFactory=0;
  FONT_DATA pFontData;
  memset(&pFontData,0,sizeof(FONT_DATA));
  
  if (data->pFont)
  {
    data->pFont->Release();
    data->pFont=0;
  }

  CoCreateInstance(CFontManagerCreator_guid,IFontManager_guid,&pFontManager);
  pFontManager->CreateFontFactory(&pFontFactory);
  
  int font_size_without_style = font_size&0xFF;
  
  pFontFactory->GetDefaultFontSettings(large_size,&pFontData);
  pFontData.size=(float)font_size_without_style;
  pFontData.style=font_size>>8;
  pFontFactory->CreateFont(&pFontData,&data->pFont);
  
  if (pFontManager) pFontManager->Release();
  if (pFontFactory) pFontFactory->Release();
  
  return(1);
#else
  return(SetFont( font_size ));
#endif
}

int dll_GetImageWidth_0( LIBRARY_DLL_DATA * data, wchar_t imageID )
{
#ifdef A2
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
#else
  return(GetImageWidth( imageID ));
#endif
}

int dll_GetImageHeight_0( LIBRARY_DLL_DATA * data, wchar_t imageID )
{
#ifdef A2
  IIconImage * pIconImage=0;
  IImageManager * pImageManager=0;
  int image_width;
  int image_height;
  
  if (imageID<100)
  {
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
    data->pFont->GetFontSize(&image_height);
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
#else
  return(GetImageHeight( imageID ));
#endif
}

void dll_OrangeLED_Control_0( LIBRARY_DLL_DATA * data, int _free_val, int or_LED_ID, int level, int fade_time )
{
#ifdef W705_R1GA031
  Illumination_LedID_SetLevel_Gradually(2,level,fade_time);
#elif W715_R1GA030
  Illumination_LedID_SetLevel_Gradually(2,level,fade_time); 
#elif W995_R1GA026
  Illumination_LedID_SetLevel_Gradually(4,level,fade_time);
#else
  OrangeLED_Control( _free_val, or_LED_ID, level, fade_time );
#endif
}

void* dll_MetaData_Desc_Create_0( LIBRARY_DLL_DATA * data, wchar_t* path, wchar_t* name )
{
#ifdef A2
  
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
#else
  return(MetaData_Desc_Create( path, name ));
#endif
}

void dll_MetaData_Desc_Destroy_0( LIBRARY_DLL_DATA * data, void* MetaData_Desc )
{
#ifdef A2
  if (((METADATA_DESC*)MetaData_Desc)->pMetaData) ((METADATA_DESC*)MetaData_Desc)->pMetaData->Release();
  delete(MetaData_Desc);
#else
  MetaData_Desc_Destroy( MetaData_Desc );
#endif
}

wchar_t* dll_MetaData_Desc_GetTags_0( LIBRARY_DLL_DATA * data, void* MetaData_Desc, int tagID )
{
#ifdef A2
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
#else
  return(MetaData_Desc_GetTags( MetaData_Desc, tagID ));
#endif
}

int dll_MetaData_Desc_GetTrackNum_0( LIBRARY_DLL_DATA * data, void* MetaData_Desc, int __NULL )
{
#ifdef A2
  int track_num;
  ((METADATA_DESC*)MetaData_Desc)->pMetaData->GetTrackNum(__NULL,&track_num);
  return(track_num);
#else
  return(MetaData_Desc_GetTrackNum( MetaData_Desc, __NULL ));
#endif
}

int dll_MetaData_Desc_GetCoverInfo_0( LIBRARY_DLL_DATA * data, void* MetaData_Desc, char* cover_type, int* size, int* cover_offset )
{
#ifdef A2
  COVER_INFO_DESC cover_info;
  ((METADATA_DESC*)MetaData_Desc)->pMetaData->GetCoverInfo(&cover_info);
  *cover_type = cover_info.cover_type;
  *size = cover_info.size;
  *cover_offset = cover_info.cover_offset;
  return(1);
#else
  return(MetaData_Desc_GetCoverInfo( MetaData_Desc, cover_type, size, cover_offset ));
#endif
}

void dll_RedLED_On_0( LIBRARY_DLL_DATA * data, int __NULL )
{
#ifdef A2
  Illumination_LedID_SetLevel(5,100);
//  remove_tail_call();
#else
  RedLED_On( __NULL );
#endif
}

void dll_RedLED_Off_0( LIBRARY_DLL_DATA * data, int __NULL )
{
#ifdef A2
  Illumination_LedID_Off(5);
//  remove_tail_call();
#else
  RedLED_Off( __NULL );
#endif
}


void dll_Display_SetBrightness_0( LIBRARY_DLL_DATA * data, int display, int brightness )
{
#ifdef A2
  Display_SetBrightness( brightness, 0 );
//  remove_tail_call();
#else
  Display_SetBrightness( display, brightness );
#endif
}


int dll_Disp_GetStrIdWidth_0( LIBRARY_DLL_DATA * data, STRID strid, int len )
{
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
  ITextRenderingManager * pTextRenderingManager=0;
  ITextRenderingFactory * pTextRenderingFactory=0;
  IRichTextLayout * pRichTextLayout=0;
  IRichText * pTextObject=0;
  
  CoCreateInstance(CTextRenderingManagerCreator_guid,ITextRenderingManager_guid,&pTextRenderingManager);
  pTextRenderingManager->CreateTextRenderingFactory(&pTextRenderingFactory);
  pTextRenderingFactory->CreateRichText(&pTextObject);
  pTextRenderingFactory->CreateRichTextLayout(pTextObject,0,0,&pRichTextLayout);
  
  int width=RichTextLayout_GetTextWidth(strid,pRichTextLayout,data->pFont);
  
  if (pTextRenderingManager) pTextRenderingManager->Release();
  if (pTextRenderingFactory) pTextRenderingFactory->Release();
  if (pRichTextLayout) pRichTextLayout->Release();
  if (pTextObject) pTextObject->Release();
  
  return(width);
#else
  return(Disp_GetStrIdWidth( strid, len ));
#endif
}


FONT_DESC* dll_GetFontDesc_0( LIBRARY_DLL_DATA * data )
{
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
  return(&data->font_desc);
#else
  return(GetFontDesc());
#endif
}


int* dll_GetFontCount_0( LIBRARY_DLL_DATA * data )
{
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
  return(&data->font_count);
#else
  return(GetFontCount());
#endif
}



// -------------------  Private area  ------------------
// Ввиду того, что нельзя использовать глобальные переменные в Public методах,
// то описываем переменнуй в DLLFILENAME_private.h используем.
// Эта переменная будет видна в пределах DLL ( func2 , func3 , main )
// -----------------------------------------------------

int main ( int Action , LIBRARY_DLL_DATA * data )
{
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
    p->dll_GC_PutChar=dll_GC_PutChar_0;
    p->dll_AB_READSTRING=dll_AB_READSTRING_0;
    p->dll_DrawString=dll_DrawString_0;
    p->dll_SetFont=dll_SetFont_0;
    p->dll_GetImageWidth=dll_GetImageWidth_0;
    p->dll_GetImageHeight=dll_GetImageHeight_0;
    p->dll_OrangeLED_Control=dll_OrangeLED_Control_0;
    p->dll_MetaData_Desc_Create=dll_MetaData_Desc_Create_0;
    p->dll_MetaData_Desc_Destroy=dll_MetaData_Desc_Destroy_0;
    p->dll_MetaData_Desc_GetTags=dll_MetaData_Desc_GetTags_0;
    p->dll_MetaData_Desc_GetTrackNum=dll_MetaData_Desc_GetTrackNum_0;
    p->dll_MetaData_Desc_GetCoverInfo=dll_MetaData_Desc_GetCoverInfo_0;
    p->dll_RedLED_On=dll_RedLED_On_0;
    p->dll_RedLED_Off=dll_RedLED_Off_0;
    p->dll_Display_SetBrightness=dll_Display_SetBrightness_0;
    p->dll_Disp_GetStrIdWidth=dll_Disp_GetStrIdWidth_0;
    p->dll_GetFontDesc=dll_GetFontDesc_0;
    p->dll_GetFontCount=dll_GetFontCount_0;
    
    
    // Private area
#ifdef A2
    p->pFont=0;
    p->font_count=1;
    p->font_desc.id=20;
    wstrcpy(p->font_desc.name,L"E_20R");
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
