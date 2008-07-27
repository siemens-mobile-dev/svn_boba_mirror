#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\var_arg.h"


typedef struct
{
  BOOK book;
  void * TextInput;
  void * NameInput;
  char * string;
  wchar_t * FileName;
  int length;
}MyBOOK;

typedef struct
{
  BOOK * book;
}MSG;

MyBOOK * CreateFileBook;

int CreateTextInput(void *, BOOK *);
int CreateNameInput(void *, BOOK *);


char * unicode2win1251(char *s, wchar_t *ws, int len)
{
  char *d=s;
  int c;
  while((c=*ws++)&&((len--)>0))
  {
    if (c==0x401) c=0xA8;
    if (c==0x404) c=0xAA;
    if (c==0x407) c=0xAF;
    if (c==0x451) c=0xB8;
    if (c==0x454) c=0xBA;
    if (c==0x457) c=0xBF;
    if ((c>=0x410)&&(c<0x450)) c-=0x350;
    *s++=c;
  }
  *s=0;
  return(d);
}

void ExitAction(BOOK *)
{
  FreeBook((BOOK*)CreateFileBook);
}

int TerminateElf(void * ,BOOK* book)
{
  FreeBook(book);
  return(1);
}

int ShowAuthorInfo(void *mess ,BOOK* book)
{
  MSG * msg = (MSG*)mess;
  MessageBox(0x6fFFFFFF,STR("CreateTXTFile\r\n\r\n(c) IronMaster"),0, 1 ,5000,msg->book);
  return(1);
}

int DestroyNameInput()
{
  if (CreateFileBook->NameInput)
  {
    GUI_Free((GUI*)CreateFileBook->NameInput);
    CreateFileBook->NameInput=0;
  }
  return(0);
}

int DestroyTextInput()
{
  if (CreateFileBook->TextInput)
  {
    GUI_Free((GUI*)CreateFileBook->TextInput);
    CreateFileBook->TextInput=0;
  }
  return(0);
}

int onAccept(void * data, BOOK *)
{
  wchar_t f_name[256];
  wstrcpy(f_name,CreateFileBook->FileName);
  wstrcat(f_name,L".txt");
  FSTAT fstat;
  if (isFileExist(((FILEITEM*)data)->path,f_name,&fstat)>=0)
  {
    int str_id;
    textidname2id(L"DB_NAME_EXIST_TXT",SID_ANY_LEN,&str_id);
    MessageBox(0x6fFFFFFF,str_id,0, 1 ,5000,0);
  }
  else
  {
    int f=_fopen(((FILEITEM*)data)->path,f_name,0x2,0x180,0);
    fwrite(f,CreateFileBook->string,CreateFileBook->length);
    fclose(f);
    FreeBook(CreateFileBook);
  }
  return(0);
}

void CreateBrowser()
{
  void * DB_Desc=DataBrowserDesc_Create();
  char option[20];
  strcpy(option,"semc/dir");
  const wchar_t * folder_list[2];
  folder_list[0]=L"/card/other";
  folder_list[1]=0;
  int str_id;
  textidname2id(L"DB_OTHER_TXT",SID_ANY_LEN,&str_id);
  DataBrowserDesc_SetHeaderText(DB_Desc,str_id);
  DataBrowserDesc_SetBookID(DB_Desc,BOOK_GetSessionID((BOOK*)CreateFileBook));
  DataBrowserDesc_SetFolders(DB_Desc,(int*)folder_list);
  DataBrowserDesc_SetFoldersNumber(DB_Desc,1);
  DataBrowserDesc_SetSelectAction(DB_Desc,1);
  DataBrowserDesc_SetSelectActionOnFolders(DB_Desc,1);
  DataBrowserDesc_SetOption(DB_Desc,option);
  DataBrowserDesc_SetOpenEmptyFolder(DB_Desc,1);
  int OK_id;
  textidname2id(L"GUI_INSERT_SK",-1,&OK_id);
  DataBrowserDesc_SetOKSoftKeyText(DB_Desc,OK_id);
  DataBrowser_Create(DB_Desc);
  DataBrowserDesc_Destroy(DB_Desc);
}

