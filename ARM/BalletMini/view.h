#ifndef _VIEW_H_
  #define _VIEW_H_

#include "../inc/pnglist.h"

#define _NOREF (0xFFFFFFFF)

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
}OMS_HEADER_COMMON;

typedef struct
{
  unsigned short res1[9];
  unsigned short tag_count;
  unsigned short part_current;
  unsigned short part_count;
  unsigned short res2;
  unsigned short Stag_count;
  unsigned short res3;
  unsigned char res4;
  unsigned short cachable;
  unsigned short res5;
}OMS_HEADER_V2;

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
  unsigned int begin;
  unsigned int end;
  int tag;
  //unsigned int form_id1;
  //unsigned int form_id2;
  unsigned int id;
  unsigned int value;
  unsigned int id2;
  char no_upload:1, reserved:7;
  //int group_id;
  //int checked;
}REFCACHE;


typedef struct
{
  z_stream *zs;                // Поток для ZLib
  
  int cached;
  
  unsigned short *rawtext;
  unsigned int rawtext_size;
  
  LINECACHE *lines_cache;
  unsigned int lines_cache_size;

  unsigned int view_pos;
  unsigned int view_line;
  unsigned int displayed_lines;
  
  WSHDR *ws;
  
  unsigned int pos_cur_ref;
  
  unsigned int pos_first_ref;
  unsigned int pos_last_ref;
  unsigned int pos_prev_ref;
  unsigned int pos_next_ref;
  
  //
  char *oms;
  int oms_size;
  int oms_pos;
  int oms_wanted;
  int parse_state;
  //
  int tag_l_count;
  //
  TAG_S *S_cache;
  int S_cache_size;
  TAG_S current_tag_s;
//  int prev_bold;
//  int prev_underline;
  
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
  
  //References
  REFCACHE work_ref;
  REFCACHE *ref_cache;
  int ref_cache_size;
  
  int page_sz;
  int loaded_sz;
  unsigned short wchar_hr;
  
  char *pageurl;    //utf8
  
}VIEWDATA;

#endif


