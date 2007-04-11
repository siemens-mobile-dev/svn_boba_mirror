#include "../inc/swilib.h"
#include "../inc/cfg_items.h"
#include "..\inc\zlib.h"
#include "history.h"
#include "conf_loader.h"
#include "main.h"
#include "clist_util.h"
#include "message_list.h"
#include "xml_parser.h"
#include "jabber.h"
#include "string_util.h"
#include "jabber_util.h"
#include "roster_icons.h"
#include "mainmenu.h"
#include "serial_dbg.h"
/*
============== Нативный Jabber-клиент ==============
Возможности текущей версии:
1. Вход в сеть
2. Сообщение об этом контакт-листу :)
3. Процедуры низкого уровня: посылка iq, логина, своего присутствия
4. Запись потока данных к/от сервера (файл jlog.txt)
5. Запись разобранных порций XML-потока (файл xml_packet.txt)
6. Приём сообщений
7. Передача сообщений
8. Вход в конференцию
9. Смена своего статуса

Управление: логиниться к серверу клиент будет автоматически,
изменение учётных данных - в конфигруационном файле.

Горячие клавиши:
1/9 - перемещение в начало/конец списка контактов
2/8 - вверх/вниз по списку
0   - показать/спрятать оффлайн-пользователей
# - следующее непрочитанное сообщение
Зелёная кнопка - написать сообщение

Экран сообщений:
0 - очистка списка сообщений контакта
Зелёная кнопка - написать сообщение

Что нужно сделать в первую очередь:
 - Придумать, как обращаться с контактами не из списка контактов
 ====================================================
(c) Kibab
(r) Rst7, MasterMind, AD

*/

// ============= Учетные данные ============= 
extern const char JABBER_HOST[];
extern const unsigned int JABBER_PORT;
extern const char USERNAME[];  
extern const char PATH_TO_PIC[];  
extern const int IS_IP;
extern const int USE_SASL; 
extern const int USE_ZLIB; 

extern const unsigned int IDLE_ICON_X; 
extern const unsigned int IDLE_ICON_Y; 

const char RESOURCE[] = "SieJC";
const char VERSION_NAME[]= "Siemens Native Jabber Client";  // НЕ МЕНЯТЬ!
const char VERSION_VERS[] = "1.2.1-Z";
const char CMP_DATE[] = __DATE__;

#ifdef NEWSGOLD
#ifdef ELKA
  const char OS[] = "NewSGOLD_ELKA_ELF-Platform";
#else
  const char OS[] = "NewSGOLD_ELF-Platform";
#endif
#else
const char OS[] = "SGOLD_ELF-Platform";
#endif


char Is_Vibra_Enabled = 1;
char Is_Compression_Enabled = 0;

const char percent_t[]="%t";
const char empty_t[]="";
const char conference_t[]="conference";
const char png_t[]=".png";
char empty_str[]="";
char logmsg[512];

JABBER_STATE Jabber_state = JS_NOT_CONNECTED;
char My_Presence = PRESENCE_OFFLINE;

// Флаг необходимости завершить работу
char Quit_Required = 0;

int connect_state=0;
/*
0 = OffLine
1 = Socket Connected
2 = нормальный режим работы, получен ответ от сервера
*/

int sock=-1;

volatile int is_gprs_online=1;


GBSTMR TMR_Send_Presence; // Посылка презенса
GBSTMR reconnect_tmr;
#ifndef NEWSGOLD
  GBSTMR redraw_tmr;
#define Redraw_Time TMR_SECOND*5
#endif

extern void kill_data(void *p, void (*func_p)(void *));

void ElfKiller(void)
{
  extern void *ELF_BEGIN;
  kill_data(&ELF_BEGIN,(void (*)(void *))mfree_adr());
}
//===============================================================================================
// ELKA Compatibility

#pragma inline
void patch_rect(RECT*rc,int x,int y, int x2, int y2)
{
  rc->x=x;
  rc->y=y;
  rc->x2=x2;
  rc->y2=y2;
}


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

GBSTMR tmr_vibra;
volatile int Vibra_Count;

void _start_vibra()
{
  void _stop_vibra(void);
  if(Is_Vibra_Enabled)
  {
    SetVibration(VIBRA_POWER);
    GBS_StartTimerProc(&tmr_vibra,TMR_SECOND>>1,_stop_vibra);
  }  
}

void _stop_vibra(void)
{
  SetVibration(0);
  if (--Vibra_Count)
  {
    GBS_StartTimerProc(&tmr_vibra,TMR_SECOND>>1,_start_vibra);
  }
}

void Vibrate(int Req_Vibra_Count)
{
  if(!Is_Vibra_Enabled)
  {
    Vibra_Count=0;
    return;
  }
  if(Vibra_Count)return;
  Vibra_Count = Req_Vibra_Count;
  _start_vibra();
}

