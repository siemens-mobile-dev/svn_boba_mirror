#ifndef lgp_H
#define lgp_H

#ifdef EN
const char on[]="on";
const char off[]="off";
const char alarm_name[]="Alarm";
const char change[]="change";
const char save[]="save";
const char ok[]="ok";
const char *wd[7]={"Mo","Tu","We","Th","Fr","Sa","Su"};
#else
const char on[]="вкл.";
const char off[]="выкл.";
const char alarm_name[]="Будильник";
const char change[]="измен.";
const char save[]="сохр.";
const char ok[]="ок";
const char *wd[7]={"Пн","Вт","Ср","Чт","Пт","Сб","Вс"};
const char *wd2[7]={"Понедельник","Вторник","Среда","Четверг","Пятница","Суббота","Воскресенье"};
#endif

#endif
