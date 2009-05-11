#include "../inc/swilib.h"
#include "conf_loader.h"

#pragma swi_number=0x80E2
__swi __arm char *RamKeyBuffer();

extern unsigned long  strtoul (const char *nptr,char **endptr,int base);
extern const int i_action;
extern const char action_parameter[256];

void OpenConfig(char *filename)
{
  WSHDR *ws;
  ws = AllocWS(256);
  WSHDR *xws;
  xws=AllocWS(256);
  int id;
  str_2ws(xws,"bcfg",255);
  str_2ws(ws, filename, 255);
  id=GetExtUid_ws(xws);
  TREGEXPLEXT *pr=get_regextpnt_by_uid(id);
  typedef unsigned int (*func)(WSHDR *p1, WSHDR *p2, int p3);
  func myproc = (func)(pr->proc);
  myproc(ws, xws, 0);
  FreeWS(ws);
  FreeWS(xws);
}

int main()
{
  // Определяем нажатую клавишу
  char *current_key="";
  current_key = RamPressedKey();
  if(strlen(current_key)==0)
  {
    ShowMSG(1, (int)"Press on key not enough long!!!");
    return 0;
  }
  // Читаем конфиг
  char *config_name = malloc(256);
  sprintf(config_name, "4:\\ZBin\\etc\\hotkeys\\ElfsInHotkeys_%s.bcfg",current_key);
  if (LoadConfigData(config_name)<0) // Ищем конфиг на диске 4
  {
    config_name[0] = '0';
    if (LoadConfigData(config_name)<0) // Ищес конфиг на диске 0
    {
      ShowMSG(1, (int)"X:\\Zbin\\etc\\hotkeys\\ not found!!!"); // Материмся
      return 0;
    }
  }
  // Смотрми а установлен ли хоткей
  char *parameter;
  parameter = (char*)action_parameter;
  switch(i_action)
  {
  case 0:
    {
      // Ничего не установлено... Открываем конфиг на редактирование
      OpenConfig(config_name);
    }
    break;
  case 1:
    {
      // Запуск файла
      WSHDR *elfname=AllocWS(256);
      wsprintf(elfname,parameter);
      ExecuteFile(elfname,NULL,NULL);
      FreeWS(elfname);
    }
    break;
  case 2:
    {
      // Шорткат
      unsigned int* addr = (unsigned int*)GetFunctionPointer(parameter);
      if (addr)
      {
        typedef void (*voidfunc)(); 
#ifdef NEWSGOLD          
        voidfunc pp=(voidfunc)*(addr+4);
#else 
        voidfunc pp=(voidfunc)addr; 
#endif 
        SUBPROC((void*)pp);
      }
    }
    break;
  case 3:
    {
      // Энтрик
      typedef void (*voidfunc)(); 
      unsigned int addr=strtoul(parameter, 0, 16 );
      voidfunc pp=(voidfunc)addr;
      SUBPROC((void*)pp);
    }
    break;
  }
  return 0;
}
