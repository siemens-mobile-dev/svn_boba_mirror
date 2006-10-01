#ifndef _SIZE_T_DEF
typedef unsigned int    size_t;
#define _SIZE_T_DEF
#endif

extern void WriteEEPROMData(int Block, void far *Data,int Offset, int Size, int Zero1, int Zero2, int Zero3);
extern void ReadEEPROMData(int Block, void far *Data,int Offset, int Size, int Zero1, int Zero2, int Zero3);

extern int EE_RAW_ClearBlock(size_t num, void far* pid, size_t msg);
extern int EE_RAW_WriteBlock(size_t num, void far* data, int, int, void far* pid, size_t msg);
