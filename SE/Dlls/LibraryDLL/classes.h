UUID IFontManager_guid={0x95,0xD0,0x5F,0xCA,0x0F,0x30,0x4F,0x92,0x94,0x3B,0x13,0x89,0x21,0x2F,0x09,0xD7};
UUID CFontManagerCreator_guid={0x82,0xAC,0x97,0x9B,0x58,0x44,0x41,0x39,0xA0,0xD3,0xE6,0xCF,0x72,0xB5,0x80,0x69};
UUID ITextRenderingManager_guid={0xCE,0x91,0x7B,0x62,0xE3,0x3A,0x4D,0xC7,0x85,0x24,0x79,0x1E,0x6F,0x01,0x03,0x09};
UUID CTextRenderingManagerCreator_guid={0xCF,0xDC,0xCD,0x9D,0xAE,0x6D,0x40,0xAE,0xB3,0x97,0x38,0xB0,0x5D,0x2D,0x35,0x9D};
UUID IImageManager_guid={0x63,0xEF,0xA5,0x31,0xFF,0xD1,0x4E,0x25,0xA6,0x24,0x60,0x0A,0x96,0x81,0x3F,0xC5};
UUID CImageManagerCreator_guid={0x86,0x2D,0xF9,0x95,0x1C,0x3B,0x4B,0x24,0xB7,0x4D,0x8F,0x98,0x70,0x1E,0x5D,0x14};
UUID IMetaData_guid={0xB1,0xEB,0x4F,0x97,0xB0,0xDD,0x42,0x09,0xB6,0x96,0xCE,0x11,0x2D,0x5E,0xD8,0xE8};
UUID CMetaDataCreator_guid={0x0B,0xCD,0x95,0x0F,0x9F,0xB6,0x4E,0x7A,0xA7,0xCD,0xCA,0x1D,0x87,0xBA,0x9A,0x27};


class IRichText;
class IRichTextLayout;
class ITheme;
class IFont;
class IFontFactory;
class IFontManager;
class ITextRenderingFactory;
class ITextRenderingManager;
class IImageManager;
class IUIImage;
class IMetaData;


typedef struct {
  long X;
  long Y;
} TUIPoint;


typedef struct {
  long Width;
  long Height;
} TUISize;

typedef struct
{
  TUIPoint Point;
  TUISize  Size;
} TUIRectangle;


typedef struct
{
    long        numGlyphs;    ///< number of glyphs on the line 
    long        startIndex;   ///< IUIRichText character index of first character in line 
    long        endIndex;     ///< IUIRichText character index of last character in line 
    TUIRectangle  boundingBox;  ///< actual "inked" area of the line -- relative to line's origin
    TUIRectangle  advanceBox;   ///< box spanning origin to next advance point -- relative to line's origin 
    unsigned char        bidiBaseLevel;///< bidi base level for line -- odd if Right-To-Left, even if Left-To-Right 
    TUIPoint      lineOrigin;   ///< absolute coordinates for line's origin, only meaningful if lineFlags has UI_LINE_DISPLAYED set 
    long        lineFlags;    ///< flags like UI_LINE_COMPOSED and UI_LINE_DISPLAYED 
    long        baseline;     ///< amount to add to lineOrigin.y to get baseline 
    long        top;          ///< amount to add to lineOrigin.y to get top of line (used for overline, and background color)
    long        bottom;       ///< amount to add to lineOrigin.y to get bottom of line (used for background color)
    long   alignment;///< alignment of line (UIText_alignLeft, UIText_alignRight, or UIText_alignCenter)
} TUILineInfo;


typedef struct
{
    long      sourceIndex; ///< character index in ÍUIRichText block 
    long      glyphID;     ///< glyph index in font 
    long      x;           ///< the x coordinate relative to the line's origin 
    long      y;           ///< the y coordinate relative to the line's origin 
    TUIRectangle boundingBox; ///< bounding box relative to the glyph's position (x,y) 
} TUIGlyphInfo;


