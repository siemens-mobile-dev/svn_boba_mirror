// (c) mamaich, mamaich@uymail.com

#include <reg167.h>
#include <regc166sv1.h>

int PrevT3CON;
int PrevP8;
int PrevDP8;
unsigned char huge *Buffer;

// base addr: 15100

void SRVWDT(void);
void CLI(void)
{
	if(IEN==0)
	{
		SRVWDT();
		return;
	}

	IEN=0;
	PrevT3CON=T3CON;
#pragma asm				
	mov     T3,   #0C62h
	mov     T3CON,#00011000111b
#pragma endasm	
}

void STI(void)
{
	if(IEN==1)
		return;
	T3CON=PrevT3CON;
	IEN=1;
}

void SRVWDT(void)
{
	if(IEN==1)
		return;
	if(T3IR)
	{
#pragma asm				
	    bclr   T3IR   
	    bmovn  P4.1, P4.1
	    mov    T3, #0C62h
#pragma endasm	
	}
}

void SEND(int a)
{
	S0TIR=0;	
	S0TBUF=(unsigned char)(a);	
	while(S0TIR==0)
		SRVWDT();
}

int RECV(void)
{
	while(S0RIR==0)
		SRVWDT();
	S0RIR=0;	
	return ((int)S0RBUF)&255;
}

void huge print(char far *c)
{
	while(*c)
		SEND(*(c++));
}

void puts(char far *Cmd)
{
	if(Cmd[0])
		print(Cmd);
	print("\r\n");
}

long RECV3(void)
{
	long R=RECV();
	R<<=8;
	R|=RECV();
	R<<=8;
	R|=RECV();
	return R;
}

unsigned int RECV2(void)
{
	unsigned int R=RECV();
	R<<=8;
	R|=RECV();
	return R;
}

void SEND2(int a)
{
	SEND(a>>8);
	SEND(a);
}

void SEND2LSB(int a)
{
	SEND(a);
	SEND(a>>8);
}

void ReadRam(void)
{
	long Addr=RECV3();
	long Size=RECV3();
	char huge*Ptr=(char huge*)Addr;
	int CRC=0;
	
	while(Size)
	{
		SEND(*Ptr);
		CRC^=*Ptr;
		Ptr++; Size--;
	}	
	SEND('O');
	SEND('K');
	SEND(CRC);
	SEND(0);
}

/*
void SimulateBoots(void)
{
	int Len;
	SEND(0xA0);
	Len=RECV();
	while(Len)
	{
		RECV();
		Len--;
	}
	RECV();	// CRC
	SEND(0xA5);
	Len=RECV();
	while(Len)
	{
		RECV();
		Len--;
	}
	RECV();	// CRC
	SEND(0xA5);
	Len=RECV();
	while(Len)
	{
		RECV();
		Len--;
	}
	RECV();	// CRC
	SEND(0xA6);
	Len=RECV() | (RECV()<<8);
	while(Len)
	{
		RECV();
		Len--;
	}
	RECV();	// CRC
	SEND('O');
	SEND('K');
}

const char far Hex[]="0123456789ABCDEF";

char ToHex(int c)
{
	return Hex[c&15];
}

void PrintHex(unsigned int H)
{
	char Buff[5];
	Buff[3]=Hex[H&15];
	Buff[2]=Hex[(H>>4)&15];
	Buff[1]=Hex[(H>>8)&15];
	Buff[0]=Hex[(H>>12)&15];
	Buff[4]=0;
	print(Buff);
}
*/

int Is4KWBlock(long A)	// 64k or 8K
{
	if((A>=0xBF0000L && A<0xC00000L) || (A>=0xFF0000L))
		return 1;	// 8K
	return 0;		// 64K or invalid
}

void EraseFlashBlock(unsigned char huge *Addr)
{
	*Addr=0x60;	// unlock block
	*Addr=0xD0;	// 
	*Addr=0x50;	// clear status
	*Addr=0x20;	// erase
	*Addr=0xd0;	// confirm
	*Addr=0x70;	// read status

	do
	{
		SRVWDT();
	} while(((*Addr)&128) == 0);
	*Addr=0xFF;	// read mode
}

void ProgramFlashWord(unsigned char huge *Addr, int Data)
{
	*Addr=0x60;	// unlock block
	*Addr=0xD0;	// 
	*Addr=0x50;	// clear status
	*Addr=0x40;	// program
	(*(int huge*)Addr)=Data;	
	*Addr=0x70;	// read status

	do
	{
		SRVWDT();
	}while(((*Addr)&128) == 0);

	*Addr=0xFF;	// read mode
	// I leave the block unlocked!
}

