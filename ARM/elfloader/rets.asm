	RSEG	DATA_N
	RSEG	SWILIB_FUNC1B8_1BB:CODE
	EXTERN	EXT2_AREA
	EXTERN	pltop
	EXTERN	pLIB_TOP
	DCD	EXT2_AREA
	DCD	pltop
	DCD	pLIB_TOP
	DCD	SFE(DATA_N)



        RSEG	CODE:CODE
        
#ifdef NEWSGOLD
#ifdef	ELKA

	PUBLIC	OldOnClose
OldOnClose:
	DCD	0xA066E5D6+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA066E374+1
	
	PUBLIC	OldShowMsg
OldShowMsg:
	DCD	0xA09554F2+1

	PUBLIC	PITgetN
PITgetN:
	DCD	0xA0A38D44
	
	PUBLIC	PITret
PITret:
	DCD	0xA095393C+1

#else

	PUBLIC	OldOnClose
OldOnClose:
	DCD	0xA02FCD7A+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA02FCB52+1
	
	PUBLIC	OldShowMsg
OldShowMsg:
	DCD	0xA097789E+1
	
	PUBLIC	PITgetN
PITgetN:
	DCD	0xA0A27720
	
	PUBLIC	PITret
PITret:
	DCD	0xA0975D3E+1
	
#endif
#else
	
	PUBLIC	OldOnClose
OldOnClose:
	DCD	0xA17143F0+1
	
	PUBLIC	OldOnCreate
OldOnCreate:
	DCD	0xA171422C+1
        
	PUBLIC	ESIOld
ESIOld:
	DCD	0xA1559B4C+1
        
        PUBLIC	PITgetN
PITgetN:
	DCD	0xA0CB75A6+1
	
	PUBLIC	PITret
PITret:
	DCD	0xA0CB75BC+1
        
        PUBLIC	KEYBret
KEYBret:
	DCD	0xA0CB602E+1        
#endif

	END