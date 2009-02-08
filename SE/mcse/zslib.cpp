#include "inc\mc.h"
#include "inc\zslib.h"
#include "inc\mui.h"


const wchar_t zip_ext[] = L"zip";

void ZipError(int zipErrNum, char* procName)
{
  sprintf(msgbuf, muitxt(ind_err_ziperr), zipErrNum, procName);
  MsgBoxError(msgbuf);
}


int IsZipOpened(int tab)
{
  return (tabs[tab]->zipInfo->uf != NULL);
}

int IsInZip()
{
  return IsZipOpened(curtab);
}

void CloseZip(ZIPINF* pzi)
{
  if (pzi->uf)
  {
    unzClose(pzi->uf);
    FreeZipInfo(pzi);
  }
}

void CloseTabZip(int tab)
{
  CloseZip(tabs[tab]->zipInfo);
}

void FreeZipInfo(ZIPINF* pzi)
{
  if (pzi)
  {
    if (pzi->pfi)
    {
      for (int i=0; i < pzi->gi.number_entry; i++)
        if (pzi->pfi[i]) delete (pzi->pfi[i]);
      delete(pzi->pfi);
    }
    
    if (pzi->pszNames)
    {
      for (int i=0; i < pzi->gi.number_entry; i++)
        if (pzi->pszNames[i]) delete (pzi->pszNames[i]);
      delete(pzi->pszNames);
    }
    
    if (pzi->password)
      delete(pzi->password);
    
    memset(pzi, 0, sizeof(ZIPINF));
  }
}
// Считываем и сохраняем информацию о содержимом архива в память
int ReadZipInfo(ZIPINF* pzi)
{
	int zerr = UNZ_OK;

	if (pzi->uf)
	{
		zerr = unzGetGlobalInfo(pzi->uf, &pzi->gi);
		if (zerr == UNZ_OK)
		{
			if (pzi->gi.number_entry > 0)
			{
				int useProgressBar = !progr_start;
				if (useProgressBar)
				{
					// запускаем прогресс бар, если уже не запущен другой
					Busy = 1;
					initprogr(ind_msg_zreading);
					progr_max = pzi->gi.number_entry;
					incprogr(0);
				}
			
				int total_entry = pzi->gi.number_entry;
				pzi->pszNames = new wchar_t *[total_entry];
				if (pzi->pszNames)
				{
					memset(pzi->pszNames, 0, total_entry*sizeof(wchar_t *));
					pzi->pfi = new unz_file_info*[total_entry];
					if (pzi->pfi) memset(pzi->pfi, 0, total_entry*sizeof(unz_file_info *));
				}
				
				if (pzi->pszNames && pzi->pfi)
				{
					zerr = unzGoToFirstFile(pzi->uf);
					if (zerr == UNZ_OK)
					{
						for (int i=0; i < pzi->gi.number_entry && zerr == UNZ_OK && !progr_stop; i++)
						{
							pzi->pszNames[i] = new wchar_t[MAX_PATH_INZIP];
							pzi->pfi[i] = new unz_file_info;
							if (pzi->pszNames[i] != NULL && pzi->pfi[i] != NULL)
							{
                                                                char zname[MAX_PATH_INZIP];
								zerr = unzGetCurrentFileInfo(pzi->uf, pzi->pfi[i], zname, MAX_PATH_INZIP,
									NULL, 0, NULL, 0);
								if (zerr == UNZ_OK)
								{
									// Фиксим слеши
									wstrreplace(pzi->pszNames[i], '\\', '/');
									
									// Переводим
									dos2utf16(pzi->pszNames[i], zname);
									
									// Переходим к следующему файлу
									if ((i+1) < pzi->gi.number_entry)
									{
										zerr = unzGoToNextFile(pzi->uf);
										if (zerr != UNZ_OK) ZipError(zerr, "unzGoToNextFile");
									}
								}
								else ZipError(zerr, "unzGetCurrentFileInfo");
							}
							else ZipError(zerr = UNZ_ERRNO, "no memory for fileinfo");
							
							if (useProgressBar) incprogr(1);
						}
					}
					else ZipError(zerr, "unzGoToFirstFile");
				}
				else ZipError(zerr = UNZ_ERRNO, "no memory");
				
				if (useProgressBar)
				{
					endprogr(); // останавливаем прогресс бар
					Busy = 0;
				}
			}
		}
		else ZipError(zerr, "unzGetGlobalInfo");
	}

	if (progr_stop) zerr = -11111;
	
	return zerr;
}


