#include "..\include\Lib_Clara.h"
#include "..\include\Dir.h"
#include "..\include\var_arg.h"
#include "conf_loader.h"
#include "config_data.h"
#include "bookman_daemon.h"
#include "shortcuts.h"
#include "main.h"


char * GetCurrentName(MyBOOK * mbk)
{
  BOOK_LIST_ITEM* elem;
  switch(mbk->ActiveTAB)
  {
  case 0:
    elem = (BOOK_LIST_ITEM*)List_Get( mbk->books_list, ListMenu_GetSelectedItem( mbk->blist ) );
    break;
  case 1:
    elem = (BOOK_LIST_ITEM*)List_Get( mbk->elfs_list, ListMenu_GetSelectedItem( mbk->elist ) );
    break;
  }
  return(elem->book_name);
}


void onCancel_SI(BOOK* book, void* )
{
  BookObj_ReturnPage( book, ACCEPT_EVENT );
}


void onPrevious_SI(BOOK* book, void* )
{
  BookObj_ReturnPage( book, ACCEPT_EVENT );
}


void onAccept_SI( BOOK* book, wchar_t* string, int len )
{
  MyBOOK* mbk = (MyBOOK*) book;
  int f;
  char * param;
  char * pos;
  char * cur_book = GetCurrentName(mbk);
  if (len)
  {
    char new_name[50];
    unicode2win1251(new_name,string,MAXELEMS(new_name));
    int new_name_len = strlen(new_name);
    if (strcmp(cur_book,new_name))
    {
      param = manifest_GetParam( mbk->ini_buf, cur_book, 0 );
      if ( param )
      {
        if (strcmp(new_name,param))
        {
          //ReWrite
          pos = strstr(mbk->ini_buf,param);
          if ( ( f = _fopen( get_path(), L"bookman.ini", 0x204, 0x180, 0 ) ) >= 0 )
          {
           int len_minus = strlen( param );    //длина старого названия
           fwrite( f, mbk->ini_buf, pos - mbk->ini_buf);     //пишем начало файла
           fwrite( f, new_name, new_name_len );      //пишем новое название
           fwrite( f, pos + len_minus, ( mbk->ini_buf_size - ( pos - mbk->shortcuts_buf )) - len_minus );      //пишем остаток файла
           fclose( f );
          }
          else
          {
            MessageBox( 0x6fFFFFFF, STR( "Can't open bookman.ini when write" ), 0, 1 , 5000, mbk );
          }
        }
        delete(param);
      }
      else
      {
        //Append
        if ( ( f = _fopen( get_path(), L"bookman.ini", 0x204, 0x180, 0 ) ) >= 0 )
        {
          char temp_buf[100];
          temp_buf[0]=0;
          strcpy(temp_buf,cur_book);
          strcat(temp_buf,": ");
          strcat(temp_buf,new_name);
          strcat(temp_buf,"\r\n");
          fwrite( f, mbk->ini_buf, mbk->ini_buf_size);     //пишем старый файла
          fwrite( f, temp_buf,strlen(temp_buf));
          fclose(f);
        }
        else
        {
          MessageBox( 0x6fFFFFFF, STR( "Can't open bookman.ini when append" ), 0, 1 , 5000, mbk );
        }
      }
    }
  }
  else
  {
    param = manifest_GetParam( mbk->ini_buf, cur_book, 0 );
    if ( param )
    {
      pos = strstr(mbk->ini_buf,cur_book);
      if ( ( f = _fopen( get_path(), L"bookman.ini", 0x204, 0x180, 0 ) ) >= 0 )
      {
        //Delete
        fwrite( f, mbk->ini_buf, pos - mbk->ini_buf);     //пишем начало файла
        char * pos_end=strstr(pos,"\r\n")+2;
        fwrite( f, pos_end, mbk->ini_buf_size-(pos_end-mbk->ini_buf));     //пишем остаток файла
        fclose( f );
      }
      else
      {
        MessageBox( 0x6fFFFFFF, STR( "Can't open bookman.ini when delete" ), 0, 1 , 5000, mbk );
      }
      delete(param);
    }
  }
  BookObj_ReturnPage( book, ACCEPT_EVENT );
}



int CreateSI( void* data, BOOK* book )
{
  MyBOOK* mbk = (MyBOOK*) book;
  int editable_strID = GetParam(GetCurrentName(mbk));

  mbk->StringInput = CreateStringInputVA( 0,
                                VAR_PREV_ACTION_PROC( onPrevious_SI ),
                                VAR_LONG_BACK_PROC( onCancel_SI ),
                                VAR_BOOK( book ),
                                VAR_OK_PROC( onAccept_SI ),
                                VAR_STRINP_TEXT( editable_strID ),
                                VAR_STRINP_MIN_LEN( 0 ),
                                VAR_STRINP_MAX_LEN( 50 ),
                                0 );
  return 0;
}


int ExitChangeName( void* data, BOOK* book )
{
  MyBOOK* mbk = (MyBOOK*) book;
  if (mbk->StringInput)
  {
    GUIObject_Destroy(mbk->StringInput);
    mbk->StringInput=0;
  }
  return 0;
}


const PAGE_MSG ChangeName_PageEvents[]@ "DYN_PAGE" = {
  PAGE_ENTER_EVENT_TAG, CreateSI,
  PAGE_EXIT_EVENT_TAG, ExitChangeName,
  NIL_EVENT_TAG, 0
};

PAGE_DESC ChangeName_page = { "BookManager_ChangeName_Page", 0, ChangeName_PageEvents };


void BookNames( BOOK* book, GUI* )
{
  MyBOOK * mbk = (MyBOOK*)book;
  mbk->ActiveTAB=GetActiveTab(mbk);
  BookObj_CallPage( book, &ChangeName_page );
}
