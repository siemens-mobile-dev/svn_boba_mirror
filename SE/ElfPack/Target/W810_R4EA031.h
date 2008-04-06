#ifndef W810_R4EA031
#error W810_R4EA031 configuration needed for this header!
#else
#ifndef _W810_R4EA031_
#define _W810_R4EA031_

#define EXT_TABLE 0x444EE010
#define RUN_CHECK 0x455C0485
#define HELPER_SIG 0x6FF0

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x455BD31D
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x455BD31D
#define ELF_RUN_PAGE_CANCEL_EVENT    0x455BD325
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x455BD3B9

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290

#endif
#endif
