//
// CAN engine header file
//

//Определяет ПУ-А или ПУ-А1
#include "devdef.h"
#ifdef PUA1
#define CAN_CS PORTE_Bit6
#define CAN_INT PORTE_Bit7
#else
#define CAN_CS PORTE_Bit2
#define CAN_INT PORTE_Bit3
#endif

#define EnaCAN CAN_CS=0
#define DisCAN CAN_CS=1

#define WaitSPI while(!(SPSR&0x80))

#define CMD_RES	0xC0
#define CMD_RD	0x03
#define CMD_WR	0x02
#define CMD_RTS	0x80
#define CMD_RS	0xA0
#define CMD_BM	0x05

#define FILTER0	0x00
#define FILTER1	0x04
#define FILTER2	0x08
#define FILTER3	0x10
#define FILTER4	0x14
#define FILTER5	0x18

#define MASK0	0x20
#define MASK1	0x24

#define TX0	0x31
#define TX1	0x41
#define TX2	0x51

#define RX0	0x61
#define RX1	0x71

#define BFPCTRL 0x0C
#define TXRTSCTRL 0x0D
#define CANSTAT 0x0E
#define CANCTRL 0x0F
#define TEC 0x1C
#define REC 0x1D
#define CNF3 0x28
#define CNF2 0x29
#define CNF1 0x2A
#define CANINTE 0x2B
#define CANINTF 0x2C
#define EFLG 0x2D
#define TX0CTRL 0x30
#define TX1CTRL 0x40
#define TX2CTRL 0x50
#define RX0CTRL 0x60
#define RX1CTRL 0x70




