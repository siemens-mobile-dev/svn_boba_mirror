#define RGB24(R,G,B,A)   (B           | (G<<8)         |  (R<<16) | (A<<25))
#define RGB16(R,G,B)   (((B>>3)&0x1F) | ((G<<3)&0x7E0) | ((R<<8)&0xF800))
#define RGB8(R,G,B)    (((B>>6)&0x3)  | ((G>>2)&0x1C)  | ((R<<5)&0xE0)) 

#define MIN(a,b) (a<b)?a:b
#define MIN3(a,b,c) MIN(a,MIN(b,c))
#define MAX(a,b) (a>b)?a:b
#define MAX3(a,b,c) MAX(a,MAX(b,c))

#define NSENSORS 6

typedef struct{
  int type;
  int corner;
  int div;
  int hhh;
  void (*init)(const int);
  void (*deinit)();
  void (*tick)();
  int (*getValue)(const int,const int);
  WSHDR *(*getText)(const int,const int);
  int (*getColor)(const int,const int);
  int (*getFont)(const int,const int);
}TSensor;