typedef struct
{
    long      numLines;         ///< number of lines in the layout 
    long      numContainersUsed;///< number of containers actually used 
    TUIRectangle boundingBox;      ///< text layout bounding box 
    TUIRectangle advanceBox;       ///< text layout advance box 
} TUILayoutInfo;


typedef struct
{
    long numLines;          ///< count of lines in the container
    long firstLineNumber;   ///< index of first line in container
    TUIRectangle boundingBox;  ///< text container bounding box
    TUIRectangle advanceBox;   ///< text container advance box
} TUIContainerInfo;


typedef enum
{
  UIFontFaceSystem = 0,       ///<The "system" font face
  UIFontFaceMonoSpace = 32,   ///<The "monospace" font face.
  UIFontFaceProportional = 64 ///<The "proportional" font face.
} TUIFontFace;


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


enum TUIEmphasisStyle
{
  UI_Emphasis_Normal = 0,     ///< normal
  UI_Emphasis_Bold = 1,       ///< bold
  UI_Emphasis_Italic = 2,     ///< italic
  UI_Emphasis_BoldItalic = 3  ///< bold italic
};


typedef enum
{
  UI_Baseline_Normal,       ///< normal baseline
  UI_Baseline_Superscript,  ///< superscript
  UI_Baseline_Subscript     ///< subscript
} TUIBaselineStyle;


typedef enum
{
  UI_Edge_Normal,          ///< normal text without any edge effect
  UI_Edge_Embossed,        ///< text has a raised appearance
  UI_Edge_Engrayed,        ///< text has a depressed appearance 
  UI_Edge_Outlined,        ///< text is just an outline  
  UI_Edge_Drop_Shadow,     ///< text has a shadow 
  UI_Edge_Raised,          ///< similar to embossed, but more of a shadow effect 
  UI_Edge_Depressed,       ///< similar to engraved, but more of a shadow effect 
  UI_Edge_Uniform          ///< normal text with a colored edge around it, "cartoon style" 
} TUIEdgeStyle;


enum TUIRenderStyle
{
  UI_Render_Default     = 0x0000,   ///< use the font engine's default rendering 
  UI_Render_Binary      = 0x0001,   ///< binary,    1 bit per pixel 
  UI_Render_Grayscale2  = 0x0002,   ///< grayscale, 2 bits per pixel 
  UI_Render_Grayscale4  = 0x0004,   ///< grayscale, 4 bits per pixel (grayscale antialiasing)
  UI_Render_Grayscale8  = 0x0008,   ///< grayscale, 4 bits per pixel 
  UI_Render_ADF_Graymap = 0x0010,   ///< ADF rendering 
  UI_Render_Vactor_Icon = 0x0020    ///< vector icon 
};


typedef struct
{
  float size;                ///< height in ppem (20.0 ppem)
  float width;               ///< width in ppem; (0 - treated as equal to size) 
  float skew;                ///< skew angle (0) 
  float orientation;         ///< glyph rotation, degrees clockwise (0.0) 

  float bold_pct;            ///< boldness value (0) 
  float stroke_pct;          ///< stroke width (0) 
  float edgeThickness;       ///< edge thickness in pixels (0) 
  float shadowLength_x;      ///< x-direction length of shadow in pixels (0) 
  float shadowLength_y;      ///< y-direction length of shadow in pixels (0) 

  u16 TUIEmphasisStyle;   ///< emphasis such as bold/italics (UI_Emphasis_Normal) 
  u16 TUIBaselineStyle;   ///< text baseline style (UI_Baseline_Normal) 
  u16 TUIEdgeStyle;       ///< text edge style (UI_Edge_Normal) 
  u16 TUIRenderStyle;     ///< rendering style (UI_Render_Default) 

} TUIFontData;

typedef unsigned char TUIFontStyle;

const TUIFontStyle UIFontStylePlain = 0;      ///<The plain style enum
const TUIFontStyle UIFontStyleBold = 1;       ///<The bold style enum.
const TUIFontStyle UIFontStyleItalic = 2;     ///<The italic style enum
const TUIFontStyle UIFontStyleUnderlined = 4; ///<The underlined style enum