//===============================================================================================
char My_JID[128];
char My_JID_full[128];

int DNR_ID=0;
int DNR_TRIES=3;

void create_connect(void)
{
  int ***p_res=NULL;
  void do_reconnect(void);
  SOCK_ADDR sa;
  //Устанавливаем соединение
  connect_state=0;
  int can_connect=0;
  GBS_DelTimer(&reconnect_tmr);
  DNR_ID=0;
  if(!IS_IP)
  {
    snprintf(logmsg,255,"Send DNR...");
    REDRAW();
    *socklasterr()=0;
    int err=async_gethostbyname(JABBER_HOST,&p_res,&DNR_ID); //03461351 3<70<19<81
    if (err)
    {
     if ((err==0xC9)||(err==0xD6))
     {
       if (DNR_ID)
       {
  	return; //Ждем готовности DNR
       }
     }
     else
     {
       snprintf(logmsg,255,"DNR ERROR %d!",err);
       REDRAW();
       GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
       return;
     }
   }
   if(p_res) 
   {
     if(p_res[3])
     {
        snprintf(logmsg,255,"DNR Ok, connecting...");
        REDRAW(); 
        DNR_TRIES=0;
        sa.ip=p_res[3][0][0];
        can_connect = 1;
     }
   }
   else
   {
    DNR_TRIES--;
    LockSched();
    ShowMSG(1,(int)"Host not found!");
    UnlockSched();
    return;
  } 
  }// Если DNS
  else
  {
      snprintf(logmsg,255,"Using IP address...");
      can_connect = 1;
      sa.ip = str2ip(JABBER_HOST);
      REDRAW(); 
  }
  
  
  if(can_connect)
  {
    sock=socket(1,1,0);
    if (sock!=-1)
    {
        sa.family=1;
	sa.port=htons(JABBER_PORT);
	if (connect(sock,&sa,sizeof(sa))!=-1)
	{
          connect_state=1;
	  REDRAW();
	}
	else
	{
	  closesocket(sock);
	  sock=-1;
	  LockSched();
	  ShowMSG(1,(int)"Can't connect!");
	  UnlockSched();
	  GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
	}
      }
      else
      {
	LockSched();
	ShowMSG(1,(int)"Can't create socket, GPRS restarted!");
	UnlockSched();
	//Не осилили создания сокета, закрываем GPRS-сессию
	GPRS_OnOff(0,1);
      }
    }	
}

void end_socket(void)
{
  if (sock>=0)
  {
    shutdown(sock,2);
    closesocket(sock);
  }
}

char* XMLBuffer;
char *Decompr_Buffer;
unsigned int XMLBufferCurPos = 0; // Позиция в кольцевом буфере
unsigned int virt_buffer_len = 0; // Виртуальная длина принятого потока
unsigned int processed_pos   = 0; // До какого места обработали
z_stream d_stream;                // Поток для ZLib
char ZLib_Stream_Init=0;          // Признак того, что инициализирован поток сжатия
unsigned int out_bytes_count = 0; // Количество отправленных данных
// Функции-заглушки для ZLib
void* zcalloc(int unk,size_t nelem, size_t elsize)
{
  return (malloc(nelem*elsize));
}

void zcfree(int unk, void* ptr)
{
  mfree(ptr);
}

// Распаковщик потока
// Код с некоторыми изменениями взят из MidletSigner
int unzip(char *compr, unsigned int comprLen, char *uncompr, unsigned int uncomprLen)
{
  int err;
  if(!ZLib_Stream_Init)
  {
    ZLib_Stream_Init=1;
    d_stream.zalloc = (alloc_func)zcalloc;
    d_stream.zfree = (free_func)zcfree;
    d_stream.opaque = (voidpf)0;
    err = inflateInit2(&d_stream,MAX_WBITS/*-MAX_WBITS*/);
    if(err!=Z_OK)
    {
      unerr:
      return err;
    }
  }
  d_stream.next_in  = (Byte*)compr;
  d_stream.avail_in = (uInt)comprLen;
  d_stream.next_out = (Byte*)uncompr;
  d_stream.avail_out = (uInt)uncomprLen;
  err = inflate(&d_stream, /*Z_NO_FLUSH*/Z_SYNC_FLUSH);
  if(err<0) goto unerr;
  return 0;
}



void get_buf_part(char* inp_buffer, unsigned int req_len)
{
  // Посчитаем, где в буфере находится текущая позиция обработанных данных
  int xz = sdiv(XML_BUFFER_SIZE,processed_pos);
  unsigned int virt_processed_len = processed_pos - XML_BUFFER_SIZE * xz;

  if(req_len+virt_processed_len < XML_BUFFER_SIZE)
  {
    memcpy(inp_buffer, XMLBuffer+virt_processed_len, req_len);
  }
  else
  {
    memcpy(inp_buffer, XMLBuffer+virt_processed_len, (XML_BUFFER_SIZE-virt_processed_len));
    memcpy(inp_buffer + (XML_BUFFER_SIZE-virt_processed_len), XMLBuffer, req_len - (XML_BUFFER_SIZE-virt_processed_len));
  }
}

