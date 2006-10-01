#define HOOK_IDLE 0x10C60

typedef struct
{
 void huge* proc;
 unsigned int unk1;
 unsigned int unk2;
} PID_DESC;

typedef struct
{
 void far* unk;
 PID_DESC far* desc;
} PID;

typedef struct 
{
 PID far *SenderPid;    // process which sent this MSG
 int Msg;
 int Param[6];
} GBS_MSG;

// Allow/disable multitask
extern void AcquireGbsLock(void);
extern void FreeGbsLock(void);

extern int  ReceiveMessage(GBS_MSG far* msg, int doaccept);
extern void SendMessage(PID far *pid_to, unsigned int mess, unsigned int p1, ... );

extern PID far* pid_MMI;
extern PID far* pid_DIALER_PROCESS;
extern PID far* pid_NETCORE_PROCESS;
extern PID far* pid_INET_TEST_PROCESS;
extern PID far* pid_MMC_FILE_SYSTEM;
extern PID far* pid_MMC_EXPLORER;


//void AcceptMessage(void);
//void ClearMessage(void);
//void PendMessage(GBS_MSG *);
//void CreateGBSproc(int cepid, const char *name, void (*onMsg)(void), int prio, int unk_zero);
//CSMROOT *CSM_root();

extern char keybQueneIdx;
extern char keybQueneBuf[];
extern int GbsLock_ifNZ;
extern int GbsLock_ifZ;
extern void far *PidAct;

