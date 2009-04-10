#ifndef W580_R8BE001_Orange
#error W580_R8BE001_Orange configuration needed for this header!
#else
#ifndef _W580_R8BE001_Orange_
#define _W580_R8BE001_Orange_

#define EXT_TABLE 0x45AEAC44
#define RUN_CHECK 0x44EF0139
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x457530CC
#define PATH_AUDIO_EXT 0x45753410
#define PATH_IMAGE_INT 0x45753170
#define PATH_IMAGE_EXT 0x4575347C
#define PATH_VIDEO_INT 0x4575321C
#define PATH_VIDEO_EXT 0x45753520
#define PATH_THEME_INT 0x457531FC
#define PATH_THEME_EXT 0x45753508
#define PATH_OTHER_INT 0x45753194
#define PATH_OTHER_EXT 0x457534D4

#define PATH_ELF_ROOT_INT     (L"/tpa/user/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/other/ZBin")
#define PATH_ELF_INT          (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/tpa/user/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/tpa/user/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/tpa/user/other/ini")
#define PATH_DEFAULT          (L"/tpa/user/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44EEF4C5
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44EEF4C1
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44EEF4D5
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44EE7F79

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