/*
Прием данных из сокета в контексте HELPER
По окончании приёма законченной порции данных
она направляется в MMI для вскрытия :)
*/
//Context:HELPER
void get_answer(void)
{
  char* buf = malloc(REC_BUFFER_SIZE);    // Выделяем буфер под приём
  zeromem(buf,REC_BUFFER_SIZE);           // Зануляем
  int rec_bytes = 0;          // Не торопимся :)
  rec_bytes = recv(sock, buf, REC_BUFFER_SIZE, 0);

  // Запись в буфер
  if(XMLBufferCurPos+rec_bytes < XML_BUFFER_SIZE) // Если пишем где-то в буфере
  {
    memcpy(XMLBuffer+XMLBufferCurPos, buf, rec_bytes);
    XMLBufferCurPos+=rec_bytes;
  }
  else // Запись переходит границы буфера
  {
    // Определяем байт, до которого содержимое буфера приёма помещается без переноса
    // в XML-буфер
    unsigned int max_byte = (XML_BUFFER_SIZE-XMLBufferCurPos);
    memcpy(XMLBuffer+XMLBufferCurPos, buf, max_byte);
    memcpy(XMLBuffer, buf+max_byte, rec_bytes - max_byte);
    XMLBufferCurPos=rec_bytes - max_byte;
  }
  
  virt_buffer_len = virt_buffer_len + rec_bytes;  // Виртуальная длина потока увеличилась

  
  if(Is_Compression_Enabled)// Если включена компрессия...
  {
    if((rec_bytes<REC_BUFFER_SIZE)) // Если принят конец пакета...
    {
      int bytecount = virt_buffer_len - processed_pos;
      char *compressed_data = malloc(bytecount); 
      get_buf_part(compressed_data, bytecount); // Получаем из буфера сжатые данные
      Decompr_Buffer = malloc(UNP_BUFFER_SIZE); // Выделяем память под распаковку
      zeromem(Decompr_Buffer, UNP_BUFFER_SIZE);
      // Распаковываем данные. 
      int err=unzip(compressed_data,bytecount,Decompr_Buffer,UNP_BUFFER_SIZE);
      if(err!=0)
      {
        char q[20];
        sprintf(q,"Ошибка ZLib: %d",err);
        LockSched();
        ShowMSG(1,(int)q);
        UnlockSched();
        inflateEnd(&d_stream);
        ZLib_Stream_Init = 0;
      }
      mfree(compressed_data);
      processed_pos = virt_buffer_len;
    
      // Готовимся к передаче данных в MMI
      IPC_BUFFER* tmp_buffer = malloc(sizeof(IPC_BUFFER)); // Сама структура
      tmp_buffer->xml_buffer = Decompr_Buffer;
      tmp_buffer->buf_size = strlen(Decompr_Buffer);
      GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,tmp_buffer,sock);
    }
  }
  else
  {  
  
  char lastchar = *(buf + rec_bytes - 1);
  
  if((rec_bytes<REC_BUFFER_SIZE)&&(lastchar=='>'))   // Приняли меньше размера буфера приёма - наверняка конец передачи
  {
    int bytecount = virt_buffer_len - processed_pos;
    
    // НАДО ОСВОБОДИТЬ В MMI!
    IPC_BUFFER* tmp_buffer = malloc(sizeof(IPC_BUFFER)); // Сама структура
    
    tmp_buffer->xml_buffer = malloc(bytecount);          // Буфер в структуре
    get_buf_part(tmp_buffer->xml_buffer, bytecount);

    tmp_buffer->buf_size = bytecount;      
/*
    // Блочное конвертирование UTF8->ANSI

    char* conv_buf=malloc(bytecount);
    int conv_size;
    conv_buf = convUTF8_to_ANSI(conv_buf,tmp_buffer->xml_buffer, bytecount, &conv_size);
    mfree(tmp_buffer->xml_buffer);
    tmp_buffer->buf_size = conv_size;
    tmp_buffer->xml_buffer = conv_buf;
*/    

    processed_pos = virt_buffer_len;

    // Посылаем в MMI сообщение с буфером
    GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,tmp_buffer,sock);
  }  
  }
  mfree(buf);
}

