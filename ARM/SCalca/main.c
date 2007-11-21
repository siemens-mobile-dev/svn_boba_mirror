#include "../inc/swilib.h"
#include <math.h>

#define PI_CONST 3.141592653589793238

typedef struct
{
  CSM_RAM csm;
  int gui_id;
}MAIN_CSM;

typedef struct
{
  GUI gui;
  int state;
  WSHDR *ws1;
  void *calc_gui;
}CHTYPE_GUI;

volatile int calc_gui_id;
volatile int chtype_gui_id;

extern double strtod(const char *_S, char **_Endptr);

extern void kill_data(void *p, void (*func_p)(void *));

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

#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}

WSHDR *ews;
const char empty_str[]="";
const char percent_t[]="%t";

double d_answer=0;

char operation[256];
int op_len=0;
int op_pos=0;

void Killer(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}

const char *keydesc[4][12]=
{
  { "1"    ,"2"   ,"3",
    "4"    ,"5"   ,"6",
    "7"    ,"8"   ,"9",
    "."    ,"0"   ,"#"},
    
  { "sin"  ,"*"   ,"/",
    "cos"  ,"+"   ,"-",
    "tan"  ,"("   ,")",
    "^2"   ,"sqrt"   ,"#"},
  
  { "asin" ,"ln"  ,"log",
    "acos" ,"e^"  ,"-",
    "atan" ,"10^" ,"pi",
    "^"    ,""    ,"#"},
  
  { "sh"   ,"ash" ,"ANS",
    "ch"   ,"ach" ,"abs",
    "th"   ,"ath" ,"!",
    "X"    ,"Y"   ,"#"}
  
};

int GetOperIndexByKey(int key)
{
  if (key>='1'&&key<='9')  return (key-'1');
  if (key=='0')   return (10);
  if (key=='*')   return (9);
  return (11);
}

void insert_operation(int op)
{
  if (op_len+1<=sizeof(operation))
  {
    for(int i=op_len; i>=op_pos; i--)
    {
      operation[i+1]=operation[i];
    }
    operation[op_pos++]=op;
    op_len++;    
  } 
}

void remove_operation(void)
{
  if (op_len && op_pos)
  {
    if (op_pos<=op_len)
    {
      int i=op_pos;
      while(i<op_len)
      {
        operation[i-1]=operation[i];
        i++;
      }
      op_pos--;
      op_len--;
    }    
  }  
}

int getXXXXwidth(int font)
{ 
  return (GetSymbolWidth('X',font)*4);
}

typedef struct {
  void *next;
  int lex;
  double value;
} NUM_STACK;

typedef struct
{
  void *next;
  double value;
} VALUES_STACK;


#pragma inline
int IsCharNumber(int c)
{
  return ((c>=0 && c<=10) || c==29);
}

#pragma inline
int GetCharByIndex(int c)
{
  return (c==29?'-':c==9?'.':c==10?'0':c+'1');
}

#pragma inline 
int IsMathFunc(int c)
{
  return (c==12 || c==15 || c==18 || c==21 || c==22 || c==24 ||
          c==25 || c==26 || c==27 || c==28 || c==30 || c==31 || c==33 || c==41);
}


/* Функция PRIOR возвpащает пpиоpитет аpифм. опеpации */

int PRIOR(int a)
{
  if (IsMathFunc(a)) return 4;
  switch(a)
  {    
  case 13:   // *
  case 14:   // /
    return 3;
    
  case 16:  // +
  case 17:  // -
    return 2;
    
  case 19:  // (
         return 1;
  }
  return (0);
}

void PushDoubleStack(double **stack, double value, int *size)
{
  int st_size=*size;
  double *st=*stack=realloc(*stack,(st_size+1)*(sizeof(double)));  
  st[st_size]=value;
  *size=st_size+1;
}

double PopDoubleStack(double **stack, int *size)
{
  int st_size=*size;
  double *st=*stack;
  double a=0;
  if (st_size)
  {
    a=st[st_size-1];
    *size=st_size-1;
  }
  return a;  
}

