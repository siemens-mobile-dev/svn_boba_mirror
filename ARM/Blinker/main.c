#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"

#define TICK 216
#define IN_TICKS(sec) (sec * TICK)

#define CFGUPD_INTV 5

// Импорт переменных
extern const int cfgEvents;
extern const unsigned int cfgMaxEv;
extern const unsigned int cfgInterval;
extern const int cfgLockOnly;
extern const unsigned int cfgPeriod;
extern const unsigned int cfgBright;

extern const int cfgKbd;
extern const int cfgDispl;
#ifndef NEWSGOLD
extern const int cfgDyn;
#else
extern const int cfgLighter;
#endif

int count = 0;
int cycl_cnt = 0;

GBSTMR mytmr;

void LightOff(void);
void Check(void);

//SetIllumination(unsigned char dev,unsigned long param1,unsigned short bright,unsigned long delay);
//0 - дисплей, 1 - клавиатура, 2 - динамический свет (x65)
void LightOn()
{
	if (cfgDispl)
		SetIllumination(0, 1, cfgBright, 0);
	if (cfgKbd)
		SetIllumination(1, 1, cfgBright, 0);   
#ifndef NEWSGOLD
	if (cfgDyn)
		SetIllumination(2, 1, cfgBright, 0);
#else
	if (cfgLighter)
		SetIllumination(4, 1, cfgBright, 0);
#endif
	GBS_StartTimerProc(&mytmr, IN_TICKS(cfgPeriod) / 20, LightOff);
}

void LightOff()
{
	if (cfgDispl)
		SetIllumination(0, 1, 0, 0);
	if (cfgKbd)
		SetIllumination(1, 1, 0, 0);   
#ifndef NEWSGOLD
	if (cfgDyn)
		SetIllumination(2, 1, 0, 0);
#else
	if (cfgLighter)
		SetIllumination(4, 1, 0, 0);
#endif
	if (--count)
		GBS_StartTimerProc(&mytmr, IN_TICKS(cfgPeriod) / 20, LightOn);
	else
		GBS_StartTimerProc(&mytmr, IN_TICKS(cfgInterval), Check); 
}

void Check(void)
{
	if (IsUnlocked() && cfgLockOnly)
		GBS_StartTimerProc(&mytmr, cfgInterval, Check);
	else
	{
		if ((count = GetMissedEventCount(cfgEvents)) > 0
			&& (cfgDispl || cfgKbd || 
#ifndef NEWSGOLD
				cfgDyn)
#else
				cfgLighter)
#endif
			)
		{
			if (count > cfgMaxEv)
				count = cfgMaxEv;
			LightOn();
		}
	}
	if (++cycl_cnt > CFGUPD_INTV)
	{
		InitConfig();
		cycl_cnt = 0;
	}
}

int main(void)
{
	InitConfig();
	Check();
	return 0;
}