/*
public void write(byte[] b, int off, int len) throws IOException {
		if (bufferUse + len <= buf.length) {// buffer nam staci len ho
			// zkopirujeme
			for (int i = 0; i < len; i++) {
				buf[bufferUse + i] = b[off + i];
			}
			bufferUse += len;
		} else {
			// nestaci buffer zapiseme maximalne 64tis byte a pak jdeme dale
			int rLen = bufferUse + len;
			boolean wasBigger = false;
			if (rLen > 65536) {
				rLen = 65536;
				wasBigger = true;
			}
			writeBlockHeader(rLen);
			ous.write(buf, 0, bufferUse);
			ous.write(b, off, rLen - bufferUse);
			bufferUse = 0;
			if (wasBigger) {// nemohly jsme zapsat cely block
				write(b, off + rLen, len - rLen);// kolotoc se opakuje
			} else {
				// jsme li ti co zapsaly posledni data
				ous.flush();
			}
		}


writeBlockHeader(len):

ous.write(0);
		ous.write(len);
		ous.write(len >> 8);
		len = ~len;
		ous.write(len);
		ous.write(len >> 8);
*/

void SendAnswer(char *str)
{
  unsigned int block_len= strlen(str);
  out_bytes_count += block_len;
//#ifdef LOG_ALL
//  Log("OUT->", str);
//#endif  

if(!Is_Compression_Enabled)
{
  send(sock,str,block_len,0);
}
else
{
  // Эмуляция компрессии ;)
  // Пишем заголовок "блока"
  unsigned int _0 = 0;
  unsigned int out_data = block_len;
  send(sock,&_0,1,0);
  send(sock,&out_data,1,0);
  out_data = block_len >> 8;
  send(sock,&out_data,1,0);
  out_data = ~block_len;
  send(sock,&out_data,1,0);
  out_data = out_data >> 8;
  send(sock,&out_data,1,0);
  out_bytes_count +=5;
  // Записали, пишем сам блок
  send(sock,str,block_len,0);
}
}

char Support_Compression = 0;
char Support_MD5_Auth = 0;
char Support_Resource_Binding = 0;

void Analyze_Stream_Features(XMLNode *nodeEx)
{
  strcat(logmsg, "\nGetting features...");
  XMLNode *Compr_feature = XML_Get_Child_Node_By_Name(nodeEx, "compression");
  if(Compr_feature)
  {
    Support_Compression = 1;
    strcat(logmsg, "\nCompression:  +");
  }
  
  XMLNode *Res_Binding_feature = XML_Get_Child_Node_By_Name(nodeEx, "bind");
  if(Res_Binding_feature)
  {
    Support_Resource_Binding = 1;
    strcat(logmsg, "\nResBind:       +");
  }
  
  XMLNode *Auth_Methods = XML_Get_Child_Node_By_Name(nodeEx, "mechanisms");
  if(Auth_Methods)
  {
    XMLNode *Ch_Node=XML_Get_Child_Node_By_Name(Auth_Methods, "mechanism");
    while(Ch_Node)
    {
      if(!strcmp(Ch_Node->value, "DIGEST-MD5"))
      {
        Support_MD5_Auth = 1;
        strcat(logmsg, "\nDIGEST-MD5:  +");
      }
      Ch_Node = Ch_Node->next;  
    }
  }
  REDRAW();   
}


/*
    Рекурсивная функция декодирования XML-потока
*/
void Process_Decoded_XML(XMLNode* node)
{
  XMLNode* nodeEx = node;
  while(nodeEx)
  {

//----------------    
    if(!strcmp(nodeEx->name,"stream:features")) 
    {
      Analyze_Stream_Features(nodeEx);
      if(USE_ZLIB && Support_Compression && Jabber_state == JS_NOT_CONNECTED)Compression_Ask();
      if(Support_MD5_Auth && (Jabber_state == JS_NOT_CONNECTED || Jabber_state==JS_ZLIB_STREAM_INIT_ACK))SUBPROC((void*)Use_Md5_Auth_Report);
      if(Support_Resource_Binding && Jabber_state == JS_SASL_NEW_STREAM_ACK)SASL_Bind_Resource();
    }

//----------------        
  
    if(!strcmp(nodeEx->name,"compressed") && Jabber_state == JS_ZLIB_INIT_ACK)
    {
      Compression_Init_Stream();
    }
    
//----------------
    if(!strcmp(nodeEx->name,"success")&& Jabber_state == JS_SASL_AUTH_ACK)
    {
      SASL_Open_New_Stream();
    }    

//----------------
    if(!strcmp(nodeEx->name,"failure")&& Jabber_state < JS_AUTH_OK)
    {
      SASL_Process_Error(nodeEx);
    }    
    
//----------------
    if(!strcmp(nodeEx->name,"challenge")&& Jabber_state == JS_SASL_NEG_ANS_WAIT)
    {
      Process_Auth_Answer(nodeEx->value);
    }    
//----------------    
    if(!strcmp(nodeEx->name,"challenge")&& Jabber_state == JS_SASL_NEGOTIATION)
    {
      Decode_Challenge(nodeEx->value);
      Send_Login_Packet();
    }

//----------------
    if(!strcmp(nodeEx->name,"message"))
    {
      Process_Incoming_Message(nodeEx);
    }
//----------------
    if(!strcmp(nodeEx->name,"iq"))
    {
      Process_Iq_Request(nodeEx);
    }
//----------------
    if(!strcmp(nodeEx->name,"stream:stream"))
    {
      connect_state = 2;
      // Если не используем SASL-авторизацию, можно послать пакет авторизации
      if(!USE_SASL)
      {
        Jabber_state = JS_CONNECTED_STATE;
        SUBPROC((void*)Send_Auth);
      }
      else
        if(nodeEx->subnode)
        {
          Process_Decoded_XML(nodeEx->subnode);
          return;
        }
    }   
//----------------
    if(!strcmp(nodeEx->name,"stream:error"))
    {
      connect_state = 0;
      REDRAW();
      Jabber_state = JS_ERROR;
      char err[]="Ошибка XML-потока";
      ShowDialog_Error(1,(int)err);
      sprintf(logmsg, err);
    } 
//----------------
    if(!strcmp(nodeEx->name,"presence"))
    {
      Process_Presence_Change(nodeEx);
    }
//----------------
    
    //if(nodeEx->subnode) Process_Decoded_XML(nodeEx->subnode);
    nodeEx = nodeEx->next;
  }
}

