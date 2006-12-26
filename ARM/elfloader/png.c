#include "..\inc\swilib.h"
#define number 8



void* xmalloc(int x,int n)
{
  return malloc(n);
}

void xmfree(int x,void* ptr)
{
  mfree(ptr);
}

typedef struct
{
  int file_handler;
  unsigned int errno;
}READ_FILE;

 
__arm void read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  READ_FILE*read;
  read=png_get_io_ptr(png_ptr);
  fread(read->file_handler, data, length, &read->errno);
}



__arm IMGHDR* create_imghdr(const char* fname)
{
  READ_FILE read;
  char buf[number];
  if ((read.file_handler=fopen(fname, A_ReadOnly+A_BIN, P_READ, &read.errno))==-1) return 0;

  if (fread(read.file_handler, &buf, number, &read.errno)!=number)
  {
  L_CLOSE_FILE:
    fclose(read.file_handler, &read.errno);
    return 0;
  }
  if  (!png_check_sig((png_bytep)buf,number)) goto  L_CLOSE_FILE;
  
  png_structp png_ptr = png_create_read_struct_2("1.2.5", (png_voidp)0, 0, 0, (png_voidp)0,(png_malloc_ptr)xmalloc,(png_free_ptr)xmfree);
  if (!png_ptr) goto L_CLOSE_FILE;
  
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }
  
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }
  
  png_set_read_fn(png_ptr, &read, read_data_fn);
  
  png_set_sig_bytes(png_ptr, number);
  
  png_read_info(png_ptr, info_ptr);
  
  png_uint_32 width, height;
  int bit_depth, color_type;
  
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);
  
  if (bit_depth == 16) png_set_strip_16(png_ptr);
  
  if (bit_depth < 8) png_set_packing(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);
  
  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png_ptr);
  
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);
  
  png_set_filler(png_ptr,0xFF,PNG_FILLER_AFTER);

    
  png_read_update_info(png_ptr, info_ptr);
  
  png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  
  char *row=malloc(rowbytes);
  char *img=malloc(width*height);
  
  for (unsigned int y = 0; y<height; y++)
  {
    png_read_row(png_ptr, (png_bytep)row, NULL);
    for (unsigned int x = 0; x<width; x++)
    {
      if (!row[x*4+3])
        img[x+y*width]=0xC0;
      else
        img[x+y*width]=(row[x*4+0] & 0xE0)|((row[x*4+1]>>3)&0x1C)|((row[x*4+2]>>6)&0x3);
    }
  }
  IMGHDR * img_h=malloc(sizeof(IMGHDR));
  img_h->w=width;
  img_h->h=height;
  img_h->bpnum=5;
  img_h->zero=0;
  img_h->bitmap=img;
  
  
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
   mfree(row);
  fclose(read.file_handler, &read.errno);
  return (img_h);
}



#define DEFAULT_FOLDER "0:\\zbin\\img\\"
#define CACHE_PNG 50

typedef struct
{
  void *next;
  char *pngname;
  IMGHDR * img;
}PNGLIST;

__no_init PNGLIST * pltop;

#pragma inline
int tolower(int C)
{
  if ((C>='A' && C<='Z')) C-='A'-'a';
  return(C);
}

char* strcpy_tolow(char *s1,const char *s2)
{
  while(*s2)
  {
    *s1++=tolower(*s2++);   
  }
  *s1=0;
  return s1;
}

         
void free_png_list(PNGLIST *pl)
{
  mfree(pl->img->bitmap);
  mfree(pl->img);
  mfree(pl->pngname);
  mfree(pl);
}

void add_to_first(const char* fname,IMGHDR* img)   // »спользуетс€ дл€ добавлени€ в начало списка новых пнг, с перемещением старых
{
  PNGLIST *cur;
  PNGLIST *pl=pltop;
  PNGLIST *pl_prev;
  int n=0;
  while(pl)
  {
    if (n==CACHE_PNG-1)
    {
      free_png_list(pl_prev->next);
      pl_prev->next=0;
      break;
    }
    pl_prev=pl;
    pl=pl->next;
    n++;
  }
  cur=malloc(sizeof(PNGLIST));
  cur->pngname=malloc(strlen(fname)+1);
  strcpy(cur->pngname,fname);
  cur->img=img;
  cur->next=pltop;
  pltop=cur;
}

// »щем пнг в кеше
int find_png_in_cache(const char* fname)
{
  PNGLIST *pl=pltop;
  PNGLIST *pl_prev;
  int i=0;
  while(pl)
  {
    if (!strcmp(pl->pngname,fname))
    {
      if (!i)  return (0);
      pl_prev->next=pl->next;
      pl->next=pltop;
      pltop=pl;
      return (0);      
    }
    pl_prev=pl;
    pl=pl->next;
    i++;
  }
  return 1;
}

unsigned int div10(unsigned int v, unsigned int divisor, char *s)
{
 unsigned int c='0';
 while(v>=divisor)
 {
  v-=divisor;
  c++;
 }
 *s=c;
 return v;
}

void print10(char *s, unsigned int v)
{
  if (v>=10000) v=div10(v,10000,s++);
  if (v>=1000) v=div10(v,1000,s++);
  if (v>=100) v=div10(v,100,s++);
  if (v>=10) v=div10(v,10,s++);
  *s++=v+'0';
  *s='\0';
}


IMGHDR* PatchGetPIT(unsigned int pic)
{
  IMGHDR * img;
  char fname[256];
  if ((pic>>28)==0xA)
  {
    strcpy_tolow(fname,(char*)pic);
  }
  else
  {
    char*next=strcpy_tolow(fname,DEFAULT_FOLDER);
    print10(next,pic);
    strcat(fname,".png");
  } 
  if ((find_png_in_cache(fname)))
  {
    if (!(img=create_imghdr(fname))) return (0);
    add_to_first(fname,img);
  }
  return (pltop->img);   
}




