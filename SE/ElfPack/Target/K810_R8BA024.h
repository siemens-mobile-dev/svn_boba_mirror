#ifndef K810_R8BA024
#error K810_R8BA024 configuration needed for this header!
#else
#ifndef _K810_R8BA024_
#define _K810_R8BA024_

#define EXT_TABLE 0x45B75F44
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45842278
#define PATH_AUDIO_EXT 0x4583FA78
#define PATH_IMAGE_INT 0x458422F8
#define PATH_IMAGE_EXT 0x4583FAE4
#define PATH_VIDEO_INT 0x4584258C
#define PATH_VIDEO_EXT 0x4583FB6C
#define PATH_THEME_INT 0x458424F8
#define PATH_THEME_EXT 0x4583FB54
#define PATH_OTHER_INT 0x4584234C
#define PATH_OTHER_EXT 0x4583FB3C

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

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C

#define PAGE_ENTER_EVENT 7
#define PAGE_EXIT_EVENT 8
#define ACCEPT_EVENT 2
#define PREVIOUS_EVENT 3
#define CANCEL_EVENT 4

#endif
#endif
