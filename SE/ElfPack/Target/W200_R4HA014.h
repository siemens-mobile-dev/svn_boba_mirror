#ifndef W200_R4HA014
#error W200_R4HA014 configuration needed for this header!
#else
#ifndef _W200_R4HA014_
#define _W200_R4HA014_

#define EXT_TABLE 0x444592D4
#define Library_Start 0x456C0000
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x445B828C
#define PATH_AUDIO_EXT 0x445B6C2A
#define PATH_IMAGE_INT 0x445B830C
#define PATH_IMAGE_EXT 0x445B6D4A
#define PATH_VIDEO_INT 0x445B83E8
#define PATH_VIDEO_EXT 0x445B6E36
#define PATH_THEME_INT 0x445B83D2
#define PATH_THEME_EXT 0x445B6DF8
#define PATH_OTHER_INT 0x445B839A
#define PATH_OTHER_EXT 0x445B6DBA

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/usb/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/usb/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/usb/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DLL              (L"/usb/other/ZBin/DLL")
#define PATH_DEFAULT          (L"/usb/other")

#define PATH_USER_INT         (L"/usb")
#define PATH_USER_EXT         (L"/usb")

#define DB_CMD_SETSMALLICON 0x36

#define PAGE_ENTER_EVENT 5
#define PAGE_EXIT_EVENT 6
#define ACCEPT_EVENT 0x0F
#define PREVIOUS_EVENT 0x10
#define CANCEL_EVENT 0x11

#endif
#endif
