#ifndef W660_R8BB001
#error W660_R8BB001 configuration needed for this header!
#else
#ifndef _W660_R8BB001_
#define _W660_R8BB001_

#define EXT_TABLE 0x45AE72C8

#define RUN_CHECK 0x44FE4CE5
#define STR_START 0xFF8
#define STR_APPLICATION 0x0469
#define STR_Saved_on_Memory_Stick 0x15A0
#define STR_Saved_in_phone_memory 0x159F
#define ELF_SMALL_ICON 0xE5D1
//#define ROOT_APP 0x201820A4
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x4587E204
#define PATH_AUDIO_EXT 0x4587E524
#define PATH_IMAGE_INT 0x4587E2A8
#define PATH_IMAGE_EXT 0x4587E590
#define PATH_VIDEO_INT 0x4587E330
#define PATH_VIDEO_EXT 0x4587E618
#define PATH_THEME_INT 0x4587E310
#define PATH_THEME_EXT 0x4587E600
#define PATH_OTHER_INT 0x4587E2CC
#define PATH_OTHER_EXT 0x4587E5E8

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44FE4071
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44FE406D
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44FE4081
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44FDCA9D

#define DB_CMD_SETSMALLICON 0x3B
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
