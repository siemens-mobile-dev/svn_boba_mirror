#include "..\inc\swilib.h"

#define number 4

int fp;
unsigned int iError;
 
void read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length){
 fread(fp, data, length, &iError);
}


int main_(){
  
  char buf[number];

  if ((fp=fopen("4:\\test.png", A_ReadOnly, P_READ, &iError))==0) return -1;

  if (fread(fp, &buf, number, &iError)!=number){fclose(fp, &iError); return -2;} 
  
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)0, 0, 0);
    if (!png_ptr) return (-3);

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return (-4);
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr,
          (png_infopp)NULL);
        return (-5);
    }

extern int setjmp(jmp_buf jmpbuf);
extern void png_set_read_fn(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
extern void  png_read_update_info(png_structp png_ptr,void *info_ptr);
extern void  png_set_palette_to_rgb(png_structp png_ptr);

   if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        fclose(fp, &iError);
        return (-6);
    }

    voidp read_io_ptr;

    png_set_read_fn(png_ptr, read_io_ptr, read_data_fn);

    png_set_sig_bytes(png_ptr, number);
    
    png_read_info(png_ptr, info_ptr);
       
    png_uint_32 width, height;
    int bit_depth, color_type;

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);
   
   if (bit_depth == 16) png_set_strip_16(png_ptr);

   if (bit_depth < 8) png_set_packing(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    
    
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
          png_set_gray_to_rgb(png_ptr);

   png_read_update_info(png_ptr, info_ptr);

   png_uint_32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);
   
   char *row=malloc(rowbytes);
   char *img=malloc(width*height);
   
   for (unsigned int y = 0; y<height; y++){
    png_read_row(png_ptr, (png_bytep)row, NULL);
    for (unsigned int x = 0; x<width; x++){
     img[x+y*width]=(row[x*4+0] & 0xE0)|((row[x*4+1]>>3)&0x1C)|((row[x*4+2]>>6)&0x3);
    }
   }

   png_read_end(png_ptr, end_info);

   png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
   
   fclose(fp, &iError);
   
   mfree(row);
   
   return (0);
}

int main(){
  char a[2]={0,0};
  a[0]=(char)(48-main_());
  ShowMSG(1,(int)&a);   
return 0;
}
