#ifndef W610_R6CA001_Orange
#error W610_R6CA001_Orange configuration needed for this header!
#else
#ifndef _W610_R6CA001_Orange_
#define _W610_R6CA001_Orange_

#define EXT_TABLE 0x45991230
#define RUN_CHECK 0x44EEE349
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45A35A0C
#define PATH_AUDIO_EXT 0x4573DE8C
#define PATH_IMAGE_INT 0x45A35A38
#define PATH_IMAGE_EXT 0x4573DEF8
#define PATH_VIDEO_INT 0x45A35A54
#define PATH_VIDEO_EXT 0x4573DF80
#define PATH_THEME_INT 0x45A359A0
#define PATH_THEME_EXT 0x4573DF68
#define PATH_OTHER_INT 0x45A35988
#define PATH_OTHER_EXT 0x4573DF50

#define PATH_ELF_ROOT_INT     (L"/usb/other")
#define PATH_ELF_ROOT_EXT     (L"/card/other")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DEFAULT          (L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44EF1D71
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44EF1D6D
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44EF1D81
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44EEA835

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
