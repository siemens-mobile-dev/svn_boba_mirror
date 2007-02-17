#include "../inc/swilib.h"
#include "main.h"
#include "jabber.h"
#include "clist_util.h"
#include "jabber_util.h"
#include "history.h"

CLIST* cltop = NULL;

unsigned int NContacts = 0;
unsigned int Active_page = 1;
unsigned int N_cont_disp=0;
unsigned int CursorPos = 1;

extern char logmsg[512];

void CList_RedrawCList()
{
  // Определяем, скока контактов поместится на странице списка
  int font_y = GetFontYSIZE(CLIST_FONT)*2 +2;
  int scr_w=ScreenW();
  N_cont_disp = sdiv(font_y,ScreenH()-CLIST_Y1)-1;
  if(!cltop)return; 
   
  LockSched(); 
  // Определяем количество страниц списка контактов
  int pages_number = sdiv(N_cont_disp, NContacts);
  if(N_cont_disp*pages_number<NContacts){pages_number++;};
  
  CLIST* ClEx = cltop;
  WSHDR* out_ws = AllocWS(256);
  int i=1;
  int start_y;
  int fcolor;
  char cur[2]=">\0";
  TRESOURCE* resEx;
  while(ClEx)
  {
    if(ClEx->ResourceCount)
    {
      resEx = ClEx->res_list;
      while(resEx)
      {
        if(i>(Active_page-1)*N_cont_disp)
        {
          if(i==CursorPos){cur[0]='>';}else{cur[0]=' ';}
          //wsprintf(out_ws,"%s TEST %d", cur, i);
          if(resEx->name)
          {
            wsprintf(out_ws,"%s %d %s/%s", cur, resEx->has_unread_msg, ClEx->name, resEx->name);
          }
          else
          {
            wsprintf(out_ws,"%s %d %s", cur, resEx->has_unread_msg, ClEx->name);
          }
          start_y = CLIST_Y1 + (i - (Active_page-1)*N_cont_disp)*font_y;
          if(resEx->has_unread_msg){fcolor=CLIST_F_COLOR_0;}else{fcolor=CLIST_F_COLOR_1;}
          DrawString(out_ws,3,start_y,scr_w-4,start_y+font_y,SMALL_FONT,0,GetPaletteAdrByColorIndex(fcolor),GetPaletteAdrByColorIndex(23));
        }
        i++;
        resEx = resEx->next;
        if(i>Active_page*N_cont_disp)break;
      }
    }
    ClEx = ClEx->next;
    if(i>Active_page*N_cont_disp)break;
  }
  UnlockSched();

  LockSched();
  sprintf(logmsg, "P=%d;C=%d;N=%d;ND=%d",Active_page, CursorPos,NContacts,N_cont_disp);
  UnlockSched();

  FreeWS(out_ws);
}

// Убить список сообщений
void KillMsgList(LOG_MESSAGE* messtop)
{
  LOG_MESSAGE* cl=messtop;
  while(cl)
  {
    LOG_MESSAGE *p;
    mfree(cl->mess);
    p=cl;
    cl=cl->next;
    mfree(p);
  }     
}


// Убить список ресурсов
void KillResourceList(TRESOURCE* res_list)
{
  TRESOURCE* cl=res_list;
  while(cl)
  {
    TRESOURCE *p;
    if(cl->name)mfree(cl->name);
    mfree(cl->full_name);
    if(cl->status_msg)mfree(cl->status_msg);
    if(cl->log)KillMsgList(cl->log);
    p=cl;
    cl=cl->next;
    mfree(p);
    p=NULL;
  }     
}

// Получить дескриптор контакта по FullJID (JID вместе с ресурсом)
CLIST* CList_FindContactByJID(char* jid)
{
  LockSched();
  CLIST* ClEx = cltop;
  while(ClEx)
  {
    if(strstr(jid,ClEx->JID)==jid)
    {
      UnlockSched();
      return ClEx;
    }
    ClEx = ClEx->next;
  }
  UnlockSched();
  return NULL;
}

