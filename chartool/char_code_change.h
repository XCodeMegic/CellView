#ifndef _CHAR_CODE_CHANGE_2015_04_24_H__
#define _CHAR_CODE_CHANGE_2015_04_24_H__

//some char code trans
//notice:use these functions ,you need call "free(void *)" to free the return value

char *UnicodeToAnsi(const wchar_t *src);

wchar_t *AnsiToUnicode(const char *src);

char *UnicodeToUTF8(const wchar_t *src);

wchar_t *UTF8ToUnicode(const char *src);


#endif