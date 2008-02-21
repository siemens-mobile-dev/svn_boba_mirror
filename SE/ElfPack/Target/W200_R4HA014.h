#ifndef W200_R4HA014
#error W200_R4HA014 configuration needed for this header!
#else
#ifndef _W200_R4HA014_
#define _W200_R4HA014_

//#define pKBD 0x4C044D48
#define EXT_TABLE 0x444592D4
#define RUN_CHECK 0x454329BD



#define STR_START 0xBAD 
#define STR_APPLICATION 0x034A 
#define STR_Saved_on_Memory_Stick 0x1004 
#define STR_Saved_in_phone_memory     0x1003

//#define ELF_SMALL_ICON 0xE550
#define ELF_SMALL_ICON 0xE59F 

//#define ROOT_APP 0x4C2DC794
//#define PID_MMI 0x100E5
#define HELPER_SIG 0x6FF0
//#define RAM_BASE (0x4C02D2C4)

#define PATH_AUDIO_INT 0x445B828C  
#define PATH_AUDIO_EXT 0x445B6C2A
#define PATH_IMAGE_INT 0x445B830C
#define PATH_IMAGE_EXT 0x445B6D4A
#define PATH_VIDEO_INT 0x445B83E8
#define PATH_VIDEO_EXT 0x445B6E36
#define PATH_THEME_INT 0x445B83D2
#define PATH_THEME_EXT 0x445B6DF8
#define PATH_OTHER_INT 0x445B839A
#define PATH_OTHER_EXT 0x445B6DBA

#define PATH_ELF_ROOT_INT		(L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT		(L"/usb/other/ZBin")
#define PATH_ELF_INT		(L"/usb/other/ZBin")
#define PATH_ELF_EXT		(L"/usb/other/ZBin")
#define PATH_ELF_DAEMONS_INT 	(L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT	(L"/usb/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT 	(L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT	(L"/usb/other/ZBin/Config")
#define	PATH_INI		(L"/usb/other/ini")
#define	PATH_DEFAULT		(L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x4542F8D9
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x4542F8D9
#define ELF_RUN_PAGE_CANCEL_EVENT    0x4542F8E1
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x4542F975

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290
#endif
#endif
