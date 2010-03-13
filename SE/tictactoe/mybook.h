#ifndef _MYBOOK_H_
#define _MYBOOK_H_

#include "..\include\Lib_Clara.h"
#include "..\classlib\cl.h"
#include "gamedisp.hpp"

class CMyBook:public CBook
{
	CGuiBase* gui;

	static const PAGE_DESC base_page;
protected:
	~CMyBook();
public:
	CMyBook();

	static int TerminateElf(CBook** pbookmanbook, CMyBook* book);
	static int ShowAuthorInfo(CBook** pbookmanbook, CMyBook* book);
	//static void OnAccept(CBookBase* book,void *);
	static void OnBack(CBookBase* book,CGuiBase *);
};

#endif
