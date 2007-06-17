#include "..\inc\swilib.h"
#include "log.h"
void Log(char* module, char* logstr)
{
  volatile int hFile;
  unsigned int io_error = 0;
  char fullname[] ="4:\\httplog.txt";
  char* buffer = malloc(65536);
  zeromem(buffer,65536);
  sprintf(buffer,"[%s]: %s\r\n",module, logstr);
  int buf_len = strlen(buffer);
  // Открываем файл на дозапись и создаём в случае неудачи
  hFile = fopen(fullname,A_ReadWrite +A_Create+ A_Append + A_BIN,P_READ+P_WRITE, &io_error);
  if(!io_error)
  {
    fwrite(hFile, buffer, buf_len-1, &io_error);
    fclose(hFile, &io_error);
  }
  mfree(buffer);

}