void __log(char* buffer, int size)
{
  char mess[20];
  sprintf(mess,"RECV:%d",size);
#ifdef LOG_TO_COM_PORT
  tx_str(buffer);
#else
  Log(mess,buffer);
#endif  
  mfree(buffer);
}

void Process_XML_Packet(IPC_BUFFER* xmlbuf)
{
  // Сюда попадаем, если от транслятора принят указатель на порцию данных   
  LockSched();
  XMLNode *data=XMLDecode(xmlbuf->xml_buffer,xmlbuf->buf_size);
  UnlockSched();

// Сюда было бы логичнее переставить блок записи, ибо тогда в логе будет идти
// сначала принятый пакет, а потом предпринятые действия
#ifdef LOG_IN_DATA  
    char* tmp_buf=malloc(xmlbuf->buf_size+1);
    zeromem(tmp_buf,xmlbuf->buf_size+1);
    memcpy(tmp_buf,xmlbuf->xml_buffer,xmlbuf->buf_size);
    SUBPROC((void*)__log,tmp_buf, xmlbuf->buf_size);
#endif

    
  if(data)
  {
#ifdef LOG_XML_TREE
    SaveTree(data);
#endif
    Process_Decoded_XML(data);
    DestroyTree(data);
  }
    
  // Освобождаем память :)
    mfree(xmlbuf->xml_buffer);
    mfree(xmlbuf);    
#ifdef NEWSGOLD
    REDRAW();
#else
#endif    
}


//===============================================================================================
// Всякий стафф с GUI
void onRedraw(MAIN_GUI *data)
{ 
  int scr_w=ScreenW();
  int scr_h=ScreenH();
  
  int font_color, bgr_color;
  if(connect_state<2)
  {
    font_color=0;
    bgr_color = 20;
  }
  if(connect_state==2)
  {
    font_color=1;
    bgr_color = 13;
  }

  if(Jabber_state==JS_AUTH_ERROR || Jabber_state==JS_ERROR)
  {
    font_color = 7;
    bgr_color  = 2;
  }  
  DrawRoundedFrame(0,SCR_START,scr_w-1,scr_h-1,0,0,0,
		   GetPaletteAdrByColorIndex(0),
		   GetPaletteAdrByColorIndex(bgr_color));
  
  CList_RedrawCList();
  
  LockSched();

  if (CList_GetUnreadMessages()>0) {
    wsprintf(data->ws1,"%d(%d/%d) IN:%d",CList_GetUnreadMessages(), CList_GetNumberOfOnlineUsers(),CList_GetNumberOfUsers(),virt_buffer_len);
  } else {
    wsprintf(data->ws1,"(%d/%d) IN:%d",CList_GetNumberOfOnlineUsers(),CList_GetNumberOfUsers(),virt_buffer_len);
  }
  UnlockSched();
  
  //рисуем селф-иконку
#ifdef USE_PNG_EXT
char mypic[128];

  if (CList_GetUnreadMessages()>0)
      Roster_DrawIcon(1, SCR_START+1, (int) Roster_getIconByStatus(mypic,50)); //иконка сообщения
  else 
    Roster_DrawIcon(1, SCR_START+1, (int) Roster_getIconByStatus(mypic, My_Presence));

#else
  int img_num=0;
  if (CList_GetUnreadMessages()>0)
    img_num=Roster_getIconByStatus(50); //иконка сообщения
  else 
    img_num=Roster_getIconByStatus(My_Presence);

  Roster_DrawIcon(1, SCR_START+1, img_num); //иконка сообщения
#endif  
  DrawString(data->ws1,16,SCR_START+3,scr_w-4,scr_h-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(font_color),GetPaletteAdrByColorIndex(23));

  if(Jabber_state!=JS_ONLINE)
  {
    wsprintf(data->ws1,"%t", logmsg);
    DrawString(data->ws1,1,SCR_START+15,scr_w-4,scr_h-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(font_color),GetPaletteAdrByColorIndex(23));
  }
  
  //DrawString(data->ws2,3,13,scr_w-4,scr_h-4-16,SMALL_FONT,0,GetPaletteAdrByColorIndex(font_color),GetPaletteAdrByColorIndex(23));
#ifdef USE_PNG_EXT 
  if(connect_state<2)
  {
    char logo_path[128];
    strcpy(logo_path, PATH_TO_PIC);
    strcat(logo_path,"SieJC_logo.png");
    DrawImg(0,70,(int)logo_path);
  }
#endif
}

