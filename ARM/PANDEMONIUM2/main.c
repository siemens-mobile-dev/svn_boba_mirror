#include "include.h" 

extern void InitConfig();

//==============================================================================
// ELKA Compatibility
#pragma inline
static void patch_header(const HEADER_DESC* head)
{
  ((HEADER_DESC*)head)->rc.x=0;
  ((HEADER_DESC*)head)->rc.y=YDISP;
  ((HEADER_DESC*)head)->rc.x2=ScreenW()-1;
  ((HEADER_DESC*)head)->rc.y2=HeaderH()-1+YDISP;
}
//==============================================================================

PandemonuimDialog * maincsm;

char basepath[128];

int main(char * exename)
{ 
  InitConfig();
  
  strcpy(basepath,exename);
  strrchr(basepath,'\\')[1]=0;
    
//create csm  
  maincsm = new PandemonuimDialog();
  maincsm->Create();  
  return 0;
}



