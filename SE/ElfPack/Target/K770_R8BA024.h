#ifndef K770_R8BA024
#error K770_R8BA024 configuration needed for this header!
#else
#ifndef _K770_R8BA024_
#define _K770_R8BA024_

#define EXT_TABLE 0x45BD9AE0
#define RUN_CHECK 0x44FF7D09
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x4588CDC0
#define PATH_AUDIO_EXT 0x4588D0E0
#define PATH_IMAGE_INT 0x4588CE64
#define PATH_IMAGE_EXT 0x4588D14C
#define PATH_VIDEO_INT 0x4588CEEC
#define PATH_VIDEO_EXT 0x4588D1D4
#define PATH_THEME_INT 0x4588CECC
#define PATH_THEME_EXT 0x4588D1BC
#define PATH_OTHER_INT 0x4588CE88
#define PATH_OTHER_EXT 0x4588D1A4

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44FF72C1
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44FF72BD
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44FF72D1
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44FEFAE1

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
