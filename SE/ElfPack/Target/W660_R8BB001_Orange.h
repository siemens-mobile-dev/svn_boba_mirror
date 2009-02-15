#ifndef W660_R8BB001_Orange
#error W660_R8BB001_Orange configuration needed for this header!
#else
#ifndef _W660_R8BB001_Orange_
#define _W660_R8BB001_Orange_

#define EXT_TABLE 0x45B14034
#define RUN_CHECK 0x45019A3D
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x458B9E50
#define PATH_AUDIO_EXT 0x458BA170
#define PATH_IMAGE_INT 0x458B9EF4
#define PATH_IMAGE_EXT 0x458BA1DC
#define PATH_VIDEO_INT 0x458B9F7C
#define PATH_VIDEO_EXT 0x458BA264
#define PATH_THEME_INT 0x458B9F5C
#define PATH_THEME_EXT 0x458BA24C
#define PATH_OTHER_INT 0x458B9F18
#define PATH_OTHER_EXT 0x458BA234

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x45018DC9
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x45018DC5
#define ELF_RUN_PAGE_CANCEL_EVENT    0x45018DD9
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x45011871

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
