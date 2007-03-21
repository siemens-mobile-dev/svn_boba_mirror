#include "..\inc\swilib.h"
#include "cfg.h"

#define idlegui_id (((int *)data)[0x2C/4])

extern const int cfgShowIn;

extern const unsigned int cfgX;
extern const unsigned int cfgY;

extern const int cfgOrient;

extern const int cfgWidth;
extern const int cfgHeight;

extern const int cfgInterval;

extern const char cfgBorderCol[4];
extern const char cfgBGCol[4];
extern const char cfg0FillCol[4];
extern const char cfg4FillCol[4];

CSM_DESC icsmd;

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
	
	DrawCanvas(CanvasData, cfgX, cfgY, cfgX + cfgWidth - 1, cfgY + cfgHeight - 1, 1);
	
	AcrossDim = cfgOrient ? cfgWidth : cfgHeight;
	intr = (AcrossDim - cfgInterval) % 2 ? cfgInterval + 1 : cfgInterval;
	WorH1 = (AcrossDim - intr) / 2;
	
	if (!cfgOrient)
	{
		DrawRectangle(cfgX, cfgY, cfgX + cfgWidth - 1, cfgY + WorH1 - 1,
					  0, cfgBorderCol, cfgBGCol);
		DrawRectangle(cfgX, cfgY + WorH1 + intr, cfgX + cfgWidth - 1, cfgY + cfgHeight - 1,
					  0, cfgBorderCol, cfgBGCol);
		
		DrawRectangle(cfgX, cfgY, cfgX + cfgWidth * ratio0 / 100 - 1, cfgY + WorH1 - 1,
					  0, cfgBorderCol, cfg0FillCol);
		DrawRectangle(cfgX, cfgY + WorH1 + intr, cfgX + cfgWidth * ratio4 / 100 - 1, cfgY + cfgHeight - 1,
					  0, cfgBorderCol, cfg4FillCol);
	}
	else
	{
		DrawRectangle(cfgX, cfgY, cfgX + WorH1 - 1, cfgY + cfgHeight - 1,
					  0, cfgBorderCol, cfgBGCol);
		DrawRectangle(cfgX + WorH1 + intr, cfgY, cfgX + cfgWidth - 1, cfgY + cfgHeight - 1,
					  0, cfgBorderCol, cfgBGCol);
		
		DrawRectangle(cfgX, cfgY + cfgHeight * (100 - ratio0) / 100 - 1, cfgX + WorH1 - 1, cfgY + cfgHeight - 1,
					  0, cfgBorderCol, cfg0FillCol);
		DrawRectangle(cfgX + WorH1 + intr, cfgY + cfgHeight * (100 - ratio4) / 100 - 1, cfgX + cfgWidth - 1, cfgY + cfgHeight - 1,
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
		if (strcmp(successed_config_filename,(char *)msg->data0)==0)
		{
			ShowMSG(1,(int)"DiskMon config updated!");
			UpdateConfig();
		}		
	}
	csm_result = old_icsm_onMessage(data,msg);

	if (IsGuiOnTop(idlegui_id))
	{
		GUI *igui=GetTopGUI();
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
