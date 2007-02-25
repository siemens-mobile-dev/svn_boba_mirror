#include "../inc/swilib.h"
#include "main.h"
#include "string_util.h"
#include "jabber_util.h"

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

SOFTKEY_DESC m_menu_sk[]=
{
  {0x0018,0x0000,(int)"�����"},
  {0x0001,0x0000,(int)"�����"},
  {0x003D,0x0000,(int)LGP_DOIT_PIC}
};

SOFTKEYSTAB m_menu_skt=
{
  m_menu_sk,3
};

WSHDR* mews;
char mTerminate=0;

int med1_onkey(GUI *data, GUI_MSG *msg)
{
  //-1 - do redraw
  if(msg->gbsmsg->submess==GREEN_BUTTON)
  {
    mTerminate = 1;
    return 1;
  }
  return 0;
  //1: close
}

void med1_ghook(GUI *data, int cmd)
{
  EDITCONTROL ec;
/*  
  if (cmd==7)
  {
    //OnRun
    ExtractEditControl(data,EDIT_GetFocus(data)-1,&ec);
    wstrcpy(ews,ec.pWS);
  }
*/  
  if(cmd==0x0A)   // �������������
  {
     DisableIDLETMR();   // ��������� ������ ������ �� ��������
  }


  if(mTerminate )//|| cmd==9)  // cmd==9 - ������� �� ����� ����-������ "��"
 {
//     char q[10];
//     sprintf(q,"N=%d",cmd);
//     ShowMSG(1,(int)q); 
   mTerminate=0;
   int param_ok=0;      // ���� ������������ ���� ����������
   size_t st_len;
// ��� �����
   ExtractEditControl(data,2,&ec);    
   wstrcpy(mews,ec.pWS);
   st_len = wstrlen(mews)*2;
   char* conf_name;
   if(st_len)
   {
      conf_name =  utf16_to_utf8((char**)mews,&st_len);
      conf_name[st_len]='\0';
      param_ok = 1;
      if(conf_name[0]=='@')
      {
        ShowDialog_Error(1,(int)"��� ����� �� ���������!");
        param_ok=0;
      }
   }
   else conf_name = NULL;
   
   char* user_name;
   ExtractEditControl(data,4,&ec);    
   wstrcpy(mews,ec.pWS);
   st_len = wstrlen(mews)*2;
   if(st_len)
   {
      user_name =  utf16_to_utf8((char**)mews,&st_len);
      user_name[st_len]='\0';
   }
   else user_name = NULL;
   
   
    extern long  strtol (const char *nptr,char **endptr,int base);
    ExtractEditControl(data,6,&ec);    // = n_messages
    wstrcpy(mews,ec.pWS);
    char ss[10];
    ws_2str(mews,ss,15);
    int n_messages = strtol (ss,0,10);
    if(user_name && conf_name && param_ok)
    {
      //ShowMSG(1,(int)user_name);
      //ShowMSG(1,(int)conf_name);
      Enter_Conference(conf_name, user_name, n_messages);
      mfree(user_name);
      mfree(conf_name);
    }
   }
  
    if(cmd==0x03)     // onDestroy
  {
    FreeWS(mews);
  }
}

extern void dummy(void);

HEADER_DESC med1_hdr={0,0,131,21,NULL,(int)"���� � MUC",0x7FFFFFFF};

INPUTDIA_DESC med1_desc=
{
  1,
  med1_onkey,
  med1_ghook,
  (void *)dummy,
  0,
  &m_menu_skt,
  {0,22,131,153},
  4,
  100,
  101,
  0,

//  0x00000001 - ��������� �� ������� ����
//  0x00000002 - ��������� �� ������
//  0x00000004 - �������� ���������
//  0x00000008 - UnderLine
//  0x00000020 - �� ���������� �����
//  0x00000200 - bold
  0,

//  0x00000002 - ReadOnly
//  0x00000004 - �� ��������� ������
//  0x40000000 - �������� ������� ����-������
  0x40000000
};

void Disp_MUC_Enter_Dialog()
{
  void *ma=malloc_adr();
  extern const char percent_t[];
  extern const char USERNAME[32];
  void *eq;
  EDITCONTROL ec;
  mews=AllocWS(256);
  
  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  
  wsprintf(mews,percent_t,"��� �����������:");
  ConstructEditControl(&ec,1,0x40,mews,256);
  AddEditControlToEditQend(eq,&ec,ma);

  wsprintf(mews,"@conference.jabber.ru");
  ConstructEditControl(&ec,3,0x40,mews,128);      //2
  AddEditControlToEditQend(eq,&ec,ma);  

  wsprintf(mews,percent_t,"���:");
  ConstructEditControl(&ec,1,0x40,mews,256);
  AddEditControlToEditQend(eq,&ec,ma);  

  wsprintf(mews,percent_t,USERNAME);
  ConstructEditControl(&ec,3,0x40,mews,80);     // 4
  AddEditControlToEditQend(eq,&ec,ma);  
  
  wsprintf(mews,percent_t,"�������� ���������:");
  ConstructEditControl(&ec,1,0x40,mews,256);      
  AddEditControlToEditQend(eq,&ec,ma);  

  wsprintf(mews,"20");
  ConstructEditControl(&ec,5,0x40,mews,2);    //6
  AddEditControlToEditQend(eq,&ec,ma);  
  
  patch_input(&med1_desc);
  patch_header(&med1_hdr);
  CreateInputTextDialog(&med1_desc,&med1_hdr,eq,1,0);
}



