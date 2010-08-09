#ifndef W300_R4EA031
#error W300_R4EA031 configuration needed for this header!
#else
#ifndef _W300_R4EA031_
#define _W300_R4EA031_

#define EXT_TABLE 0x444F1F2C
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT (wchar_t*)0x4464C292
#define PATH_AUDIO_EXT (wchar_t*)0x4464AC30
#define PATH_IMAGE_INT (wchar_t*)0x4464C312
#define PATH_IMAGE_EXT (wchar_t*)0x4464AD50
#define PATH_VIDEO_INT (wchar_t*)0x4464C3EE
#define PATH_VIDEO_EXT (wchar_t*)0x4464AE3C
#define PATH_THEME_INT (wchar_t*)0x4464C3D8
#define PATH_THEME_EXT (wchar_t*)0x4464ADFE
#define PATH_OTHER_INT (wchar_t*)0x4464C3A0
#define PATH_OTHER_EXT (wchar_t*)0x4464ADC0

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
#define DB_CMD_SETTHUMBNAILICON 0x37

#define PAGE_ENTER_EVENT 5
#define PAGE_EXIT_EVENT 6
#define ACCEPT_EVENT 0x0F
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11

#endif
#endif
