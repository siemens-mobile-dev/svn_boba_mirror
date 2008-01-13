#ifndef Z550_R6CA009
#error Z550_R6CA009 configuration needed for this header!
#else
#ifndef _Z550_R6CA009_
#define _Z550_R6CA009_


//#define pKBD 0x4C045358
#define EXT_TABLE 0x444E2B7C



#define STR_START 0xBB3
#define STR_APPLICATION 0x0354
#define STR_Saved_on_Memory_Stick 0x100A
#define STR_Saved_in_phone_memory     0x1009

//#define ELF_SMALL_ICON 0xE550
#define ELF_SMALL_ICON 0xE5AD

//#define ROOT_APP 0x4C25E6E8
//#define PID_MMI 0x100E3
#define HELPER_SIG 0x6FF0
//#define RAM_BASE (0x4C02D580)


#define PATH_AUDIO_INT 0x4462FD60
#define PATH_AUDIO_EXT 0x4462E6FE
#define PATH_IMAGE_INT 0x4462FDE0
#define PATH_IMAGE_EXT 0x4462E81E
#define PATH_VIDEO_INT 0x4462FEBC
#define PATH_VIDEO_EXT 0x4462E90A
#define PATH_THEME_INT 0x4462FEA6
#define PATH_THEME_EXT 0x4462E8CC
#define PATH_OTHER_INT 0x4462FE6E
#define PATH_OTHER_EXT 0x4462E88E

#define PATH_ELF_ROOT_INT          (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT      (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT     (L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT      (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT     (L"/card/MSSEMC/Media files/other/ZBin/Config")
#define     PATH_INI          (L"/usb/other/ini")
#define     PATH_DEFAULT          (L"/usb/other")




#define RUN_CHECK 0x45561551 	// ?????????????

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x4555E3FD
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x4555E3FD
#define ELF_RUN_PAGE_CANCEL_EVENT    0x4555E405
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x4555E499

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290
#endif
#endif

