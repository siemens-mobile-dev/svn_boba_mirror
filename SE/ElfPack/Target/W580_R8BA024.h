#ifndef W580_R8BA024
#error W580_R8BA024 configuration needed for this header!
#else
#ifndef _W580_R8BA024_
#define _W580_R8BA024_

#define EXT_TABLE 0x45AC7790

#define RUN_CHECK 0x44EBA935
#define STR_START 0xBC5
#define STR_APPLICATION 0x0319
#define STR_Saved_on_Memory_Stick 0x057D
#define STR_Saved_in_phone_memory 0x057C
#define ELF_SMALL_ICON 0xE5CF
//#define ROOT_APP 0x201820A4
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45715398
#define PATH_AUDIO_EXT 0x457156DC
#define PATH_IMAGE_INT 0x4571543C
#define PATH_IMAGE_EXT 0x45715748
#define PATH_VIDEO_INT 0x457154E8
#define PATH_VIDEO_EXT 0x457157EC
#define PATH_THEME_INT 0x457154C8
#define PATH_THEME_EXT 0x457157D4
#define PATH_OTHER_INT 0x45715460
#define PATH_OTHER_EXT 0x457157A0

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44EB9CC1
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44EB9CBD
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44EB9CD1
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44EB26F9

#define DB_CMD_SETSMALLICON 0x3B
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
