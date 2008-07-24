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
void win12512unicode(u16 *ws, char *s, int len);
int SaveCfg();

extern "C" unsigned long strtoul(const char *, char **, int);
extern "C" long strtol(const char *, char **, int);

#define IDN_CHANGES_HAVE_BEEN_MADE _T("HPB_SAVE_BEFORE_EXITD_TXT")
#define IDN_SAVE_BEFORE_EXIT _T("HPB_SAVE_BEFORE_EXITQ_TXT")

#define IDN_CHECKBOX_UNCHECKED_ICON _T("CHECKBOX_ICN")
#define IDN_CHECKBOX_CHECKED_ICON _T("CHECKMARK_IN_BOX_ICN")

#define LGP_NULL 0x6FFFFFFF
#endif
