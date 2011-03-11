#include "..\inc\swilib.h"
#include "CFontWork.h"
#define RGB16(R, G, B) (((B>>3)&0x1F) | ((G<<3)&0x7E0) | ((R<<8)&0xF800))

_cfont cfont;

void LoadFont(const char *fname)
{
  unsigned err;
  int f;
  if ((f=fopen(fname,A_ReadOnly+A_BIN,P_READ,&err))!=-1)
  {
//  int f = FileOpen("test.cfnt", fmOpenRead);
    lseek(f, 8, 0, &err, &err);

    fread(f, &cfont.height, 1, &err);
    fread(f, &cfont.width, 1, &err);

    lseek(f, 13, 0, &err, &err);

    fread(f, &cfont.regions_cnt, 2, &err);

    cfont.regions = malloc(sizeof(_region)*cfont.regions_cnt);

    lseek(f, 32, 0, &err, &err);
  
    cfont.symbol_cnt = 0;
    for(int i = 0; i < cfont.regions_cnt; i++)
    {
      fread(f, &cfont.regions[i].reg_start, 4, &err);
      fread(f, &cfont.regions[i].reg_end, 4, &err);
      cfont.symbol_cnt += (cfont.regions[i].reg_end - cfont.regions[i].reg_start)+1;
    }
  
    cfont.symbols = malloc(sizeof(_symbol)*cfont.symbol_cnt);
    
    lseek(f, 4, 1, &err, &err);
    
    int bitmap_size = 0;
    for(int i = 0; i < cfont.symbol_cnt; i++)
    {
      fread(f, &cfont.symbols[i].width, 1, &err);
      int dots = cfont.symbols[i].width*cfont.height;
      bitmap_size += (dots>>3) + ((dots&0x07)?1:0);
    }
 
    lseek(f, 4, 1, &err, &err);
    cfont.font_bitmap = malloc(bitmap_size);
    fread(f, cfont.font_bitmap, bitmap_size, &err);
  
    bitmap_size = 0;
    for(int i = 0, chr = 0; i < cfont.regions_cnt; i++)
      for(unsigned j = cfont.regions[i].reg_start; j <= cfont.regions[i].reg_end; j++, chr++)
      {
        int dots = cfont.symbols[chr].width*cfont.height;
        cfont.symbols[chr].bitmap = cfont.font_bitmap + bitmap_size;
        bitmap_size += (dots>>3) + ((dots&0x07)?1:0);
      }
  
    fclose(f, &err);
  }
}

void UnloadFont()
{
  mfree(cfont.font_bitmap);
  mfree(cfont.symbols);
  mfree(cfont.regions);
}

int RenderWChar(IMGHDR *img, int xx, int yy, unsigned c, unsigned color)
{
  unsigned char byte;
  for(int i = 0, chr = 0; i < cfont.regions_cnt; i++)
  {
    if(c >= cfont.regions[i].reg_start && c <= cfont.regions[i].reg_end)
    {
      for(int y = 0, bits = 0, byteptr = 0; y < cfont.height; y++)
        for(int x = 0; x < cfont.symbols[chr+c-cfont.regions[i].reg_start].width; x++)
        {
          if(!bits)
          {
            byte = cfont.symbols[chr+c-cfont.regions[i].reg_start].bitmap[byteptr++];
            bits = 8;
          }
          if(xx+x < img->w && yy+y < img->h && (byte&1))
            ((unsigned *)img->bitmap)[(xx+x) + (yy+y) * img->w] = color;
          byte >>= 1;
          bits--;
        }
      
      
      return xx+cfont.symbols[chr+c-cfont.regions[i].reg_start].width;
    }
    chr += cfont.regions[i].reg_end - cfont.regions[i].reg_start + 1;
  }
  return xx;
}

int RenderString(IMGHDR *img, int x, int y, WSHDR * str, int flag, unsigned color, unsigned bcolor)
{
  unsigned short *s = str->wsbody+1;
  unsigned short n = *str->wsbody;
  x+=flag&1;
  int xx = x;
  while(n)
  {
    if(*s == '\n')
    {
      y += cfont.height;
      xx = x;
    }
    else
      xx = RenderWChar(img, xx, y, *s, color);
    s++;
    n--;
  }

  if(flag&1)
  {
    for(int y = 1; y < img->h-1; y++)
      for(int x = 1; x < img->w-1; x++)
        if(((unsigned *)img->bitmap)[x + y * img->w] != color)
          if(((unsigned *)img->bitmap)[(x-1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y-1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x-1) + (y-1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y-1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x-1) + (y+1) * img->w] == color)
             ((unsigned *)img->bitmap)[(x)   + (y) * img->w] = bcolor;
    for(int y = 1; y < img->h-1; y++)
    {
        int x = 0;
        if(((unsigned *)img->bitmap)[x + y * img->w] != color)
          if(((unsigned *)img->bitmap)[(x+1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y-1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y-1) * img->w] == color)
             ((unsigned *)img->bitmap)[(x)   + (y) * img->w] = bcolor;
        x = img->w-1;
        if(((unsigned *)img->bitmap)[x + y * img->w] != color)
          if(((unsigned *)img->bitmap)[(x-1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y-1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x-1) + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x-1) + (y-1) * img->w] == color)
             ((unsigned *)img->bitmap)[(x)   + (y) * img->w] = bcolor;
    }
    for(int x = 1; x < img->w-1; x++)
    {
        int y = 0;
        if(((unsigned *)img->bitmap)[x + y * img->w] != color)
          if(((unsigned *)img->bitmap)[(x-1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y+1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x-1) + (y+1) * img->w] == color)
             ((unsigned *)img->bitmap)[(x)   + (y) * img->w] = bcolor;
        y = img->h-1;
        if(((unsigned *)img->bitmap)[x + y * img->w] != color)
          if(((unsigned *)img->bitmap)[(x-1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x)   + (y-1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x+1) + (y-1) * img->w] == color ||
             ((unsigned *)img->bitmap)[(x-1) + (y-1) * img->w] == color)
             ((unsigned *)img->bitmap)[(x)   + (y) * img->w] = bcolor;
    }
  }
  return x;
}

