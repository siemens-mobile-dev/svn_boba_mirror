#define UTF16_DIS_UNDERLINE (0xE002)
#define UTF16_ENA_UNDERLINE (0xE001)
//#define UTF16_DIS_UNK (0xE002)
//#define UTF16_ENA_UNK (0xE003)
#define UTF16_DIS_INVERT (0xE004)
#define UTF16_ENA_INVERT (0xE005)

#define UTF16_INK_RGBA (0xE006)
#define UTF16_PAPER_RGBA (0xE007)
#define UTF16_INK_INDEX (0xE008)
#define UTF16_PAPER_INDEX (0xE009)

#define UTF16_FONT_SMALL (0xE012)
#define UTF16_FONT_SMALL_BOLD (0xE013)

//E01C/E01D - left/right align
//E01E/E01F - center off/on

#ifdef NEWSGOLD
#define CBOX_CHECKED 0xE116
#define CBOX_UNCHECKED 0xE117
#else
#define CBOX_CHECKED 0xE10B
#define CBOX_UNCHECKED 0xE10C
#endif

