#include "..\\include\Lib_Clara.h"
#include "..\\include\Dir.h"
#include "..\\include\cfg_items.h"
#include "conf_loader.h"
#include "config_data.h"

/*
typedef struct
{
  char dummy[24];
  char * PNUM;
}MTCALL_EVENT_DATA;
*/

typedef struct
{
  char pos1;
  char cale_base;
  short pos3;
}CALE_SET;

typedef struct
{
  char dummy[0x2C];
  int unkID;
}ses_est;

typedef struct
{
  char dummy[0x34];
  int unkID;
}ses_term;

typedef struct
{
  wchar_t * PNUM;
  wchar_t * Name;
  int first_callstate;
  int last_callstate;
  DATETIME * startdatetime;
  DATETIME * begindatetime;
  char line;
}myList_elem;

typedef struct
{
  int ID;
  DATETIME * SesEst;
}myList_gprs_elem;

typedef struct
{
  BOOK * book;
}MSG;

BOOK * CLBook;
LIST * myList;
LIST * myList_gprs;
DATETIME * cur_date;

int incomleted_connection=0;
wchar_t * Money=0;

//Название говорящее...
void win12512unicode(wchar_t *ws, char *s, int len)
{
  int c;
  while((c=*s++)&&((len--)>0))
  {
    if (c==0xA8) c=0x401;
    if (c==0xAA) c=0x404;
    if (c==0xAF) c=0x407;
    if (c==0xB8) c=0x451;
    if (c==0xBA) c=0x454;
    if (c==0xBF) c=0x457;
    if ((c>=0xC0)&&(c<0x100)) c+=0x350;
    *ws++=c;
  }
  *ws=0;
}

// берем значения из ини-файла
void get_bcfg_key(void)
{
  InitConfig();
  if (Money) delete(Money);
  Money=new wchar_t[strlen((char*)strMoney)+1];
  win12512unicode(Money,(char*)strMoney,strlen((char*)strMoney));

}



//Функция поиска элемента в листе звонков
int myList_Find(void *r0,void *r1)
{
  if (((myList_elem*)r0)->line==((myList_elem*)r1)->line) return(0);
  return(1);
}

//Функция поиска элемента в листе жпрс
int myList_gprs_Find(void *r0,void *r1)
{
  if (((myList_gprs_elem*)r0)->ID==((myList_gprs_elem*)r1)->ID) return(0);
  return(1);
}

//Убийство моего элемента
void myList_elem_Free(void * r)
{
  myList_elem * r0=(myList_elem *)r;
  if (r0)
  {
    if (r0->PNUM)
    {
      delete(r0->PNUM);
    }
    if (r0->Name)
    {
      delete(r0->Name);
    }
    if (r0->startdatetime)
    {
      delete(r0->startdatetime);
    }
    if (r0->begindatetime)
    {
      delete(r0->begindatetime);
    }    
    delete(r0);
  }
}


//Убийство моего элемента жпрс
void myList_gprs_elem_Free(void * r)
{
  myList_gprs_elem * r0=(myList_gprs_elem *)r;
  delete(r0->SesEst);
  delete(r0);
}

//Фильтр для убийства элементов при убийстве листа (всех ненулевых)
int myList_elem_Filter(void * r0)
{
  if (r0) return(1);
  return(0);
}

//Убийство эльфа
int TerminateElf(void * ,BOOK * book)
{
  FreeBook(book);
  return(1);
}

//Показать информацию об авторе
int ShowAuthorInfo(void *mess ,BOOK * book)
{
  MSG * msg = (MSG*)mess;
  MessageBox(0x6fFFFFFF,STR("Call Logger, v2.2\n\n(c) IronMaster"),0, 1 ,5000,msg->book);
  return(1);
}

/*
//Тестируем черный список....
int BlackList(void * BlackStruct, BOOK *)
{
  (MTCALL_EVENT_DATA*)BlackStruct;
  return(0);
}
*/


