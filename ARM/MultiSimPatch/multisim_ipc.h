#ifndef _MULTISIM_IPC_H_
#define _MULTISIM_IPC_H_

//ipc send 2 patch
  #define IPC_MSIM_NAME "MultiSim"
  #define ELF_MSIM_NAME "MultiSimElf"
  #define IPC_MSIM_SWITCH 1
//  #define IPC_MSIM_5400_READ 1
//  #define IPC_MSIM_5400_WRITE 2 
  #define IPC_MSIM_GET_RAM5400 3
  #define IPC_MSIM_EEPROM_BACKDOOR 4
  #define IPC_MSIM_EEPROM_BACKDOOR_READ 4
  #define IPC_MSIM_EEPROM_BACKDOOR_WRITE 5
  #define IPC_MSIM_EEPROM_BACKDOOR_VERS 
  #define IPC_MSIM_EEPROM_AUTOSEARCH 7

//const for replies from patch
  #define PRC_MSIM_RAM5400 0x00  
  #define PRC_MSIM_READBLOCK 0x10  
  #define PRC_MSIM_READBLOCK2FILE 0x11
  #define PRC_MSIM_WRITEBLOCK 0x20
  #define PRC_MSIM_WRITEBLOCK4FILE 0x21

//ipc replies from patch
  #define IPC_MSIM_PROCESSED 0x100
  #define IPC_MSIM_PROCESSED_READBLOCK (IPC_MSIM_PROCESSED+PRC_MSIM_READBLOCK)
  #define IPC_MSIM_PROCESSED_WRITEBLOCK (IPC_MSIM_PROCESSED+PRC_MSIM_WRITEBLOCK)

  #define IPC_MSIM_PROCESSED_READBLOCK2FILE (IPC_MSIM_PROCESSED+PRC_MSIM_READBLOCK2FILE)
  #define IPC_MSIM_PROCESSED_WRITEBLOCK4FILE (IPC_MSIM_PROCESSED+PRC_MSIM_WRITEBLOCK4FILE)

  #define IPC_MSIM_SWITCHED 10  

typedef struct
{
	char msg_version;   // Версия структуры обмена
	char type;	
	int block;
	void *buf;
	int offset;
	int size;
}MSIM_IPC_MSG_EEPROM;


typedef struct
{
  int  SimCnt;
  int  CurSim;
  char SimNames[20][16];
  char SimFlags[20][4];//maybe add simnum  
}MSIM_ELF_RESP;

#endif

