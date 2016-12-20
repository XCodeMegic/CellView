#include "XButton.h"
#include "char_code_change.h"

XButton::XButton(HWND hwnd, UINT id, LPRECT rect) : XControlBase(hwnd, id, rect), m_Image(NULL), m_ImgMem(NULL)
{
	
}

XButton::XButton(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh) : XControlBase(hwnd, id, ptSrc, szWh), m_Image(NULL), m_ImgMem(NULL)
{
	
}

XButton::XButton(HWND hwnd, UINT id, int x, int y, int w, int h) : XControlBase(hwnd,id,x,y,w,h),m_Image(NULL),m_ImgMem(NULL)
{
	
}

XButton::~XButton()
{
	if (m_Image)
		delete m_Image;
	if (m_ImgMem)
		GlobalFree(m_ImgMem);
}

void	XButton::SetBkImage(Gdiplus::Image *img)
{
	m_Image = img;
}

void	XButton::SetBkImage(const char *filename)
{
	wchar_t *buf = AnsiToUnicode(filename);
	m_Image = new Gdiplus::Image(buf);
	free(buf);
}

void	XButton::SetBkImage(UINT uResourceID, LPCTSTR resType)
{
	LoadImageFromResource(&m_Image, &m_ImgMem, uResourceID, resType);
}

bool XButton::OnPaint(HDC hdc)
{
	if (!XControlBase::OnPaint(hdc)) return false;

	Gdiplus::Graphics g(hdc);
	Gdiplus::Rect rect(m_WorkRect.left, m_WorkRect.top, m_WorkRect.right - m_WorkRect.left, m_WorkRect.bottom - m_WorkRect.top);
	if (m_Image)
	{
		int w = m_Image->GetWidth();
		int h = m_Image->GetHeight();
		w = w / 4;
		g.DrawImage(m_Image, rect, (int)m_Status * w, 0, w, h, Gdiplus::Unit::UnitPixel);
	}
	return true;
}