void onCreate(MAIN_GUI *data, void *(*malloc_adr)(int))
{
  data->ws1=AllocWS(256);
  data->ws2=AllocWS(256);
  data->gui.state=1;
}

void onClose(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  FreeWS(data->ws1);
  FreeWS(data->ws2);
  data->gui.state=0;
}

void onFocus(MAIN_GUI *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *))
{
  DisableIDLETMR();
  data->gui.state=2;
}

void onUnfocus(MAIN_GUI *data, void (*mfree_adr)(void *))
{
  if (data->gui.state!=2) return;
  data->gui.state=1;
}


void QuitCallbackProc(int decision)
{
  if(!decision)Quit_Required = 1;
}

void DisplayQuitQuery()
{
  ShowDialog_YesNo(1,(int)"Покинуть SieJC?",QuitCallbackProc);  
}

void Enter_SiepatchDB()
{

  char room[]= "siepatchdb@conference.jabber.ru";
  char nick_t[]="%s_SieJC";
  char nick[100];
  sprintf(nick, nick_t, USERNAME);
//  char nick[]="Kibab_exp";
  
  char *room_nick =ANSI2UTF8(nick, strlen(nick)*2);
  char* room_name = ANSI2UTF8(room, strlen(room)*2);
  Enter_Conference(room, nick, 20);
  mfree(room_nick);
  mfree(room_name);
}

void Dump_PhoneInfo()
{
  char *xz;
  char out[100];
  for(int i=0; i<0x0C;i++)
  {
    xz = Get_Phone_Info(i);
    sprintf(out,"%02X: %s", i, xz);
    Log("IDENT", out);
  }  
}


void Disp_State()
{
  char q[40];
  sprintf(q,"Jabber_state=%d\nOut bytes: %d", Jabber_state, out_bytes_count);
  ShowMSG(0,(int)q);
}

#ifndef NEWSGOLD
volatile char IsRedrawTimerStarted=0;

void SGOLD_RedrawProc()
{
  DirectRedrawGUI();
  extern void SGOLD_RedrawProc(void);
  GBS_StartTimerProc(&redraw_tmr, Redraw_Time, SGOLD_RedrawProc);
}

void SGOLD_RedrawProc_Starter()
{
  if(IsRedrawTimerStarted)return;
  IsRedrawTimerStarted=1;
  SGOLD_RedrawProc();//GBS_StartTimerProc(&redraw_tmr, Redraw_Time, (void*)SGOLD_RedrawProc);
}
#endif

//Context:HELPER
void Test_bm()
{
  static char priv_id[]="SieJC_priv_req";
  static char bm[]="<storage xmlns='storage:bookmarks'/>";
  SendIq(NULL, IQTYPE_GET, priv_id, IQ_PRIVATE, bm);    
}

