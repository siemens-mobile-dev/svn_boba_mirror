#include "../inc/swilib.h"
#include "naticq.h"
#include "history.h"

extern const char HIST_PATH[64];

/*
  Добавлет строку в историю контакта CLIST
*/
void Add2History(CLIST *CListEx, char *header, char *message)
{
  volatile int hFile;
  unsigned int io_error = 0;
  char *fullname = malloc(128);
  char delim[] = "\r\n----------\r\n";
  unsigned int delim_len = strlen(delim);
  unsigned int hdr_len = strlen(header);
  unsigned int msg_len = strlen(message);
  unsigned int buf_len = delim_len+hdr_len+msg_len+1;
  char *buffer = malloc(buf_len);
  strcpy(buffer, delim);
  strcat(buffer, header);
  strcat(buffer, message);
  strcpy(fullname, HIST_PATH);
  sprintf(fullname,"%s\\%u.txt", fullname, CListEx->uin);
  
  // Открываем файл на дозапись и создаём в случае неудачи
  hFile = fopen(fullname,A_ReadWrite + A_Append + A_BIN,P_READ+P_WRITE, &io_error);
  if(io_error==2) // нет файла
  {
    hFile = fopen(fullname,A_ReadWrite+A_Create+ A_BIN,P_READ+P_WRITE, &io_error);
  }
  if(!io_error)
  {
    fwrite(hFile, buffer, buf_len-1, &io_error);
    fclose(hFile, &io_error);
  }
  else
  {
    char *q=malloc(41);
    sprintf(q, "Ошибка I/O  #%u", io_error);
    ShowMSG(1,(int)q); 
    mfree(q);
  }
  mfree(fullname);
  mfree(buffer);
}

void GetStatusById(char *buffer, int id)
{
  
  // ??? Тут надо разбираться со статусами
  // Похоже, не все коды верные...
switch(id)
{
case 0: {strcpy(buffer,"Online");break;}
case 1: {strcpy(buffer, "Invisible");break;}
case 2: {strcpy(buffer, "Away");break;}
case 3: {strcpy(buffer, "N/A");break;}
case 4: {strcpy(buffer, "Occupied");break;}
case 5: {strcpy(buffer, "DND");break;}
case 6: {strcpy(buffer, "Online");break;}
case 7: {strcpy(buffer, "FFC");break;}
default: {sprintf(buffer, "UNK: %d", id);}
}
}

void LogStatusChange(CLIST *CListEx)
{
  extern const int LOG_STATCH;
  if(!LOG_STATCH){return;}
  if(CListEx->state>7){return;}
  if(CListEx->log==NULL){return;}
  if(strlen(CListEx->log)==0){return;}
  char hdr[] = "(System message)";
  char msg[] = "%s меняет статус на %s\r\n";
  char *message = malloc(100);
  char *nickname = malloc(60);
  if(CListEx->name)
  {
    strcpy(nickname, CListEx->name);
  }
  else
  {
    sprintf(nickname, "%u", CListEx->uin);
  }
  char *status = malloc(20);
  GetStatusById(status, CListEx->state);
  sprintf(message, msg, nickname, status);
  Add2History(CListEx, hdr, message);
//  ShowMSG(1,(int)message);
  mfree(status);
  mfree(nickname);
  mfree(message);
}
