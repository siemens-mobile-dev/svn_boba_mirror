#include "../inc/swilib.h"
#include "main.h"
#include "string_util.h"
#include "jabber_util.h"
#include "groups_util.h"
#include "lang.h"
//===============================================================================================
// ELKA Compatibility
#pragma inline
void patch_header(HEADER_DESC* head)
{
  head->rc.x=0;
  head->rc.y=YDISP;
  head->rc.x2=ScreenW()-1;
  head->rc.y2=HeaderH()+YDISP;
}
#pragma inline
void patch_input(INPUTDIA_DESC* inp)
{
  inp->rc.x=0;
  inp->rc.y=HeaderH()+1+YDISP;
  inp->rc.x2=ScreenW()-1;
  inp->rc.y2=ScreenH()-SoftkeyH()-1;
}
//===============================================================================================

SOFTKEY_DESC jid_menu_sk[]=
{
  {0x0018,0x0000,(int)LG_SELECT},
  {0x0001,0x0000,(int)LG_BACK},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB jid_menu_skt=
{
  jid_menu_sk,0
};

WSHDR* jews;
char jTerminate=0;

int jed1_onkey(GUI *data, GUI_MSG *msg)
{
  //-1 - do redraw
  //1: close
  
  // Если зелёная кнопка либо нажата кнопка, которую мы повесили в cmd=7
  if(msg->gbsmsg->submess==GREEN_BUTTON || msg->keys==0x18)
  {
    jTerminate = 1;
    return 1;
  }
  
  if (msg->keys==0x0FF0) //Левый софт СГОЛД
  {
    return(1);
  }  
  return 0;
}

void jed1_ghook(GUI *data, int cmd)
{
  EDITCONTROL ec;
  static SOFTKEY_DESC mmmmsk={0x0018, 0x0000,(int)LG_OK};
 
  if (cmd==7)
  {
    //OnRun
    ExtractEditControl(data,2,&ec);    
    wstrcpy(jews,ec.pWS);    
#ifndef NEWSGOLD
  static const SOFTKEY_DESC sk_cancel={0x0FF0,0x0000,(int)LG_CLOSE};
#endif
#ifdef NEWSGOLD    
    SetSoftKey(data,&mmmmsk,0);
#else
    SetSoftKey(data,&mmmmsk,1);
    if (ec.pWS->wsbody[0]==0)
      SetSoftKey(data,&sk_cancel,SET_SOFT_KEY_N==0?1:0);    
#endif   
  }
  
  if(cmd==0x0A)   // Фокусирование
  {
     DisableIDLETMR();   // Отключаем таймер выхода по таймауту
  }

  if(jTerminate)
 {
   char* jid_jid = NULL;
   char* jid_name = NULL;
   char* jid_group = NULL;
   int jid_ask=0;
   int jid_subscription = 0;
   jTerminate=0;
   size_t st_len;
   st_len = wstrlen(jews)*2;
   if(st_len)
   {
      jid_jid =  utf16_to_utf8((char**)jews,&st_len);
//      jid_jid[st_len]='\0';
   } else jid_jid = NULL;
 
   ExtractEditControl(data,4,&ec);    
   wstrcpy(jews,ec.pWS);
   st_len = wstrlen(jews)*2;
   if(st_len)
   {
      jid_name =  utf16_to_utf8((char**)jews,&st_len);
//      jid_name[st_len]='\0';
   }else jid_name=NULL;
  
    ExtractEditControl(data,6,&ec);
    wstrcpy(jews,ec.pWS);
    st_len = wstrlen(jews)*2;
   if(st_len)
   {
      jid_group =  utf16_to_utf8((char**)jews,&st_len);
//      jid_group[st_len]='\0';
   } else jid_group =NULL;
   if(1==3)
   {
   extern long  strtol (const char *nptr,char **endptr,int base);
   ExtractEditControl(data,8,&ec);
   wstrcpy(jews,ec.pWS);
   char ss[10];
   ws_2str(jews,ss,15);
   int jid_ask = strtol (ss,0,10);

   ExtractEditControl(data,10,&ec);
   wstrcpy(jews,ec.pWS);
   ws_2str(jews,ss,15);
   int jid_subscription = strtol (ss,0,10);
   }
    if(jid_jid)
    {
  char answer[400];
  char* di = "roster3";
  sprintf(answer, "<item jid='%s'", jid_jid);
  if(jid_name)
    {
    sprintf(answer,"%s name='%s'",answer, jid_name);
    }
  if(jid_group)
  {
    sprintf(answer,"%s><group>%s</group></item>",answer, jid_group);
  }
  else 
  {
    strcat(answer,"/>");
  }
  SendIq(NULL, IQTYPE_SET, di, IQ_ROSTER, answer);
      mfree(jid_name);
      mfree(jid_jid);
      mfree(jid_group);
    }
   }

  if(cmd==0x03)     // onDestroy
  {
    FreeWS(jews);
  }
}

extern void dummy(GUI *data);

HEADER_DESC jed1_hdr={0,0,131,21,NULL,(int)LG_ENTERMUC,LGP_NULL};

INPUTDIA_DESC jed1_desc=
{
  1,
  jed1_onkey,
  jed1_ghook,
  (void *)dummy,
  0,
  &jid_menu_skt,
  {0,22,131,153},
  4,
  100,
  101,
  0,

//  0x00000001 - Выровнять по правому краю
//  0x00000002 - Выровнять по центру
//  0x00000004 - Инверсия знакомест
//  0x00000008 - UnderLine
//  0x00000020 - Не переносить слова
//  0x00000200 - bold
  0,

//  0x00000002 - ReadOnly
//  0x00000004 - Не двигается курсор
//  0x40000000 - Поменять местами софт-кнопки
  0x40000000
};

void Disp_JID_Enter_Dialog(CLIST* ClEx)
{
  //mode 0=edit; 1=add;
  char *jid = "";
  char *name = "";
  char *group = "";

  if(ClEx)
  {
  jid = ClEx->JID;
  name = ClEx->name;
  group = GetGroupNameByID(ClEx->group);
  }
  
  void *ma=malloc_adr();
  extern const char percent_t[];

  void *eq;
  EDITCONTROL ec;
  jews=AllocWS(256);
  
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  
  wsprintf(jews,percent_t,"JID:");
  ConstructEditControl(&ec,1,ECF_APPEND_EOL,jews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  utf8_2ws(jews, jid, 64);
  if(ClEx)ConstructEditControl(&ec,1,ECF_APPEND_EOL,jews,128);//2 если меняем контакт ЖИД недаем изменить
   else ConstructEditControl(&ec,3,ECF_APPEND_EOL,jews,128);//2
  AddEditControlToEditQend(eq,&ec,ma);  

  wsprintf(jews,percent_t,"Nick:");
  ConstructEditControl(&ec,1,ECF_APPEND_EOL,jews,256);
  AddEditControlToEditQend(eq,&ec,ma);  

  utf8_2ws(jews, name, 64);
  ConstructEditControl(&ec,3,ECF_APPEND_EOL,jews,80);     // 4
  AddEditControlToEditQend(eq,&ec,ma);  
  
  wsprintf(jews,percent_t,"Group:");
  ConstructEditControl(&ec,1,ECF_APPEND_EOL,jews,256);      
  AddEditControlToEditQend(eq,&ec,ma);  

  utf8_2ws(jews, group, 64);
  wsprintf(jews,percent_t,group);
  ConstructEditControl(&ec,3,ECF_APPEND_EOL,jews,80);    //6
  AddEditControlToEditQend(eq,&ec,ma);
  if (!ClEx)
    {
    wsprintf(jews,percent_t,"Ask:");
    ConstructEditControl(&ec,1,ECF_APPEND_EOL,jews,256);      
    AddEditControlToEditQend(eq,&ec,ma);

    wsprintf(jews,"%d",0);
    ConstructEditControl(&ec,5,ECF_APPEND_EOL,jews,1);    //8
    AddEditControlToEditQend(eq,&ec,ma);

    wsprintf(jews,percent_t,"Авторизировать:");
    ConstructEditControl(&ec,1,ECF_APPEND_EOL,jews,256);      
    AddEditControlToEditQend(eq,&ec,ma);

    wsprintf(jews,"%d",0);
    ConstructEditControl(&ec,5,ECF_APPEND_EOL,jews,1);    //10
    AddEditControlToEditQend(eq,&ec,ma);
    }
  patch_input(&jed1_desc);
  patch_header(&jed1_hdr);
  CreateInputTextDialog(&jed1_desc,&jed1_hdr,eq,1,0);
}

/*
void RemoveJID(CLIST* ClEx)
{
  if(!ClEx)return;
  char answer[400];
  sprintf(answer, "<item jid='%s' subscription='remove'/>", ClEx->JID);
  SendIq(NULL, IQTYPE_SET, "roster3", IQ_ROSTER, answer);
}
*/
