#include "..\inc\swilib.h"
#include "mc.h"
#include "mui.h"

char* yesno[2];

int fa_attr;
int fa_ro, fa_hd, fa_sy, fa_ar, fa_sb;

int empty_onkey(GUI *gui, GUI_MSG *msg){return(0);}

int file_prop_onkey(GUI *gui, GUI_MSG *msg)
{
  if (msg->keys==26)
  {
   if(pathbuf)
   {
     fa_attr=0;
     if(fa_ro) fa_attr|=FA_READONLY;
     if(fa_hd) fa_attr|=FA_HIDDEN;
     if(fa_sy) fa_attr|=FA_SYSTEM;
     if(fa_ar) fa_attr|=FA_ARCHIVE;

     int res = 1;
     EnumSel(M_SetAttr, (int) &res);

     if(!res)
       MsgBoxError(1, (int) muitxt(ind_err_attrset));
     else
       DoRefresh();
    } 
  }
  int i=EDIT_GetFocus(gui)-1;
  int key=msg->gbsmsg->submess;
  if(i==1 && ((key>='0' && key<='9') || key=='#' /*|| key=='*' ||
#ifdef NEWSGOLD
    key==RIGHT_SOFT
#else
    key==LEFT_SOFT
#endif*/
  )) return -1;
  return 0; //Do standart keys
}

void empty_ghook(GUI *gui, int cmd){};

void file_prop_ghook(GUI *gui, int cmd)
{
  EDITCONTROL ec;
  if (cmd==7)
  {
    int i=EDIT_GetFocus(gui)-1;
    if(i==8 || i==10 || i==12 || i==14 || i==16)
    {
      int j=EDIT_GetItemNumInFocusedComboBox(gui)-1;
      if(i== 8) fa_ro=j!=0?1:0;
      if(i==10) fa_hd=j!=0?1:0;
      if(i==12) fa_sy=j!=0?1:0;
      if(i==14) fa_ar=j!=0?1:0;
      if(i==16) fa_sb=j!=0?1:0;
    }
  }
  else
  if (cmd==0x0D)
  {
   int j;
   int i=EDIT_GetFocus(gui);
   WSHDR *buff=AllocWS(64);
   ExtractEditControl(gui,i,&ec);
   if ((j=EDIT_GetItemNumInFocusedComboBox(gui)))
   {
     wsprintf(buff,_t,yesno[j-1]);
   }
   else
   {
     ExtractEditControl(gui,EDIT_GetFocus(gui)-1,&ec);
     wstrcpy(buff,ec.pWS);
   }
   EDIT_SetTextToFocused(gui,buff);
   FreeWS(buff);
  }
}

void empty_locret(void){}

SOFTKEY_DESC empty_menu_sk[]={{0,0,0}};

SOFTKEYSTAB empty_menu_skt=
{
  empty_menu_sk,0
};

HEADER_DESC file_prop_hdr={0,0,0,0,NULL,0,LGP_NULL};
INPUTDIA_DESC file_prop_desc=
{
  1,
  file_prop_onkey,
  file_prop_ghook,
  (void *)empty_locret,
  0,
  &empty_menu_skt,
  {0,0,0,0},
  FONT_SMALL,
  100,
  101,
  0,
  0,
  0x40000000
};

void prep_hd(EDITCONTROL* ec)
{
 ec->ed_options.bitmask = 2|8;
 ec->ed_options.font = 2;
 ec->ed_options.pen[3] = 100;
 ec->ed_options.pen[2] = 0x80;
} 

void prep_inf(EDITCONTROL* ec)
{
 ec->ed_options.bitmask = 2;
 ec->ed_options.font = 1;
} 
  
void _FileProp(FILEINF *file)
{
  if(!pathbuf || !file)return;
  
  file_prop_hdr.lgp_id = (int) muitxt(ind_prop);

  char* sbuff=malloc(128);
  void *ma=malloc_adr();
  void *eq=AllocEQueue(ma,mfree_adr());
  int f;
  EDITCONTROL ec;
  PrepareEditControl(&ec);

  WSHDR *buff=AllocWS(MAX_PATH);

  
//Full Path
  wsprintf(buff, _tc, muitxt(ind_fullname));
  ConstructEditControl(&ec,ECT_HEADER,ECF_APPEND_EOL,buff,MAX_PATH);
  prep_hd(&ec);
  AddEditControlToEditQend(eq,&ec,ma); //0
  CurFullPath(file->sname);
  str_2ws(buff, pathbuf, MAX_PATH);
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,ECF_APPEND_EOL,buff,MAX_PATH);
  prep_inf(&ec);
  AddEditControlToEditQend(eq,&ec,ma); //1

