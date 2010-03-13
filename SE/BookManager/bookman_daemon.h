#ifndef _BOOKMAN_DAEMON_H_
#define _BOOKMAN_DAEMON_H_

//Создаем демона
void CreateDaemon( void );

//Убиваем демона
void DestroyDaemon( void );

//Для файнда
int isBookmanDaemonBook( BOOK* book );

typedef struct _DaemonBook : BOOK
{
  wchar_t ActiveTAB;
}DaemonBook;

#endif