void sum_traf_cost(DATETIME * cur_date)
{
  FSTAT _fstat;
  wchar_t fpath[200];
  wchar_t folder[20];
  wstrcpy(fpath,GetDir(DIR_OTHER | MEM_EXTERNAL));
  wstrcat(fpath,L"/Call Logger/");
  snwprintf(folder,20,L"%04d-%02d-%02d",cur_date->date.year,cur_date->date.mon,cur_date->date.day);
  wstrcat(fpath,folder);
  if (fstat(fpath,L"gprs.txt",&_fstat)>=0)
  {
    char * buffer_gprs=new char[_fstat.fsize+2];
    memset(buffer_gprs,0,_fstat.fsize+2);
    int f=_fopen(fpath,L"gprs.txt",0x1,0x180,0);
    fread(f,buffer_gprs,_fstat.fsize);
    fclose(f);
    wchar_t buffer_str[200];
    if (!wstrwstr((wchar_t*)buffer_gprs,L"Total cost"))
    {
      int rub=0;
      int kop=0;
      int hex=0;
      int len=0;
      wchar_t temp_buf[50];
      wchar_t * pos=(wchar_t *)buffer_gprs;
      while (pos=wstrwstr(pos,L"cost -"))
      {
        pos=pos+7;
        len=(wstrchr(pos,'.')-pos);
        wstrncpy(temp_buf,pos,len);
        wtoi(temp_buf,len,&hex);
        rub=rub+hex;
        hex=0;
        pos=wstrchr(pos,'.')+1;
        wstrncpy(temp_buf,pos,2);
        wtoi(temp_buf,2,&hex);
        kop=kop+hex;
        hex=0;
      }
      rub=rub+kop/100;
      kop=kop%100;
      
      pos=(wchar_t *)buffer_gprs;
      int traf_whole=0;
      int traf_fract=0;
      hex=0;
      len=0;        
      if (TrafDiv)
      {
        int traf_whole_sent=0;
        int traf_fract_sent=0;
        int traf_whole_rec=0;
        int traf_fract_rec=0;          
        if (TrafKb)
        {
          while (pos=wstrwstr(pos,L"sent -"))
          {
            pos=pos+7;
            len=(wstrchr(pos,'.')-pos);
            wstrncpy(temp_buf,pos,len);
            wtoi(temp_buf,len,&hex);
            traf_whole_sent=traf_whole_sent+hex;
            hex=0;
            pos=wstrchr(pos,'.')+1;
            wstrncpy(temp_buf,pos,2);
            wtoi(temp_buf,2,&hex);
            traf_fract_sent=traf_fract_sent+hex;
            hex=0;
          }
          traf_whole_sent=traf_whole_sent+traf_fract_sent/100;
          traf_fract_sent=traf_fract_sent%100;
          pos=(wchar_t *)buffer_gprs;
          while (pos=wstrwstr(pos,L"received -"))
          {
            pos=pos+11;
            len=(wstrchr(pos,'.')-pos);
            wstrncpy(temp_buf,pos,len);
            wtoi(temp_buf,len,&hex);
            traf_whole_rec=traf_whole_rec+hex;
            hex=0;
            pos=wstrchr(pos,'.')+1;
            wstrncpy(temp_buf,pos,2);
            wtoi(temp_buf,2,&hex);
            traf_fract_rec=traf_fract_rec+hex;
            hex=0;
          }
          traf_whole_rec=traf_whole_rec+traf_fract_rec/100;
          traf_fract_rec=traf_fract_rec%100;
          snwprintf(buffer_str,400,L"\r\nTotal cost - %d.%02d %ls\r\nTotal sent - %d.%02d Kb\r\nTotal received - %d.%02d Kb\r\n\r\n",rub,kop,Money,traf_whole_sent,traf_fract_sent,traf_whole_rec,traf_fract_rec);
        }
        else
        {
          while (pos=wstrwstr(pos,L"sent -"))
          {
            pos=pos+7;
            len=(wstrchr(pos,' ')-pos);
            wstrncpy(temp_buf,pos,len);
            wtoi(temp_buf,len,&hex);
            traf_whole_sent=traf_whole_sent+hex;
            hex=0;
          }
          pos=(wchar_t *)buffer_gprs;
          while (pos=wstrwstr(pos,L"received -"))
          {
            pos=pos+11;
            len=(wstrchr(pos,' ')-pos);
            wstrncpy(temp_buf,pos,len);
            wtoi(temp_buf,len,&hex);
            traf_whole_rec=traf_whole_rec+hex;
            hex=0;
          }
          snwprintf(buffer_str,400,L"\r\nTotal cost - %d.%02d %ls\r\nTotal sent - %d b\r\nTotal received - %d b\r\n\r\n",rub,kop,Money,traf_whole_sent,traf_whole_rec);
        }
      }
      else
      {
        if (TrafKb)
        {
          while (pos=wstrwstr(pos,L"traffic -"))
          {
            pos=pos+10;
            len=(wstrchr(pos,'.')-pos);
            wstrncpy(temp_buf,pos,len);
            wtoi(temp_buf,len,&hex);
            traf_whole=traf_whole+hex;
            hex=0;
            pos=wstrchr(pos,'.')+1;
            wstrncpy(temp_buf,pos,2);
            wtoi(temp_buf,2,&hex);
            traf_fract=traf_fract+hex;
            hex=0;
          }
          traf_whole=traf_whole+traf_fract/100;
          traf_fract=traf_fract%100;
          snwprintf(buffer_str,400,L"\r\nTotal cost - %d.%02d %ls\r\nTotal traffic - %d.%02d Kb\r\n\r\n",rub,kop,Money,traf_whole,traf_fract);
        }
        else
        {
          while (pos=wstrwstr(pos,L"traffic -"))
          {
            pos=pos+10;
            len=(wstrchr(pos,' ')-pos);
            wstrncpy(temp_buf,pos,len);
            wtoi(temp_buf,len,&hex);
            traf_whole=traf_whole+hex;
            hex=0;
          }
          snwprintf(buffer_str,400,L"\r\nTotal cost - %d.%02d %ls\r\nTotal traffic - %d b\r\n\r\n",rub,kop,Money,traf_whole);
        }
      }
      f=_fopen(fpath,L"gprs.txt",0x108,0x180,0);
      fwrite(f,buffer_str,wstrlen(buffer_str)*2);
      fclose(f);
    }
    delete(buffer_gprs);
  }      
}

