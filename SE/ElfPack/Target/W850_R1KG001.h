#ifndef W850_R1KG001
#error W850_R1KG001 configuration needed for this header!
#else
#ifndef _W850_R1KG001_
#define _W850_R1KG001_

#define EXT_TABLE 0x45BA8184
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45876138
#define PATH_AUDIO_EXT 0x45876458
#define PATH_IMAGE_INT 0x458761DC
#define PATH_IMAGE_EXT 0x458764C4
#define PATH_VIDEO_INT 0x45876264
#define PATH_VIDEO_EXT 0x4587654C
#define PATH_THEME_INT 0x45876244
#define PATH_THEME_EXT 0x45876534
#define PATH_OTHER_INT 0x45876200
#define PATH_OTHER_EXT 0x4587651C

#define PATH_ELF_ROOT_INT     (L"/tpa/user/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/other/ZBin")
#define PATH_ELF_INT          (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/tpa/user/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/tpa/user/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/tpa/user/other/ini")
#define PATH_DLL              (L"/tpa/user/other/ZBin/DLL")
#define PATH_DEFAULT          (L"/tpa/user/other")

#define PATH_USER_INT         (L"/tpa/user")
#define PATH_USER_EXT         (L"/card")

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C

#define PAGE_ENTER_EVENT 7
#define PAGE_EXIT_EVENT 8
#define ACCEPT_EVENT 2
#define PREVIOUS_EVENT 3
#define CANCEL_EVENT 4

#endif
#endif
