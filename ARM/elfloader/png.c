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
  char *row=NULL;
  char *img=NULL;
  IMGHDR * img_h=NULL;
  if ((read.file_handler=fopen(fname, A_ReadOnly+A_BIN, P_READ, &read.errno))==-1) return 0;
  
  if (fread(read.file_handler, &buf, number, &read.errno)!=number)
  {
  L_CLOSE_FILE:
    mfree(row);
    mfree(img);
    mfree(img_h);
    fclose(read.file_handler, &read.errno);
    return NULL;
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
  
  row=malloc(rowbytes);
  img=malloc(width*height);
  
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
  img_h=malloc(sizeof(IMGHDR));
  img_h->w=width;
  img_h->h=height;
  img_h->bpnum=5;
  //  img_h->zero=0;
  img_h->bitmap=img;
  
  
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  mfree(row);
  fclose(read.file_handler, &read.errno);
  return (img_h);
}


#ifdef NEWSGOLD
#define DEFAULT_FOLDER "4:\\ZBin\\img\\"
#else
#define DEFAULT_FOLDER "0:\\zbin\\img\\"
#endif
#define CACHE_PNG 50

typedef struct
{
  void *next;
  char *pngname;
  IMGHDR * img;
}PNGLIST;

volatile __no_init PNGLIST * pltop;

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

__arm unsigned int div10(unsigned int v, unsigned int divisor, char *s)
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

__arm void print10(char *s, unsigned int v)
{
  if (v>=10)
  {
    if (v>=100)
    {
      if (v>=1000)
      {
	if (v>=10000)
	{
	  v=div10(v,10000,s++);
	}
	v=div10(v,1000,s++);
      }
      v=div10(v,100,s++);
    }
    v=div10(v,10,s++);
  }
  *s++=v+'0';
  *s='\0';
}


__arm IMGHDR* PatchGetPIT(unsigned int pic)
{
  IMGHDR * img;
  unsigned int i;
  char fname[256];
  
  PNGLIST *pl;
  PNGLIST *pl_prev;
  PNGLIST *cur;
  
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
  LockSched();
  pl=(PNGLIST *)(&pltop);
  pl_prev=NULL;  
  while((pl=pl->next))
  {
    if (!strcmp(pl->pngname,fname))
    {
      //Найден, переносим в начало и выходим
      if (pl_prev)
      {
	//Только если не в самом начале
	pl_prev->next=pl->next; //Удалили из найденого места
	pl->next=(PNGLIST *)pltop; //Следующий - весь список
	pltop=pl; //А первый в списке - найденый
      }
      UnlockSched();
      return(pl->img);
    }
    pl_prev=pl; //Текущий обработанный - теперь предыдущий
  }
  UnlockSched();
  //Ничего не нашли, теперь пробуем добавить
  img=create_imghdr(fname);
  //if (!img) return (0); //Нечего добавлять
  cur=malloc(sizeof(PNGLIST)); //Создаем элемент списка
  cur->pngname=malloc(strlen(fname)+1);
  strcpy(cur->pngname,fname);
  cur->img=img;
  i=0; //Это количество элементов в списке
  LockSched();
  cur->next=(PNGLIST *)pltop; //Следующий - весь список
  pltop=cur; //Первый в списке - новый элемент
  //Теперь подрезаем конец
  pl=(PNGLIST *)(&pltop);
  do
  {
    pl_prev=pl;
    pl=pl->next;
    if (!pl)
    {
      //Закончились элементы раньше
      UnlockSched();
      return(cur->img);
    }
    i++;
  }
  while(i<=CACHE_PNG); //Пока количество элементов меньше допустимого
  pl_prev->next=NULL; //Обрежем список
  UnlockSched();
  //Остальное можно сделать с разлоченной многозадачностью
  do
  {
    //Удаляем текущий
    if (pl->img)
    { 
      mfree(pl->img->bitmap);
      mfree(pl->img);
    }
    mfree(pl->pngname);
    pl_prev=pl;
    pl=pl->next;
    mfree(pl_prev);
  }
  while(pl); //Пока есть элементы, освобождаем их
  return(cur->img);
}




