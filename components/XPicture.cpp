#include "XPicture.h"
#include "char_code_change.h"

XPicture::XPicture(HWND hwnd, UINT id, LPRECT rect, COLORREF clBack, bool DrawBK) :XControlBase(hwnd, id, rect),
m_BrushFrame(NULL), m_BrushRect(NULL), m_Image(NULL), m_ImgMem(NULL), m_DrawBK(DrawBK)
{
	InitBackColor(clBack);
}

XPicture::XPicture(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh, COLORREF clBack, bool DrawBK) :XControlBase(hwnd, id, ptSrc, szWh),
m_BrushFrame(NULL), m_BrushRect(NULL), m_Image(NULL), m_ImgMem(NULL), m_DrawBK(DrawBK)
{
	InitBackColor(clBack);
}

XPicture::XPicture(HWND hwnd, UINT id, int x, int y, int w, int h, COLORREF clBack, bool DrawBK) :XControlBase(hwnd, id, x, y, w, h),
m_BrushFrame(NULL), m_BrushRect(NULL), m_Image(NULL), m_ImgMem(NULL), m_DrawBK(DrawBK)
{
	InitBackColor(clBack);
}

XPicture::~XPicture()
{
	if (m_BrushFrame)
		delete m_BrushFrame;
	if (m_BrushRect)
		delete m_BrushRect;
	if (m_Image)
		delete m_Image;
	if (m_ImgMem)
		GlobalFree(m_ImgMem);
}

bool XPicture::OnPaint(HDC hdc)
{
	if (!XControlBase::OnPaint(hdc)) return false;

	if (!m_Image) return true;

	Gdiplus::Graphics g(hdc);
	Gdiplus::Rect rect(m_WorkRect.left + 1, m_WorkRect.top + 1, m_WorkRect.right - m_WorkRect.left - 2, m_WorkRect.bottom - m_WorkRect.top - 2);
	Gdiplus::Rect rectImg;
	rectImg.X = rect.X + rect.Width / 10;
	rectImg.Y = rect.Y + rect.Height / 5;
	rectImg.Width = rect.Width * 4 / 5;
	rectImg.Height = rect.Height * 3 / 5;

	if (m_DrawBK && (m_Status == X_BTNS_HOVER || m_Status == X_BTNS_DOWN || m_Status == X_BTNS_UP))
	{
		Gdiplus::Pen pen(m_BrushFrame, 2.0);
		g.DrawRectangle(&pen, rect);
		if (m_Status == X_BTNS_DOWN)
		{
			rect.X += 1;
			rect.Y += 1;
			rect.Width -= 2;
			rect.Height -= 2;

			rectImg.X += 1;
			rectImg.Y += 1;

			g.FillRectangle(m_BrushRect, rect);
		}
	}

	g.DrawImage(m_Image, rectImg);
}

void XPicture::InitBackColor(COLORREF color)
{
	XColor c(color);
	c.a = 0x50;
	Gdiplus::ARGB argbFrame, argbRect;

	argbRect = c._ARGB();

	c.a = 0xA0;

	if (MAX_A(c.r, c.b, c.g))
	{
		XColor x(c.a, 0xFF, c.g, c.b);
		argbFrame = x._ARGB();
	}
	else if (MAX_A(c.g, c.r, c.b))
	{
		XColor x(c.a, c.r, 0xFF, c.b);
		argbFrame = x._ARGB();
	}
	else
	{
		XColor x(c.a, c.r, c.g, 0xFF);
		argbFrame = x._ARGB();
	}
	m_BrushFrame = new Gdiplus::SolidBrush(Gdiplus::Color(argbFrame));
	m_BrushRect = new Gdiplus::SolidBrush(Gdiplus::Color(argbRect));
}

void	XPicture::SetImage(const char *filename)
{
	wchar_t *buf = AnsiToUnicode(filename);
	m_Image = new Gdiplus::Image(buf);
	free(buf);
}

void	XPicture::SetImage(UINT uResourceID, LPCTSTR resType)
{
	LoadImageFromResource(&m_Image, &m_ImgMem, uResourceID, resType);
}
