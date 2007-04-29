/*
    SieNatJabber Project
    Функции отрисовки иконок ростера
*/


// рисуем картинку с номером по заданным координатам
  void Roster_DrawIcon(char x, char y, int img_num);

// получаем номер картинки
#ifdef USE_PNG_EXT
void Roster_getIcon(char* path_to_pic, CLIST* ClEx, TRESOURCE* resEx);
#else
int Roster_getIcon(CLIST* ClEx, TRESOURCE* resEx);
#endif  
 //заменяем картинки на png 
  void Roster_fillIcons();

//вернет номер картинки по статусу
#ifdef USE_PNG_EXT
char* Roster_getIconByStatus(char* path_to_pic, char status);
#else
  int Roster_getIconByStatus(char status);
#endif