int onClockChange(void * r0, BOOK *)
{
  DATETIME * datetime=new(DATETIME);
  REQUEST_DATEANDTIME_GET(SYNC,datetime);
  if (cur_date->date.day!=datetime->date.day)
  {
    if (!incomleted_connection)
    {
      if (ConnectionManager_Connection_GetState())
      {
        incomleted_connection=1;
      }
      else
      {
        sum_traf_cost(cur_date);
      }
    }
    memcpy(cur_date,datetime,7);
  }
  delete(datetime);
  return(0);
}

//Ловим создание содинения
int onSessionEstablished(void * r0, BOOK *)
{
  if (r0)
  {
    myList_gprs_elem * elem_gprs=new(myList_gprs_elem);
    elem_gprs->ID=((ses_est*)r0)->unkID;
    elem_gprs->SesEst=new(DATETIME);
    REQUEST_DATEANDTIME_GET(SYNC,elem_gprs->SesEst);
    ListElement_AddtoTop(myList_gprs,elem_gprs);
  }
  return(0);
}

//Ловим окончание содинения
int onSessionTerminated(void * r0, BOOK *)
{
  if (r0)
  {
    int f;
    wchar_t fpath_gprs[200];
    wchar_t folder_gprs[20];
    wchar_t buffer_gprs_ptr[400];
    wchar_t * buffer_gprs=buffer_gprs_ptr;
    GPRS_SESSION_INFO * buf_ses=new(GPRS_SESSION_INFO);
    GPRS_GetLastSessionInfo(0,buf_ses);
    
    myList_gprs_elem * temp_elem_gprs=new(myList_gprs_elem);
    temp_elem_gprs->ID=((ses_term*)r0)->unkID;
    int pos=ListElement_Find(myList_gprs,temp_elem_gprs,myList_gprs_Find);
    if (pos!=0xFFFF)
    {
      myList_gprs_elem * elem_gprs=(myList_gprs_elem*)ListElement_Remove(myList_gprs,pos);
      wstrcpy(fpath_gprs,GetDir(DIR_OTHER | MEM_EXTERNAL));
      wstrcat(fpath_gprs,L"/Call Logger/");
      snwprintf(folder_gprs,20,L"%04d-%02d-%02d",elem_gprs->SesEst->date.year,elem_gprs->SesEst->date.mon,elem_gprs->SesEst->date.day);
      wstrcat(fpath_gprs,folder_gprs);
      if (fstat(fpath_gprs,L"gprs.txt",0)<0)
      {
        buffer_gprs[0]=0xFEFF;
        buffer_gprs=buffer_gprs+1;
      }
      int traf=buf_ses->SentBytes+buf_ses->RcvBytes;
      if (traf/1024<=TrafRound)
      {
        traf=TrafRound;
      }
      else
      {
        if (traf/1024%TrafRound>0)
        {
          traf=traf/1024/TrafRound*TrafRound+TrafRound;
        }
        else
        {
          traf=traf/1024;
        }
      }
      int traf_cost;
      int cost_rub;
      int cost_kop1;
      int cost_kop2;
      if (!TimeDay)
      {
        traf_cost=traf*TrafCost/1024;
        if (traf*TrafCost%1024*10/1024>=5) traf_cost=traf_cost+1;
        cost_rub=traf_cost/100;
        cost_kop1=traf_cost%100*10/100;
        cost_kop2=traf_cost%100*10%100*10/100;
      }
      else
      {
        int cur_time;
        if (!elem_gprs->SesEst->time.hour)
        {
          cur_time=24;
        }
        else
        {
          cur_time=elem_gprs->SesEst->time.hour;
        }
        if (TimeDay<TimeNight)
        {          
          if ((cur_time>=TimeDay)&&(cur_time<TimeNight))
          {
            traf_cost=traf*TrafCostDay/1024;
            if (traf*TrafCostDay%1024*10/1024>=5) traf_cost=traf_cost+1;
            cost_rub=traf_cost/100;
            cost_kop1=traf_cost%100*10/100;
            cost_kop2=traf_cost%100*10%100*10/100;
          }
          else
          {
            traf_cost=traf*TrafCostNight/1024;
            if (traf*TrafCostNight%1024*10/1024>=5) traf_cost=traf_cost+1;
            cost_rub=traf_cost/100;
            cost_kop1=traf_cost%100*10/100;
            cost_kop2=traf_cost%100*10%100*10/100;
          }
        }
        else
        {
          if (TimeNight<TimeDay)
          {
            if ((cur_time>=TimeNight)&&(cur_time<TimeDay))
            {
              traf_cost=traf*TrafCostNight/1024;
              if (traf*TrafCostNight%1024*10/1024>=5) traf_cost=traf_cost+1;
              cost_rub=traf_cost/100;
              cost_kop1=traf_cost%100*10/100;
              cost_kop2=traf_cost%100*10%100*10/100;
            }
            else
            {
              traf_cost=traf*TrafCostDay/1024;
              if (traf*TrafCostDay%1024*10/1024>=5) traf_cost=traf_cost+1;
              cost_rub=traf_cost/100;
              cost_kop1=traf_cost%100*10/100;
              cost_kop2=traf_cost%100*10%100*10/100;
            }
          }
          else
          {
            if ((cur_time>=TimeDay)&&(cur_time<TimeNight))
            {
              traf_cost=traf*TrafCostDay/1024;
              if (traf*TrafCostDay%1024*10/1024>=5) traf_cost=traf_cost+1;
              cost_rub=traf_cost/100;
              cost_kop1=traf_cost%100*10/100;
              cost_kop2=traf_cost%100*10%100*10/100;

            }
            else
            {
              traf_cost=traf*TrafCostNight/1024;
              if (traf*TrafCostNight%1024*10/1024>=5) traf_cost=traf_cost+1;
              cost_rub=traf_cost/100;
              cost_kop1=traf_cost%100*10/100;
              cost_kop2=traf_cost%100*10%100*10/100;
            }
          }
        }
      }
      if ((Hol1)||(Hol2))
      {
        int cur_day;
        char weekday;
        CALE_SET * baseday=new(CALE_SET);
        DATE_GetWeekDay((DATE*)elem_gprs->SesEst,&weekday);
        Cale_GetSettings(3,baseday);
        if (weekday>=baseday->cale_base)
        {
          cur_day=weekday-baseday->cale_base+1;
        }
        else
        {
          cur_day=7+weekday-baseday->cale_base+1;
        }
        delete(baseday);
        if ((Hol1==cur_day)||(Hol2==cur_day))
        {
          traf_cost=traf*TrafCostHol/1024;
          if (traf*TrafCostHol%1024*10/1024>=5) traf_cost=traf_cost+1;
          cost_rub=traf_cost/100;
          cost_kop1=traf_cost%100*10/100;
          cost_kop2=traf_cost%100*10%100*10/100;
        }
      }
      
      if (TrafDiv)
      {
        if (TrafKb)
        {
          int sent=buf_ses->SentBytes;
          int Kb_s=sent/1024;
          int b_s1=sent%1024*10/1024;
          int b_s2=sent%1024*10%1024*10/1024;
          if (sent%1024*10%1024*10%1024*10/1024>=5) b_s2=b_s2+1;
          if (b_s2==10)
          {
            b_s1=b_s1+1;
            b_s2=0;
          }
          if (b_s1==10)
          {
            Kb_s=Kb_s+1;
            b_s1=0;
          }
          
          int rec=buf_ses->RcvBytes;
          int Kb_r=rec/1024;
          int b_r1=rec%1024*10/1024;
          int b_r2=rec%1024*10%1024*10/1024;
          if (rec%1024*10%1024*10%1024*10/1024>=5) b_r2=b_r2+1;
          if (b_r2==10)
          {
            b_r1=b_r1+1;
            b_r2=0;
          }
          if (b_r1==10)
          {
            Kb_r=Kb_r+1;
            b_r1=0;
          }
          snwprintf(buffer_gprs,390,L"Start time - %02d:%02d:%02d, sent - %d.%d%d Kb, received - %d.%d%d Kb, cost - %d.%d%d %ls\r\n",elem_gprs->SesEst->time.hour,elem_gprs->SesEst->time.min,elem_gprs->SesEst->time.sec,Kb_s,b_s1,b_s2,Kb_r,b_r1,b_r2,cost_rub,cost_kop1,cost_kop2,Money);
        }
        else
        {
          snwprintf(buffer_gprs,390,L"Start time - %02d:%02d:%02d, sent - %d b, received - %d b, cost - %d.%d%d %ls\r\n",elem_gprs->SesEst->time.hour,elem_gprs->SesEst->time.min,elem_gprs->SesEst->time.sec,buf_ses->SentBytes,buf_ses->RcvBytes,cost_rub,cost_kop1,cost_kop2,Money);
        }
      }
      else
      {
        traf=buf_ses->SentBytes+buf_ses->RcvBytes;
        if (TrafKb)
        {
          int Kb=traf/1024;
          int b1=traf%1024*10/1024;
          int b2=traf%1024*10%1024*10/1024;
          if (traf%1024*10%1024*10%1024*10/1024>=5) b2=b2+1;
          if (b2==10)
          {
            b1=b1+1;
            b2=0;
          }
          if (b1==10)
          {
            Kb=Kb+1;
            b1=0;
          }
          snwprintf(buffer_gprs,390,L"Start time - %02d:%02d:%02d, traffic - %d.%d%d Kb, cost - %d.%d%d %ls\r\n",elem_gprs->SesEst->time.hour,elem_gprs->SesEst->time.min,elem_gprs->SesEst->time.sec,Kb,b1,b2,cost_rub,cost_kop1,cost_kop2,Money);
        }
        else
        {
          snwprintf(buffer_gprs,390,L"Start time - %02d:%02d:%02d, traffic - %d b, cost - %d.%d%d %ls\r\n",elem_gprs->SesEst->time.hour,elem_gprs->SesEst->time.min,elem_gprs->SesEst->time.sec,traf,cost_rub,cost_kop1,cost_kop2,Money);
        }
      }
      int str_len=(wstrlen(buffer_gprs))*2;
      if (fstat(fpath_gprs,L"gprs.txt",0)<0)
      {
        buffer_gprs=buffer_gprs-1;
      }
      FSTAT _fstat;
      if ((fstat(fpath_gprs,L"gprs.txt",&_fstat)<0)||(!sort_order))
      {
        f=_fopen(fpath_gprs,L"gprs.txt",0x108,0x180,0);
        fwrite(f,buffer_gprs,str_len);
      }
      else
      {
        char * new_buff=new char[_fstat.fsize+str_len];
        f=_fopen(fpath_gprs,L"gprs.txt",0x4,0x180,0);
        fread(f,new_buff,2);
        lseek(f,2,0);
        memcpy(new_buff+2,buffer_gprs,str_len);
        fread(f,new_buff+2+str_len,_fstat.fsize-2);
        lseek(f,0,0);
        fwrite(f,new_buff,_fstat.fsize+str_len);
        delete(new_buff);
      }
      fclose(f);
      if (incomleted_connection)
      {
        sum_traf_cost(elem_gprs->SesEst);
        incomleted_connection=0;
      }
      myList_gprs_elem_Free(elem_gprs);
    }
    delete(buf_ses);
    delete(temp_elem_gprs);
  }
  return(0);
}
  
