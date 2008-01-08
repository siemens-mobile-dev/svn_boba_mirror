#ifndef W810_R4EA031
#error W810_R4EA031 configuration needed for this header!
#else
#ifndef _W810_R4EA031_
#define _W810_R4EA031_
#define pKBD 0x4C045A34
#define EXT_TABLE 0x444EE010
#define RUN_CHECK 0x455C0485 	// ?????????????



#define STR_START 0xB9E
#define STR_APPLICATION 0x0342
#define STR_Saved_on_Memory_Stick 0x0FF0
#define STR_Saved_in_phone_memory     0x0FEF

//#define ELF_SMALL_ICON 0xE550
#define ELF_SMALL_ICON 0xE5AC

//#define ROOT_APP 0x4C2DC794
//#define PID_MMI 0x100E5
#define HELPER_SIG 0x6FF0
#define RAM_BASE (0x4C02C7C4)

#define PATH_AUDIO_INT 0x4464C474
#define PATH_AUDIO_EXT 0x4464AE12
#define PATH_IMAGE_INT 0x4464C4F4
#define PATH_IMAGE_EXT 0x4464AF32
#define PATH_VIDEO_INT 0x4464C5D0
#define PATH_VIDEO_EXT 0x4464B01E
#define PATH_THEME_INT 0x4464C5BA
#define PATH_THEME_EXT 0x4464AFE0
#define PATH_OTHER_INT 0x4464C582
#define PATH_OTHER_EXT 0x4464AFA2

#define PATH_ELF_ROOT_INT		(L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT		(L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT		(L"/usb/other/ZBin")
#define PATH_ELF_EXT		(L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT 	(L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT	(L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT 	(L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT	(L"/card/MSSEMC/Media files/other/ZBin/Config")
#define	PATH_INI		(L"/usb/other/ini")
#define	PATH_DEFAULT		(L"/usb/other")

#define ELF_RUN_PAGE_EV_0x10 0x455BD31D
#define ELF_RUN_PAGE_EV_0x0F 0x455BD31D
#define ELF_RUN_PAGE_EV_0x11 0x455BD325
#define ELF_RUN_PAGE_EV_0x06 0x455BD3B9

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290
#endif
#endif
