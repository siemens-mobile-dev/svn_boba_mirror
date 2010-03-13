#ifndef __INCLUDE_MEM__
#define __INCLUDE_MEM__

#include "..\include\Lib_Clara.h"

#ifdef NDEBUG

#define trace_init()
#define trace_done()

#else

void trace_init();
void trace_done();
void trace_alloc(int mt, void* ptr,char *file, int line);
void trace_free(int mt,void* p, char* file, int line);

//эти нужны обязательно
void * __deleaker_malloc(int size, char* __file__,  int __line__);
void  __deleaker_mfree(void *p, char* __file__,  int __line__);

//delete из-за delete[] макросом не переопределить, поэтому только
//включением оптимизации с галкой "function inlining"
#pragma inline=forced
inline void operator delete(void* p){ __deleaker_mfree(p, __FILE__, __LINE__); }
#pragma inline=forced
inline void operator delete[](void* p){ __deleaker_mfree(p, __FILE__, __LINE__); }

inline void* operator new(size_t size, char *file, int line){ return  __deleaker_malloc(size, file, line ); }
inline void* operator new[](size_t size, char *file, int line){ return __deleaker_malloc( size, file, line ); }

#define NEW new (__FILE__, __LINE__)
#define new NEW
/*
#pragma inline=forced
inline void* operator new(size_t size){ return  __deleaker_malloc(size, __FILE__, __LINE__ ); }
#pragma inline=forced
inline void* operator new[](size_t size){ return __deleaker_malloc( size, __FILE__, __LINE__ ); }
*/

