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

/*lpRect Ϊ�����ʾȫ������
 */
static HBITMAP CaptureScreenToBitmap(LPRECT lpRect)
{
	//��ĻDC �� �ڴ�DC
	HDC	hSrcDC, hMemDC;

	//��ĻDC���ڴ�DC ��ͼ����
	HBITMAP hBitmap, hOldBitmap;

	//��������
	int nx, ny, nx1, ny1;

	//���򳤿�	
	int nWidth, nHeight;

	//λͼ�ֱ���
	int xScm, yScm;

	//������ĻDC
	hSrcDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

	//�����ڴ�DC
	hMemDC = CreateCompatibleDC(hSrcDC);

	//��ȡ��Ļ�ֱ���
	xScm = GetDeviceCaps(hSrcDC, HORZRES);
	yScm = GetDeviceCaps(hSrcDC, VERTRES);

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = xScm;
	rect.bottom = yScm;

	//lpRect Ϊ����Ϊȫ������
	if (lpRect == NULL || IsRectEmpty(lpRect))
		lpRect = &rect;

	//ȷ������ɼ�
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

	//�����ڴ�DC ����λͼ
	hBitmap = CreateCompatibleBitmap(hSrcDC, nWidth, nHeight);

	//��λͼѡ���ڴ�DC
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	//������Ļͼ���ڴ��豸
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, nx, ny, SRCCOPY);

	//�õ�Ŀ��λͼ���(��ʵ����hBitmap)
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	//��������Դ��
	//�����������ͣ���һ���н���λͼ�滻��λͼ���Ƿ�ֹ���
	//�ڴ�DC����ʱ��Ŀ��λͼ��Դ�ͷŵ�
	DeleteDC(hSrcDC);
	DeleteDC(hMemDC);

	//����Ŀ����
	return hBitmap;
}

