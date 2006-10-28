#include "../inc/swilib.h"
#include "naticq.h"
#include "history.h"

char history_path[]="4:\\NatICQ_hist";

/*
  Добавлет строку в историю контакта CLIST
*/
void Add2History(CLIST *CListEx, char *header, char *message)
{
  volatile int hFile;
  unsigned int io_error = 0;
  char *fullname = malloc(128);
  
  unsigned int hdr_len = strlen(header);
  unsigned int msg_len = strlen(message);
  unsigned int buf_len = hdr_len+msg_len+1;
  char *buffer = malloc(buf_len);
  strcpy(buffer, header);
  strcat(buffer, message);
  strcpy(fullname, history_path);
  sprintf(fullname,"%s\\%u.txt", fullname, CListEx->uin);
  ShowMSG(1,(int)buffer);
  hFile = fopen(fullname,A_ReadWrite + A_Append + A_BIN,P_READ+P_WRITE, &io_error);
  if(io_error==2) // нет файла
  {
    hFile = fopen(fullname,A_ReadWrite+A_Create+ A_BIN,P_READ+P_WRITE, &io_error);
  }
  if(!io_error)
  {
    fwrite(hFile, buffer, buf_len-1, &io_error);
    //fwrite(hFile, header, hdr_len, &io_error);
  }
  else
  {
    char *q=malloc(21);
    sprintf(q, "Ошибка I/O  #%u", io_error);
    ShowMSG(1,(int)q); 
    mfree(q);
  }
  fclose(hFile, &io_error);
  mfree(fullname);
  mfree(buffer);
}
