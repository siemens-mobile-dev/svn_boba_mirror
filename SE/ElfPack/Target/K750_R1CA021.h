#ifndef K750_R1CA021
#error K750_R1CA021 configuration needed for this header!
#else
#ifndef _K750_R1CA021_
#define _K750_R1CA021_

#define EXT_TABLE 0x4422CC50
#define Library_Start 0x451D0000
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x441EC7A4
#define PATH_AUDIO_EXT 0x443FA5AA
#define PATH_IMAGE_INT 0x441EC84C
#define PATH_IMAGE_EXT 0x443FA688
#define PATH_VIDEO_INT 0x441EC910
#define PATH_VIDEO_EXT 0x443FA774
#define PATH_THEME_INT 0x441EC8F0
#define PATH_THEME_EXT 0x443FA736
#define PATH_OTHER_INT 0x441EC8D0
#define PATH_OTHER_EXT 0x443FA6F8

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
#define DB_CMD_SETTHUMBNAILICON 0x36

#define PAGE_ENTER_EVENT 5
#define PAGE_EXIT_EVENT 6
#define ACCEPT_EVENT 0x0F
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11

#endif
#endif