int OpenZip(ZIPINF* pzi, wchar_t* zipFileName)
{
  int zerr = UNZ_OK;
  
  unzFile uf = unzOpen(zipFileName);
  if (uf != NULL)
  {
    pzi->uf = uf;
    pzi->szCurDir[0] = '\0';
    wstrcpy(pzi->szZipPath, zipFileName);
  }
  else zerr = UNZ_ERRNO;
  
  if (zerr == UNZ_OK)
    zerr = ReadZipInfo(pzi);
  
  if (zerr != UNZ_OK)
    CloseZip(pzi);
  
  return zerr;
}

int OpenTabZip(int tab, wchar_t* zipFileName)
{
  int zerr = UNZ_ERRNO;
  
  if (!IsZipOpened(tab)) // не обрабатываем вложенные архивы
    zerr = OpenZip(tabs[tab]->zipInfo, zipFileName);
  return zerr;
}

int FillZipFiles(int tab, wchar_t* subdname)
{
  int num = 0;
  wchar_t buf[MAX_PATH_INZIP];
  wchar_t* pszDirName = subdname[0] == '/' ? subdname + 1 : subdname; // Пропустим в начале обратный слеш
  int dirLen = wstrlen(pszDirName);
  ZIPINF* pzi = tabs[tab]->zipInfo;
  
  
  if (pzi->uf)
  {
    for (int i=0; i < pzi->gi.number_entry; i++)
    {
      int ignore = 1;
      wstrncpy(buf, pzi->pszNames[i],MAX_PATH_INZIP-1);
      wchar_t* pFileNameStart = &buf[0];
      unz_file_info* pfi = pzi->pfi[i];
      if (dirLen == 0)
      {
        // Если ищем корневые элементы
        // То добавляем все файлы у которых нет слешей '\'
        wchar_t* slashPos = wstrwstr(buf, L"/");
        ignore = (slashPos != NULL);
        if (ignore)
        {
          // И добавляем все директории - у них один слеш, и тот последний
          ignore = (*(slashPos + 1) != 0);
          if (!ignore)
          {
            // Затрем этот слеш, чтобы не выводился
            *slashPos = 0;
          }
        }
      }
      else
      {
        // Сравниваем начало, если равно и потом слеш - то это элементы нашей директории
        pFileNameStart = pFileNameStart + dirLen;
        ignore = (wstrcmpni(buf, pszDirName, dirLen) != 0 || *pFileNameStart != '/');
        if (!ignore)
        {
          pFileNameStart = pFileNameStart + 1; // пропустим слеш после имени директории
          // игнорим саму текущую, директорию
          ignore = (*pFileNameStart == 0);
          if (!ignore)
          {
            // проверка на поддиректорию
            wchar_t* pFileNameEnd = wstrwstr(pFileNameStart, L"/");
            if (pFileNameEnd != NULL)
            {
              // проверка на то что это не сама поддиректория а ее элементы, оные проигнорим
              ignore = (*(pFileNameEnd+1) != 0);
              if (!ignore)
              {
                *pFileNameEnd = 0; // грохнем слеш в конце
              }
            }
          }
        }
      }
      if (!ignore)
      {
        AddFile(tab, i, pFileNameStart, pfi->uncompressed_size, pfi->external_fa&ZIP_DIRECTORY?FA_DIRECTORY:0, pfi->dosDate,
                pfi->compressed_size, (pfi->external_fa & ZIP_DIRECTORY) ? TYPE_ZIP_DIR : TYPE_ZIP_FILE);
        num++;
      }
    }
  }
  return num;
}


#define LOCALHEADERMAGIC	(0x04034b50)

