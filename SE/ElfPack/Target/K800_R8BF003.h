#ifndef K800_R8BF003
#error K800_R8BF003 configuration needed for this header!
#else
#ifndef _K800_R8BF003_
#define _K800_R8BF003_

#define EXT_TABLE 0x45B623A4
#define RUN_CHECK 0x44FC7BA5
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x45838490
#define PATH_AUDIO_EXT 0x45835C90
#define PATH_IMAGE_INT 0x45838510
#define PATH_IMAGE_EXT 0x45835CFC
#define PATH_VIDEO_INT 0x458387A4
#define PATH_VIDEO_EXT 0x45835D84
#define PATH_THEME_INT 0x45838710
#define PATH_THEME_EXT 0x45835D6C
#define PATH_OTHER_INT 0x45838566
#define PATH_OTHER_EXT 0x45835D54

#define PATH_ELF_ROOT_INT     (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT     (L"/card/other/ZBin")
#define PATH_ELF_INT          (L"/usb/other/ZBin")
#define PATH_ELF_EXT          (L"/card/other/ZBin")
#define PATH_ELF_DAEMONS_INT  (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT  (L"/card/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT   (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT   (L"/card/other/ZBin/Config")
#define PATH_INI              (L"/usb/other/ini")
#define PATH_DEFAULT          (L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44FBFB79
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44FBFB79
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44FBFB81
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44FBFBF5

#define DB_CMD_SETSMALLICON 0x3B
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