int onKey(MAIN_GUI *data, GUI_MSG *msg)
{
  if(Quit_Required)return 1; //Происходит вызов GeneralFunc для тек. GUI -> закрытие GUI
#ifndef NEWSGOLD
  SGOLD_RedrawProc_Starter();
#endif
  //DirectRedrawGUI();
  if(msg->gbsmsg->msg==LONG_PRESS)
  {
    switch(msg->gbsmsg->submess)
    {  
    case DOWN_BUTTON:
    case '8':
      {
        CList_MoveCursorDown();
        break;
      }

    case UP_BUTTON:
    case '2':
      {
        CList_MoveCursorUp();
        break;
      }
    }
  }
  if (msg->gbsmsg->msg==KEY_DOWN)
  {
    switch(msg->gbsmsg->submess)
    {
    
    case ENTER_BUTTON:
      {
        Display_Message_List(CList_GetActiveContact());
        break;
      }
    
    case LEFT_SOFT:
      {
        MM_Show();
        break;
      }
    case RIGHT_SOFT:
      {
        DisplayQuitQuery();
        break;
      }
    case GREEN_BUTTON:
      if ((connect_state==0)&&(sock==-1))
      {
        GBS_DelTimer(&reconnect_tmr);
	DNR_TRIES=3;
        SUBPROC((void *)create_connect);
      }
      
      if(connect_state==2 && Jabber_state==JS_ONLINE)
      {
        Init_Message(CList_GetActiveContact(), NULL);                        
      }
      
      break;

    case '1':
      {
        CList_MoveCursorHome();
        break;
      }      
    
    case '4':
      {
        Enter_SiepatchDB();
        break;
      }
    case '5':
      {
        CList_Display_Popup_Info(CList_GetActiveContact());
        break;
      }

    case '6':
      {
        Disp_State();
        break;
      }  
      
    case '7':
      {
        SUBPROC((void*)Test_bm);
        break;
      } 
      
    case DOWN_BUTTON:
    case '8':
      {
        CList_MoveCursorDown();
        break;
      }

    case UP_BUTTON:
    case '2':
      {
        CList_MoveCursorUp();
        break;
      }


    case '9':
      {
        CList_MoveCursorEnd();
        break;
      }        
      
    case '0':
      {
        
        CList_ToggleOfflineDisplay();
        break;
      }        
      
    case '*':
      {
        Is_Vibra_Enabled = !(Is_Vibra_Enabled);
        break;
      }
    case '#': //решеткой бегаем между непрочитанными
      {
        nextUnread();
        break;
      }
    }    
  }
  //  onRedraw(data);
  return(0);
}

int method8(void){return(0);}

int method9(void){return(0);}

const void * const gui_methods[11]={
  (void *)onRedraw,	//Redraw
  (void *)onCreate,	//Create
  (void *)onClose,	//Close
  (void *)onFocus,	//Focus
  (void *)onUnfocus,	//Unfocus
  (void *)onKey,	//OnKey
  0,
  (void *)kill_data, //onDestroy,	//Destroy
  (void *)method8,
  (void *)method9,
  0
};

const RECT Canvas={0,0,0,0};

void maincsm_oncreate(CSM_RAM *data)
{
  MAIN_GUI *main_gui=malloc(sizeof(MAIN_GUI));
  MAIN_CSM*csm=(MAIN_CSM*)data;
  zeromem(main_gui,sizeof(MAIN_GUI));
  patch_rect((RECT*)&Canvas,0,0,ScreenW()-1,ScreenH()-1);
  main_gui->gui.canvas=(void *)(&Canvas);
  main_gui->gui.flag30=2;
  main_gui->gui.methods=(void *)gui_methods;
  main_gui->gui.item_ll.data_mfree=(void (*)(void *))mfree_adr();
  csm->csm.state=0;
  csm->csm.unk1=0;
  csm->gui_id=CreateGUI(main_gui);
  DNR_TRIES=3;
  XMLBuffer = malloc(XML_BUFFER_SIZE);
  zeromem(XMLBuffer, XML_BUFFER_SIZE);
 
  SUBPROC((void *)create_connect);
#ifdef LOG_ALL  
  // Определим адреса некоторых процедур, на случай,
  // если клиент будет падать - там могут быть аборты...
  void* Process_XML_Packet_ADR = (void*)Process_XML_Packet;
  void* Process_Decoded_XML_ADR = (void*) Process_Decoded_XML;
  char msg[80];
  sprintf(msg,"@Process_XML_Packet=0x%X, @Process_Decoded_XML=0x%X\r\n",Process_XML_Packet_ADR, Process_Decoded_XML_ADR);
  Log("SYSTEM", msg);
#endif
}

void maincsm_onclose(CSM_RAM *csm)
{
  GBS_DelTimer(&tmr_vibra);
  GBS_DelTimer(&TMR_Send_Presence);
#ifndef NEWSGOLD
  GBS_DelTimer(&redraw_tmr);
#endif  
  GBS_DelTimer(&reconnect_tmr);
  SetVibration(0);
  CList_Destroy();
  MUCList_Destroy();
  KillBMList();
  Destroy_SASL_Ctx();
  mfree(XMLBuffer);
  
  if(ZLib_Stream_Init)
  {
    inflateEnd(&d_stream);
  }
  SUBPROC((void *)end_socket);
  SUBPROC((void *)ElfKiller);
}

void do_reconnect(void)
{
  if (is_gprs_online)
  {
    DNR_TRIES=3;
    SUBPROC((void*)create_connect);
  }
}

