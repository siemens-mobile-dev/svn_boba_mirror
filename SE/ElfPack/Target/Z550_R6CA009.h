#ifndef Z550_R6CA009
#error Z550_R6CA009 configuration needed for this header!
#else
#ifndef _Z550_R6CA009_
#define _Z550_R6CA009_

#define EXT_TABLE 0x444E2B7C
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT (wchar_t*)0x4462FD60
#define PATH_AUDIO_EXT (wchar_t*)0x4462E6FE
#define PATH_IMAGE_INT (wchar_t*)0x4462FDE0
#define PATH_IMAGE_EXT (wchar_t*)0x4462E81E
#define PATH_VIDEO_INT (wchar_t*)0x4462FEBC
#define PATH_VIDEO_EXT (wchar_t*)0x4462E90A
#define PATH_THEME_INT (wchar_t*)0x4462FEA6
#define PATH_THEME_EXT (wchar_t*)0x4462E8CC
#define PATH_OTHER_INT (wchar_t*)0x4462FE6E
#define PATH_OTHER_EXT (wchar_t*)0x4462E88E

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/MSSEMC/Media files/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DLL              (L"/usb/other/ZBin/DLL")
#define PATH_DEFAULT          (L"/usb/other")

#define PATH_USER_INT         (L"/usb")
#define PATH_USER_EXT         (L"/card/MSSEMC/Media files")

#define DB_CMD_SETSMALLICON 0x36

#define PAGE_ENTER_EVENT 5
#define PAGE_EXIT_EVENT 6
#define ACCEPT_EVENT 0x0F
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11

#endif
#endif