void ParseOperation(double **stack, int *size, int operation)
{
  double a, b, ans;
  switch (operation)
  {
  case 12:    // sin
    a=PopDoubleStack(stack, size);
    ans=sin(a*PI_CONST/180);
    PushDoubleStack(stack, ans, size);
    break;
  case 13:    // *
    a=PopDoubleStack(stack, size);
    b=PopDoubleStack(stack, size);
    ans=b*a;
    PushDoubleStack(stack, ans, size);
    break;
  case 14:    // /
    a=PopDoubleStack(stack, size);
    b=PopDoubleStack(stack, size);
    ans=b/a;
    PushDoubleStack(stack, ans, size);
    break;
  case 15:    // cos
    a=PopDoubleStack(stack, size);
    ans=cos(a*PI_CONST/180);
    PushDoubleStack(stack, ans, size);
    break;
  case 16:    // +
    a=PopDoubleStack(stack, size);
    b=PopDoubleStack(stack, size);
    ans=b+a;
    PushDoubleStack(stack, ans, size);
    break;    
  case 17:    // -
    a=PopDoubleStack(stack, size);
    b=PopDoubleStack(stack, size);
    ans=b-a;
    PushDoubleStack(stack, ans, size);
    break;
  case 18:    // tan
    a=PopDoubleStack(stack, size);
    ans=tan(a*PI_CONST/180);
    PushDoubleStack(stack, ans, size);
    break;
  case 21:    // ^2
    a=PopDoubleStack(stack, size);
    ans=pow(a, 2);
    PushDoubleStack(stack, ans, size);
    break;
  case 22:    // sqrt
    a=PopDoubleStack(stack, size);
    ans=sqrt(a);
    PushDoubleStack(stack, ans, size);
    break;
  case 24:    // asin
    a=PopDoubleStack(stack, size);
    ans=asin(a);
    PushDoubleStack(stack, ans, size);
    break;
  case 25:   // ln
    a=PopDoubleStack(stack, size);
    ans=log(a);
    PushDoubleStack(stack, ans, size);
    break;
  case 26:   // log 10
    a=PopDoubleStack(stack, size);
    ans=log10(a);
    PushDoubleStack(stack, ans, size);
    break;
  case 27:   // acos
    a=PopDoubleStack(stack, size);
    ans=acos(a);
    PushDoubleStack(stack, ans, size);
    break;
  case 28:    // e^
    a=PopDoubleStack(stack, size);
    ans=exp(a);
    PushDoubleStack(stack, ans, size);
    break;
  case 30:    // atan
    a=PopDoubleStack(stack, size);
    ans=atan(a);
    PushDoubleStack(stack, ans, size);
    break;
  case 31:    // 10^
    a=PopDoubleStack(stack, size);
    ans=pow(10, a);
    PushDoubleStack(stack, ans, size);
    break;
  case 33:     // ^
    a=PopDoubleStack(stack, size);
    b=PopDoubleStack(stack, size);
    ans=pow(b, a);
    PushDoubleStack(stack, ans, size);
    break;
  case 41:    // abs
    a=PopDoubleStack(stack, size);
    ans=fabs(a);
    PushDoubleStack(stack, ans, size);
    break;
  }
}

void calc_answer()
{
  char *s;
  int c;
  int i=op_len;
  char stack[256];
  int stack_depth=0;
  s=operation;
  double *double_stack=NULL;
  int double_sp=0;
  char *d,value[128];
  double ans=0;
  
  while(i)
  {
    c=*s++;
    if (IsCharNumber(c))
    {
      double a;
      d=value;
      do {
        *d++=GetCharByIndex(c);
        i--;
        c=*s++;
      } while (IsCharNumber(c) && i);
      *d=0;
      a=strtod(value, NULL);
      PushDoubleStack(&double_stack,a, &double_sp);
      if (!i) break;
    }
    if (c==32)  // пи
    {
      double a=PI_CONST;
      PushDoubleStack(&double_stack,a, &double_sp);
    }
    else if (c==38)   // ANS
    {
      PushDoubleStack(&double_stack, d_answer, &double_sp);
    }
    else if (c==20)   // Закрывающаяся скобка
    {
      while(stack[stack_depth-1]!=19)
      {
        ParseOperation(&double_stack, &double_sp, stack[--stack_depth]);
      }
      stack_depth--;  // удаляем саму открывающуюся скобку
    }
    else if (c==19)  // Открывающаяся скобка
    {
      stack[stack_depth++]=c;
    }
    else if (c==13 || c==14 || c==16 || c==17 || IsMathFunc(c))  // Если знак операции
    {
      if (!stack_depth)  // Если стек пуст
      {
        stack[stack_depth++]=c;
      }
      else if (PRIOR(c)>PRIOR(stack[stack_depth-1]))  // Если приоритет поступившей операции больше того что на вершине стека
      {
        stack[stack_depth++]=c;
      }
      else
      {
        while(stack_depth && (PRIOR(stack[stack_depth-1])>=PRIOR(c)))
        {
          ParseOperation(&double_stack, &double_sp, stack[--stack_depth]);
        }
        stack[stack_depth++]=c;
      }
    }
    i--;
  }
  while(stack_depth)
  {
    ParseOperation(&double_stack, &double_sp, stack[--stack_depth]);
  }
  ans=PopDoubleStack(&double_stack, &double_sp);
  mfree(double_stack);
  d_answer=ans;
}
  
