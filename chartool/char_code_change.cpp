#include <Windows.h>
#include "char_code_change.h"

char *UnicodeToAnsi(const wchar_t *src)
{
	int len = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	char *Result = (char*)malloc(sizeof(char)*(len + 1));
	if (Result == NULL)
		return NULL;
	memset((void*)Result,0,len + 1);
	WideCharToMultiByte(CP_ACP, 0, src,-1, Result, len + 1, NULL, NULL);
	return Result;
}

wchar_t *AnsiToUnicode(const char *src)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
	wchar_t *Result = (wchar_t*)malloc(sizeof(wchar_t)*(len + 1));
	if (Result == NULL)
		return NULL;
	memset((void*)Result,0,len + 1);
	MultiByteToWideChar(CP_ACP, 0, src, -1, Result, (len + 1));
	return Result;
}

char *UnicodeToUTF8(const wchar_t *src)
{
	int len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
	char *Result = (char *)malloc(sizeof(char)* (len + 1));
	if (Result == NULL)
		return NULL;
	memset((void*)Result,0,len + 1);
	WideCharToMultiByte(CP_UTF8, 0, src, -1, Result, (len + 1), NULL, NULL);
	return Result;
}

wchar_t *UTF8ToUnicode(const char *src)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	wchar_t *Result = (wchar_t*)malloc(sizeof(wchar_t)*(len + 1));
	if (Result == NULL)
		return NULL;
	memset((void*)Result,0,len + 1);
	MultiByteToWideChar(CP_UTF8, 0, src, -1, Result, (len + 1));
	return Result;
}
