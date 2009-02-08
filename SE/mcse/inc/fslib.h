#ifndef _FSLIB_H
#define _FSLIB_H

wchar_t* GetFileDir(wchar_t* fname, wchar_t* buf);
wchar_t* GetFileName(wchar_t* fname);
wchar_t* GetFileExt(wchar_t* fname);

int fsrm(wchar_t* path, int ip);
int fscp(wchar_t* src, wchar_t* dst, int ip);
int fsmv(wchar_t* src, wchar_t* dst);
int GetFilesCnt(wchar_t* path);
#endif
