#include "../inc/swilib.h"
#include "clist_util.h"
#include "xml_parser.h"

// Служебная функция, правильно и безопасно склеивает 
// поля вкарда
unsigned int result_len = 0;
char *Add_vCard_Value(char *dest, char *par_name, char *val)
{
  char *param_pair = malloc(1024);
  zeromem(param_pair, 1024);
  snprintf(param_pair, 1023, "%s: %s\n", par_name, val);  // binary-safe
  unsigned int now_dest_len, parpair_len;
  now_dest_len = strlen(dest);
  parpair_len = strlen(param_pair);
  while(now_dest_len+parpair_len>result_len)
  {
    result_len += 1024;
    dest = realloc(dest, result_len);
  }
  strcat(dest,param_pair);
  mfree(param_pair);
  return dest;
}

// Обработчик vCard
void Process_vCard(char *from, XMLNode *vCard)
{
  result_len = 1024;
  char* result=malloc(result_len);
  strcpy(result, "vCard:\n");  
  // Обходим все присланные ноды vCard
  XMLNode* vCard_Node = vCard->subnode;
  while(vCard_Node)
  {
    if(vCard_Node->name && vCard_Node->value) // сюда не попадает Photo, и слава Богу
      Add_vCard_Value(result, vCard_Node->name, vCard_Node->value);
    vCard_Node = vCard_Node->next;
  }    
  //Формируем сообщение
  CList_AddMessage(from, MSG_SYSTEM, result);
  mfree(result);
}
//EOL,EOF
