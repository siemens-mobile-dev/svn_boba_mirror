#include "..\inc\swilib.h"
#include "..\inc\cfg_items.h"
#include "conf_loader.h"
#include "nsd.h"
#include "IdleLinks.h"
#include "keyhook.h"
#include "main.h"

extern void kill_data(void *p,void (*func_p)(void *));
void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

int main(void)
{
  NSD_Create();
  return 0;
}
