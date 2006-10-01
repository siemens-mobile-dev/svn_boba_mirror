typedef struct
{
 void far *constr_data;
 void far *begin;
 unsigned int dummy;
 unsigned int free;
 void far *p2;
}_heap_;

/*
//Заголовок данных в куче
typedef struct
{
 struct VAR_HEAP_HDR far * next;
 unsigned int siz;
 char name[8];
}VAR_HEAP_HDR;
*/

extern _heap_* AllHeapsPointers[];
extern _heap_ SIMHeap_mo;         //Free 1800 bytes
extern _heap_ DBAL_Heap;          //Free 7928 bytes
extern _heap_ ImageHeap_mo;       //Free 750 bytes
extern _heap_ OMA_Heap;           //Free 2902 bytes
extern _heap_ DisplayQueue_;      //Free 3984 bytes
extern _heap_ SystemFARHeap0;     //Free 668 bytes
extern _heap_ SystemFARHeap1;     //Free 13292 bytes
extern _heap_ SystemFARHeap2;     //Free 18 bytes
//uni_vars, mp3 playlist, daemons
extern _heap_ SystemFARHeap3;     //Free 16376 bytes
extern _heap_ SystemFARHeap4;     //Free 24 bytes
extern _heap_ SystemFARHeap6;     //Free 236 bytes
extern _heap_ SystemFARHeap8;     //Free 16376 bytes
extern _heap_ SystemFARHeap9;     //Free 30 bytes
extern _heap_ SystemFARHeap10;    //Free 76 bytes
extern _heap_ I2CQueue;           //Free 496 bytes
extern _heap_ DSM2BFB_my_heap;    //Free 746 bytes
extern _heap_ L1Heap;             //Free 0 bytes
extern _heap_ ssHeap;             //Free 500 bytes
extern _heap_ XRC_my_heap;        //Free 568 bytes
extern _heap_ EX_Heap;            //Free 15718 bytes
//for other use MMIMalloc, MMIHeapFree
extern _heap_ MMIHeap_mo;         //Free 14450 bytes
extern _heap_ RAP_Heap_;          //Free 3072 bytes
extern _heap_ FS_Heap;            //Free 1016 bytes
extern _heap_ SSTEXTEHeap_mo;     //Free 90 bytes
extern _heap_ l2farHeap;          //Free 15200 bytes
extern _heap_ FSS_FarHeap_;       //Free 320 bytes
extern _heap_ MessageMem;         //Free 1534 bytes
extern _heap_ TimerMem;           //Free 406 bytes
extern _heap_ SIZE_1_HEAP;        //Free 126 bytes
extern _heap_ SIZE_2_HEAP;        //Free 1800 bytes
extern _heap_ SIZE_3_HEAP;        //Free 0 bytes
extern _heap_ SIZE_4_HEAP;        //Free 456 bytes
extern _heap_ SIZE_5_HEAP;        //Free 2736 bytes
extern _heap_ CbsPageHeap;        //Free 450 bytes
extern _heap_ l2Heap;             //Free 34 bytes
extern _heap_ rrnbheap;           //Free 182 bytes

extern void* HeapMalloc (_heap_ far * _heap_pnt,unsigned int siz);
extern void  HeapFree   (_heap_ far * _heap_pnt,void far *mem);

extern void* FarMemAlloc(_heap_ far* _heap_pnt,unsigned int siz);
extern void  FarMemFree (_heap_ far* _heap_pnt,void far *mem);

//Explorer
extern void* EX_heap_malloc_with_lock   (unsigned int siz);
extern void  EX_heap_free_with_lock     (void far *mem);

//MMI
extern void* MMIMalloc                  (unsigned int sz);
extern void  MMIHeapFree                (void far *mem);

