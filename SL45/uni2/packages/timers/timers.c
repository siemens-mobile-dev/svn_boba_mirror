#include "reg167.h"
#include "..\..\include\_sl45.h"
#include "..\..\include\_uni2.h"


void hookSetIdleBackTimer()
{
 return_hookSetIdleBackTimer(120000); //ms
}


void My_StartTimerLightOff(void)
{
 if (*(int far*)0x3BAEA!=0) return;
 StartTimer2((GBSTMR*)0x3BAE6, 15*217, (void huge*)0xC1FB48); //step=1/217 s
}

void My_StartTimerInputSymbol(void)
{
//173
 StartTimer2((GBSTMR*)0x3B9AC, 80, (void huge*)0xC1E2FE); //step=1/217 s
}

void My_StartTimerReenterSymbol(void)
{
//130
 StartTimer2((GBSTMR*)0x3B9A8, 64, (void huge*)0xC1E2A6); //step=1/217 s
}

void My_StartTimerInquirySymbol(void)
{
//86
 StartTimer2((GBSTMR*)0x3B9A8, 50, (void huge*)0xC1E2A6); //step=1/217 s
}

void hookSetTimerAutoKeyLock(char far*data, long ms,void huge* proc ,int _1)
{
 SetTimerAutoKeyLock(data,60000,proc,_1);
 ms=ms;
}