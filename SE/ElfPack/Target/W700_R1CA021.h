#ifndef W700_R1CA021
#error W700_R1CA021 configuration needed for this header!
#else
#ifndef _W700_R1CA021_
#define _W700_R1CA021_

#define EXT_TABLE 0x4422CE80
#define HELPER_SIG 0x6FF0

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

#define PATH_ELF_ROOT_INT     (L"/tpa/user/other")
#define PATH_ELF_ROOT_EXT     (L"/card/MSSEMC/Media files/other")
#define PATH_ELF_INT          (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/tpa/user/other/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/MSSEMC/Media files/other/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/tpa/user/other/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/MSSEMC/Media files/other/Config")
#define PATH_INI              (L"/tpa/user/other/ini")
#define PATH_DLL              (L"/tpa/user/other/DLL")
#define PATH_DEFAULT          (L"/tpa/user/other")

#define PATH_USER_INT         (L"/tpa/user")
#define PATH_USER_EXT         (L"/card/MSSEMC/Media files")

#define DB_CMD_SETSMALLICON 0x35

#define PAGE_ENTER_EVENT 5
#define PAGE_EXIT_EVENT 6
#define ACCEPT_EVENT 0x0F
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11

#endif
#endif
