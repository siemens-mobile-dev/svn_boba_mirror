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
  char * param=0;
  char * pos;
  char * pos_uni_pair;
  char * cur_name;
  char * orig_name = GetCurrentName(mbk);
  
  char new_name[50];
  unicode2win1251(new_name,string,MAXELEMS(new_name));
  
  if (mbk->ini_buf) param = manifest_GetParam( mbk->ini_buf, orig_name, 0 );
  
  if (param) cur_name = param;
  else cur_name = orig_name;
  
  int new_name_len = strlen(new_name);
  int cur_name_len = strlen(cur_name);
  int orig_name_len = strlen(orig_name);
  
  char uni_pair[100];
  uni_pair[0]=0;
  strcpy(uni_pair,orig_name);
  strcat(uni_pair,": ");
  
  if ( (!len) || ( (!strcmp(orig_name,new_name)) && (new_name_len==orig_name_len) ) )
  {
    if ( param )
    {
      pos = strstr(mbk->ini_buf,uni_pair);
      if ( ( f = _fopen( get_path(), L"bookman.ini", 0x204, 0x180, 0 ) ) >= 0 )
      {
        //Delete
        fwrite( f, mbk->ini_buf, pos - mbk->ini_buf);     //пишем начало файла
        char * pos_end=strstr(pos,"\r\n")+2;
        fwrite( f, pos_end, mbk->ini_buf_size - (pos_end - mbk->ini_buf));     //пишем остаток файла
        fclose( f );
      }
      else
      {
        MessageBox( EMPTY_SID, STR( "Can't open bookman.ini when delete" ), NOIMAGE, 1 , 5000, mbk );
      }
    }
  }
  else
  {
    if ( (strcmp(cur_name,new_name)) || ( (!strcmp(cur_name,new_name)) && (new_name_len!=cur_name_len) ) )
    {
      if ( param )
      {
        //ReWrite
        pos_uni_pair = strstr(mbk->ini_buf,uni_pair);
        pos = pos_uni_pair + strlen(uni_pair);
        if ( ( f = _fopen( get_path(), L"bookman.ini", 0x204, 0x180, 0 ) ) >= 0 )
        {
          int len_minus = strlen( param );    //длина старого названия
          fwrite( f, mbk->ini_buf, pos - mbk->ini_buf);     //пишем начало файла
          fwrite( f, new_name, new_name_len );      //пишем новое название
          fwrite( f, pos + len_minus, ( mbk->ini_buf_size - ( pos - mbk->ini_buf )) - len_minus );      //пишем остаток файла
          fclose( f );
        }
        else
        {
          MessageBox( EMPTY_SID, STR( "Can't open bookman.ini when write" ), NOIMAGE, 1 , 5000, mbk );
        }
        
      }
      else
      {
        //Append
        if ( ( f = _fopen( get_path(), L"bookman.ini", 0x204, 0x180, 0 ) ) >= 0 )
        {
          strcat(uni_pair,new_name);
          strcat(uni_pair,"\r\n");
          fwrite( f, mbk->ini_buf, mbk->ini_buf_size);     //пишем старый файла
          fwrite( f, uni_pair,strlen(uni_pair));
          fclose(f);
        }
        else
        {
          MessageBox( EMPTY_SID, STR( "Can't open bookman.ini when append" ), NOIMAGE, 1 , 5000, mbk );
        }
      }
    }
  }
  if (param) delete(param);
  
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
