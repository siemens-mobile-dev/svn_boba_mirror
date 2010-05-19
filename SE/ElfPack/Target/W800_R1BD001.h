#ifndef W800_R1BD001
#error W800_R1BD001 configuration needed for this header!
#else
#ifndef _W800_R1BD001_
#define _W800_R1BD001_

#define EXT_TABLE 0x4422C6D0
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x441EC49C
#define PATH_AUDIO_EXT 0x443FA95A
#define PATH_IMAGE_INT 0x441EC544
#define PATH_IMAGE_EXT 0x443FAA38
#define PATH_VIDEO_INT 0x441EC608
#define PATH_VIDEO_EXT 0x443FAB24
#define PATH_THEME_INT 0x441EC5E8
#define PATH_THEME_EXT 0x443FAAE6
#define PATH_OTHER_INT 0x441EC5C8
#define PATH_OTHER_EXT 0x443FAAA8

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

#define PAGE_ENTER_EVENT 0x5
#define PAGE_EXIT_EVENT 0x6
#define ACCEPT_EVENT 0xF
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11

#endif
#endif
