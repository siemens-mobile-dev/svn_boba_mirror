#ifndef W600_R7CA017
#error W600_R7CA017 configuration needed for this header!
#else
#ifndef _W600_R7CA017_
#define _W600_R7CA017_

#define EXT_TABLE 0x444507E0
#define RUN_CHECK 0x45506235
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x445AD9EA
#define PATH_AUDIO_EXT 0x445AD9EA
#define PATH_IMAGE_INT 0x445ADA6A
#define PATH_IMAGE_EXT 0x445ADA6A
#define PATH_VIDEO_INT 0x445ADB46
#define PATH_VIDEO_EXT 0x445ADB46
#define PATH_THEME_INT 0x445ADB30
#define PATH_THEME_EXT 0x445ADB30
#define PATH_OTHER_INT 0x445ADAF8
#define PATH_OTHER_EXT 0x445ADAF8

#define PATH_ELF_ROOT_INT        (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT        (L"/usb/other/ZBin")
#define PATH_ELF_INT             (L"/usb/other/ZBin")
#define PATH_ELF_EXT             (L"/usb/other/ZBin")
#define PATH_ELF_DAEMONS_INT     (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT     (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT      (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT      (L"/usb/other/ZBin/Config")
#define PATH_INI                 (L"/usb/other/ini")
#define PATH_DEFAULT             (L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x45503965
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x45503965
#define ELF_RUN_PAGE_CANCEL_EVENT    0x4550396D
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x455039D5

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290
#endif
#endif

