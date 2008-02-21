#ifndef W550_R4CB020
#error W550_R4CB020 configuration needed for this header!
#else
#ifndef _W550_R4CB020_
#define _W550_R4CB020_
//#define pKBD 0x4C0467DC
#define EXT_TABLE 0x4444E170
#define RUN_CHECK 0x454EE341    // ?????????????



#define STR_START 0xB94
#define STR_APPLICATION 0x033F
#define STR_Saved_on_Memory_Stick 0x0FE5
#define STR_Saved_in_phone_memory 0x0FE4

#define ELF_SMALL_ICON 0xE550
//#define ELF_SMALL_ICON 0xE5AD

//#define ROOT_APP 0x4C2DC794
//#define PID_MMI 0x100E5
#define HELPER_SIG 0x6FF0
#define RAM_BASE (0x4C02E834)

#define PATH_AUDIO_INT 0x445B0F9E
#define PATH_AUDIO_EXT 0x445B0F9E
#define PATH_IMAGE_INT 0x445B101E
#define PATH_IMAGE_EXT 0x445B101E
#define PATH_VIDEO_INT 0x445B10FA
#define PATH_VIDEO_EXT 0x445B10FA
#define PATH_THEME_INT 0x445B10E4
#define PATH_THEME_EXT 0x445B10E4
#define PATH_OTHER_INT 0x445B10AC
#define PATH_OTHER_EXT 0x445B10AC

#define PATH_ELF_ROOT_INT        (L"/usb/other/ZBin")
#define PATH_ELF_ROOT_EXT        (L"/usb/other/ZBin")
#define PATH_ELF_INT             (L"/usb/other/ZBin")
#define PATH_ELF_EXT             (L"/usb/other/ZBin")
#define PATH_ELF_DAEMONS_INT     (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_DAEMONS_EXT     (L"/usb/other/ZBin/Daemons")
#define PATH_ELF_CONFIG_INT      (L"/usb/other/ZBin/Config")
#define PATH_ELF_CONFIG_EXT      (L"/usb/other/ZBin/Config")
#define PATH_INI                 (L"/usb/other/ini")
#define PATH_DEFAULT             (L"/usb/other")

#define ELF_RUN_PAGE_PREVIOUS_EVENT  0x454EBA71
#define ELF_RUN_PAGE_ACCEPT_EVENT    0x454EBA71
#define ELF_RUN_PAGE_CANCEL_EVENT    0x454EBA79
#define ELF_RUN_PAGE_PAGE_EXIT_EVENT 0x454EBAE1

#define DB_CMD_SETSMALLICON 0x36
#define DB_DB_EXT_C1 0x290
#endif
#endif
