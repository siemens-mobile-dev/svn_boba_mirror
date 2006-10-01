#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"

void PLSMalloc(void)
{
 pPls=HeapMalloc(&SystemFARHeap3, sizeof(*pPls));
 memset(pPls,0,sizeof(*pPls));
 MP3_Started=1;
}

void PLSHeapFree(void)
{
 HeapFree(&SystemFARHeap3,pPls);
 pPls=0;
 MP3_Started=0;
 if (Is26MHz) Set13MHz();
}

void MP3UpKey(void)
{
 pPls->Action=-1;
 #pragma asm
 mov     r12, #35E8h ;pid_MMI
 #pragma endasm
}

void MP3DownKey(void)
{
 pPls->Action=1;
 #pragma asm
 mov     r12, #35E8h ;pid_MMI
 #pragma endasm
}


void InitOrder(int far* Order, int Count)
{
 int i;
 for(i=0; i<Count; i++) Order[i]=i;
}

void Shuffle(int far *Order, int Count)
{
 int i;
 for(i=0; i<3*Count; i++) {
  int p1=random(Count);
  int p2=random(Count);
  int t=Order[p1];
  Order[p1]=Order[p2];
  Order[p2]=t;
 }
}


void My_Org_Mp3File(char far *Mp3Struct, long PlPos)
{
// MP3Struct offsets:
// 56h: dd 30 dup(?)    - pointers to file names in playlist
// 3Ah: dd              - pointer to some struct, which contains at 0ffset 26h pointer to contents of playlist file
// 3Eh: dd              - number of songs in playlist
 char far* Playlist=*(char far**)(0x26+*(char far**)(Mp3Struct+0x3A));
 char far* Song;
 char far* tmp;
 int Len;
 int PPos;
 long MShuffle;
/////////////////////////////////////////////////////////////////////////////////////
 if(Playlist!=0) {
  if(Playlist!=pPls->Playlist)       // When playlist is auto-generated - it contains filenames, and it is allocated somewhere
   strcpy(pPls->Playlist,Playlist);
  pPls->Count=1;       // Calculate number of songs in list, empty playlists not supported
  Playlist=pPls->Playlist;
  for(; *Playlist; ) {
   if(*Playlist==0x0A) {
    pPls->Count++;
    while(*Playlist && ((unsigned char)*Playlist)<32) Playlist++;
   }
   Playlist++;
  }
 // Erase trailing partial song title
  Playlist--;
  if(!(Playlist[0]=='3' && (Playlist[-1]=='p' || Playlist[-1]=='P') && 
      (Playlist[-2]=='m' || Playlist[-2]=='M') && Playlist[-3]=='.') )
       pPls->Count--;

  while(!(Playlist[0]=='3' && (Playlist[-1]=='p' || Playlist[-1]=='P') && 
         (Playlist[-2]=='m' || Playlist[-2]=='M') && Playlist[-3]=='.') )
          {
           *Playlist=0;
           Playlist--;
          }
  InitOrder(pPls->Order,pPls->Count);
  ReadEEPROMData(5164,&MShuffle,0,4,0,0,0);
  if (MShuffle&0x10) Shuffle(pPls->Order,pPls->Count);
  if (MShuffle&0x100) pPls->Repeat=1;
   else pPls->Repeat=0;
  pPls->Pos=-1; //Playlist first loaded
  pPls->Action=0;
 }
/////////////////////////////////////////////////////////////////////////////////////
 Playlist=pPls->Playlist;
 Song=Playlist;           
 if (pPls->Action==0) {
  pPls->Pos++;
  if (!pPls->Repeat)
   if (pPls->Pos==pPls->Count) 
    GeneralFunc(1);//Return to prev screen
 } else {
  pPls->Pos+=pPls->Action;
  pPls->Action=0;
 }
 while(pPls->Pos<0) pPls->Pos+=pPls->Count;
 while(pPls->Pos>=pPls->Count) pPls->Pos-=pPls->Count;
 PPos=pPls->Order[pPls->Pos];
 for(; *Playlist; ) {
  if(PPos<=0) break;
  if(*Playlist==0x0A) {
   while(*Playlist && ((unsigned char)*Playlist)<32) Playlist++;
   Song=Playlist;
   PPos--;
  }
  Playlist++;
 }
 Len=0;
 while((unsigned char)Song[Len]>31) Len++;

 tmp=*(char far**)(Mp3Struct+0x56);

 if(tmp) {
  memcpy(tmp,Song,Len);
  tmp[Len]=0;
 }
 Org_Mp3File(Mp3Struct,0);
 PlPos=PlPos;
}

void MP3_From_Menu(int t)
{
 if (MP3_Started) return;
 MP3Player(t);
}

void MP3_From_Explorer(char far* d1, char far* d2)
{
 if (MP3_Started) return;
 PlayMP3file(d1,d2);
}