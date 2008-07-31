#ifndef _MAIN_H_
  #define _MAIN_H_

typedef struct
{
  BOOK  book;
  GUI *bcfg;
  GUI *sel_bcfg;
  GUI_ONEOFMANY *cbox_gui;
  GUI *text_input;
  GUI *yesno;
  GUI *color;
  GUI *coord;
  GUI *tinput;
  GUI *dinput;
  GUI *font_select;
  LIST *list;
  CFG_HDR *cur_hp;
  unsigned long old_crc;
  u16 check_box_unchecked;
  u16 check_box_checked;
  STRID changes_have_been_made;
  STRID save_before_exit;
}MyBOOK;

typedef struct
{
  BOOK * book;
}MSG;

int TerminateElf(void * ,BOOK* book);
int ShowAuthorInfo(void *mess ,BOOK* book);
int MainPageOnCreate(void *, BOOK *bk);
int MainPageOnClose(void *, BOOK *bk);
int isBcfgEditBook(BOOK * struc);
GUI *create_ed(BOOK *book, CFG_HDR *need_to_focus);
void win12512unicode(wchar_t *ws, const char *s, int len);
int SaveCfg();

extern "C" unsigned long wcstoul(const wchar_t *, wchar_t **, int);
extern "C" long wcstol(const wchar_t *, wchar_t **, int);

#define IDN_CHANGES_HAVE_BEEN_MADE L"HPB_SAVE_BEFORE_EXITD_TXT"
#define IDN_SAVE_BEFORE_EXIT L"HPB_SAVE_BEFORE_EXITQ_TXT"

#define IDN_CHECKBOX_UNCHECKED_ICON L"CHECKBOX_ICN"
#define IDN_CHECKBOX_CHECKED_ICON L"CHECKMARK_IN_BOX_ICN"

#define LGP_NULL 0x6FFFFFFF
#endif
