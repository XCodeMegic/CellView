#pragma once

#include "XControlBase.h"

class XButton : public XControlBase
{
public:
	XButton(HWND hwnd, UINT id, LPRECT rect);
	XButton(HWND hwnd, UINT id, LPPOINT ptSrc, LPSIZE szWh);
	XButton(HWND hwnd, UINT id, int x, int y, int w, int h);
	virtual ~XButton();
public:
	//XControlBase method
	virtual bool OnPaint(HDC hdc);
	virtual XCtrlType GetCtrlType() { return XCONTROL_BUTTON; }

private:
	Gdiplus::Image *	m_Image;
	HGLOBAL				m_ImgMem;
public:
	void	SetBkImage(Gdiplus::Image *img);
	void	SetBkImage(const char *filename);
	void	SetBkImage(UINT uResourceID, LPCTSTR resType);
};