//Оновное действо.... Все в куче....
int OnCallManager(void * CallManStruct, BOOK *)
{
  //Ловим всех кроме idle
  if (((CALLMANAGER_EVENT_DATA*)CallManStruct)->CallState)
  {
    //Создаем структурку
    myList_elem* temp_elem=new(myList_elem);
    int length=PNUM_len(((CALLMANAGER_EVENT_DATA*)CallManStruct)->PNUM);
    //Проверяем наличие элемента в листе
    if (((CALLMANAGER_EVENT_DATA*)CallManStruct)->CallID==0xFF)
    {
      if (myList->FirstFree)
      {
        temp_elem->line=1;
      }
      else
      {
        temp_elem->line=0;
      }
    }
    else
    {
      temp_elem->line=((CALLMANAGER_EVENT_DATA*)CallManStruct)->CallID;
    }
    temp_elem->PNUM=new wchar_t[length+1];
    char * sp=new char[length+1];
    memset(sp,0,length+1);
    PNUM2str(sp,((CALLMANAGER_EVENT_DATA*)CallManStruct)->PNUM,length,length+1);
    str2wstr(temp_elem->PNUM,sp);
    delete(sp);
    if (ListElement_Find(myList,temp_elem,myList_Find)==0xFFFF)
    {
      temp_elem->startdatetime=new DATETIME;
      REQUEST_DATEANDTIME_GET(SYNC,temp_elem->startdatetime);
      temp_elem->first_callstate=((CALLMANAGER_EVENT_DATA*)CallManStruct)->CallState;
      temp_elem->last_callstate=0;
      if (CallStatusDesc_GetName(CallID_GetCallStatusDesc(temp_elem->line)))
      {
        if (CallStatusDesc_GetName(CallID_GetCallStatusDesc(temp_elem->line))[0])
        {
          temp_elem->Name=new wchar_t[wstrlen(CallStatusDesc_GetName(CallID_GetCallStatusDesc(temp_elem->line))+1)];
          wstrcpy(temp_elem->Name,CallStatusDesc_GetName(CallID_GetCallStatusDesc(temp_elem->line))+2);
        }
        else
        {
          temp_elem->Name=new wchar_t[14];
          temp_elem->Name[0]=0;
        }
      }
      else
      {
        temp_elem->Name=new wchar_t[20];
        temp_elem->Name[0]=0;
        wstrcpy(temp_elem->Name,L"Emergency");
      }
        
      temp_elem->begindatetime=new DATETIME;
      temp_elem->begindatetime->time.hour=0;
      temp_elem->begindatetime->time.min=0;
      temp_elem->begindatetime->time.sec=0;
      ListElement_AddtoTop(myList,temp_elem);
    }
    else
    {
      if (((CALLMANAGER_EVENT_DATA*)CallManStruct)->CallState==6)
      {
        DATETIME * enddatetime=new DATETIME;
        REQUEST_DATEANDTIME_GET(SYNC,enddatetime);
        myList_elem * elem=(myList_elem*)ListElement_Remove(myList,ListElement_Find(myList,temp_elem,myList_Find));
        int f;
        int sub;
        int m;
        int s;
        wchar_t fpath[200];
        wchar_t fname[20];
        wchar_t folder[20];
        wchar_t fpath_ini[200];
        fpath[0]=0;
        fname[0]=0;
        folder[0]=0;
        wstrcpy(fpath,GetDir(DIR_OTHER | MEM_EXTERNAL));
        wstrcat(fpath,L"/Call Logger/");
        wchar_t buffer_ptr[400];
        wchar_t * buffer=buffer_ptr;
        memset(buffer,0,200);
        if (!elem->Name[0])
        {
          snwprintf(elem->Name,8,L"NoName");
        }
        if (elem->last_callstate==0)
        {
          if (elem->first_callstate==2)
          {
            //Исходящий вызов
            wstrcat(fname,L"out.txt");
          }
          if (elem->first_callstate==4)
          {
            //Пропущенный вызов
            wstrcat(fname,L"missed.txt");
          }
          m=0;
          s=0;
        }
        else
        {
          if (elem->first_callstate==2)
          {
            //Исходящий вызов
            wstrcat(fname,L"out.txt");
          }
          if (elem->first_callstate==4)
          {
            //Входящий вызов
            wstrcat(fname,L"in.txt");
          }
          //Если вызов начался в 23, а закончился в 00, меняем 00 на 24
          if ((elem->begindatetime->time.hour==23)&&(enddatetime->time.hour==0)) enddatetime->time.hour=24;
          //Получаем время разговора в секундах
          sub=(enddatetime->time.hour-elem->begindatetime->time.hour)*3600+(enddatetime->time.min-elem->begindatetime->time.min)*60+enddatetime->time.sec-elem->begindatetime->time.sec;
          //Выделяем минуты и секунды
          m=sub/60;
          s=sub%60;
        }
        //Получаем название папки
        snwprintf(folder,20,L"%04d-%02d-%02d",elem->startdatetime->date.year,elem->startdatetime->date.mon,elem->startdatetime->date.day);
        //Получаем полный путь
        wstrcat(fpath,folder);
        //Проверяем существование файла, если нету то добавляем в буфер хидер юникодного файла
        if (fstat(fpath,fname,0)<0)
        {
          buffer[0]=0xFEFF;
          buffer=buffer+1;
        }
        wstrcpy(fpath_ini,GetDir(DIR_INI));
        wstrcat(fpath_ini,L"/Call Logger");
        FSTAT _fstat;
        int reg_flag=0;
        if (fstat(fpath_ini,L"regions.ini",&_fstat)>=0)
        {
          char * buffer_ini=new char[_fstat.fsize];
          f=_fopen(fpath_ini,L"regions.ini",0x1,0x180,0);
          fread(f,buffer_ini,_fstat.fsize);
          fclose(f);
          char * PNUM_str=new char[wstrlen(elem->PNUM)+1];
          int ptr=0;
          int count=wstrlen(elem->PNUM)/2;
          wstr2strn(PNUM_str,elem->PNUM,count);
          if (strstr(buffer_ini,PNUM_str))
          {
            ptr=1;
            while (count<=wstrlen(elem->PNUM))
            {
              count=count+1;
              wstr2strn(PNUM_str,elem->PNUM,count);
              if (strstr(buffer_ini,PNUM_str)==0) break;
            }
            count=count-1;
          }
          else
          {
            while (count>0)
            {
              count=count-1;
              wstr2strn(PNUM_str,elem->PNUM,count);
              if (strstr(buffer_ini,PNUM_str))
              {
                ptr=1;
                break;
              }
            }
          }
          if (ptr)
          {
            char * region;
            wstr2strn(PNUM_str,elem->PNUM,count);
            if (region=manifest_GetParam(buffer_ini,PNUM_str,0))
            {
              wchar_t * s2ws = new wchar_t[strlen(region)+1];
              win12512unicode(s2ws,region,strlen(region));
              if ((elem->last_callstate==0)&&(elem->first_callstate==4))
              {
                snwprintf(buffer,390,L"%ls, %ls (%ls) - %02d:%02d:%02d\r\n",elem->Name,s2ws,elem->PNUM,elem->startdatetime->time.hour,elem->startdatetime->time.min,elem->startdatetime->time.sec);
              }
              else
              {
                snwprintf(buffer,390,L"%ls, %ls (%ls) - %02d:%02d:%02d, %02d:%02d\r\n",elem->Name,s2ws,elem->PNUM,elem->startdatetime->time.hour,elem->startdatetime->time.min,elem->startdatetime->time.sec,m,s);
              }
              delete(s2ws);
              mfree(region);
              reg_flag=1;
            }
          }
          delete(buffer_ini);
          delete(PNUM_str);
        }
        if (!reg_flag)
        {
          if ((elem->last_callstate==0)&&(elem->first_callstate==4))
          {
            
            //Печатаем "отчет о звонке" в буфер без региона
            snwprintf(buffer,390,L"%ls (%ls) - %02d:%02d:%02d\r\n",elem->Name,elem->PNUM,elem->startdatetime->time.hour,elem->startdatetime->time.min,elem->startdatetime->time.sec);
          }
          else
          {
            snwprintf(buffer,390,L"%ls (%ls) - %02d:%02d:%02d, %02d:%02d\r\n",elem->Name,elem->PNUM,elem->startdatetime->time.hour,elem->startdatetime->time.min,elem->startdatetime->time.sec,m,s);
          }
        }
        int str_len=(wstrlen(buffer))*2;
        if (fstat(fpath,fname,0)<0)
        {
          buffer=buffer-1;
        }
        if ((fstat(fpath,fname,&_fstat)<0)||(!sort_order))
        {
          f=_fopen(fpath,fname,0x108,0x180,0);
          fwrite(f,buffer,str_len);
        }
        else
        {
          char * new_buff=new char[_fstat.fsize+str_len];
          f=_fopen(fpath,fname,0x104,0x180,0);
          fread(f,new_buff,2);
          lseek(f,2,0);
          memcpy(new_buff+2,buffer,str_len);
          fread(f,new_buff+2+str_len,_fstat.fsize-2);
          lseek(f,0,0);
          fwrite(f,new_buff,_fstat.fsize+str_len);
          delete(new_buff);
        }
        fclose(f);
        delete(enddatetime);
        myList_elem_Free(elem);
      }
      if (((CALLMANAGER_EVENT_DATA*)CallManStruct)->CallState==1)
      {
        myList_elem * elem=(myList_elem*)ListElement_GetByIndex(myList,ListElement_Find(myList,temp_elem,myList_Find));
        if (elem->last_callstate!=1)
        {
          elem->last_callstate=1;
          elem->begindatetime=new DATETIME;
          REQUEST_DATEANDTIME_GET(SYNC,elem->begindatetime);
        }
      }
      //Убиваем временный элемент если не засунули в лист
      delete(temp_elem->PNUM);
      delete(temp_elem);
    }
  }
  return(1);
}


