#ifndef W300_R4EA031
#error W300_R4EA031 configuration needed for this header!
#else
#ifndef _W300_R4EA031_
#define _W300_R4EA031_

#define EXT_TABLE 0x444F1F2C
#define RUN_CHECK 0x4554891D
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x4464C292
#define PATH_AUDIO_EXT 0x4464AC30
#define PATH_IMAGE_INT 0x4464C312
#define PATH_IMAGE_EXT 0x4464AD50
#define PATH_VIDEO_INT 0x4464C3EE
#define PATH_VIDEO_EXT 0x4464AE3C
#define PATH_THEME_INT 0x4464C3D8
#define PATH_THEME_EXT 0x4464ADFE
#define PATH_OTHER_INT 0x4464C3A0
#define PATH_OTHER_EXT 0x4464ADC0

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/MSSEMC/Media files/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/MSSEMC/Media files/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/MSSEMC/Media files/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DEFAULT          (L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x455457B5
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x455457B5
#define ELF_RUN_PAGE_CANCEL_EVENT    0x455457BD
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x45545851

#define DB_CMD_SETSMALLICON 0x36
#define DB_CMD_SETTHUMBNAILICON 0x37
#define DB_DB_EXT_C1 0x290

#endif
#endif
