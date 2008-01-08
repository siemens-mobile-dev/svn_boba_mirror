#ifndef W700_R1CA021
#error W700_R1CA021 configuration needed for this header!
#else
#ifndef _W700_R1CA021_
#define _W700_R1CA021_


#define pKBD 0x4C05DDCC
#define EXT_TABLE 0x4422CE80
#define RUN_CHECK 0x450F94FD


// ID строк из лэнгпака
#define STR_START 0xe7b
#define STR_APPLICATION 0x0319
#define STR_Saved_on_Memory_Stick 0x03FB
#define STR_Saved_in_phone_memory 0x03FA

// первая иконка из зоопарка
#define ELF_SMALL_ICON 0xE816
//#define ROOT_APP 0x4C1826CC
//#define PID_MMI 0x100E4
#define HELPER_SIG 0x6FF0

// из патча на замену графы
#define RAM_BASE (0x4C04B1DC)
// пути к стандартным папкам
#define PATH_AUDIO_INT 0x441ECB20
#define PATH_AUDIO_EXT 0x443FAD2E
#define PATH_IMAGE_INT 0x441ECBC8
#define PATH_IMAGE_EXT 0x443FAE0C
#define PATH_VIDEO_INT 0x441ECC8C
#define PATH_VIDEO_EXT 0x443FAEF8
#define PATH_THEME_INT 0x441ECC6C
#define PATH_THEME_EXT 0x443FAEBA
#define PATH_OTHER_INT 0x441ECC4C
#define PATH_OTHER_EXT 0x443FAE7C

#define PATH_ELF_ROOT_INT          (L"/tpa/user/other")
#define PATH_ELF_ROOT_EXT          (L"/card/MSSEMC/Media files/other")
#define PATH_ELF_INT          (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/tpa/user/other/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/MSSEMC/Media files/other/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/tpa/user/other/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/MSSEMC/Media files/other/Config")
#define PATH_INI              (L"/tpa/user/other/ini")
#define PATH_DEFAULT          (L"/tpa/user/other")

#define ELF_RUN_PAGE_EV_0x10 0x450F5BDD
#define ELF_RUN_PAGE_EV_0x0F 0x450F5BDD
#define ELF_RUN_PAGE_EV_0x11 0x450F5BE5
#define ELF_RUN_PAGE_EV_0x06 0x450F5C3D

#define DB_CMD_SETSMALLICON 0x35
#define DB_DB_EXT_C1 0x258

#endif
#endif
