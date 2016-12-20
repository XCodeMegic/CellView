#pragma once
class XGdiPlusInit
{
private:
	ULONG_PTR ptr;
public:
	XGdiPlusInit();
	~XGdiPlusInit();
};

typedef enum
{
	Save_bmp,
	Save_png,
	Save_jpg
}XPicSaveType;

extern bool SaveScreenPicture(XPicSaveType type, LPCWSTR lfFileName);

