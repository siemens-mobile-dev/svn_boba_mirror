#ifndef _STRING_UTIL_H_
  #define _STRING_UTIL_H_

/*
  Преобразование буфера данных из кодировки UTF-8 в ANSI
IN:
  - tmp_out: куда положить результат. Буфер уже должен существовать
             и в нем должно быть достаточно места
  - UTF8_str: откуда брать данные для преобразования
  - size: сколько длина буфера для преобразования (UTF8_str)
  - fact - куда положить итоговый размер данных в буфере

OUT:  результирующий буфер. 
*/
void* convUTF8_to_ANSI(char* tmp_out, char *UTF8_str, unsigned int size, int* fact);

char* convUTF8_to_ANSI_STR(char *UTF8_str);

void ascii2ws(WSHDR *ws, const char *s);

// Преобразование в нижний регистр
char* str2lower(char *st);

// Функция из интернета. UTF16->UTF8
char *utf16_to_utf8(char **s, size_t *len);

// UTF8 для джаббера
char* Correct_UTF8_String(char* utf8_jid);

// А это вообще чтобы всем было хорошо (с) Чеботарёв А.А.
char* ANSI2UTF8(char* ansi_str, unsigned int maxlen);
#endif
