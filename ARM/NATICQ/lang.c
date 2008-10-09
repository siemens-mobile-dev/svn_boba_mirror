#include "..\inc\swilib.h"
#include "lang.h"


char * lgpData[LGP_DATA_NUM];
int lgpLoaded;
int loaded;

void lgpInitLangPack()
{
  PreFreeLangPack();
  
  int hFile;
  unsigned int io_error = 0;
  char * buf; // Буфер под файл
  char line[128]; // Текущая строка
  int line_size = 0; // Длина текущей строки
  int cur_id = 0; // Текущий LGP_ID
  int buf_pos = 0; // Позиция в буфере
  FSTATS fstat;
  
  char lang_file[128];
  extern const char TEMPLATES_PATH[64];
  strcpy(lang_file,TEMPLATES_PATH);
  strcat(lang_file,"\\lang.txt");

  if (GetFileStats(lang_file, &fstat, &io_error)!=-1) // Получаем размер файла
  {
    if((hFile=fopen(lang_file, A_ReadOnly + A_BIN, P_READ, &io_error))!=-1) // Открываем файл для чтения
    {
      if (buf =(char *)malloc(fstat.size + 1))// Выделяем память под буфер
      {
        buf[fread(hFile, buf, fstat.size, &io_error)]=0; // Читаем файл в буфер

        while(buf[buf_pos] && buf_pos < fstat.size && cur_id < LGP_DATA_NUM) // Пока не конец файла и не заполнены все поля массива
        {
          if (buf[buf_pos]=='\n' || buf[buf_pos]=='\r') // Если конец строки
          {
            if (line_size) // Если длина строки > 0
            {
              lgpData[cur_id] = (char *)malloc(line_size+1);
              memcpy(lgpData[cur_id], line, line_size); 
              lgpData[cur_id][line_size] = NULL; // Конец строки
              cur_id ++;
              line_size = NULL;
            }
          }
          else
            line[line_size++]=buf[buf_pos]; 
          buf_pos++;
        }
        if (line_size && cur_id < LGP_DATA_NUM)
        {
          lgpData[cur_id] = (char *)malloc(line_size+1);
          memcpy(lgpData[cur_id], line, line_size);
          lgpData[cur_id][line_size] = NULL;
          cur_id ++;
          line_size = NULL;
        }
        mfree(buf); 
        loaded = 1;
        fclose(hFile, &io_error);
        lgpUpdateClHdr(); 
        for (int i = 0; i < LGP_DATA_NUM; i ++)
        {
          if (!lgpData[i])
          {
            lgpData[i] = (char *)malloc(32);
            strcpy(lgpData[i], "Error! Update lang.txt!");
          }
        }
        return;
      }
      fclose(hFile, &io_error);
    }
  }
  

  lgpData[LGP_AlreadyStarted]=        "Already started!";
  lgpData[LGP_ClTitle]=               "Contacts...";
  lgpData[LGP_ClT9Inp]=               "T9 Key: ";
  lgpData[LGP_ClError]=               "error";
  lgpData[LGP_ClLoopback]=            "Loopback";
  lgpData[LGP_GroupNotInList]=        "Not in list";
  
  //=================
  lgpData[LGP_Menu]=                  "Menu";
  lgpData[LGP_Options]=               "Options";
  lgpData[LGP_Close]=                 "Close";
  lgpData[LGP_Clear]=                 "< C";
  lgpData[LGP_DoIt]=                  "Do it!";
  lgpData[LGP_PasteSm]=               "Paste it!";
  //-----------------
  lgpData[LGP_Select]=                "Select";
  lgpData[LGP_Text]=                  "Text";
  lgpData[LGP_Back]=                  "Back";
  //-----------------
  lgpData[LGP_Add]=                   "Add!";
  lgpData[LGP_AddCnt]=                "Add Contact";
  lgpData[LGP_EnterUin]=              "Please enter uin:";
  //=================
  lgpData[LGP_AddRen]=                "Add/Rename";
  //lgpData[LGP_SetNick]=             Не стал добавлять, так как строчки с %
  //lgpData[LGP_SmlDesc]=
  lgpData[LGP_AddSmil]=               "Add Smiles";
  //=================
  lgpData[LGP_AddCont]=               "Add contact...";
  lgpData[LGP_AuthReq]=               "Please authorize me...";
  lgpData[LGP_AuthGrant]=             "You are authorized!";
  //=================
  lgpData[LGP_MnuStatus]=             "Status";
  lgpData[LGP_MnuXStatus]=            "X-Status";
  lgpData[LGP_MnuAddCont]=            "Add Contact";
  lgpData[LGP_MnuVibra]=              "Vibra mode";
  lgpData[LGP_MnuSound]=              "Sound mode";
  lgpData[LGP_MnuShowOff]=            "Off. contacts";
  lgpData[LGP_MnuShowGroup]=          "Groups";
  lgpData[LGP_MnuEdCfg]=              "Edit config";
  lgpData[LGP_MnuDisconnect]=         "Disconnect";
  lgpData[LGP_MnuPing]=               "Ping";
  lgpData[LGP_MnuAbout]=              "About";
  //=================
  lgpData[LGP_MnuQuote]=              "Quote";
  lgpData[LGP_MnuAddSml]=             "Add smile";
  lgpData[LGP_MnuShInfo]=             "Get short info";
  lgpData[LGP_MnuAddRen]=             "Add/rename";
  lgpData[LGP_MnuSAuthReq]=           "Send Auth Req";
  lgpData[LGP_MnuSAuthGrt]=           "Send Auth Grant";
  lgpData[LGP_MnuOpenLog]=            "Open logfile";
  lgpData[LGP_MnuClearCht]=           "Clear log";
  
  lgpData[LGP_MnuEdNextAct]=          "Next active";
  lgpData[LGP_MnuEdPrevAct]=          "Prev. active";
  //=================
  lgpData[LGP_ChgStatus]=             "Change Status";
  lgpData[LGP_ChgXStatus]=            "Change X-Status";
  
  lgpData[LGP_StOnline]=              "Online";
  lgpData[LGP_StAway]=                "Away";
  lgpData[LGP_StNa]=                  "Not Available";
  lgpData[LGP_StDnd]=                 "Do Not Disturb";
  lgpData[LGP_StOccup]=               "Occupied";
  lgpData[LGP_StFfc]=                 "Free For Chat";
  lgpData[LGP_StInvis]=               "Invisible";
  lgpData[LGP_StDepression]=          "Depression";
  lgpData[LGP_StEvil]=                "Evil";
  lgpData[LGP_StHome]=                "Home";
  lgpData[LGP_StLunch]=               "Lunch";
  lgpData[LGP_StWork]=                "Work";
  
  lgpData[LGP_Comment]=               "Comment:";
  //=================
  
  lgpData[LGP_All_Can_See]=           "Visible for all";
  lgpData[LGP_Vislist_Can_See]=       "Visible only for list seeing";
  lgpData[LGP_Invislist_Cannot_See]=  "Visible for all, except for list blind";
  lgpData[LGP_ContactList_Can_See]=   "Visible only for list of contacts";
  lgpData[LGP_Nobody_Can_See]=        "Invisible for all";
  
  lgpData[LGP_EnterName]=             "Enter a name";
  lgpData[LGP_ChooseGroup]=           "Choose a group";
  lgpData[LGP_PrivStatus]=            "Private status";
  lgpData[LGP_ManageList]=            "Management by list";
  
  //=================
  
  lgpData[LGP_MsgIllegmsgcon]=        "Illegal message ENIP_SOCK_CONNECTED!";
  lgpData[LGP_MsgIllegmsgrea]=        "Illegal message ENIP_DATA_READ";
  lgpData[LGP_MsgCantConn]=           "Can't connect!";
  lgpData[LGP_MsgCantCrSc]=           "Can't create socket, GPRS restarted!";
  lgpData[LGP_MsgHostNFnd]=           "Host not found!";
  lgpData[LGP_MsgVibraDis]=           "Vibrа disabled!";
  lgpData[LGP_MsgVibraEna]=           "Vibra enabled!";
  lgpData[LGP_MsgSndDis]=             "Sounds disabled!";
  lgpData[LGP_MsgSndEna]=             "Sounds enabled!";
  lgpData[LGP_MsgNoUinPass]=          "Please setup UIN/PASS!";
  lgpData[LGP_MsgSmileNotFnd]=        "Can't find smiles!";
  
  //=================
  
  lgpData[LGP_GrsKeyExit]=            "Exit";
  lgpData[LGP_GrsKeyClist]=           "Clist";
  
  lgpData[LGP_LangCode]=              "en";
  
  lgpUpdateClHdr();
}

void lgpFreeLangPack(void)
{
  if (!lgpLoaded) return;
  for (int i = 0; i < LGP_DATA_NUM; i ++)
  {
    if (lgpData[i])
      mfree(lgpData[i]);
  }
}

void PreFreeLangPack()
{
  for (int i = 0; i < LGP_DATA_NUM; i ++)
    lgpData[i] = NULL;
  loaded = NULL;
}


