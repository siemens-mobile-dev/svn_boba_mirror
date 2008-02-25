#include "..\\include\Lib_Clara.h"

union SIGNAL
{
  SIGSELECT signo;
  struct HELPER_SIGNAL hsig;
};

const SIGSELECT sg[]={3,HELPER_SIG,HELPER_SIG+1,HELPER_SIG+2};
EP_DATA * getepd(void);

void HelperEntryPoint(void)
{
  union SIGNAL *sig;
  for(;;)
  {
    sig=receive(sg);
    switch(sig->signo)
    {
    case HELPER_SIG:

      {
	if (sig->hsig.PROC) sig->hsig.PROC(sig->hsig.param1,sig->hsig.param2);
      }
      break;
    case HELPER_SIG+1:
      {
      }
      break;
    }
    free_buf(&sig);
  }
}


void StartHelper(void)
{
  __get_epd;

  epd->HPID=create_process(0,"HelperProc",HelperEntryPoint,512,30,0,0,0,0,0);
  start(epd->HPID);

}

void SUBPROCExec (void(*PROC)(int,void*),int p1 , void * p2)
{
  __get_epd;
  union SIGNAL *sig;

  sig = alloc(sizeof(union SIGNAL),HELPER_SIG);
  sig->hsig.PROC=PROC;
  sig->hsig.param1=p1;
  sig->hsig.param2=p2;
  send(&sig,epd->HPID);
}

void MMIPROCExec (void(*PROC)(int,void*),int p1 , void * p2)
{
  union SIGNAL *sig;

  sig = alloc(sizeof(union SIGNAL),HELPER_SIG);
  sig->hsig.PROC=PROC;
  sig->hsig.param1=p1;
  sig->hsig.param2=p2;
  send(&sig,(*PID_MMI())&~0x8000);
}

void tst(int r0, void *r1)
{
}


int ParseHelperMessage(struct HELPER_SIGNAL * mess)
{
  int PID=GetCurrentPID();

  switch(mess->signo)
  {
  case HELPER_SIG:
    if (mess->PROC) mess->PROC(mess->param1,mess->param2);
    break;
  case HELPER_SIG+1:
    break;
  case HELPER_SIG+2:
    break;
    /*
  default:
    {
      if(PID==PID_MMI)
      {
	__get_epd;
//        LIST ** OSE_H=(LIST**)pOSE_HOOK_LIST;
//	if (*(LIST**)pOSE_HOOK_LIST)
	{
	  LIST *OSE_Hook_List=epd->OseHookList;
	  {
	    SIGSELECT sig=mess->signo;
//	    if (OSE_Hook_List)
	    {
	      int n=OSE_Hook_List->FirstFree;
	      if (n)
	      {
		OSE_HOOK_ITEM **osehook=(OSE_HOOK_ITEM**)(OSE_Hook_List->listdata);
		n--;
		do
		{
		  OSE_HOOK_ITEM *item=osehook[n];
		  //                *((int *)(0x4C04B1CC+12))=(int)osehook;
		  if (item->signo==sig) item->HOOK(mess);
		  //osehook++;
		}
		while(n--);
	      }
	    }
	  }
	}
      }
    }
    break;
    */
  }

  return(PID);
}