// Узнать, есть ли уже такой ресурс у контакта, по FullJID
TRESOURCE* CList_IsResourceInList(char* jid)
{
  CLIST* ClEx = CList_FindContactByJID(jid);
  if(!ClEx)return NULL;
  LockSched();
  TRESOURCE* ResEx = ClEx->res_list;
  while(ResEx)
  {
    if(!strcmp(ResEx->full_name, jid))
    {
      UnlockSched();
      return ResEx;
    }
    ResEx = ResEx->next;
  }
  UnlockSched();
  return NULL;
}

// Поменять статус у контакта 
void CList_Ch_Status(TRESOURCE* resource,
                       char status,
                       char* status_msg
                       )
{
  
}


TRESOURCE* CList_AddResourceWithPresence(char* jid, char status, char* status_msg)
{
  TRESOURCE* qq = CList_IsResourceInList(jid);
  
  // Если такой ресурс уже есть, его не добавляем. 
  // Нужно ему статус поменять.
  if(qq)
  {
    CList_Ch_Status(qq, status, status_msg);
    return qq;
  }
  CLIST* ClEx = cltop;
  LockSched();
  while(ClEx)
  {
    
    if(strstr(jid,ClEx->JID)==jid) // Ага, именно так, ибо это соответствует началу строки!
    {
      TRESOURCE* ResEx=malloc(sizeof(TRESOURCE));//ClEx->res_list;
      char *resname_ptr=Get_Resource_Name_By_FullJID(jid);
      ResEx->name = malloc(strlen(resname_ptr)+1);
//      strcpy(ResEx->name, "(dummy)");
      ResEx->full_name = malloc(strlen(jid)+1);
      strcpy(ResEx->full_name, jid);
      strcpy(ResEx->name, resname_ptr);
      if(status_msg)
      {
        ResEx->status_msg = malloc(strlen(status_msg)+1);
        strcpy(ResEx->status_msg, status_msg);
      }
      else
      {
        ResEx->status_msg = NULL;
      }
            
      ResEx->status = status;
      ResEx->virtual = 0;
      ResEx->has_unread_msg=0;
      ResEx->log = NULL;
      
      // Удаляем псевдоресурс, если он есть
      if(ClEx->res_list->virtual==1)
      {
        KillResourceList(ClEx->res_list);
        ClEx->ResourceCount=0;
        NContacts--;
        ClEx->res_list=NULL;
      }    
      
      TRESOURCE* existing_res=ClEx->res_list;
      
      if(!existing_res)
      {
        ClEx->res_list = ResEx;
        ClEx->ResourceCount=1;
      }
      else
      {
        while(existing_res->next){existing_res=existing_res->next;}
        existing_res->next = ResEx;
        ClEx->ResourceCount++;
      }
      NContacts++;
      ResEx->next=NULL;
      CursorPos = 1;
      Active_page=1;
      UnlockSched();
      return ResEx;
    }
    ClEx = ClEx->next;
  }
CursorPos = 1;
Active_page=1;
UnlockSched();  
return NULL;
}


// Добавить к листу контакт. Возвращает структуру созданного контакта.
CLIST* CList_AddContact(char* jid,
                          char* name,
                          JABBER_SUBSCRIPTION subscription,
                          char group)
{
  CLIST* Cont_Ex = malloc(sizeof(CLIST));

  // Имя может быть не заполнено
  if(name)
  {
    Cont_Ex->name = malloc(strlen(name)+1);
    strcpy(Cont_Ex->name, name);
  }
  else
  {
    Cont_Ex->name = malloc(strlen(jid)+1);
    strcpy(Cont_Ex->name, jid);
  }

  Cont_Ex->JID = malloc(strlen(jid)+1);
  strcpy(Cont_Ex->JID, jid);
  Cont_Ex->subscription = subscription;
  Cont_Ex->group = group;
  Cont_Ex->next=NULL;
   
  TRESOURCE* ResEx = malloc(sizeof(TRESOURCE));
  ResEx->log=NULL;
  ResEx->next=NULL;
  ResEx->status=0;
  ResEx->status_msg=NULL;
  ResEx->has_unread_msg=0;
  ResEx->virtual=1; // По этому признаку потом его убъём
  ResEx->name = NULL;
  ResEx->full_name = malloc(strlen(jid)+1);
  strcpy(ResEx->full_name, jid);
//  strcat(ResEx->full_name, "/");
//  strcat(ResEx->full_name, ResEx->name);
  Cont_Ex->res_list=ResEx;
  Cont_Ex->ResourceCount=1;

  LockSched();
  if(!cltop)
  {
    cltop = Cont_Ex;
  }
  else
  {
    CLIST* tmp=cltop;
    while(tmp->next){tmp=tmp->next;}
    tmp->next=Cont_Ex;
  }

  NContacts++;
  UnlockSched();
  CursorPos = 1;
  Active_page=1;
  return Cont_Ex;
}


