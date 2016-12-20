#pragma once

#include "XControlBase.h"

class XPicture : public XControlBase
{
public:
	XPicture(HWND hwnd, UINT id, LPRECT rect, COLORREF clBack, bool DrawBK = true);
	XPicture(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh, COLORREF clBack, bool DrawBK = true);
	XPicture(HWND hwnd, UINT id, int x, int y, int w, int h, COLORREF clBack, bool DrawBK = true);
	virtual ~XPicture();
protected:
	//XControlBase method
	virtual bool OnPaint(HDC hdc);
	virtual XCtrlType GetCtrlType() { return XCONTROL_PICTURE; }

private:
	Gdiplus::Image*		m_Image;
	Gdiplus::Brush*		m_BrushFrame;
	Gdiplus::Brush*		m_BrushRect;

	bool				m_DrawBK;

	//mem
	HGLOBAL				m_ImgMem;

private:
	void InitBackColor(COLORREF color);
public:
	void	SetImage(const char *filename);
	void	SetImage(UINT uResourceID, LPCTSTR resType);
};