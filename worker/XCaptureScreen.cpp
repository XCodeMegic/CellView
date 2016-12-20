#include <Windows.h>
#include <gdiplus.h>
#include "XCaptureScreen.h"

#pragma comment(lib,"gdiplus.lib")


XGdiPlusInit::XGdiPlusInit()
{
	Gdiplus::GdiplusStartupInput g_gdiStartupInput;
	
	Gdiplus::GdiplusStartup(&ptr, &g_gdiStartupInput, NULL);
}


XGdiPlusInit::~XGdiPlusInit()
{
	Gdiplus::GdiplusShutdown(ptr);
}

/*lpRect 为空则表示全屏拷贝
 */
static HBITMAP CaptureScreenToBitmap(LPRECT lpRect)
{
	//屏幕DC 和 内存DC
	HDC	hSrcDC, hMemDC;

	//屏幕DC和内存DC 的图像句柄
	HBITMAP hBitmap, hOldBitmap;

	//区域坐标
	int nx, ny, nx1, ny1;

	//区域长宽	
	int nWidth, nHeight;

	//位图分辨率
	int xScm, yScm;

	//创建屏幕DC
	hSrcDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

	//创建内存DC
	hMemDC = CreateCompatibleDC(hSrcDC);

	//获取屏幕分辨率
	xScm = GetDeviceCaps(hSrcDC, HORZRES);
	yScm = GetDeviceCaps(hSrcDC, VERTRES);

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = xScm;
	rect.bottom = yScm;

	//lpRect 为空则为全屏拷贝
	if (lpRect == NULL || IsRectEmpty(lpRect))
		lpRect = &rect;

	//确保区域可见
	nx = lpRect->left;
	ny = lpRect->top;
	nx1 = lpRect->right;
	ny1 = lpRect->bottom;
	nx = nx < 0 ? 0 : nx;
	ny = ny < 0 ? 0 : ny;
	nx1 = nx1 > xScm ? xScm : nx1;
	ny1 = ny1 > yScm ? yScm : ny1;
	nWidth = nx1 - nx;
	nHeight = ny1 - ny;

	//创建内存DC 所用位图
	hBitmap = CreateCompatibleBitmap(hSrcDC, nWidth, nHeight);

	//将位图选入内存DC
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	//拷贝屏幕图像到内存设备
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, nx, ny, SRCCOPY);

	//得到目标位图句柄(其实就是hBitmap)
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	//清除相关资源，
	//这里稍作解释，上一步中将旧位图替换新位图，是防止清除
	//内存DC操作时将目标位图资源释放掉
	DeleteDC(hSrcDC);
	DeleteDC(hMemDC);

	//返回目标句柄
	return hBitmap;
}

