#ifndef Z610_R6DA001
#error Z610_R6DA001 configuration needed for this header!
#else
#ifndef _Z610_R6DA001_
#define _Z610_R6DA001_

//#define pKBD 0x4C05DD60
#define EXT_TABLE 0x45A4E168
#define RUN_CHECK 0x44FAE065
#define STR_START 0x0FBA
#define STR_APPLICATION 0x0460
#define STR_Saved_on_Memory_Stick 0x0580
#define STR_Saved_in_phone_memory 0x057F
#define ELF_SMALL_ICON 0xE5D2
//#define ROOT_APP 0x201820A4
//#define PID_MMI 0x100E4
#define HELPER_SIG 0x6FF0//???0x6FF0
//#define RAM_BASE (0x4C04B1D4)

#define PATH_AUDIO_INT 0x45802BC4
#define PATH_AUDIO_EXT 0x45802EE4
#define PATH_IMAGE_INT 0x45802C68
#define PATH_IMAGE_EXT 0x45802F50
#define PATH_VIDEO_INT 0x45802CF0
#define PATH_VIDEO_EXT 0x45802FD8
#define PATH_THEME_INT 0x45802CD0
#define PATH_THEME_EXT 0x45802FC0
#define PATH_OTHER_INT 0x45802C8C
#define PATH_OTHER_EXT 0x45802FA8

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44FA6239
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44FA6239
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44FA6241
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44FA62B5

#define DB_CMD_SETSMALLICON 0x3B
#define DB_DB_EXT_C1 0x2EC //???0x2EC

#endif
#endif
