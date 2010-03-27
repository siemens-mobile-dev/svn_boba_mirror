#include "..\\include\Lib_Clara.h"
#include "vars.h"

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
				if (sig->hsig.PROC)
					sig->hsig.PROC(sig->hsig.param1,sig->hsig.param2);
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
	
	epd->HPID=create_process(0,"HelperProc",HelperEntryPoint,32768,30,0,0,0,0,0);
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
		if (mess->PROC)
			mess->PROC(mess->param1,mess->param2);
		break;
	case HELPER_SIG+1:
		break;
	case HELPER_SIG+2:
		break;
	default:
	}
	
	return(PID);
}
