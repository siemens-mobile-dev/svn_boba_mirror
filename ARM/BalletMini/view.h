#ifndef _VIEW_H_
  #define _VIEW_H_

#include "../inc/pnglist.h"


typedef struct
{
  DYNPNGICONLIST dp;
  int w_char;
  int index;
}OMS_DYNPNGLIST;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  unsigned short magic;
  unsigned long size;
  unsigned short res1[9];
  unsigned short tag_count;
  unsigned short part_current;
  unsigned short part_count;
  unsigned short res2;
  unsigned short Stag_count;
  unsigned short res3;
  unsigned char res4;
  unsigned short cachable;
//  unsigned short res5;
}OMS_HEADER;
#pragma pack(pop)

typedef struct
{
  unsigned long 
  reserved:8,
  red:5, green:6, blue: 5,
  unk0:1,bold:1,underline:1,unk3:1,center:1,right:1,unk6:1,unk7:1;
}TAG_S;

typedef struct
{
  unsigned short red:5, green:6, blue: 5;
}TAG_D;

typedef struct
{
  unsigned int pos;
  unsigned short ink1;
  unsigned short ink2;
  unsigned short paper1;
  unsigned short paper2;
  unsigned short pixheight;
  unsigned short bold:1, underline:1, ref:1;
}LINECACHE;


typedef struct
{
  unsigned short *rawtext;
  unsigned int rawtext_size;
  
  LINECACHE *lines_cache;
  unsigned int lines_cache_size;

  unsigned int *ref_cache;
  unsigned int ref_cache_size;
  
  unsigned int view_pos;
  unsigned int view_line;
  
  WSHDR *ws;
  //
  char *oms;
  int oms_size;
  int oms_pos;
  int oms_wanted;
  int parse_state;
  //
  char **I_cache;
  int I_cache_size;
  TAG_S *S_cache;
  int S_cache_size;
  TAG_S current_tag_s;
  TAG_D current_tag_d;
  //
  unsigned int iw;
  unsigned int ih;
  //
  unsigned int tag_o_count;
  //
  unsigned int ref_mode;
  //
  OMS_DYNPNGLIST *dynpng_list;
}VIEWDATA;

#endif