//ֱ�ӱ���bmpԴͼ�񣬱����ļ��Ƚϴ�
static int SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName)
{
	//�豸DC
	HDC hdc;

	//��ǰ�ֱ�����ÿ��������ռ�ֽ���
	WORD	iBits;

	//λͼ��ÿ��������ռ�ֽ���
	WORD	wBitCount;

	//��ɫ���С
	DWORD	dwPaletteSize = 0;

	//λͼ�������ֽڴ�С
	DWORD	dwBmBitsSize;

	//λͼ�ļ���С
	DWORD	dwDIBSize;

	//д���ļ��ֽ���
	DWORD	dwWritten;

	//λͼ���Խṹ
	BITMAP	Bitmap;

	//λͼ �ļ�ͷ �ṹ
	BITMAPFILEHEADER	bmfHdr;

	//λͼ��Ϣͷ�ṹ
	BITMAPINFOHEADER	bi;

	//��Ϣͷ�ṹָ��
	LPBITMAPINFOHEADER	lpbi;

	//���־��
	HANDLE	/*�ļ����*/fh, /*�ڴ���*/hDib, /*��ɫ����*/hPal;

	HPALETTE	hOldPal = NULL;

	//����λͼ�ļ�ÿ��������ռ�ֽ���
	hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
	DeleteDC(hdc);

	wBitCount = iBits <= 1 ? 1 : (iBits <= 4 ? 4 : (iBits <= 8 ? 8 : (iBits <= 24 ? 24 : 32)));

	//�����ɫ���С
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount)*sizeof(RGBQUAD);

	//����λͼ��Ϣͷ�ṹ
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

	//����λͼռ���ڴ��С ,��֤ÿ��Ϊ4�ֽ�������
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//�����ڴ�
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);

	//������Ϣ�ṹ���ڴ��
	*lpbi = bi;

	//�����ɫ��
	hPal = GetStockObject(DEFAULT_PALETTE);
	{
		hdc = ::GetDC(NULL);// ��ȡ����DC
		hOldPal = SelectPalette(hdc, (HPALETTE)hPal, FALSE);
		RealizePalette(hdc);
	}

	//��ȡ�õ�ɫ�����µ�����ֵ
	GetDIBits(hdc, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
		(BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	//�ָ���ɫ��
	if (hOldPal)
	{
		SelectPalette(hdc, hOldPal, TRUE);
		RealizePalette(hdc);
		::ReleaseDC(NULL, hdc);
	}
	
	//����λͼ�ļ�
	fh = CreateFileA(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	//����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42; //"BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPINFOHEADER)+(DWORD)sizeof(BITMAPFILEHEADER)+dwPaletteSize;

	//д���ļ�ͷ
	WriteFile(fh, &bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	//д����������
	WriteFile(fh, lpbi, sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize, &dwWritten, NULL);

	//�����Դ
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

//��ע��: δ���ͷ��ڴ��
static bool SaveBitmapToStream(IStream **stream, HBITMAP hBitmap)
{
	//�豸DC
	HDC hdc;

	//��ǰ�ֱ�����ÿ��������ռ�ֽ���
	WORD	iBits;

	//λͼ��ÿ��������ռ�ֽ���
	WORD	wBitCount;

	//��ɫ���С
	DWORD	dwPaletteSize = 0;

	//λͼ�������ֽڴ�С
	DWORD	dwBmBitsSize;

	//λͼ�ļ���С
	DWORD	dwDIBSize;

	//д���ļ��ֽ���
	DWORD	dwWritten;

	//λͼ���Խṹ
	BITMAP	Bitmap;

	//λͼ �ļ�ͷ �ṹ
	BITMAPFILEHEADER	bmfHdr;

	//λͼ��Ϣͷ�ṹ
	BITMAPINFOHEADER	bi;

	//��Ϣͷ�ṹָ��
	LPBITMAPINFOHEADER	lpbi;

	//�ڴ��ָ��
	LPVOID	lpBuffer;

	//���־��
	HANDLE	/*�ļ����*/fh, /*�ڴ���*/hDib, /*��ɫ����*/hPal;

	HPALETTE	hOldPal = NULL;

	//����λͼ�ļ�ÿ��������ռ�ֽ���
	hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hdc, BITSPIXEL) * GetDeviceCaps(hdc, PLANES);
	DeleteDC(hdc);

	wBitCount = iBits <= 1 ? 1 : (iBits <= 4 ? 4 : (iBits <= 8 ? 8 : (iBits <= 24 ? 24 : 32)));

	//�����ɫ���С
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount)*sizeof(RGBQUAD);

	//����λͼ��Ϣͷ�ṹ
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

	//����λͼռ���ڴ��С ,��֤ÿ��Ϊ4�ֽ�������
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//�����ڴ�
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER));
	//lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);

	lpBuffer = GlobalLock(hDib);

	//������Ϣ�ṹ���ڴ��
	//*lpbi = bi;

	//�����ɫ��
	hPal = GetStockObject(DEFAULT_PALETTE);
	{
		hdc = ::GetDC(NULL);// ��ȡ����DC
		hOldPal = SelectPalette(hdc, (HPALETTE)hPal, FALSE);
		RealizePalette(hdc);
	}

	//��ȡ�õ�ɫ�����µ�����ֵ
	GetDIBits(hdc, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpBuffer + sizeof(BITMAPINFOHEADER)+dwPaletteSize + sizeof(BITMAPFILEHEADER),
		(BITMAPINFO*)&bi, DIB_RGB_COLORS);

	//�ָ���ɫ��
	if (hOldPal)
	{
		SelectPalette(hdc, hOldPal, TRUE);
		RealizePalette(hdc);
		::ReleaseDC(NULL, hdc);
	}

	//����λͼ�ļ�ͷ
	bmfHdr.bfType = 0x4D42; //"BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPINFOHEADER)+(DWORD)sizeof(BITMAPFILEHEADER)+dwPaletteSize;

	//����λͼ�ļ�ͷ
	memcpy(lpBuffer, &bmfHdr, sizeof(BITMAPFILEHEADER));

	//����λͼ��Ϣͷ
	memcpy((LPSTR)lpBuffer + sizeof(BITMAPFILEHEADER), &bi, sizeof(BITMAPINFOHEADER));

	GlobalUnlock(hDib);
	//���
	if (FAILED(CreateStreamOnHGlobal(hDib, TRUE, stream)))
		return false;
	return true;
}

//---------------------- ���ڲ��Կ������ Begin -------------------------------------------
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
//---------------------- ���ڲ��Կ������ End -------------------------------------------

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
