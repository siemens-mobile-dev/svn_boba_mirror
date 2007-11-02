#ifndef _MAIN_H_
  #define _MAIN_H_


typedef struct {
  void *next;
  char *title;
  char *description;
}RSS_ITEM;

extern RSS_ITEM *rss_first;
extern RSS_ITEM *rss_last;


#endif