void method0(CHTYPE_GUI *data)
{
  unsigned int scr_w=ScreenW();
  unsigned int scr_h=ScreenH();
  unsigned int need_height;
  unsigned int need_width;
  unsigned int start_y;
  unsigned int start_x;
  unsigned int XXXXwidth=getXXXXwidth(FONT_SMALL);
  unsigned int Ysize=GetFontYSIZE(FONT_SMALL);
  
  need_height=Ysize*4+5*2*3+5*2;
  need_width=XXXXwidth*3+5*2*2+5*2;
  
  start_y=need_height<scr_h?(scr_h-need_height)>>1:0;
  start_x=need_width<scr_w?(scr_w-need_width)>>1:0;
  
  DrawRoundedFrame(start_x,start_y,start_x+need_width,start_y+need_height,3,3,0,
                   GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(20));
  
  for (int y=0; y<4; y++)
  {
    for (int x=0; x<3; x++)
    {
      unsigned int x_frame=start_x+XXXXwidth*x+5*2*x;
      unsigned int y_frame=start_y+Ysize*y+5*2*y;
      
      
      wsprintf(data->ws1,percent_t,keydesc[data->state][y*3+x]);
      
      unsigned int str_width=Get_WS_width(data->ws1,FONT_SMALL);
      unsigned int x_str=x_frame+((XXXXwidth-str_width)>>1)+5;
      
      DrawRoundedFrame(x_frame,y_frame,x_frame+XXXXwidth+5*2,y_frame+Ysize+5*2,3,3,0,
                       GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(20));
      DrawString(data->ws1,x_str,y_frame+5,x_str+str_width,y_frame+Ysize+5,
                 FONT_SMALL,0,GetPaletteAdrByColorIndex(0),GetPaletteAdrByColorIndex(23));
    }
  }

}

void method1(CHTYPE_GUI *data,void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(128);
  data->gui.state=1;
}

void method2(CHTYPE_GUI *data,void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  data->gui.state=0;
}

void method3(CHTYPE_GUI *data,void *(*malloc_adr)(int),void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void method4(CHTYPE_GUI *data,void (*mfree_adr)(void *))
{
  if (data->gui.state!=2)
    return;
  data->gui.state=1;
}


int method5(CHTYPE_GUI *data,GUI_MSG *msg)
{
  int i=msg->gbsmsg->submess;
  DirectRedrawGUI();
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    if (((i>='0')&&(i<='9'))||(i=='*'))
    {
      insert_operation(GetOperIndexByKey(i)+((data->state)*12));
      return (1);
    }
    if (i=='#')
    {
      if (++data->state>3) return (1);
      return (-1);
    }
    if (i==RIGHT_SOFT)
    {
      return (1);
    }
  }
  return(0);
}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)method0,	//Redraw
  (void *)method1,	//Create
  (void *)method2,	//Close
  (void *)method3,	//Focus
  (void *)method4,	//Unfocus
  (void *)method5,	//OnKey
  0,
  (void *)kill_data, //method7,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};


RECT Canvas={0,0,0,0};
void OpenChangeTypeGui(void *calc_gui)
{
  CHTYPE_GUI *chtype_gui=malloc(sizeof(CHTYPE_GUI));
  zeromem(chtype_gui,sizeof(CHTYPE_GUI));
  patch_rect((RECT*)&Canvas,0,0,ScreenW()-1,ScreenH()-1);
  chtype_gui->gui.canvas=(void *)(&Canvas);
  chtype_gui->gui.flag30=2;
  chtype_gui->gui.methods=(void *)gui_methods;
  chtype_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  chtype_gui->calc_gui=calc_gui;
  chtype_gui->state=1;
  chtype_gui_id=CreateGUI(chtype_gui);
}


void ed1_locret(void){}


int ed1_onkey(GUI *data, GUI_MSG *msg)
{
  int focus;
  int i;
  i=msg->gbsmsg->submess;
  
  focus=EDIT_GetFocus(data);
  if (focus==4)
  {
    if (msg->keys==0xFFE)
    {
      remove_operation();
      return (-1);    
    }
    
    if (msg->gbsmsg->msg==KEY_DOWN)
    {
      if (((i>='0')&&(i<='9'))||(i=='*'))
      {
        insert_operation(GetOperIndexByKey(i));
        return(-1);
      }
      if (i=='#')
      {
        if (IsGuiOnTop(calc_gui_id))
        {
          if(!chtype_gui_id)
          {
            OpenChangeTypeGui(data);
            return(-1);
          }
        }
      } 
    }
    
    if (msg->gbsmsg->msg==KEY_DOWN || msg->gbsmsg->msg==LONG_PRESS)
    {
      if (i==LEFT_BUTTON)
      {
        if (op_pos) op_pos--;
        return(-1);
      }
      if (i==RIGHT_BUTTON)
      {
        if (op_pos<op_len) op_pos++;
        return(-1);
      }
    }
  }
  
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    if (i==GREEN_BUTTON) 
    {
      char revpn[256];
      calc_answer();
      sprintf(revpn, "%1.15lg", d_answer);
      wsprintf(ews, revpn);
      EDIT_SetTextToEditControl(data,2,ews);
      return(-1);
    }
  }
  return(0);
}