const PAGE_MSG CreateFile_PageEvents[]@ "DYN_PAGE" ={
  ELF_TERMINATE_EVENT , TerminateElf,
  ELF_SHOW_INFO_EVENT  , ShowAuthorInfo,
  ACCEPT_EVENT_TAG, onAccept,
  NIL_EVENT_TAG,0
};


PAGE_DESC base_page ={"CreateFile_BasePage",0,CreateFile_PageEvents};


void AcceptAction_FileName(BOOK *,wchar_t * string,int len)
{
  if (FSX_isNameValid(string))
  {
    int str_id;
    textidname2id(L"DB_ILLEGAL_CHAR_TXT",SID_ANY_LEN,&str_id);
    MessageBox(0x6fFFFFFF,str_id,0, 1 ,5000,0);
  }
  else
  {
    if (CreateFileBook->FileName) delete(CreateFileBook->FileName);
    CreateFileBook->FileName=new wchar_t[len+1];
    wstrcpy(CreateFileBook->FileName,string);
    int str_id;
    textidname2id(L"MSG_UI_MOVE_MESSAGE_SELECT_FOLDER_TXT",SID_ANY_LEN,&str_id);
    MessageBox(0x6fFFFFFF,str_id,0, 1 ,5000,0);
    CreateBrowser();
  }
}


void AcceptAction(BOOK *,wchar_t * string,int len)
{
  CreateFileBook->length=len;
  if (CreateFileBook->string) delete(CreateFileBook->string);
  CreateFileBook->string=new char[len+1];
  unicode2win1251(CreateFileBook->string,string,len);
  int str_id;
  textidname2id(L"MSG_UI_FILENAMEP_TXT",SID_ANY_LEN,&str_id);
  CreateFileBook->NameInput=CreateStringInput(0,
                                                         VAR_PREV_ACTION_PROC(DestroyNameInput),
                                                         VAR_LONG_BACK_PROC(ExitAction),
                                                         VAR_BOOK(CreateFileBook),
                                                         VAR_OK_PROC(AcceptAction_FileName),
                                                         VAR_STRINP_FIXED_TEXT(str_id),
                                                         VAR_STRINP_MIN_LEN(1),
                                                         VAR_STRINP_MAX_LEN(40),
                                                         0);
}


void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}


void onCloseCreateFileBook(BOOK *)
{
  if (CreateFileBook)
  {
    if (CreateFileBook->string) delete(CreateFileBook->string);
    if (CreateFileBook->FileName) delete(CreateFileBook->FileName);
    DestroyNameInput();
    DestroyTextInput();
    SUBPROC(elf_exit);
  }
}


int CreateCreateFileBook()
{
  CreateFileBook = new MyBOOK;
  CreateBook(CreateFileBook,onCloseCreateFileBook,&base_page,"CreateFile",-1,0);
  CreateFileBook->NameInput=0;
  CreateFileBook->string=0;
  CreateFileBook->FileName=0;
  int str_id;
  textidname2id(L"BL_BLOG_TEXTP_TXT",SID_ANY_LEN,&str_id);
  CreateFileBook->TextInput=CreateStringInput(0,
                                              VAR_PREV_ACTION_PROC(ExitAction),
                                              VAR_LONG_BACK_PROC(ExitAction),
                                              VAR_BOOK(CreateFileBook),
                                              VAR_STRINP_FIXED_TEXT(str_id),
                                              VAR_STRINP_MIN_LEN(1),
                                              VAR_OK_PROC(AcceptAction),
                                              0);
  return(0);
}


int isCreateFileBook(BOOK * book)
{
  if(!strcmp(book->xbook->name,"CreateFile")) return(1);
  return(0);
}


int main (void)
{
  if (!FindBook(isCreateFileBook))
  {
    CreateCreateFileBook();
    return(0);
  }
  else
  {
    MessageBox(0x6fFFFFFF,STR("Already runed"),0, 1 ,5000,0);
  }
}
