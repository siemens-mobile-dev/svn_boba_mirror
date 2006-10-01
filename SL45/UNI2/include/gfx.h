#define ScreenBufferA   (unsigned char far*)0x960C
#define ScreenBufferB   (unsigned char far*)0xAC0C

typedef struct
{
  char  w;
  char  h;
  int   bpnum; //For BW=1, 8bit=5, 16bit=8, 0x80 - packed
  void far* bitmap;
}IMGHDR;

extern void far* AM_QueryDisplayOfThisProcess(void);
extern IMGHDR far* GetBitmapResource(void far* Display, int imgID);
//DplClass_GetBitmapResource(AM_QueryDisplayOfThisProcess(),id)

extern void StoreImageAsImageID(int imgID, IMGHDR far *img, unsigned char far *imgBuff);
extern void DrawImage(int x, int y, int width, int height, IMGHDR far* img);
extern void DrawPredefinedImage(int x, int y, int imgID);

extern void ClearRect(int x,int y,int w,int h); 
extern void FillRect(int x,int y,int w,int h,int color); 
// fill styles
#define white      0x0
#define xor        0x1
#define black      0x2

extern void DrawString(int x,int y,int w,int h,const char far *str,int font);
// font types
#define font_4x7        0x0
#define wap_big         0x1
#define normal_big      0x2 //+bold
#define digit_4x7       0x4 //+bold
#define digit_5x5       0x6
#define normal          0x7 //+bold
#define wap             0x9
#define digit_big       0xA
#define bold            0x1 
typedef struct
{
 unsigned int X1;
 unsigned int Y1;
 unsigned int X2;
 unsigned int Y2;
}rectXYXY;

typedef struct{
  char dummy[0x1A];
} DRWOBJ;

extern XYWHtoRectXYXY(rectXYXY far *, int x, int y, int w, int h);
extern CreateColorBitmap(DRWOBJ* drwobj, rectXYXY far *, unsigned int flag1, IMGHDR far* img);
extern DrawObject(DRWOBJ* drwobj);
extern PrepDrawObj_type01(
 DRWOBJ* drwobj,
 rectXYXY far*,
 //0x80 - исп. буфер для строки
 unsigned int flag1,
 WSTR *,
 unsigned int font,
 //0-left,1-center,2-right
 unsigned int flag2);

void InvalidateScreenRect(int x,int y,int w,int h);


/*
Коды рисунков в юникоде(только стандартный шрифт?)
0xE400, "..."
0xE401, телефон домашний
0xE402, клин какой то
0xE403, будильник
0xE404, морда авто
0xE405, книга
0xE406, <C
0xE407, зелёная труба=взять трубку
0xE408, динамик
0xE409, замок
0xE40A, колокольчик=аларм
0xE40B, красная труба=полопжить трубку
0xE40C, [CB] инвертированное
0xE40D, конверт
0xE40E, конверт c ! = память заполнена
0xE40F, ">"
0xE410, непрочитанный конверт
0xE411, контур стрелки вверх с подчёркиванием+заполненная стрелка вниз с надчёркиванием
0xE412, мелкая точка посередине
0xE413, [!] инверованный
0xE414, катушки=херня типа ()_()
0xE415, "->"
0xE416, "<-"
0xE417, добавить в t9
0xE418, переключить слово в t9
0xE419, симка
0xE41A, "@"
0xE41B, ключ на симке
0xE41C, мобила боком
0xE41D, губы
0xE41E, vip
0xE42F, смайл
0xE420, положенная трубка
0xE421, мобила вертикально
0xE422, стрелка вниз над скобкой
0xE423, стрелка вверх над скобкой
0xE424, закрытая книга (ластик=)
0xE425, лупа
0xE426, "<"
0xE427, ">"
0xE428, вверх
0xE429, вниз
*/    

// after hooks of draw
extern void DrawAllSofts(void far* data1, void far* data2, int soft);
extern void DrawMainScreen(void);
extern void DrawShutdownAnimation(void);
