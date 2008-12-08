#ifndef K810_R8BA024_Orange_Gen
#error K810_R8BA024_Orange_Gen configuration needed for this header!
#else
#ifndef _K810_R8BA024_Orange_Gen_
#define _K810_R8BA024_Orange_Gen_

#define EXT_TABLE 0x45BA5630 
#define RUN_CHECK 0x4500113D 
#define HELPER_SIG 0x6FF0

#define PATH_AUDIO_INT 0x4587DF90
#define PATH_AUDIO_EXT 0x4587B790
#define PATH_IMAGE_INT 0x4587E010
#define PATH_IMAGE_EXT 0x4587B7FC
#define PATH_VIDEO_INT 0x4587E2A4
#define PATH_VIDEO_EXT 0x4587B884
#define PATH_THEME_INT 0x4587E210
#define PATH_THEME_EXT 0x4587B86C
#define PATH_OTHER_INT 0x4587E064
#define PATH_OTHER_EXT 0x4587B854

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

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x44FFD599
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x44FFD599 
#define ELF_RUN_PAGE_CANCEL_EVENT    0x44FFD5A1
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x44FFD615

#define DB_CMD_SETSMALLICON 0x3B
#define DB_CMD_SETTHUMBNAILICON 0x3C
#define DB_DB_EXT_C1 0x2EC

#endif
#endif
