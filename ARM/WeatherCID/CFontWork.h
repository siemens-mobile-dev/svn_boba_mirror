typedef struct 
{
  unsigned reg_start;
  unsigned reg_end;
} _region;
typedef struct 
{
  unsigned char width;
  unsigned char *bitmap;
} _symbol;
typedef struct 
{
 unsigned char width, height;
 short regions_cnt;
 _region *regions;
 int symbol_cnt;
 unsigned char *font_bitmap;
 _symbol *symbols;
} _cfont;
  
extern void LoadFont(const char *fname);
extern void UnloadFont();
extern int RenderWChar(IMGHDR *img, int xx, int yy, unsigned c, unsigned color);
extern int RenderString(IMGHDR *img, int x, int y, WSHDR * str, int flag, unsigned color, unsigned bcolor);