// Добавить сообщение в список сообщений контакта
void CList_AddMessage(char* jid, MESS_TYPE mtype, char* mtext)
{
  TTime now_time;
  TDate now_date;
  GetDateTime(&now_date,&now_time);
  char datestr[127];
  sprintf(datestr, "%s: %02d:%02d %02d-%02d\r\n",jid,now_time.hour,now_time.min,now_date.day,now_date.month);

  
  CLIST* contEx = CList_FindContactByJID(jid);
  if(!contEx)
  {
//    LockSched();
//    ShowMSG(1,(int)"No contact for message found!");
//    UnlockSched();
    Log("MESS_LOST",mtext);
    return;
  }
  TRESOURCE* cont = CList_IsResourceInList(jid);
  if(!cont)
  {
    // У контакта нет ресурсов или такого ресурса нет. В общую кучу добавим.
//    LockSched();
//    ShowMSG(1,(int)"Message added to default resource!");
//    UnlockSched();
    Add2History(contEx, datestr,mtext);
    contEx->res_list->has_unread_msg++;
    return;
  }
  
//  LockSched();
//  ShowMSG(1,(int)"Resource for message found OK!");
//  UnlockSched();
  
  cont->has_unread_msg++;
  LOG_MESSAGE* mess = malloc(sizeof(LOG_MESSAGE));
  mess->mess = malloc(strlen(mtext)+1);
  mess->mtype=mtype;
  LockSched();
  if(!cont->log)
  {
    cont->log = mess;
  }
  else
  {
    LOG_MESSAGE* tmp=cont->log;
    while(tmp->next){tmp=tmp->next;}
    tmp->next=mess;
  }
  mess->next=NULL;
  UnlockSched();
  Add2History(CList_FindContactByJID(jid), datestr,mtext);
}

/*
//Уничтожить список
void FreeCLIST(void)
{
  CLIST *cl=(CLIST*)cltop;
  cltop=0;
  while(cl)
  {
    CLIST *p;
//    if (cl->log) mfree(cl->log);
//    if (cl->answer) mfree(cl->answer);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
  }
}

*/

// Уничтожить список контактов  
void CList_Destroy()
{
  LockSched();
  CLIST* cl=cltop;
  cltop=NULL;
  while(cl)
  {
    CLIST *p;
    if(cl->res_list) KillResourceList(cl->res_list);
    cl->ResourceCount=0;
    mfree(cl->JID);
    mfree(cl->name);
    p=cl;
    cl=(CLIST*)(cl->next);
    mfree(p);
    p=NULL;
  }  
  UnlockSched();
}

// Управление курсором
void CList_MoveCursorUp()
{
  if(!NContacts)return;
  if(CursorPos==1)
  {
    CursorPos=NContacts;
    Active_page = sdiv(N_cont_disp, NContacts)+1;
  }
  else
  {
    CursorPos--;
    if(CursorPos<=(Active_page-1)*N_cont_disp){Active_page--;}
  }
  REDRAW();
};
void CList_MoveCursorDown()
{
  if(!NContacts)return;
  CursorPos++;
  if(CursorPos>NContacts){CursorPos=1;Active_page=1;}
  if(Active_page*N_cont_disp<CursorPos){Active_page++;}
  REDRAW();
};

void CList_MoveCursorHome(){};
void CList_MoveCursorEnd(){};
