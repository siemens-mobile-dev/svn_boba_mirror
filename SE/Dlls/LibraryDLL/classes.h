char IFontManager_guid[]={0x95,0xD0,0x5F,0xCA,0x0F,0x30,0x4F,0x92,0x94,0x3B,0x13,0x89,0x21,0x2F,0x09,0xD7};
char CFontManagerCreator_guid[]={0x82,0xAC,0x97,0x9B,0x58,0x44,0x41,0x39,0xA0,0xD3,0xE6,0xCF,0x72,0xB5,0x80,0x69};
char ITextRenderingManager_guid[]={0xCE,0x91,0x7B,0x62,0xE3,0x3A,0x4D,0xC7,0x85,0x24,0x79,0x1E,0x6F,0x01,0x03,0x09};
char CTextRenderingManagerCreator_guid[]={0xCF,0xDC,0xCD,0x9D,0xAE,0x6D,0x40,0xAE,0xB3,0x97,0x38,0xB0,0x5D,0x2D,0x35,0x9D};
char IImageManager_guid[]={0x63,0xEF,0xA5,0x31,0xFF,0xD1,0x4E,0x25,0xA6,0x24,0x60,0x0A,0x96,0x81,0x3F,0xC5};
char CImageManagerCreator_guid[]={0x86,0x2D,0xF9,0x95,0x1C,0x3B,0x4B,0x24,0xB7,0x4D,0x8F,0x98,0x70,0x1E,0x5D,0x14};
char IMetaData_guid[]={0xB1,0xEB,0x4F,0x97,0xB0,0xDD,0x42,0x09,0xB6,0x96,0xCE,0x11,0x2D,0x5E,0xD8,0xE8};
char CMetaDataCreator_guid[]={0x0B,0xCD,0x95,0x0F,0x9F,0xB6,0x4E,0x7A,0xA7,0xCD,0xCA,0x1D,0x87,0xBA,0x9A,0x27};


class IRichText;
class IRichTextLayout;
class ITheme;
class IFont;
class IFontFactory;
class IFontManager;
class ITextRenderingFactory;
class ITextRenderingManager;
class IImageManager;
class IIconImage;
class IMetaData;


typedef struct
{
  char ValidLAI;
  char unk1;
  char MCC[3];
  char MNC[2];
  char PLMN_x;
  u16 LAC;
  char ValidRAC;
  char ValidCI;
#if defined (DB3200) || defined (DB3210) || defined (DB3350)
  int unk2;
#endif
  int CI;
  char RAT;
  char Coverage;
  char CSReg;
  char PSReg;
  char Priority;
  char EPriority;
  char EGPRS;
  char DTMSup;
  char HSDPA;
  char unk3;
  char MSC;
  char SGSN;
}CONNECTION_INFO_DESC;


typedef struct
{
  wchar_t artist[0x100];
  wchar_t title[0x100];
  wchar_t album[0x100];
  wchar_t year[0x100];
  wchar_t genre[0x100];
  wchar_t x6[0x100];
  wchar_t x7[0x100];
  IMetaData * pMetaData;
}METADATA_DESC;


typedef struct
{
  char cover_type;
  int size;
  int cover_offset;
}COVER_INFO_DESC;


typedef struct
{
  float size;
  int x2;
  int x3;
  int x4;
  int x5;
  int x6;
  int x7;
  int x8;
  int x9;
  u16 style;
  char x11;
  u16 x12;
}FONT_DATA;


enum style {
  normal = 0,
  bold = 1,
  italic = 2
};


enum default_size_ID {
  small_size = 0x0,
  medium_size = 0x8,
  large_size = 0x10
};


typedef struct
{
  int x1;
  int y1;
  int width;
  int height;
}WINDOW_RECT;



class IUnknown
{
public:
  virtual void* pguid();
  virtual void* QueryInterface();
  virtual void* AddRef();
  virtual void* Release();
};


class IRichTextLayout: public IUnknown
{
public:
  virtual void method1();
  virtual void method2();
  virtual void method3();
  virtual void method4();
  virtual void method5();
  virtual void method6();
  virtual void method7();
  virtual void GetTextObject(IRichText * pTextObject);
  virtual void SetRightBorder(int x);
  virtual void method10();
  virtual void method11();
  virtual void method12();
  virtual void method13();
  virtual void method14();
  virtual void method15();
  virtual void method16();
  virtual void method17();
  virtual void method18(void * buf);
  virtual void GetTextInfo(void * buf);
  virtual void method20();
  virtual void DrawString(IUnknown * pGC,int x1,int y1,WINDOW_RECT * rect);
  virtual void method22(IUnknown * pGC,int x1,int y1,WINDOW_RECT * rect);
  virtual void method23();
  virtual void method24();
  virtual void method25();
};


class ITheme: public IUnknown
{
public:
  virtual void method1();
  virtual void GetFont(char * DispName,char * FontName,IUnknown ** pFont);
  virtual void method3();
};


