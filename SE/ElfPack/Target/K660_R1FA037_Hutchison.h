#ifndef K660_R1FA037_Hutchison
#error K660_R1FA037_Hutchison configuration needed for this header!
#else
#ifndef _K660_R1FA037_Hutchison_
#define _K660_R1FA037_Hutchison_

#define EXT_TABLE 0x11A38500
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT (wchar_t*)0x1198B8A8
#define PATH_AUDIO_EXT (wchar_t*)0x11988B28
#define PATH_IMAGE_INT (wchar_t*)0x1198B928
#define PATH_IMAGE_EXT (wchar_t*)0x11988B94
#define PATH_VIDEO_INT (wchar_t*)0x1198BB6C
#define PATH_VIDEO_EXT (wchar_t*)0x11988C0C
#define PATH_THEME_INT (wchar_t*)0x1198BAD8
#define PATH_THEME_EXT (wchar_t*)0x11988BF4
#define PATH_OTHER_INT (wchar_t*)0x1198B9A8
#define PATH_OTHER_EXT (wchar_t*)0x11988BDC

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DLL              (L"/usb/other/ZBin/DLL")
#define PATH_DEFAULT          (L"/usb/other")

#define PATH_USER_INT         (L"/usb")
#define PATH_USER_EXT         (L"/card")

#define DB_CMD_SETSMALLICON 0x3A
#define DB_CMD_SETTHUMBNAILICON 0x3B

#define PAGE_ENTER_EVENT 0xE
#define PAGE_EXIT_EVENT 0xF
#define ACCEPT_EVENT 0x2
#define PREVIOUS_EVENT 0x3
#define CANCEL_EVENT 0x4

#endif
#endif
