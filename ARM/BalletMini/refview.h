//
//  ALL THE RENDERER NEED
//  add content by add items util, then render by this util
//  this -> addItems
//  this !-> refCache, just hold pos of the ref
//

// линия кончается с данными параметрами, т.е. это параметры следующей строки
// выравнивание, же, - для данной линии
typedef struct
{
  unsigned int pos;  // позиция последего символа в строке
  unsigned short ink1;
  unsigned short ink2;
  unsigned short paper1;
  unsigned short paper2;
  unsigned short pixheight;
  unsigned short bold:1, underline:1, ref:1,center:1,right:1,centerAtAll:1;
}LINECACHE;

typedef struct
{
  unsigned short *rawtext;
  unsigned int rawtext_size;
  
  LINECACHE *lines_cache;
  unsigned int lines_cache_size;
  
  unsigned int view_pos;  // del
  unsigned int view_line;
  int pixdisp;
  
  WSHDR *ws;
  
  unsigned int pos_cur_ref;
  
  unsigned int pos_first_ref;
  unsigned int pos_last_ref;
  unsigned int pos_prev_ref;
  unsigned int pos_next_ref;

}REFVIEW;