//Size
  wsprintf(buff, _tc, muitxt(ind_size));
  ConstructEditControl(&ec,ECT_HEADER,ECF_APPEND_EOL,buff,MAX_PATH);
  prep_hd(&ec);
  AddEditControlToEditQend(eq,&ec,ma); //2
  if(IsMultiChk()>1 || file->attr & FA_DIRECTORY)
  {
    CHKDS_PARAM param;
    param.gsize=0;
    param.count=0;
    EnumSel(M_DirSize, (int)&param);

    sz2s(param.gsize, sbuff);
    wsprintf(buff, "%t: %t\n%t: %d\n%t: %d" , muitxt(ind_global), sbuff, muitxt(ind_folders), param.count>>16, muitxt(ind_files), param.count & 0xffff);
  } 
  else
  {
/*    sz2s(file->size, sbuff);
    wsprintf(buff, _t, sbuff);*/
    wsprintf(buff, _d_t, file->size, muitxt(ind_byte));
  }
  ConstructEditControl(&ec,ECT_READ_ONLY,ECF_APPEND_EOL | ECF_SKIP,buff,MAX_PATH);
  prep_inf(&ec);
  AddEditControlToEditQend(eq,&ec,ma); //3
  
//Date
  wsprintf(buff, _tc, muitxt(ind_date));
  ConstructEditControl(&ec,ECT_HEADER,ECF_APPEND_EOL,buff,MAX_PATH);
  prep_hd(&ec);
  AddEditControlToEditQend(eq,&ec,ma); //4
  fdt2s(file->time, sbuff);
  str_2ws(buff, sbuff, 16);
  ConstructEditControl(&ec,ECT_READ_ONLY,ECF_APPEND_EOL | ECF_SKIP,buff,MAX_PATH);
  prep_inf(&ec);
  AddEditControlToEditQend(eq,&ec,ma); //5
  
//Attr
  wsprintf(buff, _tc, muitxt(ind_attr));
  ConstructEditControl(&ec,ECT_HEADER,ECF_APPEND_EOL,buff,MAX_PATH);
  prep_hd(&ec);
  AddEditControlToEditQend(eq,&ec,ma); //6
  {
   wsprintf(buff, _tc, muitxt(ind_readonly));
   ConstructEditControl(&ec,ECT_READ_ONLY,ECF_SKIP,buff,MAX_PATH);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //7
   f=fa_ro=file->attr & FA_READONLY?1:0;
   wsprintf(buff,_t,yesno[f]);
   ConstructComboBox(&ec,ECT_COMBO_BOX,ECF_APPEND_EOL,buff,32,0,2,f+1);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //8*
  }
  {
   wsprintf(buff, _tc, muitxt(ind_hidden));
   ConstructEditControl(&ec,ECT_READ_ONLY,ECF_SKIP,buff,MAX_PATH);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //9
   f=fa_hd=file->attr & FA_HIDDEN?1:0;
   wsprintf(buff,_t,yesno[f]);
   ConstructComboBox(&ec,ECT_COMBO_BOX,ECF_APPEND_EOL,buff,32,0,2,f+1);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //10*
  }
  {
   wsprintf(buff, _tc, muitxt(ind_system));
   ConstructEditControl(&ec,ECT_READ_ONLY,ECF_SKIP,buff,MAX_PATH);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //11
   f=fa_sy=file->attr & FA_SYSTEM?1:0;
   wsprintf(buff,_t,yesno[f]);
   ConstructComboBox(&ec,ECT_COMBO_BOX,ECF_APPEND_EOL,buff,32,0,2,f+1);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //12*
  }
  {
   wsprintf(buff, _tc, muitxt(ind_arh));
   ConstructEditControl(&ec,ECT_READ_ONLY,ECF_SKIP,buff,MAX_PATH);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //13
   f=fa_ar=file->attr & FA_ARCHIVE?1:0;
   wsprintf(buff,_t,yesno[f]);
   ConstructComboBox(&ec,ECT_COMBO_BOX,ECF_APPEND_EOL,buff,32,0,2,f+1);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //14*
  }
  if(file->attr & FA_DIRECTORY)
  {
   wsprintf(buff, _tc, muitxt(ind_subdirs));
   ConstructEditControl(&ec,ECT_HEADER,ECF_NORMAL_STR | ECF_APPEND_EOL,buff,MAX_PATH);
   prep_hd(&ec);
   prep_inf(&ec);
   ec.ed_options.bitmask=ec.ed_options.bitmask | 8;
   AddEditControlToEditQend(eq,&ec,ma); //15
   f=0;
   wsprintf(buff,_t,yesno[f]);
   ConstructComboBox(&ec,ECT_COMBO_BOX,ECF_APPEND_EOL,buff,32,0,2,f+1);
   prep_inf(&ec);
   AddEditControlToEditQend(eq,&ec,ma); //16*
  }
  CreateInputTextDialog(&file_prop_desc,&file_prop_hdr,eq,1,0);

  mfree(sbuff);
  FreeWS(buff);
}