class IFont: public IUnknown
{
public:
  virtual int method1(char * buf);
  virtual int GetDefaultFontSizeID(char * buf);
  virtual int GetFontStyle(char * buf);
  virtual int method4(int * size);
  virtual int GetFontSize(int * font_size);
  virtual int isBold();
  virtual int isItalic();
  virtual int isBoldItalic();
  virtual int isXXX();
  virtual int CopyFont(IFont * pFont);
};


class IFontFactory: public IUnknown
{
public:
  virtual void method1();
  virtual void GetDefaultFont(IUnknown * pFont);
  virtual void GetDefaultFontSettings(int size,FONT_DATA * pfontData);
  virtual void CreateFont(FONT_DATA * pfontData,IFont ** pFont);
};


class IFontManager: public IUnknown
{
public:
  virtual void CreateFontFactory(IFontFactory ** pFontFactory);
};


class IRichText: public IUnknown
{
public:
  virtual void method1();
  virtual void method2();
  virtual void InsertText(char * text,int text_len,int text_type,int __NULL);
  virtual void method4();
  virtual void RemoveText(const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void method6();
  virtual void method7();
  virtual void method8();
  virtual void method9();
  virtual void method10();
  virtual void method11();
  virtual void method12();
  virtual void method13();
  virtual void method14();
  virtual void SetTextAlign(int x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetBackgroundColor(int x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetTextDirection(int textdir,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetLetterSpacing(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetColor_0x4(int color,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetFont(void * pFont);
  virtual void SetIndention(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetInlineObjectID(wchar_t imageID,int pos,int pos2);
  virtual void SetInlineObjectMetric(wchar_t imageID,void * i_desc);
  virtual void SetColor(int x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void Set(int x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetLeftIndent(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetLineSpacing(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetUpperLineIndent(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetLineSpacingFix(int x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void Set_0xA(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void method31();
  virtual void SetOverline(int isOverlined,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetColor_0xE(int color,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetRightIndent(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void Set_0x10(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetUpperIndent(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetStrikethrough(int isStrikethroughed,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetColor_0x13(int color,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetForegroundColor(int color,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetIndentionFix(int x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetUnderline(int isUnderlined,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetColor_0x17(int color,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void SetIndentionForMultyline(float x,const int _0x8000000A,const int _0x7FFFFFF5);
  virtual void GetTextAlign();
  virtual void GetBackgroundColor();
  virtual void GetTextDirection();
  virtual void GetLetterSpacing();
  virtual void GetColor_0x4();
  virtual void GetFont();
  virtual void GetIndention();
  virtual void method51();
  virtual void method52();
  virtual void GetColor_0x19();
  virtual void Get_0x8();
  virtual void GetLeftIndent();
  virtual void GetLineSpacing();
  virtual void GetUpperLineIndent();
  virtual void GetLineSpacingFix();
  virtual void Get_0xA();
  virtual void method60();
  virtual void GetOverline();
  virtual void GetColor_0xE();
  virtual void GetRightIndent();
  virtual void Get_0x10();
  virtual void GetUpperIndent();
  virtual void GetStrikethrough();
  virtual void GetColor_0x13();
  virtual void GetForegroundColor();
  virtual void GetIndentionFix();
  virtual void GetUnderline();
  virtual void GetColor_0x17();
  virtual void GetIndentionForMultyline();
};


class ITextRenderingFactory: public IUnknown
{
public:
  virtual void method1();
  virtual void method2();
  virtual void CreateRichText(IRichText ** pRichText);
  virtual void method4();
  virtual void CreateRichTextLayout(IRichText * pRichText,int __NULL1,int __NULL2,IRichTextLayout ** pRichTextLayout);
};


class ITextRenderingManager: public IUnknown
{
public:
  virtual void CreateTextRenderingFactory(ITextRenderingFactory ** pTextRenderingFactory);
};


class IImageManager: public IUnknown
{
public:
  virtual void CreateIconImage_External(wchar_t * fpath,wchar_t * fname,int _NULL1,int _NULL2,IIconImage ** pIconImage);
  virtual void method2();
  virtual void CreateIconImage_Internal(int imageID,IIconImage ** pIconImage);
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
  virtual void method4();
#endif
  virtual void CreateIconImage_External_Indirect(void * buf_image,int size,wchar_t * image_type,int image_type_len,void * unk_buf,int unk_wstr_len,IIconImage ** pIconImage);
  virtual void method6();
  virtual void DrawIconEx(IIconImage * pIconImage,IUnknown * pGC,int x,int y,int image_width,int image_height);
};


class IIconImage: public IUnknown
{
public:
  virtual void GetImageSize(int * image_width,int __NULL1,int * image_height,int __NULL2);
};

class IMetaData: public IUnknown
{
public:
  virtual int SetFile(wchar_t* path, wchar_t* name);
  virtual int method2(void * unk1, void * unk2);
  virtual int GetTag(int tagID, wchar_t * tag);
  virtual int GetTrackNum(int unk, int * track_num);
  virtual int GetCoverInfo(COVER_INFO_DESC * cover_info);
};
