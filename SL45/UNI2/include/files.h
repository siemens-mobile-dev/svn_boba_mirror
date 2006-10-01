#ifndef _STR_DEF
typedef char far*               STR;
typedef unsigned int far*       WSTR;
#define _STR_DEF
#endif

extern int GBSS_po_open(const STR name,int flag1, int flag2, int far *gbss_buf);
extern int GBSS_po_read(int handle, void far *buf, int bnum, int far *gbss_buf);
extern int GBSS_po_write(int handle, void far *buf, int bnum, int far *gbss_buf);
extern int GBSS_po_close(int handle, int far *gbss_buf);

//only over FilesysICall
extern int FileOpen(const STR name,int flag1,int flag2);
//oflag is an Integer constant combinations defined in FCNTL.H:
#define _O_RDONLY       0x0000  /* open for reading only */
#define _O_WRONLY       0x0001  /* open for writing only */
#define _O_RDWR         0x0002  /* open for reading and writing */
#define _O_APPEND       0x0008  /* writes done at eof */
#define _O_CREAT        0x0100  /* create and open file */
#define _O_TRUNC        0x0200  /* open and truncate */
#define _O_EXCL         0x0400  /* open only if file doesn't already exist */
#define _O_TEXT         0x4000  /* file mode is text (translated) */
#define _O_BINARY       0x8000  /* file mode is binary (untranslated) */
#define _O_NOINHERIT    0x0080  /* child process doesn't inherit file */
#define _O_TEMPORARY    0x0040  /* temporary file bit */
#define _O_SHORT_LIVED  0x1000  /* temporary storage file, try not to flush */
#define _O_SEQUENTIAL   0x0020  /* file access is primarily sequential */
#define _O_RANDOM       0x0010  /* file access is primarily random */
//The pmode argument is required only when _O_CREAT is specified.
//If the file already exists, pmode is ignored.
//Otherwise, pmode specifies the file permission settings,
//which are set when the new file is closed the first time.
#define _S_IREAD        0000400         /* read permission, owner */
#define _S_IWRITE       0000200         /* write permission, owner */
#define _S_IEXEC        0000100         /* execute/search permission, owner */
extern int FileSeek(int handle,long pos, int ref,long far *result);
//ref - 0 - from begin //1 - from current //2 - from end
extern int FileRead(int handle,void far *buf, int bnum);
extern int FileWrite(int handle,void far *buf, int bnum);
extern int FileClose(int handle);

struct FILE_STAT
{ 
 unsigned int flag1;//=8081 - file, =8041 - folder 
 unsigned int Zero; 
 unsigned long filesize; 
 char Unk[20]; // Dates? 
}; 

extern int GetFileStat(const STR fname, struct FILE_STAT far*);
extern int FileStat(int file_hndl, struct FILE_STAT far*); 

extern int Rename(const STR source, const STR dest);

//FAM3
extern void FilesysICall(void huge *proc);
extern void FilesysICall_I(void huge *proc, int i);
extern void FilesysICall_UI(void huge *proc, unsigned int i);
extern void FilesysICall_II(void huge *proc, int i, int i2);
extern void FilesysICall_L(void huge *proc, long i);
extern void FilesysICall_UL(void huge *proc, unsigned long i);
extern void FilesysICall_FP(void huge *proc, void far *fp);







//cardexplorer
typedef struct
{
 char type;
 char marked;
 WORD unk1;
 WORD unk2;
 WORD unk3;
 STR far filename;
} EXPL_FILELIST_ITEM;

typedef struct
{
 char far* Zero1;
 char far* Data1; //.... IDLE_DIALOG
 char far* Data2; //CardExplorerDialog
 WORD Unk1;
 WORD Unk2;
 char far* Zero2;
 char far* Zero3;
 char far* Zero4; 
 void huge *HeapFree1;
 char far* Data3;
 char far* Data4;
 void huge *HeapFree2;
 WORD Unk3;
 WORD Unk4;
 char drive;//'A'
 char path[129];
 char filename[128];
 int far* UnkData[50];
 unsigned int total_folders;
 unsigned int total_files;
 WORD IsUpdated1;
 unsigned int total_marked_unmarked; //Ерунда какаято, всё время увеличивается
 WORD Unk5;
 unsigned int curr_file;
 WORD Unk6;
 WORD IsUpdated2;
 char far *filenames;
 EXPL_FILELIST_ITEM far *filelist;
 char far* Zero5;
 char far* Zero6;
 char far* Zero7;
 char far* Zero8;
 char far* Unk7;
 char far* Zero9;
} EXPL_DATA;

typedef struct
{
 WORD total_files;
 WORD unk2;
 WORD unk4;
 WORD unk6;
 WORD unk8;
 WORD unkA;
 WORD total_files2;
 WORD unkE;
 WORD unk10;
 WORD unk12;
 WORD unk14;
 WORD unk16;
 WORD unk18;
 char far *filenames;
 EXPL_FILELIST_ITEM far *filelist;
} EXPL_REQ;

extern void SendMessage2toMMCEXPL(EXPL_REQ far *data, STR catalog, const STR masks[], int one, int mask_filetype, unsigned long magic);
extern void FreeDataConstructedByEXPL(EXPL_REQ far *data);
