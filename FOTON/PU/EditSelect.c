//-----------------------------------------------------
// Процедуры редактирования данных
// Реализованы в двух вариантах
// Edit??? - вариант для ввода с клавиатуры
// Select??? - вариант для последовательного увеличения/уменьшения параметра
// Процедуры Edit??? возвращают 1 если было редактирование, 0 - в противном случае
// На входе процедуры Edit??? требуют введенный символ, позицию курсора,
// позицию первого символа параметра и указатель на параметр
// На входе процедуры Select??? требуют позицию курсора,
// позицию первого символа параметра, направление изменения (+1 или -1) и указатель на параметр
// Все процедуры проверяют попадание курсора в допустимую позицию редактирования
// для каждого параметра и производит необходимые изменения (+1,-1 или символ)
// в необходимой позиции.
// Перепечатывание процедуры не выполняют
//------------------------------------------------------

// Редактирование int в виде 4-х шестнадцатиричных цифр
char Edit_hi(char c, char pos, char lp,int *pn)
{
  char *n;
  n=(char*)pn;
  pos-=lp;
  if (isxdigit(c)&&(pos<4))
  {
    if (isalpha(c))
      c=toupper(c)-'A'+0x0A;
    else
      c=c-'0';
    switch (pos)
    {
    case 0: *(n+1)=(*(n+1)&0x0F)|(c<<4);
    break;
    case 1: *(n+1)=(*(n+1)&0xF0)|(c);
    break;
    case 2: *n=(*n&0x0F)|(c<<4);
    break;
    case 3: *n=(*n&0xF0)|(c);
    break;
    }
    return(1);
  }
  else
  {
    return(0);
  }
}

//Редактирование char в виде двух шестнадцатиричных цифр
char Edit_hb(char c, char pos, char lp,char *pn)
{
  pos-=lp;
  if (isxdigit(c)&&(pos<2))
  {
    if (isalpha(c))
      c=toupper(c)-'A'+0x0A;
    else
      c=c-'0';
    switch (pos)
    {
    case 0: *pn=(*pn&0x0F)|(c<<4);
    break;
    case 1: *pn=(*pn&0xF0)|(c);
    break;
    }
    return(1);
  }
  else
  {
    return(0);
  }
}

//Редактирование char в виде 8-ми бит
char Edit_bb(char c, char pos, char lp,char *pn)
{
  pos-=lp;
  if (((c=='0')||(c=='1'))&&(pos<8))
  {
    *pn&=~(0x80>>pos);
    if (c=='1') *pn|=(0x80>>pos);
    return(1);
  }
  else
  {
    return(0);
  }
}

//Редактирование char в виде двух десятичных цифр
char Edit_dc(char c, char pos, char lp,char *n)
{
  pos-=lp;
  if (isdigit(c)&&(pos<2))
  {
    switch (pos)
    {
    case 0: *n=(*n%10)+(c-'0')*10;
    break;
    case 1: *n=*n-*n%10+c-'0';
    break;
    }
    return(1);
  }
  else
  {
    return(0);
  }
}

//Редактирование char в виде трех десятичных цифр
char Edit_db(char c, char pos, char lp,char *n)
{
  char h,m,l;
  
  h=*n/100;
  l=*n%10;
  m=(*n-(h*100)-l)/10;
  
  pos-=lp;
  if (isdigit(c)&&(pos<3))
  {
    switch (pos)
    {
    case 0: h=c-'0'; break;
    case 1: m=c-'0'; break;
    case 2: l=c-'0'; break;
    }
    *n=h*100+m*10+l;
    return(1);
  }
  else
  {
    return(0);
  }
}

//Перебирание int в виде 4-х шестнадцатиричных цифр
void Select_hi(char pos,char lp,signed char d,int *pn)
{
  char *n;
  n=(char*)pn;
  switch (pos-lp)
  {
  case 0: *(n+1)+=d<<4;
  break;
  case 1: *(n+1)=*(n+1)&0xF0|*(n+1)+d&0x0f;
  break;
  case 2: *n+=d<<4;
  break;
  case 3: *n=*n&0xF0|*n+d&0x0f;
  break;
  }
}

//Перебирание char в виде двух шестнадцатиричных цифр
void Select_hb(char pos,char lp,signed char d,char *pn)
{
  switch (pos-lp)
  {
  case 0: *pn+=d<<4;
  break;
  case 1: *pn=*pn&0xF0|*pn+d&0x0f;
  break;
  }
}

//Перебирание char в виде 8-ми бит
void Select_bb(char pos,char lp,signed char d,char *pn)
{
  pos-=lp;
  if (pos<8)
  {
    if (d>0) *pn|=(0x80>>pos); else *pn&=~(0x80>>pos);
  }
}

//Перебирание char в виде двух десятичных цифр
void Select_dc(char pos,char lp,signed char d,char *n)
{
  switch (pos-lp)
  {
  case 0: *n+=d*10;
  break;
  case 1: *n+=d;
  break;
  }
  if (*n>99) *n=0;
}

//Перебирание char в виде трех десятичных цифр
void Select_db(char pos,char lp,signed char d,char *n)
{
  switch (pos-lp)
  {
  case 0: *n+=d*100;
  break;
  case 1: *n+=d*10;
  break;
  case 2: *n+=d;
  break;
  }
}

