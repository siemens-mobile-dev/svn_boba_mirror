#ifndef W580_R6CA001_Orange
#error W580_R6CA001_Orange configuration needed for this header!
#else
#ifndef _W580_R6CA001_Orange_
#define _W580_R6CA001_Orange_

#define EXT_TABLE  0x45ACB380
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45740CD4
#define PATH_AUDIO_EXT 0x45741018
#define PATH_IMAGE_INT 0x45740D78
#define PATH_IMAGE_EXT 0x45741084
#define PATH_VIDEO_INT 0x45740E24
#define PATH_VIDEO_EXT 0x45741128
#define PATH_THEME_INT 0x45740E04
#define PATH_THEME_EXT 0x45741110
#define PATH_OTHER_INT 0x45740D9C
#define PATH_OTHER_EXT 0x457410DC

#define PATH_ELF_ROOT_INT     (L"/tpa/user/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/other/ZBin")
#define PATH_ELF_INT          (L"/tpa/user/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/tpa/user/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/tpa/user/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/tpa/user/other/ini")
#define PATH_DLL              (L"/usb/other/ZBin/DLL")
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
