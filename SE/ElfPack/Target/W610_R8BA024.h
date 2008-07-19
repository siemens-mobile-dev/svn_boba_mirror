#ifndef W610_R8BA024
#error W610_R8BA024 configuration needed for this header!
#else
#ifndef _W610_R8BA024_
#define _W610_R8BA024_

#define EXT_TABLE 0x45971284
#define RUN_CHECK 0x44EC2945
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45A0BCBC
#define PATH_AUDIO_EXT 0x45708A60
#define PATH_IMAGE_INT 0x45A0BCE8
#define PATH_IMAGE_EXT 0x45708ACC
#define PATH_VIDEO_INT 0x45A0BD04
#define PATH_VIDEO_EXT 0x45708B54
#define PATH_THEME_INT 0x45A0BC50
#define PATH_THEME_EXT 0x45708B3C
#define PATH_OTHER_INT 0x45A0BC38
#define PATH_OTHER_EXT 0x45708B24

#define PATH_ELF_ROOT_INT     (L"/usb/other")
#define PATH_ELF_ROOT_EXT     (L"/card/other")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DEFAULT          (L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44EC6499
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44EC6495
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44EC64AB
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44EBEE1D

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