//Запрос пароля и установка режима доступа
//На входе требует позицию вывода запроса (0 - верхняя строка, 40 - нижняя строка)
void TestPWD(char xpos)
{
  char c;
  static int pass1=0,pass2=0;
  static unsigned long timer=0; // Таймер для сохранения пароля 5мин
  static int mm=2;
  
  char pos=13;
  PWDmode=0;
  
  _CLI();
  if (TimeDate>timer) {mm=2;pass1=0; pass2=0;} // Если таймер 5мин, забыть введенный пароль
  _SEI();
  do
  {
    memset(CON+xpos,' ',40);
    if (mm==2)
    {
      sprintf_P(CON+xpos,(STR)"Код доступа У2: %04X",pass1,pass2); //Печать
    }
    else
    {
      sprintf_P(CON+xpos,(STR)"Код доступа У3: %04X%04X",pass1,pass2); //Печать
    }
    c=16;
    do //Цикл забивания "*"
    {
      if (c!=pos) CON[c+xpos]='*'; //кроме позиции курсора
      c++;
    }
    while (c!=(mm==2?20:24));
    CursorPos=xpos+pos;
    c=KBD(0); //Ожидание клавиши
    if (c==kREDRAW) memset(CON,' ',80);
    if ((c>=32 && c<128)||(c>=192)) //Символьная клавиша
    {
      if (Edit_hi(c,pos,13,&pass1)) c=kRT; //Пытаемся редактировать
      if (Edit_hi(c,pos,17,&pass2)) c=kRT;
    }
    switch(c)
    {
    case kLT: //Стрелка влево
      switch (pos)
      {
      case 13: break; //Левее некуда
      case 16: pos=13; break;
      default: pos--; //Курсор влево
      }
      break;
    case kRT: //Стрелка вправо
      switch (pos)
      {
      case 13: pos=16; break;
      case 23: c=kESC; break; //Правее некуда
      case 19:
        if (mm==2) {c=kESC; break;}
      default: pos++; //Курсор вправо
      }
      break;
    case kUP: //Увеличение
      if (pos==13) mm=3;
      Select_hi(pos,16,1,&pass1); //Пытаемся увеличивать
      Select_hi(pos,20,1,&pass2);
      break;
    case kDW: //Уменьшение
      if (pos==13) mm=2;
      Select_hi(pos,16,-1,&pass1); //Пытаемся уменьшать
      Select_hi(pos,20,-1,&pass2);
      break;
    }
  }
  while (c!=kESC); //Пока не выход
  
  _CLI();
  timer=TimeDate+300; //Установка таймера 5мин
  _SEI();
  
  //Проверка паролей
  if (mm==2)
  {
    if ((pass1==PWDlevel2[0])/*&&(pass2==PWDlevel2[1])*/)
    {
      PWDmode=2; //Совпал пароль 2-го уровня
    }
  }
  else
  {
    if ((pass1==PWDlevel3[0])&&(pass2==PWDlevel3[1]))
    {
      PWDmode=3; //Совпал пароль 3-го уровня
    }
  }
}

//Запрос пароля и установка режима доступа Не использует KBD();
//На входе требует позицию вывода запроса (0 - верхняя строка, 40 - нижняя строка)
char XTestPWD(char xpos)
{
  char c;
  int pass1=0,pass2=0;
  char pos=13;
  
  do
  {
    memset(CON+xpos,' ',40);
    sprintf_P(CON+xpos,(STR)"Код доступа: %04X",pass1,pass2); //Печать
    c=13;
    do //Цикл забивания "*"
    {
      if (c!=pos) CON[c+xpos]='*'; //кроме позиции курсора
      c++;
    }
    while (c!=17);
    CursorPos=xpos+pos;
    UpdateConsole();
    while((c=KEY)==0) if (FIFO2EventLog()) Timer30sec=0; //Ожидание клавиши
    KEY=0;
    if (c==kESC) return 0;
    if ((c>=32 && c<128)||(c>=192)) //Символьная клавиша
    {
      if (Edit_hi(c,pos,13,&pass1)) c=kRT; //Пытаемся редактировать
      if (Edit_hi(c,pos,17,&pass2)) c=kRT;
    }
    switch(c)
    {
    case kLT: //Стрелка влево
      switch (pos)
      {
      case 13: break; //Левее некуда
      default: pos--; //Курсор влево
      }
      break;
    case kRT: //Стрелка вправо
      switch (pos)
      {
      case 16: c=kESC; break; //Правее некуда
      default: pos++; //Курсор вправо
      }
      break;
    case kUP: //Увеличение
      Select_hi(pos,13,1,&pass1); //Пытаемся увеличивать
      Select_hi(pos,17,1,&pass2);
      break;
    case kDW: //Уменьшение
      Select_hi(pos,13,-1,&pass1); //Пытаемся уменьшать
      Select_hi(pos,17,-1,&pass2);
      break;
    }
  }
  while (c!=kESC); //Пока не выход
  //Проверка паролей
  KEY=kREDRAW;
  c=0;
  if ((pass1==PWDlevel2[0])/*&&(pass2==PWDlevel2[1])*/)
  {
    c=2; //Совпал пароль 2-го уровня
  }
  //  if ((pass1==PWDlevel3[0])&&(pass2==PWDlevel3[1]))
  //  {
  //    c=3; //Совпал пароль 3-го уровня
  //  }
  return(c);
}