int IsItZipFile(wchar_t* fname)
{
  if (CONFIG_ZIP_DETECT_BY == 0) // by content
  {
    unsigned char buf[4];
    uLong header = 0;
    int f =w_fopen(fname, WA_Read, 0x1FF, NULL);
    if (f>=0)
    {
      if (w_fread(f, &buf, 4) == 4)
        header = (uLong)buf[0] + (((uLong)buf[1])<<8) + (((uLong)buf[2])<<16) + (((uLong)buf[3])<<24);
      w_fclose(f);
    }
    return (header == LOCALHEADERMAGIC);
  }
  else // by extension
  {
    return (wstrcmpi(GetFileExt(fname), zip_ext) == 0);
  }
}

/*
int ExtractCurrentFile(ZIPINF* pzi, int ind, wchar_t* extractDir, int usePaths, int ip)
{
  unzFile uf = pzi->uf;
  wchar_t *filePathInZip = pzi->pszNames[ind];
  int res = UNZ_OK;
  int fout = 0;
  wchar_t extractFilePath[MAX_PATH];
  wchar_t fnbuf[MAX_PATH];
  
  if (filePathInZip == NULL)
  {
    // Считываем имя сами если нужно
    unz_file_info fi;
    filePathInZip = fnbuf;
    unzGetCurrentFileInfo(uf, &fi, filePathInZip, MAX_PATH, NULL, 0, NULL, 0);
  }
  uInt size_buf = WRITEBUFFERSIZE;
  void* buf = (void*)malloc(size_buf);
  if (buf == NULL)
  {
    ZipError(1, "ExtractCurrentFile.malloc");
    return 1;
  }
  
  wchar_t* fileNameInZip = GetFileName(filePathInZip);
  if ((*fileNameInZip) == '\0')
  {
    if (usePaths)
    {
      // Директория... создаем
      sprintf(extractFilePath, _s_s, extractDir, filePathInZip);
      mktree(extractFilePath);
    }
  }
  else
  {
    // File
    if (usePaths)
      sprintf(extractFilePath, _s_s, extractDir, filePathInZip);
    else
      sprintf(extractFilePath, _s_s, extractDir, fileNameInZip);
    
    if (fexists(extractFilePath))
    {
      // ToDo: добавить проверку - если Temp, то переписывать не спрашивая, иначе - диалог
      if (!unlink(extractFilePath, &err))
        ZipError(res = 2, "ExtractCurrentFile.fsrm");
    }
    if (res == UNZ_OK)
    {
      fout = fopen(extractFilePath, A_ReadWrite+A_Create+A_Truncate+A_BIN, P_READ+P_WRITE, &err);
      
      	// some zipfile don't contain directory alone before file 
      if (fout == -1 && usePaths && fileNameInZip != filePathInZip)
      {
        char c = *(fileNameInZip - 1);
        *(fileNameInZip - 1) = '\0';
        mktree(filePathInZip);
        *(fileNameInZip - 1) = c;
        fout = fopen(extractFilePath, A_ReadWrite+A_Create+A_Truncate+A_BIN, P_READ+P_WRITE, &err);
      }
      if (fout == -1)
        ZipError(res = 3, "ExtractCurrentFile.fopen");
    }
    if (res == UNZ_OK)
    {
      // Собственно извлекаем
      
      // Откроем файл
      if (unzOpenCurrentFilePassword(uf, pzi->password) == UNZ_OK)
      {
        int isnewprogr = 0;
        if (ip && !progrsp_start)
        {
          if (!progr_start)
          {
            initprogr(ind_msg_zreading);
            progr_max = 1;
            incprogr(0);
            isnewprogr = 1;
          }
          progrsp_start = 1;
          progrsp_max = pzi->pfi[ind]->uncompressed_size;
          incprogrsp(0);
        }
        int cb = 0;
        do
        {
          if (ip && progr_stop)
          {
            res = UNZ_ERRNO;
            break;
          }
          cb = (int)unzReadCurrentFile(uf, buf, size_buf);
          if (cb < 0)
          {
            ZipError(cb, "ExtractCurrentFile.unzReadCurrentFile");
            res = 5;
            break;
          }
          else if (cb > 0)
          {
            if (fwrite(fout, buf, cb, &err) != cb)
            {
              ZipError(res = 6, "ExtractCurrentFile.fwrite");
              break;
            }
            if (ip && progrsp_start) incprogrsp(cb);
          }
        }
        while (cb > 0);
        if (ip && progrsp_start)
        {
          endprogrsp();
          progrsp_start = 0;        
          if (isnewprogr)
          {
            endprogr();
            progr_start = 0;
          }
        }
        if (fout) { fclose(fout, &err); fout = 0; }
        if (res == UNZ_OK)    
        {
          
					// Выставляем дату время извлеченному файлу
					// change_file_date(write_filename, file_info.dosDate, file_info.tmu_date);
        }
      }
      else ZipError(res = 7, "ExtractCurrentFile.unzOpenCurrentFile");
      if (fout) fclose(fout, &err);
    }
    if (res == UNZ_OK)
    {
      if (unzCloseCurrentFile(uf) != UNZ_OK)
        ZipError(res = 8, "ExtractCurrentFile.unzCloseCurrentFile");
    }
    else unzCloseCurrentFile(uf); // don't lose the error 
  }
  mfree(buf);
  return res;
}



int ExtractFileByID(ZIPINF* pzi, int id, wchar_t* extractDir, int usePaths, int ip)
{
  // Позиционируемся на нужном файле
  if (SetCurrentFileInZip(pzi, id) == UNZ_OK)
  {
    // И извлекаем его
    return ExtractCurrentFile(pzi, id, extractDir, usePaths, ip);
  }
  else return 1;
}

int ExtractFile(ZIPINF* pzi, wchar_t* fname, wchar_t* extractDir, int usePaths)
{
  // Позиционируемся на нужном файле
  int found = 0;
  wchar_t buf[MAX_PATH + 1];
  strcpy(buf + 1, fname);
  buf[0] = '/'; // В начало запишем слеш на тот случай если fname без слеша
  
  wchar_t* nameWithoutSlash = ( (buf[1] == '/') ? (buf + 2) : (buf + 1) );
  wchar_t* nameWithSlash = ( (buf[1] == '/') ? (buf + 1) : (buf) );
  
  for (int i = 0; i < pzi->gi.number_entry; i++)
  {
    found = (wstrcmpi(nameWithoutSlash, pzi->pszNames[i]) == 0);
    if (!found)
    {
      // Пробуем поискать со слешем
      found = (wstrcmpi(nameWithSlash, pzi->pszNames[i]) == 0);
    }
    if (found)
    {
      // Извлекаем его, если нашли
      return ExtractFileByID(pzi, i, extractDir, usePaths, 0);
    }
  }
  return 1;
}

// Кэш на один :) зип файл
ZIPINF zi;

void ZipBufferExtractBegin()
{
}

int ZipBufferExtract(FN_ITM* pi, wchar_t* extractDir)
{
  if (pi && pi->zipPath && pi->full)
  {
    ZIPINF* pzi = &zi;
    int usingZipFromTab = 0;
    
    for (int i = 0; i < MAX_TABS; i++)
    {
      if (IsZipOpened(i) && wstrcmpi(tabs[i]->zipInfo->szZipPath, pi->zipPath) == 0)
      {
        // Если уже открыт в табе, то используем его
        pzi = tabs[i]->zipInfo;
        usingZipFromTab = 1;
        break;
      }
    }
    if (!usingZipFromTab)
    {
      // Если файл уже открыт, но имена не совпадают, то закрываем старый
      if (pzi->uf != 0 && wstrcmpi(pzi->szZipPath, pi->zipPath) != 0)
        CloseZip(pzi);
      
      // Если файл еще не открыт - то открываем
      if (pzi->uf == 0)
        if (OpenZip(pzi, pi->zipPath) != UNZ_OK)
          return UNZ_ERRNO;
    }
    
    if (pi->ftype == TYPE_ZIP_FILE)
      return ExtractFile(pzi, pi->full, extractDir, 0);
    else if (pi->ftype == TYPE_ZIP_DIR)
      return ExtractDir(pzi, pi->full, extractDir, 1);
	}

    return 0;
}
*/
