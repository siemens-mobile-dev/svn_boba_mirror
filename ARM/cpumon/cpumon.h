#define RGB16(R, G, B) (((B>>3)&0x1F) | ((G<<3)&0x7E0) | ((R<<8)&0xF800))

typedef struct{
  int type;
  void (*init)(const int);
  void (*deinit)();
  void (*tick)(const unsigned int);
  unsigned int (*getValue)(const unsigned int,const unsigned int);
  unsigned int (*getColor)(const unsigned int,const unsigned int);
}TSensor;
