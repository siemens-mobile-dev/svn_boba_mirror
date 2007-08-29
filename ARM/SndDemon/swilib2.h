typedef  unsigned char byte;
typedef  unsigned short word;
typedef  unsigned int dword;

#define PTT_BUTTON 0x27



#pragma swi_number=0x80D6
__swi __arm unsigned int* RamNetOnline();

#ifdef NEWSGOLD
#pragma swi_number=0x1FF
__swi __arm void MEDIA_PLAYLAST();

#pragma swi_number=0x1F8
__swi __arm unsigned int _GetPlayStatus(void);
#endif