typedef enum
{
  UIFontSizeMedium = 0,     ///<The "medium" system-dependent font size
  UIFontSizeSmall = 8,      ///<The "small" system-dependent font size
  UIFontSizeLarge = 16      ///<The "large" system-dependent font size
} TUIFontSize;


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
  virtual void SetControl(void * pIUIRichTextLayoutControl);
  virtual void GetControl(void ** pIUIRichTextLayoutControl);
  virtual void SetOptions(void * pIUIRichTextLayoutOptions);
  virtual void GetOptions(void * pIUIRichTextLayoutOptions);
  virtual void Subscribe(void * pICBUIRichTextLayout,u16 msgBase,int clientData,unsigned long * pSubscriptionHandle);
  virtual void Unsubscribe(unsigned long SubscriptionHandle);
  virtual void SetText(IRichText * pTextObject);
  virtual void GetText(IRichText ** pTextObject);
  virtual void Compose(int lineWidth);
  virtual void ComposeLine(int lineWidth,int ascenderLimit,int descenderLimit,int lineNumber,long * pStartIndex,long * pEndIndex,long * pCharsLeftToCompose);
  virtual void ComposeLineInit(long x,long y,long lineNumber);
  virtual void PositionLine(long x,long y,long lineNumber);
  virtual void ComposeContainers(TUIRectangle * pComposeRect);
  virtual void GetWidthOfLongestRemainingLineBreakSequence(int * pWidth,long * pStartIndex,long * pEndIndex);
  virtual void GetLineInfo(long lineNumber,TUILineInfo *pInfo);
  virtual void MapCharIndex(long index,long * pLineNumber);
  virtual void GetGlyphInfo(long lineNumber,long glyphIndex,bool needBoundingBox,TUIGlyphInfo *pInfo);
  virtual void GetNumberOfLines(long *pNumberOfLines);
  virtual void GetLayoutInfo(TUILayoutInfo *pLayoutInfo);
  virtual void GetContainerInfo(long containerIndex,TUIContainerInfo *pContainerInfo);
  virtual void Display(IUnknown * pGC,long x,long y,TUIRectangle *pInvalidateRect);
  virtual void DisplayContainers(IUnknown * pGC,long x,long y,TUIRectangle *pInvalidateRect);
  virtual void DisplayLine(IUnknown * pGC,long x,long y,TUIRectangle *pInvalidateRect);
  virtual void AddTextContainer(IUnknown *pIUITextContainer,long containerID,TUIPoint *pContainerOrigin);
  virtual void RemoveTextContainer(long containerID);
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
  virtual int GetFace(TUIFontFace * buf);
  virtual int GetSize(TUIFontSize * buf);
  virtual int GetStyle(TUIFontStyle * buf);
  virtual int GetFontAscent(long * pAscent);
  virtual int GetFontHeight(long * font_size);
  virtual int IsBold();
  virtual int IsItalic();
  virtual int IsPlain();
  virtual int IsUnderlined();
  virtual int GetFontStyle(IUnknown * pIUIFontStyle);
};


class IFontFactory: public IUnknown
{
public:
  virtual void GetFont(TUIFontFace Face,TUIFontSize Size,TUIFontStyle Style,IFont ** pFont);
  virtual void GetDefaultFont(IFont ** pFont);
  virtual void GetDefaultFontSettings(TUIFontSize Size,TUIFontData * pFontData);
  virtual void CreateDefaultFont(TUIFontData * pfontData,IFont ** pFont);
  virtual void CreateFontFromFamilyName(IUnknown * pIFamilyName,TUIFontData * pfontData,IFont ** pFont);
  virtual void CreateFontFromFile(IUnknown * pIFullPath,TUIFontData * pfontData,IFont ** pFont);
};


class IFontManager: public IUnknown
{
public:
  virtual void GetFontFactory(IFontFactory ** pFontFactory);
};

typedef unsigned long FUint32;
typedef long FSint32;
typedef unsigned long TUnsigned;
typedef bool TBool;

