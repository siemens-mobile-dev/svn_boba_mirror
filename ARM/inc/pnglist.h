typedef struct
{
  void *next;
  char *pngname;
  IMGHDR * img;
}PNGLIST;

typedef struct
{
  void *next;
  int icon;
  IMGHDR * img;
}DYNPNGICONLIST;

typedef struct
{
  PNGLIST * pltop;
  char *bitmap;
  DYNPNGICONLIST *dyn_pltop;
}PNGTOP_DESC;  
