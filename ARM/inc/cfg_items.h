#define CFG_UINT 1
#define CFG_STR_UTF8 2
#define CFG_INT 3
#define CFG_STR_WIN1251 4
#define CFG_CBOX 5

//#pragma pack(1)
typedef struct
{
  int type;
  char name[32];
  unsigned int min;
  unsigned int max;
}CFG_HDR;
//#pragma pack()