//直接保存bmp源图像，保存文件比较大
static int SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName)
{
	//设备DC
	HDC hdc;

	//当前分辨率下每个像素所占字节数
	WORD	iBits;

	//位图中每个像素所占字节数
	WORD	wBitCount;

	//调色板大小
	DWORD	dwPaletteSize = 0;

	//位图中像素字节大小
	DWORD	dwBmBitsSize;

	//位图文件大小
	DWORD	dwDIBSize;

	//写入文件字节数
	DWORD	dwWritten;

	//位图属性结构
	BITMAP	Bitmap;

	//位图 文件头 结构
	BITMAPFILEHEADER	bmfHdr;

	//位图信息头结构
	BITMAPINFOHEADER	bi;

	//信息头结构指针
	LPBITMAPINFOHEADER	lpbi;

	//各种句柄
	HANDLE	/*文件句柄*/fh, /*内存句柄*/hDib, /*调色板句柄*/hPal;

	HPALETTE	hOldPal = NULL;

	//计算位图文件每个像素所占字节数
	hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
	DeleteDC(hdc);

	wBitCount = iBits <= 1 ? 1 : (iBits <= 4 ? 4 : (iBits <= 8 ? 8 : (iBits <= 24 ? 24 : 32)));

	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount)*sizeof(RGBQUAD);

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	//计算位图占用内存大小 ,保证每行为4字节整数倍
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//分配内存
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);

	//拷贝信息结构至内存块
	*lpbi = bi;

	//处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	{
		hdc = ::GetDC(NULL);// 获取桌面DC
		hOldPal = SelectPalette(hdc, (HPALETTE)hPal, FALSE);
		RealizePalette(hdc);
	}

	//获取该调色板下新的像素值
	GetDIBits(hdc, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
		(BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal)
	{
		SelectPalette(hdc, hOldPal, TRUE);
		RealizePalette(hdc);
		::ReleaseDC(NULL, hdc);
	}
	
	//创建位图文件
	fh = CreateFileA(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	//设置位图文件头
	bmfHdr.bfType = 0x4D42; //"BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPINFOHEADER)+(DWORD)sizeof(BITMAPFILEHEADER)+dwPaletteSize;

	//写入文件头
	WriteFile(fh, &bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	//写入其余内容
	WriteFile(fh, lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize, &dwWritten, NULL);

	//清除资源
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

//请注意: 未有释放内存块
static bool SaveBitmapToStream(IStream **stream, HBITMAP hBitmap)
{
	//设备DC
	HDC hdc;

	//当前分辨率下每个像素所占字节数
	WORD	iBits;

	//位图中每个像素所占字节数
	WORD	wBitCount;

	//调色板大小
	DWORD	dwPaletteSize = 0;

	//位图中像素字节大小
	DWORD	dwBmBitsSize;

	//位图文件大小
	DWORD	dwDIBSize;

	//写入文件字节数
	DWORD	dwWritten;

	//位图属性结构
	BITMAP	Bitmap;

	//位图 文件头 结构
	BITMAPFILEHEADER	bmfHdr;

	//位图信息头结构
	BITMAPINFOHEADER	bi;

	//信息头结构指针
	LPBITMAPINFOHEADER	lpbi;

	//内存块指针
	LPVOID	lpBuffer;

	//各种句柄
	HANDLE	/*文件句柄*/fh, /*内存句柄*/hDib, /*调色板句柄*/hPal;

	HPALETTE	hOldPal = NULL;

	//计算位图文件每个像素所占字节数
	hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
	DeleteDC(hdc);

	wBitCount = iBits <= 1 ? 1 : (iBits <= 4 ? 4 : (iBits <= 8 ? 8 : (iBits <= 24 ? 24 : 32)));

	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount)*sizeof(RGBQUAD);

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	//计算位图占用内存大小 ,保证每行为4字节整数倍
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//分配内存
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER));
	//lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);

	lpBuffer = GlobalLock(hDib);

	//拷贝信息结构至内存块
	//*lpbi = bi;

	//处理调色板
	hPal = GetStockObject(DEFAULT_PALETTE);
	{
		hdc = ::GetDC(NULL);// 获取桌面DC
		hOldPal = SelectPalette(hdc, (HPALETTE)hPal, FALSE);
		RealizePalette(hdc);
	}

	//获取该调色板下新的像素值
	GetDIBits(hdc, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpBuffer + sizeof(BITMAPINFOHEADER)+dwPaletteSize + sizeof(BITMAPFILEHEADER),
		(BITMAPINFO*)&bi, DIB_RGB_COLORS);

	//恢复调色板
	if (hOldPal)
	{
		SelectPalette(hdc, hOldPal, TRUE);
		RealizePalette(hdc);
		::ReleaseDC(NULL, hdc);
	}

	//设置位图文件头
	bmfHdr.bfType = 0x4D42; //"BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPINFOHEADER)+(DWORD)sizeof(BITMAPFILEHEADER)+dwPaletteSize;

	//拷贝位图文件头
	memcpy(lpBuffer, &bmfHdr, sizeof(BITMAPFILEHEADER));

	//拷贝位图信息头
	memcpy((LPSTR)lpBuffer + sizeof(BITMAPFILEHEADER), &bi, sizeof(BITMAPINFOHEADER));

	GlobalUnlock(hDib);
	//完成
	if (FAILED(CreateStreamOnHGlobal(hDib, TRUE, stream)))
		return false;
	return true;
}

//---------------------- 用于测试拷贝结果 Begin -------------------------------------------
#define TEST_WINDOW_IMAGE 0
#if TEST_WINDOW_IMAGE
static Gdiplus::Image *m_Image;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_PAINT)
	{
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(hwnd, &ps);

		if (m_Image)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			Gdiplus::Rect rc(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
			Gdiplus::Graphics g(dc);
			g.DrawImage(m_Image, rc);
		}

		EndPaint(hwnd, &ps);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void showWnd(){
	WNDCLASS wndclass;
	memset(&wndclass, 0, sizeof(WNDCLASS));

	wndclass.hInstance = GetModuleHandle(NULL);
	wndclass.lpszClassName = TEXT("xshow");
	wndclass.lpfnWndProc = WndProc;

	RegisterClass(&wndclass);

	HWND h = CreateWindow(TEXT("xshow"), TEXT("show"), WS_BORDER | WS_CAPTION, 0, 0, 640, 480, NULL, NULL, wndclass.hInstance, NULL);

	if (!h)
		return;
	ShowWindow(h, SW_SHOW);
	UpdateWindow(h);

	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
#endif
//---------------------- 用于测试拷贝结果 End -------------------------------------------

static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

static bool SaveScreenPictureToFILE(LPWSTR ctTyp, LPCWSTR lfFileName)
{
	HBITMAP h = NULL;

	CLSID clsid;

	bool ret = false;

	if (GetEncoderClsid(ctTyp, &clsid) < 0) return false;


	h = CaptureScreenToBitmap(NULL);

	if (!h) return false;

	IStream *stream = NULL;

	if (SaveBitmapToStream(&stream, h))
	{
		Gdiplus::Image image(stream);

		image.Save(lfFileName, &clsid);

		stream->Release();

		ret = true;
	}
	else
		ret = false;

	DeleteObject(h);
	return ret;
}

bool SaveScreenPicture(XPicSaveType type, LPCWSTR lfFileName)
{
	LPWSTR strContentType = NULL;

	switch (type)
	{
	case Save_bmp:
		strContentType = L"image/bmp";
		break;
	case Save_png:
		strContentType = L"image/png";
		break;
	case Save_jpg:
		strContentType = L"image/jpeg";
		break;
	}
	return SaveScreenPictureToFILE(strContentType, lfFileName);
}
