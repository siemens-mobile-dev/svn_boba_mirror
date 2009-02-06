#include "../inc/swilib.h" 
#include "conf_loader.h"

extern const int b_welcome_message;
extern const char s_welcome_message[128];
extern const int i_questions;

WSHDR* run_elf;
int is_safe;
char app_path[512];
char app_exe[512];
char Message[128];
unsigned int err;
int in;
    
void ShowWelcome()
{
  if (is_safe == 1)
  {
    if (b_welcome_message == 1)
      {
        ShowMSG(1, (int)s_welcome_message);
      }
  } 
}

void run(int res)
{
  if (!res)
  {
    ExecuteFile(run_elf,0,0); 
  }
}

void runall(int res)
{
  if(!res)
  {
    if (isdir(app_path, &err) == 1)
    {
      sprintf(app_path, "%s%s", app_path, "*.elf");
      DIR_ENTRY dE;
      WSHDR*wS;
      wS=AllocWS(196);
      if (FindFirstFile(&dE,app_path,&err))
      {
        do
        {
          wsprintf(wS,"%s\\%s",dE.folder_name,dE.file_name);
          if ((is_safe == 0) | ((is_safe == 1) & (i_questions == 1)))
          {
            run_elf = wS;  
            run(0);
          }
          if ((is_safe == 1) & (i_questions == 2))
          {
              run_elf=AllocWS(64);
              str_2ws(run_elf,app_exe,strlen(app_exe)+1);  
              sprintf(Message, "%s\\%s", dE.folder_name,dE.file_name);
              ExecuteFile(run_elf, 0, Message);
              FreeWS(run_elf);
          }
        }while(FindNextFile(&dE,&err));
      }
      FindClose(&dE,&err);
      FreeWS(wS);
    }
    else
    {
      sprintf(Message, "%s%s", app_path, " not found!");
      ShowMSG(1, (int)Message);
    }
  }
  ShowWelcome();
}

int main(const char *exename, const char *filename)
{
  char *path=strrchr(exename,'\\');
  int l;
  if (!path) return 0; //Фигня какая-то
  path++;
  l=path-exename;
  memcpy(app_path,exename,l);
  strcpy(app_exe, exename);
  InitConfig();
  unsigned int err;
  int f;
  if(f=fopen(filename,A_ReadOnly+A_BIN,P_READ,&err)!=-1)
  {
    fclose(f,&err);
    sprintf(Message, "%s%s", "Run ", filename, "?");
    run_elf = AllocWS(64);
    str_2ws(run_elf,filename,strlen(filename)+1);
    MsgBoxYesNo(1, (int)Message, run);
  }
  else
  {
    if (*RamPressedKey()== '#')
    {
      is_safe = 1; 
    }
    else
    {
      is_safe = 0;
    }
    sprintf(app_path, "%s%s", app_path, "Deamons\\");
    if ((is_safe == 1) & (i_questions == 0))
    {
      ShowWelcome();
      return 0;
    }
    if ((is_safe == 1) & (i_questions == 1)) 
    {
      MsgBoxYesNo(1, (int)"Load deamons?", runall);
    }
    if ((is_safe == 0) | ((is_safe == 1) & (i_questions == 2)))
    {
      runall(0);
    }
  }
  return 0;
}
