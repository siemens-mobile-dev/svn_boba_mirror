#ifndef _MYBOOK_H_
#define _MYBOOK_H_

#include "..\include\Lib_Clara.h"
#include "..\classlib\cl.h"
#include "mydisp.h"

class CMyBook:public CBook
{
	CGuiT<CMyDisp>* gui;
	
	static const PAGE_DESC base_page;
protected:
	~CMyBook();
public:
	CMyBook();
	
	//  static int Accept(void *msg, CMyBook* book);
	//  static int Cancel(void *msg, CMyBook* book);
	
	static void OnBack(CBookBase* book,CGuiBase *);
};

#endif
