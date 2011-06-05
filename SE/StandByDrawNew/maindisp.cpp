#include "maindisp.hpp"

char* CMainDisp::getName()
{
	return "StandByDrawNew_DISP";
}

void CMainDisp::onDraw(int a, int b, int c)
{
	TEXTID strid = STR("test");
	DrawString(strid, 0, 
			   20,25,100,40, 
			   0, 0, 0xFF0000FF, 0xFF0000FF);
	TextID_Destroy(strid);	
}
