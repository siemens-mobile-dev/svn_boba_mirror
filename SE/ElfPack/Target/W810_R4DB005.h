#ifndef W810_R4DB005
#error W810_R4DB005 configuration needed for this header!
#else
#ifndef _W810_R4DB005_
#define _W810_R4DB005_

#define EXT_TABLE 0x444E9A98
#define RUN_CHECK 0x455B5945
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x44647CFA
#define PATH_AUDIO_EXT 0x44646698
#define PATH_IMAGE_INT 0x44647D7A
#define PATH_IMAGE_EXT 0x446467B8
#define PATH_VIDEO_INT 0x44647E56
#define PATH_VIDEO_EXT 0x446468A4
#define PATH_THEME_INT 0x44647E40
#define PATH_THEME_EXT 0x44646866
#define PATH_OTHER_INT 0x44647E08
#define PATH_OTHER_EXT 0x44646828

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/MSSEMC/Media files/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DEFAULT          (L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x455B27DD
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x455B27DD
#define ELF_RUN_PAGE_CANCEL_EVENT    0x455B27E5
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x455B2879

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290

#endif
#endif
