extern void OnTimer(unsigned long ms,void huge *);
extern void OnTimer1(void);
extern void OnTimer2(void);
extern void DisableTimer(void);

typedef struct{
  int param0;
  int param1;
  int param2;
  int param3;
} GBSTMR;

//tmr - free 8 bytes
extern void StartTimer2(GBSTMR far* tmr, unsigned long step, void huge *); //step=1/217 s
extern void DisableTimerX(GBSTMR far* tmr);

extern unsigned int _hour;
extern unsigned int _minute;
extern unsigned int _day;
extern unsigned int _month;
extern unsigned int _year;

typedef struct
{
 unsigned int hour;
 unsigned int minute;
 unsigned int second;
}TIME;

typedef struct
{
 unsigned int year;
 unsigned int month;
 unsigned int day;
}DATE;

extern void CopyDate(DATE far*);
extern void CopyTime(TIME far*);

extern unsigned int GetWeekDay(DATE far*);

extern unsigned int GetDateFormat(void);
extern void DoDate0(DATE far*, char far* dest, int flag1, int flag2);//2-GetDataFormat^=1,1-#7
extern void DoTime0(TIME far*, char far* dest, int flag1, int flag2);//2-#1,1-#24



extern unsigned int HaveAlarm(void);
