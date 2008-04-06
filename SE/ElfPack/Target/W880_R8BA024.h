#ifndef W880_R8BA024
#error W880_R8BA024 configuration needed for this header!
#else
#ifndef _W880_R8BA024_
#define _W880_R8BA024_

#define EXT_TABLE 0x45BB1C5C
#define RUN_CHECK 0x44FE26F1
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45B10850
#define PATH_AUDIO_EXT 0x45B10B70
#define PATH_IMAGE_INT 0x45B108F4
#define PATH_IMAGE_EXT 0x45B10BDC
#define PATH_VIDEO_INT 0x45B1097C
#define PATH_VIDEO_EXT 0x45B10C64
#define PATH_THEME_INT 0x45B1095C
#define PATH_THEME_EXT 0x45B10C4C
#define PATH_OTHER_INT 0x45B10918
#define PATH_OTHER_EXT 0x45B10C34

#define PATH_ELF_ROOT_INT     (L"/tpa/user/other")
#define PATH_ELF_ROOT_EXT     (L"/card/other")
#define PATH_ELF_INT          (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/tpa/user/other/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/tpa/user/other/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/Config")
#define PATH_INI              (L"/tpa/user/other/ini")
#define PATH_DEFAULT          (L"/tpa/user/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44FE1A75
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44FE1A71
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44FE1A85
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44FDA5C9

#define DB_CMD_SETSMALLICON 0x3B
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
