#ifndef W910_R1FA035
#error W910_R1FA035 configuration needed for this header!
#else
#ifndef _W910_R1FA035_
#define _W910_R1FA035_

#define EXT_TABLE 0x11A0A4E0
#define Library_Start 0x12231000
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x1195DF30
#define PATH_AUDIO_EXT 0x1195B1B0
#define PATH_IMAGE_INT 0x1195DFB0
#define PATH_IMAGE_EXT 0x1195B21C
#define PATH_VIDEO_INT 0x1195E1F4
#define PATH_VIDEO_EXT 0x1195B294
#define PATH_THEME_INT 0x1195E160
#define PATH_THEME_EXT 0x1195B27C
#define PATH_OTHER_INT 0x1195E030
#define PATH_OTHER_EXT 0x1195B264

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