typedef enum
{
  UIEncoding_Encoding_Not_Specified = 0,
  UIEncoding_ASCII,           ///< plain ASCII text 
  UIEncoding_UTF8,            ///< text encoded in UTF-8 
  UIEncoding_UTF16,           ///< text encoded in UTF-16 
  UIEncoding_UTF32            ///< text encoded in UTF-32 
} TUIEncoding;

typedef enum 
{
  UILineSpacing_Single_Line,        ///< single spacing
  UILineSpacing_150_Line,           ///< one and a half line spacing
  UILineSpacing_Double_Line,        ///< double spacing
  UILineSpacing_Exact_Line_Spacing, ///< line spacing set to lineSpacingAmount
  UILineSpacing_Multiple_Line       ///< line spacing set to multiple of line spacing * lineSpacingAmount
} TUILineSpacing;

typedef enum
{
  UIText_alignLeft,        ///< left aligned
  UIText_alignRight,       ///< center aligned
  UIText_alignCenter,      ///< right aligned
  UIText_alignLeftDynamic, ///< left aligned but first character can override
  UIText_alignRightDynamic ///< right aligned but first character can override
} TUITextAlignment;

typedef enum
{
  UITextDirection_Netural,   ///< text with no inherent direction 
  UITextDirection_LTR,       ///< left-to-right text 
  UITextDirection_RTL        ///< right-to-left text
} TUITextDirection;

typedef enum
{
  UILine_None,        ///< no line
  UILine_SingleSolid  ///< single solid line
} TUILineStyle;

typedef struct 
{
  float xAdvance;              ///< x-dimension advance width
  float yAdvance;              ///< y-dimension advance width
  TUIRectangle boundingBox;     ///< bounding box of image, with respect to its origin
} TUIInlineObjectMetrics;

//startIndex = 0x8000000A
//endIndex = 0x7FFFFFF5


