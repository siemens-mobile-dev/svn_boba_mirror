#ifndef Z530_R6DA001
#error Z530_R6DA001 configuration needed for this header!
#else
#ifndef _Z530_R6DA001_
#define _Z530_R6DA001_


//#define pKBD  0x4C045144
#define EXT_TABLE 0x444E2C54



#define STR_START 0x0BAC
#define STR_APPLICATION 0x0FD0
#define STR_Saved_on_Memory_Stick 0x1002
#define STR_Saved_in_phone_memory     0x1001

//#define ELF_SMALL_ICON 0xE550
#define ELF_SMALL_ICON 0xE5C9

//#define ROOT_APP 0x4C25E9C0
//#define PID_MMI 0x100E3
#define HELPER_SIG 0x6FF0
//#define RAM_BASE (0x4C02D580)


#define PATH_AUDIO_INT 0x4462D036
#define PATH_AUDIO_EXT 0x4462B9D4
#define PATH_IMAGE_INT 0x4462D0B6
#define PATH_IMAGE_EXT 0x4462BAF4
#define PATH_VIDEO_INT 0x4462D192
#define PATH_VIDEO_EXT 0x4462BBE0
#define PATH_THEME_INT 0x4462D17C
#define PATH_THEME_EXT 0x4462BBA2
#define PATH_OTHER_INT 0x4462D144
#define PATH_OTHER_EXT 0x4462BB64

#define PATH_ELF_ROOT_INT          (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT          (L"/usb/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/usb/other/ZBin")
#define PATH_ELF_DAEMONS_INT      (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT     (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT      (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT     (L"/usb/other/ZBin/Config")
#define     PATH_INI          (L"/usb/other/ini")
#define     PATH_DEFAULT          (L"/usb/other")




#define RUN_CHECK 0x454D71A1

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x454D403D
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x454D403D
#define ELF_RUN_PAGE_CANCEL_EVENT    0x454D4045
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x454D40D9

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290
#endif
#endif
