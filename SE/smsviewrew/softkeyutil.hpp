#ifndef _SOFTKEYUTIL_H_
#define _SOFTKEYUTIL_H_

#include "..\include\lib_clara.h"

class CSoftKeyDesc
{
private:
	CSoftKeyDesc();
	~CSoftKeyDesc();
	static int getstructtype();
	static int structtype;
public:
	bool IsVisible();
	u16 GetAction();
	TEXTID GetButtonText();
	void (*GetProc())(BOOK*,GUI*);
};

CSoftKeyDesc* FindSoftkey( DISP_OBJ* disp, BOOK* book, TEXTID strid );

#endif
