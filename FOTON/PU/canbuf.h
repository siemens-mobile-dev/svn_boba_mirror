//
// CAN buffers structures
//

typedef struct _TCANTXBUF
{
 struct //must be reversed
 {
  WORD
  	EIDHI:	2,
  	dummy2:	1,
  	EXIDE:	1,
  	dummy1: 1,
  	SID:	11;
 };
 WORD EID; //must be reversed
 struct
 {
  char	
  	DLC:	4,
  	dummy4:	2,
  	RTR:	1,
	dummy3:	1;
 };
 char D[8];
} TCANTXBUF;

typedef struct _TCANRXBUF
{
 struct //must be reversed
 {
  WORD
  	EIDHI:	2,
  	dummy1:	1,
  	IDE:	1,
  	SRR:	1,
  	SID:	11;
 };
 WORD EID; //must be reversed
 struct
 {
  char	
  	DLC:	4,
  	dummy2:	2,
  	RTR:	1,
	dummy3:	1;
 };
 char D[8];
} TCANRXBUF;