class IRichText: public IUnknown
{
public:
  virtual void SetString(void* pIString); 										//Set a string (UTF16)
  virtual void GetString(void** ppIString); 										//Get a string (UTF16)
  virtual void InsertText(void * text,FSint32 text_len,TUIEncoding encoding,FSint32 insertIndex); 			//Inserts a text run by copy to a IUIRichText object.
  virtual void InsertTextUTF16(u16 *text,TUnsigned length_text,FSint32 insertIndex); 					//Inserts a text run by copy to a IUIRichText object. 
  virtual void RemoveText(FSint32 startIndex,FSint32 endIndex); 							//Deletes text from a text object that has copied text.
  virtual void InsertRun(void *text,FSint32 text_len,TUIEncoding encoding,void *pInsertHere); 				//Insert a text run by reference.
  virtual void ChangeRun(void *new_text,FSint32 length_text,TUIEncoding encoding,void *old_text,FSint32  hints); 	//Change a referenced text run.
  virtual void RemoveRun(void *text); 											//Removes a referenced text run from a IUIRichText object.
  virtual void GetRunRange(void *text,FSint32 *startIndex,FSint32 *endIndex); 						//Gets the index range of a run in a IUIRichText.
  virtual void GetParagraphRange(FSint32 paragraph,FSint32 *pStartIndex,FSint32 *pEndIndex); 				//Gets the index range of a paragraph in a TsText.
  virtual void GetParagraphCount(FSint32 *pParagraphCount); 								//Gets the number of paragraphs in a TsText.
  virtual void GetParagraph(FSint32 char_index,FSint32 *pParagraphNumber); 						//Gets the paragraph number for a given index in TsText.
  virtual void GetLength(FSint32 *pLength); 										//Returns the text length in characters.
  virtual void GetChar(FSint32 index,FUint32 *pChar); 									//Returns the character at a given index as a UTF32 value.
  virtual void SetAlignment(TUITextAlignment value,FSint32 startIndex,FSint32 endIndex); 				//Sets the paragraph alignment attribute.
  virtual void SetBackgroundColor(int color,FSint32 startIndex,FSint32 endIndex);					//Sets background color attribute.
  virtual void SetBiDiOverride(TUITextDirection textdir,FSint32 startIndex,FSint32 endIndex);				//Sets the bidirectional override attribute.
  virtual void SetCharSpacing(float value,FSint32 startIndex,FSint32 endIndex); 					//Sets additional spacing between characters.
  virtual void SetEdgeColor(int color,FSint32 startIndex,FSint32 endIndex); 						//Sets edge color attribute.
  virtual void SetFontStyle(void *pIUIFontStyle,FSint32 startIndex,FSint32 endIndex);					//Sets the font style attribute.
  virtual void SetIndent(TUITextAlignment align,FSint32 startIndex,FSint32 endIndex);					//Sets the amount the first line of each paragraph is indented.
  virtual void SetInlineObjectID(wchar_t imageID,FSint32 startIndex,FSint32 endIndex);
  virtual void SetInlineObjectMetrics(wchar_t imageID,TUIInlineObjectMetrics *pValue);
  virtual void SetJustification(TBool value,FSint32 startIndex,FSint32 endIndex);
  virtual void SetLanguage(int language,FSint32 startIndex,FSint32 endIndex);
  virtual void SetLeftIndent(float value,FSint32 startIndex,FSint32 endIndex);						//Sets the amount the left side of each paragraph in the range is indented.
  virtual void SetLinePaddingAfter(float value,FSint32 startIndex,FSint32 endIndex);					//Sets an amount of extra space to pad after each line.
  virtual void SetLinePaddingBefore(float value,FSint32 startIndex,FSint32 endIndex);					//Sets an amount of extra space to pad before each line.
  virtual void SetLineSpacing(TUILineSpacing value,FSint32 startIndex,FSint32 endIndex);				//Sets line spacing attribute.
  virtual void SetLineSpacingAmount(float value,FSint32 startIndex,FSint32 endIndex);					//Sets line spacing amount attribute.
  virtual void SetLineSpacingAttributeMode(TBool isParaAttrib);
  virtual void SetOverline(TUILineStyle value,FSint32 startIndex,FSint32 endIndex);					//Sets text overline attribute.
  virtual void SetOverlineColor(int color,FSint32 startIndex,FSint32 endIndex);						//Sets overline color attribute.
  virtual void SetRightIndent(float value,FSint32 startIndex,FSint32 endIndex);						//Sets the amount the right side of each paragraph in the range is indented.
  virtual void SetSpaceAfter(float value,FSint32 startIndex,FSint32 endIndex);
  virtual void SetSpaceBefore(float value,FSint32 startIndex,FSint32 endIndex);
  virtual void SetStrikeout(TUILineStyle value,FSint32 startIndex,FSint32 endIndex);
  virtual void SetStrikeoutColor(int color,FSint32 startIndex,FSint32 endIndex);
  virtual void SetTextColor(int color,FSint32 startIndex,FSint32 endIndex);
  virtual void SetTextDirection(TUITextDirection value,FSint32 startIndex,FSint32 endIndex);				//Sets the paragraph base text direction attribute.
  virtual void SetUnderline(TUILineStyle value,FSint32 startIndex,FSint32 endIndex);
  virtual void SetUnderlineColor(int color,FSint32 startIndex,FSint32 endIndex);
  virtual void SetWordSpacing(float value,FSint32 startIndex,FSint32 endIndex);						//Sets additional spacing between words.
  virtual void GetAlignment(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,TUITextAlignment *pAlignment);
  virtual void GetBackgroundColor(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,int *pBackgroundColor);
  virtual void GetBiDiOverride(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,TUITextDirection *pValue);
  virtual void GetCharSpacing(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *pSpacing);
  virtual void GetEdgeColor(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,int *pEdgeColor);
  virtual void GetFontStyle(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,void **ppIFontStyle);
  virtual void GetIndent(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *pIndent);
  virtual void GetInlineObjectID(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,wchar_t *id);
  virtual void GetInlineObjectMetrics(wchar_t id,TUIInlineObjectMetrics *pValue);
  virtual void GetJustification(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,bool *justification_value);
  virtual void GetLanguage(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,void *pLanguage);
  virtual void GetLeftIndent(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *indent_value);
  virtual void GetLinePaddingAfter(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *line_padding_value);
  virtual void GetLinePaddingBefore(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *line_padding_value);
  virtual void GetLineSpacing(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,TUILineSpacing *pLineSpacing);
  virtual void GetLineSpacingAmount(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *pLineSpacingAmount);
  virtual bool GetLineSpacingAttributeMode();
  virtual void GetOverline(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,TUILineStyle *pValue);
  virtual void GetOverlineColor(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,int *color);
  virtual void GetRightIndent(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *indent_value);
  virtual void GetSpaceAfter(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *space);
  virtual void GetSpaceBefore(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *space);
  virtual void GetStrikeout(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,TUILineStyle *strikeout_style);
  virtual void GetStrikeoutColor(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,int *color);
  virtual void GetTextColor(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,int *color);
  virtual void GetTextDirection(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,TUITextDirection *pTextDirection);
  virtual void GetUnderline(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,TUILineStyle *pUnderline);
  virtual void GetUnderlineColor(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,int *pUnderlineColor);
  virtual void GetWordSpacing(FSint32 index,FSint32 *startIndex,FSint32 *endIndex,float *word_spacing);
};