int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
  MAIN_CSM *csm=(MAIN_CSM*)data;
  {

#define idlegui_id (((int *)icsm)[DISPLACE_OF_IDLEGUI_ID/4])
    CSM_RAM *icsm=FindCSMbyID(CSM_root()->idle_id);
    if (IsGuiOnTop(idlegui_id))
    {
      GUI *igui=GetTopGUI();
      if (igui)
      {
	void *idata=GetDataOfItemByID(igui,2);
	if (idata)
	{
	  void *canvasdata=((void **)idata)[DISPLACE_OF_IDLECANVAS/4];

#ifdef USE_PNG_EXT
char mypic[128];

  if (CList_GetUnreadMessages()>0)
     Roster_getIconByStatus(mypic,50); //иконка сообщения
  else 
    Roster_getIconByStatus(mypic, My_Presence);
          DrawCanvas(canvasdata,IDLE_ICON_X,IDLE_ICON_Y,15,64,1);          
	  DrawImg(IDLE_ICON_X,IDLE_ICON_Y,(int)mypic);
#else          
          int mypic=0;
          if (CList_GetUnreadMessages()>0)
            mypic=Roster_getIconByStatus(50); //иконка сообщения
          else 
            mypic=Roster_getIconByStatus(My_Presence);
          DrawCanvas(canvasdata,IDLE_ICON_X,IDLE_ICON_Y,15,64,1);          
	  DrawImg(IDLE_ICON_X,IDLE_ICON_Y,mypic);          
#endif
	}
      }
    } 
  }
  if(Quit_Required)
  {
    csm->csm.state=-3;
  }
  if (msg->msg==MSG_GUI_DESTROYED)
  {
    if ((int)msg->data0==csm->gui_id)
    {
      csm->csm.state=-3;
    }
  }
  if (msg->msg==MSG_HELPER_TRANSLATOR)
  {
    switch((int)msg->data0)
    {
    case LMAN_DISCONNECT_IND:
      is_gprs_online=0;
      return(1);
    case LMAN_CONNECT_CNF:
      is_gprs_online=1;
      return(1);
    case ENIP_DNR_HOST_BY_NAME:
      if ((int)msg->data1==DNR_ID)
      {
	if (DNR_TRIES) SUBPROC((void *)create_connect);
      }
      return(1);
    }
    if ((int)msg->data1==sock)
    {
      //Если наш сокет
      if ((((unsigned int)msg->data0)>>28)==0xA)
      {
        //Пакет XML-данных готов к обработке и передаётся на обработку в контексте MMI
        Process_XML_Packet((IPC_BUFFER*)msg->data0);
        return(0);
      }
      switch((int)msg->data0)
      {
      case ENIP_SOCK_CONNECTED:
        if (connect_state==1)
        {
          //Соединение установлено, посылаем пакет Welcome
          SUBPROC((void*)Send_Welcome_Packet);
          REDRAW();
        }
        else
        {
          ShowMSG(1,(int)"Illegal message ENIP_SOCK_CONNECTED!");
        }
        break;
      case ENIP_SOCK_DATA_READ:
        if (connect_state>=1)
        {
          //Если посылали Welcome, передаём на принятие в контекст HELPER
          SUBPROC((void *)get_answer);
        }
        else
        {
          ShowMSG(1,(int)"Illegal message ENIP_DATA_READ");
        }
        break;
      case ENIP_SOCK_REMOTE_CLOSED:
        //Закрыт со стороны сервера
        if (connect_state) SUBPROC((void *)end_socket);
        break;
      case ENIP_SOCK_CLOSED:
        connect_state=0;
        Jabber_state = JS_NOT_CONNECTED;
        sock=-1;
        Vibrate(4);
        REDRAW();
        //GBS_StartTimerProc(&reconnect_tmr,TMR_SECOND*120,do_reconnect);
        break;
      }
    }
  }
  return(1);
}


const int minus11=-11;

unsigned short maincsm_name_body[140];

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
  WSHDR *ws=AllocWS(256);
  wsprintf((WSHDR *)(&MAINCSM.maincsm_name),"SieJC",ws);
  FreeWS(ws);
}

// Проверка, что платформа для компиляции выбрана правильно

unsigned short IsGoodPlatform()
{ 
#ifdef NEWSGOLD  
  return  isnewSGold();
#else
  return  !isnewSGold();
#endif    
}

Check_Settings_Cleverness()
{
  if(!USE_SASL && USE_ZLIB)ShowMSG(0,(int)"ZLib не работает без SASL!");
}

int main(char *exename, char *fname)
{
  if(!IsGoodPlatform())
  {
    ShowMSG(1,(int)"Target platform mismatch!!");
    return 0;
  }
  char dummy[sizeof(MAIN_CSM)];
  InitConfig(fname);
  if(!strlen(USERNAME))
  {
    ShowMSG(1,(int)"Введите логин/пароль!");
    return 0;
  }
  UpdateCSMname(); 
  LockSched();
  CreateCSM(&MAINCSM.maincsm,dummy,0);
  UnlockSched();
  Check_Settings_Cleverness();  
  return 0;
}