void ed1_ghook(GUI *data, int cmd)
{
  char *s;
  int c;
  int i;
  static SOFTKEY_DESC sk={0x0FFF,0x0000,(int)"Options"};
  static SOFTKEY_DESC sk_del={0x0FFE,0x0FFE,(int)"< C"};
  if (cmd==2)
  {
    op_len=0;    
  }
  if (cmd==7)
  {
    i=EDIT_GetFocus(data);
    if (i==4)
    {
      int k=0;
      SetSoftKey(data,&sk,SET_SOFT_KEY_N);
      CutWSTR(ews,0);
      if (op_len)
      {
        SetSoftKey(data,&sk_del,SET_SOFT_KEY_N==1?0:1);
        s=operation;
        i=0;
        while (i<op_len)
        {
          const char *op;
          c=*s++;
          op=keydesc[0][c];
          while(c=*op++)
          {
            wsAppendChar(ews,c);
            if (i<op_pos) k++;
          }
          i++;
        }
      }
      EDIT_SetTextToEditControl(data,4,ews);
      EDIT_SetCursorPos(data,k+1);
    }
  }
  if (cmd==0x0A)
  {
    DisableIDLETMR();
  }
}

HEADER_DESC ed1_hdr={0,0,0,0,NULL,(int)"Calculator",LGP_NULL};

SOFTKEY_DESC menu_sk[] =
{
  {0x0018, 0x0000, (int)"Options"},
  {0x0001, 0x0000, (int)"Close"},
  {0x003D, 0x0000, (int)LGP_DOIT_PIC}
};

SOFTKEYSTAB menu_skt =
{
  menu_sk, 0
};

INPUTDIA_DESC ed1_desc=
{
  1,
  ed1_onkey,
  ed1_ghook,
  (void *)ed1_locret,
  0,
  &menu_skt,
  {0,0,0,0},
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
  0x40000000 // Поменять местами софт-кнопки
};


int create_ed(void)
{
  void *ma=malloc_adr();
  void *eq;
  EDITCONTROL ec;

  PrepareEditControl(&ec);
  eq=AllocEQueue(ma,mfree_adr());
  
  wsprintf(ews,percent_t,"Answer:");
  ConstructEditControl(&ec,ECT_HEADER,0x40,ews,ews->wsbody[0]);
  AddEditControlToEditQend(eq,&ec,ma);  // 1
  
  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,0,128);
  AddEditControlToEditQend(eq,&ec,ma);  // 2   Ответ 
  
  wsprintf(ews,percent_t,"----------");
  ConstructEditControl(&ec,ECT_HEADER,0x40,ews,ews->wsbody[0]);
  AddEditControlToEditQend(eq,&ec,ma);   // 3

  ConstructEditControl(&ec,ECT_NORMAL_TEXT,0x40,0,128);
  AddEditControlToEditQend(eq,&ec,ma);  // 4 

  patch_header(&ed1_hdr);
  patch_input(&ed1_desc);
  return CreateInputTextDialog(&ed1_desc,&ed1_hdr,eq,1,0);
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
    if ((int)msg->data0==chtype_gui_id)
    {
      chtype_gui_id=0;
    }
  }
  return(1);
}

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  ews=AllocWS(256);
  csm->gui_id=calc_gui_id=create_ed();
}

void maincsm_onclose(CSM_RAM *csm)
{
  FreeWS(ews);
  SUBPROC((void *)Killer);
}

const int minus11=-11;

unsigned short maincsm_name_body[20];

const struct
{
  CSM_DESC maincsm;
  WSHDR maincsm_name;
}MAINCSM =
{
  {
    maincsm_onmessage,
    maincsm_oncreate,
#ifdef NEWSGOLD
0,
0,
0,
0,
#endif
maincsm_onclose,
sizeof(MAIN_CSM),
1,
&minus11
  },
  {
    maincsm_name_body,
    NAMECSM_MAGIC1,
    NAMECSM_MAGIC2,
    0x0,
    139
  }
};

void UpdateCSMname(void)
{
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name), "SCalca");
}


int main(const char *elf_name, const char *fname)
{
  MAIN_CSM csm;
  UpdateCSMname();
  LockSched();
  CreateCSM(&MAINCSM.maincsm,&csm,0);
  UnlockSched();
  return 0;
}
