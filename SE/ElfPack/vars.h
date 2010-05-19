#ifndef _VARS_H_
#define _VARS_H_

#ifdef DBG
#define _printf(...) {debug_printf("\nOS: ElfPack -> ");debug_printf(__VA_ARGS__);}
#else
#define _printf(...)
#endif

#define __get_epd EP_DATA * epd = getepd();

void ELFExtrRegister(EP_DATA * epd);


#endif
