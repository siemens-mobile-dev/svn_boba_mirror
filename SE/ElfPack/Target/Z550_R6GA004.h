#ifndef Z550_R6GA004
#error Z550_R6GA004 configuration needed for this header!
#else
#ifndef _Z550_R6GA004_
#define _Z550_R6GA004_

#define RUN_CHECK 0x455741A5
#define HELPER_SIG 0x6FF0
#define EXT_TABLE 0x444E2ECC

#define PATH_AUDIO_INT 0x44630322
#define PATH_AUDIO_EXT 0x4462ECC0
#define PATH_IMAGE_INT 0x446303A2
#define PATH_IMAGE_EXT 0x4462EDE0
#define PATH_VIDEO_INT 0x4463047E
#define PATH_VIDEO_EXT 0x4462EECC
#define PATH_THEME_INT 0x44630468
#define PATH_THEME_EXT 0x4462EE8E
#define PATH_OTHER_INT 0x44630430
#define PATH_OTHER_EXT 0x4462EE50

#define PATH_ELF_ROOT_INT          (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT      (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT     (L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT      (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT     (L"/card/MSSEMC/Media files/other/ZBin/Config")
#define PATH_INI          (L"/usb/other/ini")
#define PATH_DEFAULT          (L"/usb/other")


#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x45571051
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x45571051
#define ELF_RUN_PAGE_CANCEL_EVENT    0x45571059
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x455710ED

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290
#endif
#endif

