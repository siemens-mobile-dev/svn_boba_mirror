#ifndef __INCLUDE_MEM__
#define __INCLUDE_MEM__

#include "..\include\Lib_Clara.h"

#ifdef NDEBUG

#define trace_init()
#define trace_done()

void* operator new(size_t sz);
void* operator new[](size_t sz);
void operator delete(void * p);
void operator delete[](void * p);

void* operator new(size_t size, void* p);
void* operator new[](size_t size, void* p);
void operator delete (void*, void*);
void operator delete[] (void*, void*);

#else

#define DELEAKER

extern int __deleaker_skip;

enum trace_types
{
	trace_memory,
	trace_strid,
	trace_iconid,
	trace_timer,
	trace_file,
	trace_hook,
	trace_dll,
	trace_gc,
	trace_gvi,
	trace_gui,
	trace_book,
	trace_process,

	trace_unallocated,

	trace_typescount
};


void trace_init();
void trace_done();
void trace_alloc(int mt, void* ptr, const char* file, int line);
void* trace_alloc_ret(int mt, void* ptr, void* badvalue, const char* file, int line);
void trace_free(int mt,void* p, const char* file, int line);
void  __deleaker_mfree( const char* __file__, int __line__, void *p );
#pragma swi_number=0x103
__swi __arm void*  __original_malloc( int size );

//delete из-за delete[] макросом не переопределить, поэтому только
//включением оптимизации с галкой "function inlining"
#pragma inline=forced
inline void operator delete(void* p){ __deleaker_mfree( __FILE__, __LINE__, p ); }
#pragma inline=forced
inline void operator delete[](void* p){ __deleaker_mfree( __FILE__, __LINE__, p ); }


inline void* operator new(size_t sz){return __original_malloc(sz);};
inline void* operator new[](size_t sz){return __original_malloc(sz);};

inline void operator delete (void*, void*) { }
inline void operator delete[] (void*, void*) { }

inline void* operator new(size_t size, void* p){ __deleaker_skip=1; return p; }
inline void* operator new[](size_t size, void* p){ __deleaker_skip=1; return p; }


struct NewRecorder
{
	NewRecorder(const char* file, int lineNo)
		: mFile(file), mLineNo(lineNo)
		{
		}

		template <class T>
			T * operator<<(T* t) const
			{
				if(t)trace_alloc(trace_memory, (void*)t, mFile, mLineNo);
				return t;
			}

private:
	const char* mFile;
	const int mLineNo;
};

#define new NewRecorder(__FILE__,__LINE__) << new
