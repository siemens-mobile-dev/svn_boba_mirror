#ifndef _URLSTACK_H_
#define _URLSTACK_H_

#define PageSTACK_SIZE (32)

char *PushPageToStack(void);
char *PopPageFromStack(void);
char *ForwardPageFromStack(void);
void FreePageStack(void);
void UpPageStack(void);
void* getPageParams(void);
unsigned int getViewLine(void* data);
unsigned int getPosCurRef(void* data);
void setPageParams(unsigned int view_line, unsigned int pos_cur_ref);
int CheckPageStackTop(void);
void InitUrlStack(void);
char* getCurrPageName();

#endif /* _URLSTACK_H_ */
