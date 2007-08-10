#include "..\inc\swilib.h"
#include "cfg.h"

#define idlegui_id (((int *)data)[0x2C/4])

extern const int cfgShowIn;

extern const RECT cfgRect;
extern const int cfgOrient;

extern const int cfgInterval;

extern const char cfgBorderCol[4];
extern const char cfgBGCol[4];
extern const char cfg0FillCol[4];
extern const char cfg4FillCol[4];

CSM_DESC icsmd;

#pragma inline=forced
int toupper(int c)
{
  if ((c >= 'a') && (c <= 'z'))
    c += 'A' - 'a';
  return(c);
}

int strcmp_nocase(const char *s1,const char *s2)
{
  int i;
  int c;
  while(!(i = (c = toupper(*s1++)) - toupper(*s2++))) 
    if (!c)
      break;
  return(i);
}

void UpdateConfig()
{
	if (LoadConfigData("4:\\ZBin\\etc\\DiskMon.bcfg") < 0)
	{
		LoadConfigData("0:\\ZBin\\etc\\DiskMon.bcfg");
	}
}

void IdleExecute(void *CanvasData)
{
	unsigned int AcrossDim, WorH1, intr, flex_free, flex_total;
        unsigned int Width, Height;
        
	unsigned char ratio0, ratio4;
	///////
	if ((cfgShowIn == 1 && IsUnlocked()) || (cfgShowIn == 2 && !IsUnlocked()))
		return;
	
	flex_free  = GetFreeFlexSpace(0, &intr) >> 10;
	flex_total = GetTotalFlexSpace(0, &intr) >> 10;
	ratio0 = 100 * (flex_total - flex_free) / flex_total;
	
	flex_free  = GetFreeFlexSpace(4, &intr) >> 10;
	flex_total = GetTotalFlexSpace(4, &intr) >> 10;
	ratio4 = 100 * (flex_total - flex_free) / flex_total;
	
	DrawCanvas(CanvasData, cfgRect.x, cfgRect.y, cfgRect.x2, cfgRect.y2, 1);
	
        Width  = cfgRect.x2 - cfgRect.x;
        Height = cfgRect.y2 - cfgRect.y;
        
	AcrossDim = cfgOrient ? Width : Height;
	intr = (AcrossDim - cfgInterval) % 2 ? cfgInterval + 1 : cfgInterval;
	WorH1 = (AcrossDim - intr) / 2;
	
	if (!cfgOrient)
	{
		DrawRectangle(cfgRect.x, cfgRect.y, cfgRect.x2, cfgRect.y + WorH1 - 1,
					  0, cfgBorderCol, cfgBGCol);
		DrawRectangle(cfgRect.x, cfgRect.y + WorH1 + intr, cfgRect.x2, cfgRect.y2,
					  0, cfgBorderCol, cfgBGCol);
		
		DrawRectangle(cfgRect.x, cfgRect.y, cfgRect.x + Width * ratio0 / 100 - 1, cfgRect.y + WorH1 - 1,
					  0, cfgBorderCol, cfg0FillCol);
		DrawRectangle(cfgRect.x, cfgRect.y + WorH1 + intr, cfgRect.x + Width * ratio4 / 100 - 1, cfgRect.y + Height - 1,
					  0, cfgBorderCol, cfg4FillCol);
	}
	else
	{
		DrawRectangle(cfgRect.x, cfgRect.y, cfgRect.x + WorH1 - 1, cfgRect.y2,
					  0, cfgBorderCol, cfgBGCol);
		DrawRectangle(cfgRect.x + WorH1 + intr, cfgRect.y, cfgRect.x2, cfgRect.y2,
					  0, cfgBorderCol, cfgBGCol);
		
		DrawRectangle(cfgRect.x, cfgRect.y + Height * (100 - ratio0) / 100 - 1, cfgRect.x + WorH1 - 1, cfgRect.y2,
					  0, cfgBorderCol, cfg0FillCol);
		DrawRectangle(cfgRect.x + WorH1 + intr, cfgRect.y + Height * (100 - ratio4) / 100 - 1, cfgRect.x2, cfgRect.y2,
					  0, cfgBorderCol, cfg4FillCol);
	}
}

int (*old_icsm_onMessage)(CSM_RAM*,GBS_MSG*);
void (*old_icsm_onClose)(CSM_RAM*);


void MyIDLECSM_onClose(CSM_RAM *data)
{
	extern void seqkill(void *data, void(*next_in_seq)(CSM_RAM *), void *data_to_kill, void *seqkiller);
	extern void *ELF_BEGIN;
	seqkill(data, old_icsm_onClose, &ELF_BEGIN, SEQKILLER_ADR());
}

int MyIDLECSM_onMessage(CSM_RAM* data,GBS_MSG* msg)
{
	int csm_result;
	if(msg->msg == MSG_RECONFIGURE_REQ) 
	{
		extern const char *successed_config_filename;
		if (strcmp_nocase(successed_config_filename, (char *)msg->data0) ==0)
		{
			ShowMSG(1, (int)"DiskMon config updated!");
			UpdateConfig();
		}		
	}
	csm_result = old_icsm_onMessage(data, msg);

	if (IsGuiOnTop(idlegui_id))
	{
		GUI *igui = GetTopGUI();
		if (igui)
		{
#ifdef ELKA
			{
				void *canvasdata = BuildCanvas();
#else
			void *idata = GetDataOfItemByID(igui, 2);
			if (idata)
      		{
				void *canvasdata = ((void **)idata)[DISPLACE_OF_IDLECANVAS / 4];
#endif          
				IdleExecute(canvasdata);
			}
		}
	}
	return(csm_result);
}


int main(void)
{
	UpdateConfig();
	LockSched();
	CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
	memcpy(&icsmd, icsm->constr, sizeof(icsmd));
	old_icsm_onMessage = icsmd.onMessage;
	old_icsm_onClose=icsmd.onClose;
	icsmd.onMessage = MyIDLECSM_onMessage;
	icsmd.onClose=MyIDLECSM_onClose;
	icsm->constr = &icsmd;
	UnlockSched();
	return 0;
}
