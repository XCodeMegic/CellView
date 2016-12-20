#include "XLabel.h"



static DWORD Ansi2Unicode(const char *inbuf, wchar_t *outbuf, DWORD outbufSize)
{
	DWORD dwRet = MultiByteToWideChar(CP_ACP, 0, inbuf, -1, outbuf, outbufSize);
	return dwRet;
}

static Gdiplus::SizeF GetTextBounds(const Gdiplus::Font *font, const Gdiplus::StringFormat& strFormat, const wchar_t* szText, DWORD dwText)
{
	Gdiplus::GraphicsPath g;
	Gdiplus::FontFamily fontfamily;
	font->GetFamily(&fontfamily);

	g.AddString(szText, dwText, &fontfamily, font->GetStyle(), font->GetSize(), Gdiplus::PointF(0, 0), &strFormat);

	Gdiplus::RectF rcBound;
	g.GetBounds(&rcBound);
	return Gdiplus::SizeF(rcBound.Width, rcBound.Height);
}

//-------------------------- XLabel ------------------------------------------------

XLabel::XLabel(HWND hwnd, UINT id, LPRECT rect, COLORREF clBack, bool defaultFont, bool DrawBK) : XControlBase(hwnd, id, rect),
m_BrushFont(NULL), m_Font(NULL), m_BrushFrame(NULL), m_BrushRect(NULL), m_DrawBK(DrawBK)
{
	m_String = "label";
	InitBackColor(clBack);
	if (defaultFont)
		SetFont(0,0);
}

XLabel::XLabel(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh, COLORREF clBack, bool defaultFont, bool DrawBK) : XControlBase(hwnd, id, ptSrc, szWh),
m_BrushFont(NULL), m_Font(NULL), m_BrushFrame(NULL), m_BrushRect(NULL), m_DrawBK(DrawBK)
{
	m_String = "label";
	InitBackColor(clBack);
	if (defaultFont)
		SetFont(0, 0);
}

XLabel::XLabel(HWND hwnd, UINT id, int x, int y, int w, int h, COLORREF clBack, bool defaultFont, bool DrawBK) : XControlBase(hwnd, id, x, y, w, h),
m_BrushFont(NULL), m_Font(NULL), m_BrushFrame(NULL), m_BrushRect(NULL), m_DrawBK(DrawBK)
{
	m_String = "label";
	InitBackColor(clBack);
	if (defaultFont)
		SetFont(0, 0);
}

XLabel::~XLabel()
{
	if (m_BrushFont)
		delete m_BrushFont;
	if (m_BrushFrame)
		delete m_BrushFrame;
	if (m_BrushRect)
		delete m_BrushRect;
	if (m_Font)
		delete m_Font;
}

void XLabel::InitBackColor(COLORREF color)
{
	XColor c(color);
	c.a = 0x30;
	Gdiplus::ARGB argbFrame,argbRect;

	argbRect = c._ARGB();

	if (MAX_A(c.r, c.b, c.g))
	{
		XColor x(0x80, 0xFF, c.g, c.b);
		argbFrame = x._ARGB();
	}
	else if (MAX_A(c.g, c.r, c.b))
	{
		XColor x(0x80, c.r, 0xFF, c.b);
		argbFrame = x._ARGB();
	}
	else
	{
		XColor x(0x80, c.r, c.g, 0xFF);
		argbFrame = x._ARGB();
	}
	m_BrushFrame = new Gdiplus::SolidBrush(Gdiplus::Color(argbFrame));
	m_BrushRect = new Gdiplus::SolidBrush(Gdiplus::Color(argbRect));
}

bool XLabel::OnPaint(HDC hdc)
{
	if (!XControlBase::OnPaint(hdc)) return false;
	if (!m_Font)
	{
		m_lfont.lfHeight = -MulDiv(m_lfont.lfHeight, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		m_Font = new Gdiplus::Font(hdc, &m_lfont);
	}
	Gdiplus::Graphics g(hdc);
	Gdiplus::Rect rect(m_WorkRect.left + 1, m_WorkRect.top + 1, m_WorkRect.right - m_WorkRect.left - 2, m_WorkRect.bottom - m_WorkRect.top - 2);

	wchar_t *buf = new wchar_t[m_String.length() + 1];
	DWORD dwSize = Ansi2Unicode(m_String.c_str(), buf, m_String.length() + 1);
	Gdiplus::StringFormat fmt;
	Gdiplus::SizeF szText = GetTextBounds(m_Font, fmt, buf, dwSize);
	
	Gdiplus::Rect rectString;
	rectString.Width = szText.Width;
	rectString.Height = szText.Height;
	rectString.X = rect.X + (rect.Width - rectString.Width) / 2;
	rectString.Y = rect.Y + (rect.Height - rectString.Height) / 2;



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

			rectString.Y += 4;

			g.FillRectangle(m_BrushRect, rect);
		}
	}
	
	Gdiplus::PointF pt(rectString.X, rectString.Y);
	g.DrawString(buf, dwSize, m_Font, pt, &fmt, m_BrushFont);
	delete[] buf;
	return true;
}

void XLabel::SetFont(int dwSize, COLORREF color, bool bBold, bool bItalic)
{
	LOGFONT *lft = &m_lfont;
	memset(lft, 0, sizeof LOGFONT);
	lft->lfHeight = dwSize;
	lft->lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	lft->lfItalic = bItalic ? TRUE : FALSE;
	lft->lfCharSet = GB2312_CHARSET;
	lft->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lft->lfOutPrecision = OUT_DEFAULT_PRECIS;
	lft->lfQuality = DEFAULT_QUALITY;
	lft->lfPitchAndFamily = DEFAULT_PITCH;
	wsprintf(lft->lfFaceName, "ËÎÌו");

	XColor c(color);
	c.a = 0xFF;
	Gdiplus::ARGB argb = c._ARGB();
	m_BrushFont = new Gdiplus::SolidBrush(Gdiplus::Color(argb));

}

void XLabel::SetText(std::string &str) 
{ 
	m_String = str; 
	m_OldStatus = X_BTNS_NEEDRP;
	InvalidateRect(m_MainWnd, &m_WorkRect, TRUE);
}