class ITextRenderingFactory: public IUnknown
{
public:
  virtual void CreateFontResourceManager(IUnknown * pIUIFontResourceManager);
  virtual void CreateFontStyleManager(IUnknown * pIUIFontStyleManager);
  virtual void CreateRichText(IRichText ** pRichText);
  virtual void CreateTextContainer(IUnknown * pIUITextContainer);
  virtual void CreateRichTextLayout(IRichText * pRichText,IUnknown * pIUIRichTextLayoutControl,IUnknown * pIUIRichTextLayoutOptions,IRichTextLayout ** pRichTextLayout);
  virtual void CreateSingleLineLayout(IUnknown * pIUIRichTextLayoutControl,IUnknown * pIUIRichTextLayoutOptions,IUnknown ** pIUISingleLineLayout);
  virtual void CreateRichTextLayoutControl(IUnknown ** pIUIRichTextLayoutControl);
  virtual void CreateRichTextLayoutOptions(IUnknown ** pIUIRichTextLayoutOptions);
  virtual void CreatePolygon(IUnknown **pIUIPolygon);
  virtual void CreateBasicTextHandler(IUnknown ** pIUIBasicTextHandling);
};


class ITextRenderingManager: public IUnknown
{
public:
  virtual void GetTextRenderingFactory(ITextRenderingFactory ** pTextRenderingFactory);
  virtual void GetFontConfigFilePath(IUnknown ** pIString);
  virtual void GetFontEngineConfigFilePath(IUnknown ** pIString);
};


class IImageManager: public IUnknown
{
public:
  virtual void CreateFromPath(wchar_t * fpath,wchar_t * fname,char * pMime,long mimeLength,IUIImage ** pUIImage);
  virtual void CreateFromCanvas(IUnknown * pGC,IUIImage ** pUIImage);
  virtual void CreateFromIcon(long imageID,IUIImage ** pUIImage);
#if defined(DB3200) || defined(DB3210) || defined(DB3350)
  virtual void CreateFromIconWithIconSet(long imageID,IUnknown * pIconSet,IUIImage ** pUIImage);
#endif
  virtual void CreateFromMemory(char * buf_image,long bufferSize,wchar_t * extension,long extensionLength,char * pMime,long mimeLength,IUIImage ** pUIImage);
  virtual void CreateFromSimpleLockableBuffer(IUnknown * pISimpleLockableBuffer,long imageDataSize,wchar_t * extension,long extensionLength,char * pMime,long mimeLength,IUIImage ** pUIImage);
  virtual void Draw(IUIImage * pUIImage,IUnknown * pGC,TUIRectangle rect);
};


class IUIImage: public IUnknown
{
public:
  virtual void GetDimensions(long * pWidth,void * pWidthUnit,long * pHeight,void * pHeightUnit);
  virtual void CreateRenderer(IUnknown * pIUIImageRenderer);
  virtual void IsAnimation(bool * IsAnimation);
  virtual void IsScalable(bool * IsScalable);
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
