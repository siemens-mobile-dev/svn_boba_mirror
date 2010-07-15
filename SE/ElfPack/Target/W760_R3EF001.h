#ifndef W760_R3EF001
#error W760_R3EF001 configuration needed for this header!
#else
#ifndef _W760_R3EF001_
#define _W760_R3EF001_

#define EXT_TABLE 0x11EF99A8
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x11E4C4C8
#define PATH_AUDIO_EXT 0x11E49720
#define PATH_IMAGE_INT 0x11E4C548
#define PATH_IMAGE_EXT 0x11E4978C
#define PATH_VIDEO_INT 0x11E4C830
#define PATH_VIDEO_EXT 0x11E4982C
#define PATH_THEME_INT 0x11E4C79C
#define PATH_THEME_EXT 0x11E49814
#define PATH_OTHER_INT 0x11E4C5C8
#define PATH_OTHER_EXT 0x11E497D4

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

#define DB_CMD_SETSAVEDICON 0x39
#define DB_CMD_SETSMALLICON 0x3A
#define DB_CMD_SETTHUMBNAILICON 0x3B

#define PAGE_ENTER_EVENT 0xF
#define PAGE_EXIT_EVENT 0x10
#define ACCEPT_EVENT 2
#define PREVIOUS_EVENT 3
#define CANCEL_EVENT 4

#endif
#endif