static int ReconfigElf(void *mess ,BOOK *book)
{
  RECONFIG_EVENT_DATA *reconf=(RECONFIG_EVENT_DATA *)mess;
  int result=0;
  if (wstrcmpi(reconf->path,successed_config_path)==0 && wstrcmpi(reconf->name,successed_config_name)==0)
  {
    get_bcfg_key();
  }
  return(result);
}

const PAGE_MSG CL_PageEvents[]@ "DYN_PAGE" ={
  ELF_TERMINATE_EVENT , TerminateElf,
  ELF_SHOW_INFO_EVENT  , ShowAuthorInfo,
  ON_CALLMANAGER_EVENT_TAG, OnCallManager,
  UI_CONNECTIONMANAGER_SESSION_ESTABLISHED_EVENT_TAG, onSessionEstablished,
  UI_CONNECTIONMANAGER_SESSION_TERMINATED_EVENT_TAG, onSessionTerminated,
  CLOCK_DATEANDTIME_IND_EVENT_TAG,onClockChange,
  ELF_RECONFIG_EVENT,ReconfigElf,
//  0xD6F,BlackList,
  NIL_EVENT_TAG,0
};

PAGE_DESC base_page ={"CL_BasePage",0,CL_PageEvents};


void elf_exit(void)

{
  kill_data(&ELF_BEGIN, (void(*)(void*))mfree_adr());
}


void onCloseCLBook(BOOK * CLBook)
{
  List_FreeElements(myList,myList_elem_Filter,myList_elem_Free);
  List_Free(myList);
  List_FreeElements(myList_gprs,myList_elem_Filter,myList_gprs_elem_Free);
  List_Free(myList_gprs);
  if (Money) delete(Money);
  delete(cur_date);
  SUBPROC(elf_exit);
}

BOOK * CreateCLBook()
{
  CLBook=new(BOOK);
  CreateBook(CLBook,onCloseCLBook,&base_page,"Call Logger",-1,0);
  myList=List_New();
  myList_gprs=List_New();
  cur_date=new(DATETIME);
  REQUEST_DATEANDTIME_GET(SYNC,cur_date);
  get_bcfg_key();
  return(CLBook);
}


int myFind(BOOK* book)
{
        if(!strcmp(book->xbook->name,"Call Logger")) return(1);
        return(0);
}


int main (void)
{
  if (FindBook(myFind))
  {
    MessageBox(0x6fFFFFFF,STR("Already runed"),0, 1 ,5000,0);
    SUBPROC(elf_exit);
  }
  else
  {
    CreateCLBook();
    return(0);
  }
}
