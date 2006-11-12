#ifdef NEWSGOLD

 	RSEG	RET_ADRS:CODE

	PUBLIC	OldOnClose
OldOnClose:
	DCD	0xA02FCD7A+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA02FCB52+1
	
	PUBLIC	OldShowMsg
OldShowMsg:
	DCD	0xA097789E+1

#else
	RSEG	RET_ADRS:CODE

	PUBLIC	OldOnClose
OldOnClose:
	DCD	0xA17143F0+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA171422C+1
        
	PUBLIC	OldTxtReg
OldTxtReg:
	DCD	0xA171422C+1        
	
	PUBLIC	ESIOld
ESIOld:
	DCD	0xA1559B4C+1
#endif

	END