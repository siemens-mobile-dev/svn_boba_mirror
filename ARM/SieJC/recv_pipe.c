//Context:HELPER
char *Rstream_p; //Указатель на собираемый пакет
int Rstream_n; //Количество байт

void get_answer(void)
{
  char rb[1024];
  int i=recv(sock,rb,sizeof(rb),0);
  int j;
  int err;
  char *p;
  int c;
  if (i<0)
  {
    end_socket();
    return;
  }
  if (i==0) return;
  if (Is_Compression_Enabled)
  {
    //Используем ZLib для переноса данных в собираемый пакет
    d_stream.next_in  = (Byte*)rb;
    d_stream.avail_in = (uInt)i;
    do
    {
      i*=2; //Ожидаемый размер буфера i*2 байт - прогноз на распаковку ;)
      d_stream.next_out = (Byte*)((Rstream_p=realloc(Rstream_p,Rstream_n+i+1))+Rstream_n); //Новый размер собираемого пакета
      d_stream.avail_out = (uInt)i;
      err = inflate(&d_stream, /*Z_NO_FLUSH*/Z_SYNC_FLUSH);
      switch (err)
      {
      case Z_NEED_DICT:
	//ret = Z_DATA_ERROR;     /* and fall through */
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
	//(void)inflateEnd(&strm);
	end_socket();
	return;
      }
      Rstream_n+=(i-d_stream.avail_out);
    }
    while(d_stream.avail_out==0);
    
  }
  else
  {
    memcpy((Rstream_p=realloc(Rstream_p,Rstream_n+i+1))+Rstream_n,rb,i);
    Rstream_n+=i;
  }
  //Теперь считаем теги
  Rstream_p[Rstream_n]=0; //Ограничим строку \0 для упрощения
  p=Rstream_p;
  i=0; //Баланс тегов
  j=0; //Баланс скобок
  while((p=strstr(p,"<stream>"))) {i--; p++;} //Костыль - пропуск тегов stream, для них нет закрывающих
  p=Rstream_p;
  while((c=*p++))
  {
    if (c=='<')
    {
      j++;
      if (*p!='/') i++; else i--;
    }
    if (c=='>')
    {
      j--;
      if ((!i)&&(!j))
      {
	//Сошелся баланс, отдаем на обработку
	int bytecount=p-Rstream_p;
	IPC_BUFFER* tmp_buffer=malloc(sizeof(IPC_BUFFER)); // Сама структура
	memcpy(tmp_buffer->xml_buffer=malloc(bytecount),Rstream_p,tmp_buffer->buf_size=bytecount); // Буфер в структуре
	memcpy(p=Rstream_p,Rstream_p+bytecount,(Rstream_n-=bytecount)+1); //Обработаные в начало и заодно \0
	GBS_SendMessage(MMI_CEPID,MSG_HELPER_TRANSLATOR,0,tmp_buffer,sock); //Обработаем готовый блок
      }
    }
  }
}