void WriteFlash(void)
{
	long Addr=0xA00000L+(((long)RECV2())<<12L);
	int BlockSize=16; 	// = 65536/4096
	int i;
	int ADDCRC=0;
	int XORCRC=0;
	unsigned int huge *Buff=(unsigned int huge *)Buffer;
	long FlashBase=0xA00000L;
	if(Addr>=0xC00000L)
		FlashBase=0xC00000L;

	if(Is4KWBlock(Addr))
		BlockSize = 2;	// = 8192/4096

	SEND2LSB(FlashBase/16384);
	SEND2LSB(Addr%16384);
	SEND2LSB(Addr/16384);
	SEND2LSB(0xC4);
	SEND2LSB(0x89);
	SEND2LSB(BlockSize);
	SEND2LSB(-1);
	while(BlockSize>0)
	{
		for(i=0; i<2048; i++)
		{
			int t=RECV();
			int t1=RECV();
			XORCRC^=t^t1;
			t|=t1<<8;
			*Buff=t;
			Buff++;
		}

		BlockSize--;
	}
	if(RECV()!=XORCRC)
	{
		SEND2LSB((int)0xBBBB);
		return;
	}
	SEND2LSB(FlashBase/16384);
	SEND2LSB(Addr/16384);
	SEND2LSB(Addr%16384);

	// erasing
	EraseFlashBlock((unsigned char huge*)Addr);

	SEND2LSB(0x0202);
	SEND2LSB(FlashBase/16384);
	SEND2LSB(Addr/16384);
	SEND2LSB(Addr%16384);

	// writing

	if(Is4KWBlock(Addr))
		BlockSize = 2;	
	else
		BlockSize = 16;	

	Buff=(unsigned int huge *)Buffer;
	while(BlockSize>0)
	{
		for(i=0; i<2048; i++)
		{
			ProgramFlashWord((unsigned char huge*)Addr,*Buff);
			Addr+=2;
			ADDCRC+=*Buff;
			Buff++;
		}
		BlockSize--;
	}

	SEND2LSB(0x0303);
	SEND2LSB(ADDCRC);
	SEND('O');
	SEND('K');
}

void ChangeSpeed(void)
{
	int Speed=RECV();
	SEND('h');
	// Set 115200 
	switch(Speed)
	{
	case 0:	// 57600
		S0BG=0xD;
		S0FDV=0x1FC;
		break;
	case 1:	// 115200
		S0BG=6;
		S0FDV=0x1FC;
		break;
	case 2:	// 234000
		S0BG=2;
		S0FDV=0x1B3;
		break;
	case 3:	// 460800?
		S0BG=0;
		S0FDV=0x122;
		break;
	};

	RECV();	// skip 'A'
	SEND('H');
}

void huge main ()
{
	int Cmd;
	int i;

	print("v 0.2\r\n");
	CLI();

	PrevP8=P8;
	PrevDP8=DP8;

// Enable writing to the second flash
#pragma asm				
	bset P8.1
	bset DP8.1
	db 0D1h, 080h
	db 0E6h, 09Fh, 06h, 00h
S0FDV		DEFR	0FEB6h
#pragma endasm				

//	Using 64K memory at 080000
	Buffer=(unsigned char huge*)0x080000;

	puts("OK");
	do
	{
		Cmd=RECV();
		switch(Cmd)
		{
		case 'A':
			SEND('R');
			break;
		case 'H':
			ChangeSpeed();
			break;
		case 'I':
			for(i=0; i<560; i++)
				SEND('0');
			break;
		case 'R':
			ReadRam();
			break;
		case 'F':
			WriteFlash();
			break;
		case 'Q':
		case 'Z':
		case 'X':
			Cmd='X';
			break;
		default:
			puts("Illegal command");
			break;
		}

	} while(Cmd!='X');
	puts("Bye");
	
// Restore previous register values						 
	P8=PrevP8;
	DP8=PrevDP8;
#pragma asm
	db 0D1h, 080h
	db 0E6h, 09Fh, 00h, 00h
#pragma endasm

	STI();

// Restore old COM-speed
#pragma asm
	db 0D7h, 060h, 00Fh, 000h
	db 0F2h, 05Ah, 0DCh, 02Bh
	db 0F2h, 0D8h, 0DEh, 02Bh
	db 0F2h, 05Bh, 0E0h, 02Bh
#pragma endasm
//	puts("Exiting...");
}    
